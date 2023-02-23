#!/bin/bash

ROOTFS_DIR=$1
rm -rf $ROOTFS_DIR/mnt/system/usr

rm -rf $ROOTFS_DIR/mnt/system/lib/libav*
rm -rf $ROOTFS_DIR/mnt/system/lib/libboost_system*
rm -rf $ROOTFS_DIR/mnt/system/lib/libcli*
rm -rf $ROOTFS_DIR/mnt/system/lib/libcnpy*
rm -rf $ROOTFS_DIR/mnt/system/lib/libcrypto*
rm -rf $ROOTFS_DIR/mnt/system/lib/libjson-c*
rm -rf $ROOTFS_DIR/mnt/system/lib/libnanomsg*
rm -rf $ROOTFS_DIR/mnt/system/lib/libsqlite3*
rm -rf $ROOTFS_DIR/mnt/system/lib/libthttpd*
rm -rf $ROOTFS_DIR/mnt/system/lib/libwebsockets*
rm -rf $ROOTFS_DIR/mnt/system/lib/libopencv_*

rm -rf $ROOTFS_DIR//usr/bin/gator*

du -sh $ROOTFS_DIR/* |sort -rh
du -sh $ROOTFS_DIR/mnt/system/lib/* |sort -rh