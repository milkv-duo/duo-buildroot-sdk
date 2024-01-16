#!/bin/sh

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
