#! /bin/sh

VERSION="0.1"

execute ()
{
    $* >/dev/null
    if [ $? -ne 0 ]; then
        echo
        echo "ERROR: executing $*"
        echo
        exit 1
    fi
}

version ()
{
  echo
  echo "`basename $1` version $VERSION"
  echo "Script to create bootable SD card for Linux"
  echo

  exit 0
}

usage ()
{
  echo "
Usage: `basename $1` <options> [ files for install partition ]

Mandatory options:
  --device              SD block device node (e.g /dev/sdd)

Optional options:
  --label               sd volume label
  --version             Print version.
  --help                Print this help message.
"
  exit 1
}

check_if_main_drive ()
{
  mount | grep " on / type " > /dev/null
  if [ "$?" != "0" ]
  then
    echo "-- WARNING: not able to determine current filesystem device"
  else
    main_dev=`mount | grep " on / type " | awk '{print $1}'`
    echo "-- Main device is: $main_dev"
    echo $main_dev | grep "$device" > /dev/null
    [ "$?" = "0" ] && echo "++ ERROR: $device seems to be current main drive ++" && exit 1
  fi

}


unmount_all_partitions ()
{
  for i in `ls -1 $device*`; do
    echo "unmounting device '$i'"
    umount $i 2>/dev/null
  done
  mount | grep $device
}

# Check if the script was started as root or with sudo
user=`id -u`
[ "$user" != "0" ] && echo "++ Must be root/sudo ++" && exit
label="SD"
# Process command line...
while [ $# -gt 0 ]; do
  case $1 in
    --help | -h)
      usage $0
      ;;
    --device) shift; device=$1; shift; ;;
    --label) shift; label=$1; shift; ;;
    --version) version $0;;
    *) copy="$copy $1"; shift; ;;
  esac
done

test -z $device && usage $0

if [ ! -z $sdkdir ] && [ ! -d $sdkdir ]; then
   echo "ERROR: $sdkdir does not exist"
   exit 1;
fi

if [ ! -b $device ]; then
   echo "ERROR: $device is not a block device file"
   exit 1;
fi

check_if_main_drive

#check_if_big_size

#echo "************************************************************"
#echo "*         THIS WILL DELETE ALL THE DATA ON $device         *"
#echo "*                                                          *"
#echo "*         WARNING! Make sure your computer does not go     *"
#echo "*                  in to idle mode while this script is    *"
#echo "*                  running. The script will complete,      *"
#echo "*                  but your SD card may be corrupted.      *"
#echo "*                                                          *"
#echo "*         Press <ENTER> to confirm....                     *"
#echo "************************************************************"
#read junk

#udevadm control -s
unmount_all_partitions

execute "dd if=/dev/zero of=$device bs=1024 count=1024"

sync

cat << END | fdisk $device
n
p
1



t
b
w
END

sleep 1

# handle various device names.
PARTITION1=${device}1
if [ ! -b ${PARTITION1} ]; then
        PARTITION1=${device}p1
fi

# make partitions.
echo "Formatting ${device} ..."
if [ -b ${PARTITION1} ]; then
	mkfs.fat -F 32 -s 128 -n ${label} ${PARTITION1}
else
	echo "Cant find boot partition in /dev"
  exit 1
fi

echo "completed!"
exit 0
