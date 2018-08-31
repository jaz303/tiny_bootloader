# tiny_bootloader

The goal: a simple C bootloader for 8- and 32-bit microcontrollers that will fit in &le; 2KB ROM.

Bring your own IO driver (e.g. UART), flash read/write, entry point.

Project is roughly working (see ATmega324PB example linked below) but there's still a bit of work to do.

Initial target devices:

  - [ATmega324PB](https://github.com/jaz303/tiny_bootloader/tree/master/examples/ATmega324PB)
  - ATmega328PB
  - ATSAMD21 series

## TODO

  - Document protocol
  - Support protocol extensions by routing all commands with `0x80` bit set to user-defined handler
  - Add `CMD_READ_PAGE`
  - Javascript client
  - Golang client
  - Optional CRC32 for verification
