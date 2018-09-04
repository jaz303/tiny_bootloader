# Tiny Bootloader

The goal: a simple C bootloader for 8- and 32-bit microcontrollers that will fit in &le; 2KB ROM.

Bring your own IO driver (e.g. UART), flash read/write, entry point.

Project is roughly working (see ATmega324PB example linked below) but there's still a bit of work to do.

Initial target devices:

  - [ATmega324PB (&le;1KB)](https://github.com/jaz303/tiny_bootloader/tree/master/examples/ATmega324PB)
  - ATmega328PB
  - ATSAMD21 series

## Overview

Tiny Bootloader models the target microcontroller as a series of random-access, contiguous pages that can be read, written or verified; page size and count are implementation-defined. Ideally, hosts communicate with Tiny Bootloader via a stream-friendly protocol (e.g. UART or SPI), although other implementations (such as I2C) are possible with a bit of work. Framing is handled internally via byte-stuffing.

## Project Goals

Tiny Bootloader's first goal is portability; all architecture-specific code lives inside the host program. If you absolutely must have the smallest possible bootloader, an optimised, architecture-specific solution (such as [Optiboot](https://github.com/Optiboot/optiboot) for the Atmel AVR platform) will beat Tiny Bootloader.

The second goal is to have the smallest possible size without comprising portability. As such, the feature set is small, and should any exotic (read: large) features (such as CRC32) be added in the future, they will be opt-in and not part of the core feature set. As a rule of thumb, bootloaders using Tiny Bootloader should fit in &lt; 2KiB of ROM

The final goal is to develop robust client applications for multiple languages and platforms. The initial targets are cross-platform JavaScript and Go, with POSIX C a close third. It should be easy to flash any Tiny Bootloader-enabled device with the same set of tools, providing that easy access is provided to the bootloader itself at a hardware level (e.g. button).

## Notes

Despite its name, Tiny Bootloader is not limited to loading application code. It can also be used, for example, to load VM bytecode, configuration data, or anything else - all you need to do to inject code into a smaller area is to reduce the page count and start at a different base address. In the future, Tiny Bootloader will be extended to support an optional memory map so that clients can interrogate available sections.

## Implementation

To implement Tiny Bootloader, all you need to do is implement a few macros:

  - `TINY_BOOTLOADER_PAGE_SIZE`: page size, in bytes
  - `TINY_BOOTLOADER_PAGE_COUNT`: number of pages
  - `TINY_BOOTLOADER_READ_BYTE()`: blocks until a byte has been read from the comms channel and returns it as a `char`
  - `TINY_BOOTLOADER_WRITE_BYTE(ch)`: blocks until a `char` has been written to the comms channel
  - `TINY_BOOTLOADER_WRITE_PAGE(page, data)`: writes data (`char*`) to the specified page number; returns `0` on success, `-1` on error
  - `TINY_BOOTLOADER_COMPARE_PAGE(page, data)`: compares data (`char*`) to that currently stored in the specified page number; returns `0` on match and `-1` when a difference is found
  - `TINY_BOOTLOADER_REBOOT()`: reboots the chip after a successful update; whether this jumps straight into the application or re-enters the bootloader is up to you
  
Aside from the macro definitions a Tiny Bootloader skeleton program looks like this:

```c
// MACRO DEFINITIONS HERE

char bootloader_requested() {
  // check if bootloader has been requested (e.g. button press, GPIO low, etc)
  return 0;
}

void bootloader_init() {
  // get ready to enter bootloader; enable comms channel etc.
}

#include "tiny_bootloader.h"
#define TINY_BOOTLOADER_IMPL
#include "tiny_bootloader.h"

int main() {
  if (bootloader_requested()) {
    bootloader_init();
    bootloader_run(); // defined in tiny_bootloader.h
  } else {
    asm("JMP 0"); // jump to main program
  }
  while (1);
}
```

## TODO

  - Document protocol
  - Support protocol extensions by routing all commands with `0x80` bit set to user-defined handler
  - Experiment with consistent-overhead byte stuffing
  - Add `CMD_READ_PAGE`
  - Javascript client
  - Golang client
  - Optional CRC32 for verification
  - Optional memory map
