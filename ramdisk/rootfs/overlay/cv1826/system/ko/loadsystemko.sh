#!/bin/sh
${CVI_SHOPTS}
#
# Start to insert kernel modules
#
insmod /mnt/system/ko/cv182x_base.ko
insmod /mnt/system/ko/cv182x_vip.ko
insmod /mnt/system/ko/cvi_mipi_rx.ko
insmod /mnt/system/ko/cv182x_sensor_i2c.ko
insmod /mnt/system/ko/cvi_mipi_tx.ko
#insmod /mnt/system/ko/cv182x_wdt.ko
insmod /mnt/system/ko/cv182x_clock_cooling.ko

insmod /mnt/system/ko/cv182x_tpu.ko
insmod /mnt/system/ko/cv182x_jpeg.ko
insmod /mnt/system/ko/cv182x_vcodec.ko
#insmod /mnt/system/ko/cv182x_rtc.ko

echo 3 > /proc/sys/vm/drop_caches
dmesg -n 4

#usb hub control
/etc/uhubon.sh host

exit $?
