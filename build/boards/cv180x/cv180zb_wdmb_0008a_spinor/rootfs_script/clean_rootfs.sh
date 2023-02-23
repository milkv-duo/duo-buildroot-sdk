#!/bin/bash

SYSTEM_DIR=$1
rm -rf $SYSTEM_DIR/mnt/system/usr
rm -rf $SYSTEM_DIR/mnt/system/lib

rm -rf $SYSTEM_DIR/etc/init.d/S01syslogd
rm -rf $SYSTEM_DIR/etc/init.d/S02klogd
rm -rf $SYSTEM_DIR/etc/init.d/S02sysctl
rm -rf $SYSTEM_DIR/etc/init.d/S20urandom
rm -rf $SYSTEM_DIR/etc/init.d/S40network
rm -rf $SYSTEM_DIR/etc/init.d/S23ntp

rm -rf $SYSTEM_DIR/bin/ntpd
rm -rf $SYSTEM_DIR/mnt/cfg/secure.img

#del cv181x_mipi_tx.ko
rm -rf $SYSTEM_DIR/mnt/system/ko/cv181x_mipi_tx.ko
sed -i "/cv181x_mipi_tx.ko/d" $SYSTEM_DIR/mnt/system/ko/loadsystemko.sh

if [ $BUILD_FOR_DEBUG != "y" ]
then
#del dmesg cmd if CONFIG_PRINTK=n
#sed -i "/dmesg/d" $SYSTEM_DIR/mnt/system/ko/loadsystemko.sh

#del debugfs node
sed -i "/debugfs/d" $SYSTEM_DIR/etc/fstab
fi

du -sh $SYSTEM_DIR/* |sort -rh
