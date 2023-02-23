#!/bin/sh
${CVI_SHOPTS}
#
# Start to insert kernel modules
#
insmod /mnt/system/ko/cv183x_base.ko
insmod /mnt/system/ko/cv183x_vip.ko
insmod /mnt/system/ko/cvi_mipi_rx.ko
insmod /mnt/system/ko/cv183x_sensor_i2c.ko
insmod /mnt/system/ko/cvi_mipi_tx.ko
insmod /mnt/system/ko/cv183x_wdt.ko
insmod /mnt/system/ko/cv183x_clock_cooling.ko

insmod /mnt/system/ko/cv183x_tpu.ko
insmod /mnt/system/ko/cv183x_jpeg.ko
insmod /mnt/system/ko/cv183x_vcodec.ko
insmod /mnt/system/ko/cv183x_rtc.ko

#usb hub control
/etc/uhubon.sh host

GP_REG3=0x0300008C
CHIP_ID=`devmem $GP_REG3`

if [ $CHIP_ID = "0x00001838" ]; then
	# set VDDC_TPU 1V
	devmem 0x030001E4 32 0x00000003
	devmem 0x030001E4 32 0x00000001
	i2cset -f -y 0 0x68 0x0 0x28
	i2cset -f -y 0 0x68 0x1 0xe6
	sleep 0.1 # delay 100ms
	# workaround for PMIC (development board)
	i2cset -f -y 0 0x68 0x1 0xA6
elif [ $CHIP_ID = "0x00001835" ]; then
	# workaround for PMIC (development board)
	i2cset -f -y 0 0x68 0x1 0xA6
fi
exit $?
