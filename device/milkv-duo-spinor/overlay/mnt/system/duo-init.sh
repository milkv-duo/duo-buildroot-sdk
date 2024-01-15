#!/bin/sh

# Set Pin-29(GP22) to GPIO
devmem 0x0502707c 32 0x111
devmem 0x03001068 32 0x3

# Set Pin-19(GP14) to GPIO
duo-pinmux -w GP14/GP14 > /dev/null

# insmod pwm module
insmod /mnt/system/ko/cv180x_pwm.ko
