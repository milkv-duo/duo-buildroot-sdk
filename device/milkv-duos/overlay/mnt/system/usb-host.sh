#!/bin/sh

usb_en=453
usb_select=510

function set_gpio()
{
	local gpio_num=$1
	local gpio_val=$2
	local gpio_path="/sys/class/gpio/gpio${gpio_num}"

	if test -d ${gpio_path}; then
		echo "GPIO ${gpio_num} already exported" >> /tmp/usb.log 2>&1
	else
		echo ${gpio_num} > /sys/class/gpio/export
	fi

	echo out > ${gpio_path}/direction
	sleep 0.1
	echo ${gpio_val} > ${gpio_path}/value
}

set_gpio ${usb_select} 1
sleep 0.5
set_gpio ${usb_en} 1
sleep 0.5

echo host > /proc/cviusb/otg_role >> /tmp/usb.log 2>&1

