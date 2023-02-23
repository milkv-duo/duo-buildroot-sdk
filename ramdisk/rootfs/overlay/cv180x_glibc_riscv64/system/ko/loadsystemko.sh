#!/bin/sh
${CVI_SHOPTS}
#
# Start to insert kernel modules
#
insmod /mnt/system/ko/cv180x_sys.ko
insmod /mnt/system/ko/cv180x_base.ko
insmod /mnt/system/ko/cv180x_rtos_cmdqu.ko
insmod /mnt/system/ko/cv180x_fast_image.ko
insmod /mnt/system/ko/cvi_mipi_rx.ko
insmod /mnt/system/ko/snsr_i2c.ko
insmod /mnt/system/ko/cv180x_vi.ko
insmod /mnt/system/ko/cv180x_vpss.ko
insmod /mnt/system/ko/cv180x_dwa.ko
#insmod /mnt/system/ko/cv180x_vo.ko
#insmod /mnt/system/ko/cv180x_mipi_tx.ko
insmod /mnt/system/ko/cv180x_rgn.ko

#insmod /mnt/system/ko/cv180x_wdt.ko
insmod /mnt/system/ko/cv180x_clock_cooling.ko

insmod /mnt/system/ko/cv180x_tpu.ko
insmod /mnt/system/ko/cv180x_vcodec.ko
insmod /mnt/system/ko/cv180x_jpeg.ko
insmod /mnt/system/ko/cvi_vc_driver.ko MaxVencChnNum=9 MaxVdecChnNum=9
#insmod /mnt/system/ko/cv180x_rtc.ko

echo 3 > /proc/sys/vm/drop_caches
dmesg -n 4

#usb hub control
#/etc/uhubon.sh host

exit $?
