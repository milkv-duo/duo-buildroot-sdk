#!/bin/bash

SYSTEM_DIR=$1
rm -rf $SYSTEM_DIR/mnt/system/usr
rm -rf $SYSTEM_DIR/mnt/system/lib
rm -rf $SYSTEM_DIR/mnt/system/lib/libsns_gc*
rm -rf $SYSTEM_DIR/mnt/system/lib/libsns_imx*
rm -rf $SYSTEM_DIR/mnt/system/lib/libsns_sc*
rm -rf $SYSTEM_DIR/mnt/system/lib/libcipher.so

rm -rf $SYSTEM_DIR/mnt/system/m2m-deinterlace.ko
rm -rf $SYSTEM_DIR/mnt/system/efivarfs.ko

rm -rf $SYSTEM_DIR/etc/init.d/S23ntp
rm -rf $SYSTEM_DIR/bin/ntpd

du -sh $SYSTEM_DIR/* |sort -rh
