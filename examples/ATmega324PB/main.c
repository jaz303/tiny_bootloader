#define F_CPU 8000000UL
#define BAUD 38400

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

void (*main_program)(void) = 0x0000;

// NB page size is in bytes, not words
#define TINY_BOOTLOADER_PAGE_SIZE SPM_PAGESIZE
#define TINY_BOOTLOADER_PAGE_COUNT 240
#define TINY_BOOTLOADER_READ_BYTE() uart_read()
#define TINY_BOOTLOADER_WRITE_BYTE(b) uart_write(b)
#define TINY_BOOTLOADER_WRITE_PAGE(p, d) write_page(p, d)
#define TINY_BOOTLOADER_COMPARE_PAGE(p, d) compare_page(p, d)
#define TINY_BOOTLOADER_REBOOT() reboot()

inline char bootloader_requested() {
	DDRC &= ~(1 << 6); // C6 input
	PORTC |= (1 << 6); // C6 pullup enable
	return !(PINC & (1 << 6));
}

inline void init() {
	// Disable prescaler
	CLKPR = (1 << CLKPCE);
	CLKPR = 0x00;
	
	// Set up UART
	unsigned int ubrr = F_CPU / 8 / BAUD - 1;
	UCSR0A = (1 << U2X); // Double speed
	UCSR0C = (1 << UCSZ1) | (1 << UCSZ0); // Asynchronous, 8N1
	UBRR0H = (unsigned char)(ubrr >> 8); // Baud rate (H)
	UBRR0L = (unsigned char)ubrr; // Baud rate (L)
	UCSR0B = (1 << RXEN) | (1 << TXEN); // Enable transmitter and receiver
}

char uart_read() {
	while (!(UCSR0A & (1 << RXC)));
	return UDR0;
}

inline void uart_write(char c) {
	while (!(UCSR0A & (1 << UDRE)));
	UDR0 = c;
}

char write_page(uint16_t page, char *data) {
	uint32_t page_address = page * TINY_BOOTLOADER_PAGE_SIZE;
	boot_page_erase(page_address);
	boot_spm_busy_wait();
	for (uint16_t i = 0; i < TINY_BOOTLOADER_PAGE_SIZE; i += 2) {
		uint16_t word = *data++;
		word |= (*data++) << 8;
		boot_page_fill(page_address + i, word);
	}
	boot_page_write(page_address);
	boot_spm_busy_wait();
	boot_rww_enable();
	return 0;
}

char compare_page(uint16_t page, char *data) {
	uint16_t rom_start = page * TINY_BOOTLOADER_PAGE_SIZE;
	for (uint16_t i = 0; i < TINY_BOOTLOADER_PAGE_SIZE; ++i) {
		char byte = pgm_read_byte((void*)(rom_start + i));
		if (byte != data[i]) {
			return -1;
		}
	}
	return 0;
}

void reboot() {
	main_program();
	
	//wdt_enable(WDTO_60MS);
	for(;;) {}
}

#include "tiny_bootloader.h"
#define TINY_BOOTLOADER_IMPL
#include "tiny_bootloader.h"

int main(void)
{
	//MCUSR &= ~WDRF;
	//wdt_disable();
	
	if (bootloader_requested()) {
		cli();
		init();	
		bootloader_run();
	} else {
		main_program();
	}
	while (1);
}
