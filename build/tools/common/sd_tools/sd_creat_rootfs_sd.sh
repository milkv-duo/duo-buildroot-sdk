if [ "$#" -ne "1" ]
then
	echo "usage: sudo ./sd_creat_rootfs_sd.sh /dev/sdX"
	echo ""
	echo "       The script is used to create two sd boot partitions, "
	echo "       one is fat32 with 128M, the other is ext4 with 3072M."
	echo "       You can modify the capacities in this script as you wish!"
	echo ""
	echo "Note:  Please backup you sdcard files before using this script!"

	exit
fi

LANGUAGE=en_US
LANG=en_US.UTF-8

basepath=$(dirname $0)
device=$1
vfat_part=${device}1
vfat_cap="128M"
vfat_label="IMAGES"

ext4_part=${device}2
ext4_cap="3072M"
ext4_label="rootfs"

echo "umount ${device}*"
umount ${device}*
$basepath/expect_fdisk.exp $device $vfat_cap $ext4_cap

echo "mkfs.vfat $vfat_part"
mkfs.vfat $vfat_part
echo "mlabel -i $vfat_part ::$vfat_label"
mlabel -i $vfat_part ::$vfat_label
echo "mkfs.ext4 $ext4_part -F"
mkfs.ext4 $ext4_part -F
echo "e2label $ext4_part $ext4_label"
e2label $ext4_part $ext4_label


sync

exit
