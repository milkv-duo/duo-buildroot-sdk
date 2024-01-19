#!/bin/sh

usb_en=453
usb_select=510

function set_gpio()
{
	local gpio_num=$1
	local gpio_val=$2
	local gpio_path="/sys/class/gpio/gpio${gpio_num}"

	if test -d ${gpio_path}; then
		echo "GPIO ${gpio_num} already exported" >> /tmp/rndis.log 2>&1
	else
		echo ${gpio_num} > /sys/class/gpio/export
	fi

	echo out > ${gpio_path}/direction
	sleep 0.1
	echo ${gpio_val} > ${gpio_path}/value
}

set_gpio ${usb_en} 0
sleep 0.5
set_gpio ${usb_select} 0
sleep 0.5

/etc/uhubon.sh device >> /tmp/rndis.log 2>&1
/etc/run_usb.sh probe rndis >> /tmp/rndis.log 2>&1
/etc/run_usb.sh start rndis >> /tmp/rndis.log 2>&1

sleep 0.5
ifconfig usb0 192.168.42.1

count=`ps | grep dnsmasq | grep -v grep | wc -l`
if [ ${count} -lt 1 ] ;then
  echo "/etc/init.d/S80dnsmasq start" >> /tmp/rndis.log 2>&1
  /etc/init.d/S80dnsmasq start >> /tmp/rndis.log 2>&1
fi
