#include "bootloader.h"

#include <stdio.h>

#define TINY_BOOTLOADER_PAGE_SIZE           64
#define TINY_BOOTLOADER_PAGE_COUNT          16
#define TINY_BOOTLOADER_REBOOT()        
#define TINY_BOOTLOADER_READ_BYTE()         ((char)getchar())
#define TINY_BOOTLOADER_WRITE_BYTE(b)       (putchar(b))
#define TINY_BOOTLOADER_WRITE_PAGE(p, d)    (0)
#define TINY_BOOTLOADER_COMPARE_PAGE(p, d)  (0)

#define TINY_BOOTLOADER_IMPL
#include "bootloader.h"
#undef TINY_BOOTLOADER_IMPL

int main(int argc, char *argv[]) {
    bootloader_run();
}