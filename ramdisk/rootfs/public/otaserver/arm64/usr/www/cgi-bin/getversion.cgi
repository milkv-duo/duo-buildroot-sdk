#!/bin/sh
echo "Content-type: text/plain"
echo ""
cat /proc/version
echo "<br/>"
sysver="`strings /mnt/system/usr/lib/libsys.so |grep '^cv[0-9][0-9][0-9][0-9x]_*'`"
if [ -f /etc/sdk-release ];then
eval `cat /etc/sdk-release`
elif [ -f /mnt/system/sdk-release ];then
eval `cat /mnt/system/sdk-release`
elif [ -f /mnt/system/etc/sdk-release ];then
eval `cat /mnt/system/etc/sdk-release`
fi
echo "MMF version $sysver-$SDK_VERSION"
echo ""
