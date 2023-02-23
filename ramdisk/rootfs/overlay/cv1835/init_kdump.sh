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
/bin/busybox mount -t proc proc /proc
/bin/busybox mount -t sysfs sysfs /sys
echo "init.sh"

#Create all the symlinks to /bin/busybox
/bin/busybox --install -s
chown root:root /bin/busybox
chown root:root /bin/su
chmod 4755 /bin/su

#Create things under /dev
mdev -s
devmem 0x50010008 32 0x0
#Save vmcore to data partition
i=0
part=""

rand()
{
    min=$1
    max=$(($2 - min + 1))
    num=$(head -n 10 /dev/urandom | cksum | awk -F ' ' '{print $1}')
    echo $((num%max + min))
}

while [ true ]; do
	if [ -e "/dev/mmcblk0p7" ]; then
		part="/dev/mmcblk0p7"
		break
	else
		i=$(($i+1))
		busybox mdev -s
		echo "wait for mmcblk0p7 created!"
	fi
	if [ $i -ge 10 ];then
		echo "WARN: mmcblk0p7 not created!"
		break
	fi
done
if [ "$part" = "" ]; then
	i=0
	while [ true ]; do
		if [ -e "/dev/mmcblk0p6" ]; then
			part="/dev/mmcblk0p6"
			break
		else
			i=$(($i+1))
			busybox mdev -s
			echo "wait for mmcblk0p6 created!"
		fi
		if [ $i -ge 10 ];then
			echo "WARN: mmcblk0p6 not created!"
			break
		fi
	done
fi
if [ "$part" != "" ]; then
	#Mount data partition
	mount -t ext4 -o rw $part /mnt

	#Choose a unique file name
	sleep $(rand 0 19)
	filename=$(date +"%Y%m%d_%H%M%S")
	echo "copy vmcore_$filename to $part..."
	cp /proc/vmcore /mnt/vmcore_$filename
	sync
	umount /mnt

	#Reboot
	echo "reboot..."
	reboot -f
fi

#Redirect output
exec 0</dev/console
exec 1>/dev/console
exec 2>/dev/console

#Go!
exec /sbin/init $*
