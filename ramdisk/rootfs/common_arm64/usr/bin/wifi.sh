#!/bin/sh
#
# Start the network....
#

# Debian ifupdown needs the /run/network lock directory

if [ -e "/sys/class/net/wlan0" ]; then
  mkdir -p /run/network

  case "$1" in
    start)
        printf "Starting network:"
        /sbin/ifconfig wlan0 up
        echo "ctrl_interface=/var/run/wpa_supplicant" > /etc/network/wpa_supplicant.conf
        ssid=$(cat /etc/network/ifcfg-wlan0|grep ssid=|sed 's/ssid=//')
        psk=$(cat /etc/network/ifcfg-wlan0|grep psk=|sed 's/psk=//')
        /usr/bin/wpa_passphrase $ssid "$psk" >> /etc/network/wpa_supplicant.conf
        /usr/bin/wpa_supplicant -iwlan0 -D wext -c/etc/network/wpa_supplicant.conf &
        bootproto_wifi=$(cat /etc/network/ifcfg-wlan0|grep BOOTPROTO=|sed 's/BOOTPROTO=//')
        if [ $bootproto_wifi = static ];then
          #echo "static"
          ip_wifi=$(cat /etc/network/ifcfg-wlan0|grep IPADDR=|sed 's/IPADDR=//')
          netmask_wifi=$(cat /etc/network/ifcfg-wlan0|grep NETMASK=|sed 's/NETMASK=//')
          gateway_wifi=$(cat /etc/network/ifcfg-wlan0|grep GATEWAY=|sed 's/GATEWAY=//')
          /sbin/ifconfig wlan0 $ip_wifi netmask $netmask_wifi up
          #echo "/sbin/ifconfig wlan0 $ip_wifi netmask $netmask_wifi up"
          /sbin/route add default gw $gateway_wifi
          echo "nameserver 8.8.8.8" > /etc/resolv.conf
        else
          #echo "dhcp"
          /sbin/udhcpc -b -i wlan0 -R &
        fi
        [ $? = 0 ] && echo "OK" || echo "FAIL"
	  ;;
    stop)
        printf "Stopping network: "
        /sbin/ifconfig wlan0 down
        [ $? = 0 ] && echo "OK" || echo "FAIL"
	  ;;
    restart|reload)
        "$0" stop
        "$0" start
	  ;;
    *)
	  echo "Usage: $0 {start|stop|restart}"
	  exit 1
esac
fi
exit $?

