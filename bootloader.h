#ifndef TINY_BOOTLOADER_IMPL

void bootloader_run(void);

#else

#include <stdint.h>

#define MSG_START               0x7D
#define MSG_ESC                 0x7E
#define MSG_END                 0x7F
#define MSG_XOR                 0x20

#define COMMAND_SIZE            4

#define CMD_PING				0x01
#define CMD_DESCRIBE			0x02
#define CMD_WRITE_PAGE          0x03
#define CMD_VERIFY_PAGE         0x04
#define CMD_REBOOT              0x05

#define STATUS_OK               (-1)
#define STATUS_OP_FAILED        (-2)
#define STATUS_UNKNOWN_COMMAND  (-3)
#define STATUS_BAD_REQUEST      (-4)
#define STATUS_OVERFLOW         (-5)
#define STATUS_INVALID_PAGE     (-6)

#define BUFFER_SIZE             (COMMAND_SIZE + TINY_BOOTLOADER_PAGE_SIZE)

static char buffer[BUFFER_SIZE];

static int read_command() {
	char state = 0;
	int pos;
	while (1) {
		char ch = TINY_BOOTLOADER_READ_BYTE();
		if (ch == MSG_START) {
			state = 1;
			pos = 0;
			continue;
		}
		switch (state) {
			case 1:
			{
				if (ch == MSG_END) {
					state = 0;
					return pos;
				} else if (ch == MSG_ESC) {
					state = 2;
				} else if (pos == BUFFER_SIZE) {
					state = 3;
				} else {
					buffer[pos++] = ch;
				}
				break;
			}
			case 2:
			{
				if (pos == BUFFER_SIZE) {
					state = 3;
				} else {
					buffer[pos++] = ch ^ MSG_XOR;
					state = 1;
				}
				break;
			}
			case 3:
			{
				if (ch == MSG_END) {
					return STATUS_OVERFLOW;
				}
				break;
			}
		}
	}
}

void send_reply(int len) {
	TINY_BOOTLOADER_WRITE_BYTE(MSG_START);
	for (int i = 0; i < len; ++i) {
		char b = buffer[i];
		if (b == MSG_START || b == MSG_ESC || b == MSG_END) {
			TINY_BOOTLOADER_WRITE_BYTE(MSG_ESC);
			b ^= MSG_XOR;
		}
		TINY_BOOTLOADER_WRITE_BYTE(b);
	}
	TINY_BOOTLOADER_WRITE_BYTE(MSG_END);
}

void send_status(char status) {
	buffer[0] = status;
	send_reply(1);
}

void bootloader_run(void) {
	while (1) {
		int len = read_command();
		if (len < 0) {
			send_status(len);
			continue;
		} else if (len == 0) {
			send_status(STATUS_BAD_REQUEST);
			continue;
		}
		switch (buffer[0]) {
			case CMD_PING:
			{
				buffer[0] = STATUS_OK;
				send_reply(len);
				break;
			}
			case CMD_DESCRIBE:
			{
				buffer[0] = STATUS_OK;
				buffer[1] = (TINY_BOOTLOADER_PAGE_SIZE >> 8);
				buffer[2] = TINY_BOOTLOADER_PAGE_SIZE;
				buffer[3] = (TINY_BOOTLOADER_PAGE_COUNT >> 8);
				buffer[4] = TINY_BOOTLOADER_PAGE_COUNT;
				send_reply(5);
				break;
			}
			case CMD_WRITE_PAGE:
			case CMD_VERIFY_PAGE:
			{
				if (len != BUFFER_SIZE) {
					send_status(STATUS_BAD_REQUEST);
				} else {
					uint16_t page = (buffer[2] << 8) | buffer[3];
					if (page >= TINY_BOOTLOADER_PAGE_COUNT) {
						send_status(STATUS_INVALID_PAGE);
					} else {
						char res;
						if (buffer[0] == CMD_WRITE_PAGE) {
							res = TINY_BOOTLOADER_WRITE_PAGE(page, buffer + COMMAND_SIZE);
						} else {
							res = TINY_BOOTLOADER_COMPARE_PAGE(page, buffer + COMMAND_SIZE);
						}
						send_status(res == 0 ? STATUS_OK : STATUS_OP_FAILED);
					}
				}
				break;
			}
			case CMD_REBOOT:
			{
				send_status(STATUS_OK);
				TINY_BOOTLOADER_REBOOT();
				break;
			}
			default:
			{
				send_status(STATUS_UNKNOWN_COMMAND);
				break;
			}
		}
	}
}

#endif
