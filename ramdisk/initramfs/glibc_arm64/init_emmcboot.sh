#!/bin/sh
${CVI_SHOPTS}

# Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# Neither the name of ARM nor the names of its contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

#Mount filesystems needed by mount and mdev
mount -t proc proc /proc
mount -t sysfs sysfs /sys

echo "This script just mounts and boots the rootfs!"

#Create things under /dev
busybox mdev -s

until [ -e "/dev/mmcblk0p3" ]; do
	busybox mdev -s
	echo "wait for mmcblk0p3 created!"
done

#Check and repair rootfs
busybox touch /etc/mtab
e2fsck -p /dev/mmcblk0p3
e2fsck -p /dev/mmcblk0p4
e2fsck -p /dev/mmcblk0p6
e2fsck -p /dev/mmcblk0p7

#mount the rootfs
mount -t ext4 -o rw /dev/mmcblk0p3 /mnt
if [ $? -eq 0 ]; then
	if [ ! -L "/mnt/init" -a ! -f "/mnt/init" ]; then
		umount /mnt
		mount -t ext4 -o rw /dev/mmcblk0p6 /mnt
	fi
else
	mount -t ext4 -o rw /dev/mmcblk0p6 /mnt
fi


#clean up
umount /proc
umount /sys

exec switch_root /mnt /init

#/bin/sh

