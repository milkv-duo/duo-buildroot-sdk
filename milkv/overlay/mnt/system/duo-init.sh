#!/bin/sh

# set PWR_GPIO[4] to gpio
devmem 0x0502707c 32 0x111
devmem 0x03001068 32 0x3

