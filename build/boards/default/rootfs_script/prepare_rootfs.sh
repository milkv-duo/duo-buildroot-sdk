#!/bin/bash
function print_usage(){
  echo "usage: $0 path"
  exit 1
}

if [[ -z $1 ]];then
  print_usage
fi

pushd $1

sed -i 's/\/etc\/resolv.conf/\/run\/resolv.conf/g' usr/default.script
sed -i 's/\/etc\/resolv.conf/\/run\/resolv.conf/g' usr/share/udhcpc/default.script

# modify rootfs/init to remove busybox install
cp init_ramboot.sh.sqsh init
rm init.*sh

mkdir -p mnt/tmp
mkdir -p mnt/system
mkdir -p mnt/data
mkdir -p mnt/usb
mkdir -p mnt/sd
mkdir -p mnt/nfs
mkdir -p mnt/cfg
mkdir -p dev
mkdir -p sys/dev
mkdir -p proc
mkdir -p tmp
mkdir -p var/empty
mkdir -p var/lib
mkdir -p var/lock
mkdir -p var/log
mkdir -p var/run
mkdir -p var/spool
mkdir -p root
mkdir -p run
mkdir -p overlay

chmod 0600 etc/ssh/*_key
chmod 0600 etc/ssh/*.pub
ln -sf /proc/mounts etc/mtab

popd
