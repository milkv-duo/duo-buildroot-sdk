GPIO_HUBPORT_EN=449
GPIO_ROLESEL=450
GPIO_HUBRST=451
SYS_GPIO=/sys/class/gpio

hub_on() {
  echo "turn on usb hub"
  if [ ! -d $SYS_GPIO/gpio$GPIO_HUBPORT_EN ]; then
      echo $GPIO_HUBPORT_EN >/sys/class/gpio/export
  fi

  if [ ! -d $SYS_GPIO/gpio$GPIO_ROLESEL ]; then
      echo $GPIO_ROLESEL >/sys/class/gpio/export
  fi

  if [ ! -d $SYS_GPIO/gpio$GPIO_HUBRST ]; then
      echo $GPIO_HUBRST >/sys/class/gpio/export
  fi

  echo "out" >/sys/class/gpio/gpio$GPIO_HUBPORT_EN/direction
  echo "out" >/sys/class/gpio/gpio$GPIO_ROLESEL/direction
  echo "out" >/sys/class/gpio/gpio$GPIO_HUBRST/direction

  echo 1 >/sys/class/gpio/gpio$GPIO_HUBPORT_EN/value
  echo 0 >/sys/class/gpio/gpio$GPIO_ROLESEL/value
  echo 0 >/sys/class/gpio/gpio$GPIO_HUBRST/value
}

hub_off() {
  echo "turn off usb hub"
  if [ ! -d $SYS_GPIO/gpio$GPIO_HUBPORT_EN ]; then
      echo $GPIO_HUBPORT_EN >/sys/class/gpio/export
  fi

  if [ ! -d $SYS_GPIO/gpio$GPIO_ROLESEL ]; then
      echo $GPIO_ROLESEL >/sys/class/gpio/export
  fi

  if [ ! -d $SYS_GPIO/gpio$GPIO_HUBRST ]; then
      echo $GPIO_HUBRST >/sys/class/gpio/export
  fi

  echo "out" >/sys/class/gpio/gpio$GPIO_HUBPORT_EN/direction
  echo "out" >/sys/class/gpio/gpio$GPIO_ROLESEL/direction
  echo "out" >/sys/class/gpio/gpio$GPIO_HUBRST/direction

  echo 0 >/sys/class/gpio/gpio$GPIO_HUBPORT_EN/value
  echo 1 >/sys/class/gpio/gpio$GPIO_ROLESEL/value
  echo 1 >/sys/class/gpio/gpio$GPIO_HUBRST/value
}

inst_mod() {
  insmod /mnt/system/ko/configfs.ko
  insmod /mnt/system/ko/libcomposite.ko
  insmod /mnt/system/ko/u_serial.ko
  insmod /mnt/system/ko/usb_f_acm.ko
  insmod /mnt/system/ko/cvi_usb_f_cvg.ko
  insmod /mnt/system/ko/usb_f_uvc.ko
  insmod /mnt/system/ko/usb_f_fs.ko
  insmod /mnt/system/ko/u_audio.ko
  insmod /mnt/system/ko/usb_f_uac1.ko
  insmod /mnt/system/ko/usb_f_serial.ko
  insmod /mnt/system/ko/usb_f_mass_storage.ko
  insmod /mnt/system/ko/u_ether.ko
  insmod /mnt/system/ko/usb_f_ecm.ko
  insmod /mnt/system/ko/usb_f_eem.ko
  insmod /mnt/system/ko/usb_f_rndis.ko
}

case "$1" in
  host)
	insmod /mnt/system/ko/dwc2.ko
  echo host > /proc/cviusb/otg_role
	;;
  device)
	echo device > /proc/cviusb/otg_role
	;;
  *)
	echo "Usage: $0 host"
	echo "Usage: $0 device"
	exit 1
esac
exit $?
