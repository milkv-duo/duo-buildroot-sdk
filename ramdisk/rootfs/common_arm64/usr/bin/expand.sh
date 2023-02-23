sgdisk -e /dev/mmcblk0
partprobe /dev/mmcblk0
outp=`sgdisk -p /dev/mmcblk0|tail -n 1`
outp=${outp:3:1}
parted /dev/mmcblk0 resizepart $outp 100%
partprobe /dev/mmcblk0
grep -v rootfs /proc/mounts > /etc/mtab
resize2fs "/dev/mmcblk0p${outp}"

echo "" > /dev/mmcblk0p5
sync
