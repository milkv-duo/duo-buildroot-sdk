#!/bin/sh

if [ "$1" = "online" ] && [ "$2" = "acc" ]; then
  echo "online mode capture."
  insmod /mnt/system/ko/gator.ko
  /usr/bin/gatord_acc &
  exit 1;
elif [ "$1" = "online" ] && [ "$2" = "avg" ]; then
  echo "online mode capture."
  insmod /mnt/system/ko/gator.ko
  /usr/bin/gatord_avg &
  exit 1;
fi

duration=$3
if [ -n "$duration" ]; then
  echo "offline mode capture for $duration seconds."
else
  echo "offline mode capture."
  echo "Need to kill gator_main and gator_child process after capture."
	duration=0;
fi

#offline mode for local apc folder
if [ "$1" = "offline" ] && [ "$2" = "acc" ]; then
  insmod /mnt/system/ko/gator.ko
  cp /usr/bin/gatord_acc /mnt/data
  /mnt/data/gatord_acc -t $duration -o `date +%Y%m%d_%H%M%S` &
  exit 1;
elif [ "$1" = "offline" ] && [ "$2" = "avg" ]; then
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
echo "avg/acc setup gatord version:"
echo "	gatord is original version"
echo "	gatord_avg is for the average counters."
echo "	gatord_acc is for the accumulate counters."
echo ""
echo "Usage ./gator.sh [online/offline] [avg/acc] [offline_duaration]"
echo "Example: "
echo "online capture by ethernet"
echo "1. /usr/bin/gator.sh online avg"
echo "2. /usr/bin/gator.sh online acc"
echo ""
echo "offline capture for 10 seconds"
echo "3. /usr/bin/gator.sh offline avg 10"
echo "4. /usr/bin/gator.sh offline acc 10"
