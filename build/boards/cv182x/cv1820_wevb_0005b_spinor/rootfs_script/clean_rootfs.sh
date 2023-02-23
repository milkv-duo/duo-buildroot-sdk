#!/bin/bash

ROOTFS_DIR=$1
rm -rf $ROOTFS_DIR/mnt/system/usr
rm -rf $ROOTFS_DIR/mnt/system/lib

du -sh $ROOTFS_DIR/* |sort -rh
du -sh $ROOTFS_DIR/mnt/system/lib/* |sort -rh
