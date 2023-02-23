#!/bin/sh
SCRIPT_PATH=`dirname $0`
cd $SCRIPT_PATH
insmod cv183x_base.ko
insmod cv183x_vip.ko cif_auto=0 smooth=0
insmod cvi_mipi_tx.ko smooth=0

mdev -s
chmod +x ./progress
./progress ./upgrading.jpg &
sh ./update-script.sh_

