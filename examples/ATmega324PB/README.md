# ATmega324PB

Developed and tested on the ATMega324PB Xplained Pro devkit with AtmelStudio 7.0.

Host communication is via UART0 on the EXT1 header (pins 13 and 14).

Requires 862 bytes of program memory when compiled in Release mode.

## How to use

  - Create a new bootloader project, add `tiny_bootloader.h` and `main.c`
  - Configure as appropriate (clock speed, UART interface, baud rate etc.)
  - Set the `BOOTRST` fuse and configure `BOOTSZ` appropriately based on the reported Program Memory Usage when compiling the project
  - Configure the linker to locate both `.text` and `.bootloader` sections at the correct bootloader offset (as determined in the previous step) (`Project > XXX_Properties... > Toolchain > AVR/GNU Linker > Memory Settings`, then add entries like `.text=0x3C00` and `.bootloader=0x3C00`) 
  - Compile and program the board; the bootloader is entered by holding `SW0` during reset  
