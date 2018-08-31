# tiny_bootloader

The goal: a simple C bootloader for 8- and 32-bit microcontrollers that will fit in &le; 2KB ROM.

Bring your own IO driver (e.g. UART), flash read/write, entry point.

Work in progress right now.

Initial target devices:

  - [ATmega324PB](https://github.com/jaz303/tiny_bootloader/tree/master/examples/ATmega324PB)
  - ATmega328PB
  - ATSAMD21 series
