#!/bin/sh

if [ "$1" = "online" ] ; then
  echo "online mode capture."
  insmod /mnt/system/ko/gator.ko
  /usr/bin/gatord_avg &
  exit 1;
fi

duration=$2
if [ -n "$duration" ]; then
  echo "offline mode capture for $duration seconds."
else
  echo "offline mode capture."
  echo "Need to kill gator_main and gator_child process after capture."
	duration=0;
fi

#offline mode for local apc folder
if [ "$1" = "offline" ] ; then
  insmod /mnt/system/ko/gator.ko
  cp /usr/bin/gatord_avg /mnt/data
  /mnt/data/gatord_avg -t $duration -o `date +%Y%m%d_%H%M%S` &
  exit 1;
fi

echo "This scripts insmod gator.ko and run gatord."
echo "online/offline setup gatord mode:"
echo "	online mode is connected by ethernet."
echo "	offline mode gernerate local apc folder."
echo ""
echo "avg setup gatord version:"
echo "	gatord is original version"
echo "	gatord_avg is for the average counters."
echo ""
echo "Usage ./gator.sh [online/offline] [offline_duaration]"
echo "Example: "
echo "online capture by ethernet"
echo "1. /usr/bin/gator.sh online"
echo ""
echo "offline capture for 10 seconds"
echo "2. /usr/bin/gator.sh offline 10"
