#!/bin/sh
print_log (){
  msg=$1
  opt=""
  if [ -n "$2" ];then
    opt="$1"
    msg=$2
  fi
  logger "$msg"
  echo $opt "$msg" | tee -a /var/log/OTA
}

print_exit() {
  logger "$1"
  echo "$1" | tee -a /var/log/OTA
  #rm $OTA_FILE
  #rm -rf /mnt/data/.ota;sync
  exit $2
}

check_storage_type() {
  [ -z "$1" ] && $1="Unknown"
  case "$1" in
  emmc)
  grep -q DATA /proc/mtd && print_exit "Device storage is not emmc type" 4
  grep -q mtdpart /proc/cmdline && print_exit "Device storage is not emmc type" 4
  return 0
  ;;
  spinand)
  ;;
  *)
  ;;
  esac
  print_exit "$1 is not support yet..." 5
}

print_log "RUN `basename $0`"
rm -rf /mnt/data/.ota
mkdir -p /mnt/data/.ota

if [ ! -f "$OTA_FILE" ];then
print_exit "OTA File not exist?" 1
fi

# Customer can do their own check criteria;below is a demonstration

print_log -n "Check board type : "
# Check board type
unzip -q -d /mnt/data/.ota $OTA_FILE META/misc_info.txt || print_exit "Missing misc_info.txt?" 6
eval $(cat /mnt/data/.ota/META/misc_info.txt)
check_storage_type "$STORAGE_TYPE"
print_log "Success"

print_log -n "Unzip packages : "
# Start unzip
unzip -o $OTA_FILE -d /mnt/data/.ota
#tar xf $OTA_FILE -C /mnt/data/.ota
if [ $? -eq 0 ];then
  print_log "Success"
  eval $(grep ^UPDATE_SCRIPT /init.sh.recoverysh)
  METADATA=/mnt/data/.ota/META/metadata.txt
  if [ ! -f "$UPDATE_SCRIPT" ] || [ ! -f "$METADATA" ];then
    print_exit "Wrong Upgrade.zip format? $UPDATE_SCRIPT or $METADATA miss" 2
  fi
  # Check image md5sum
  print_log -n "Check image integrity : "
  cd /mnt/data/.ota
  md5sum -c META/metadata.txt 2>>/var/log/OTA
  if [ "$?" -ne "0" ];then
    print_exit "Fail" 3
  fi
  # Make sure executable
  chmod +x /mnt/data/.ota/utils/cimg2raw
  print_log "Success"
  # Make sure ip don't change
  killall -9 udhcpc
  echo "OTA Upload Success" >>/var/log/OTA
  sync;sleep 1
  sh /bin/reboot_recovery
else
  print_exit "Unzip fail!?" 1
fi
rm $OTA_FILE
exit 0
