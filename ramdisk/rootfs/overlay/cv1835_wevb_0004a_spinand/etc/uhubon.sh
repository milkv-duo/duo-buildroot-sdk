hub_on() {
  echo "run host mode"
}

hub_off() {
  echo "run device mode"
}

inst_mod() {
  insmod /mnt/system/ko/configfs.ko
  insmod /mnt/system/ko/libcomposite.ko
  insmod /mnt/system/ko/u_serial.ko
  insmod /mnt/system/ko/usb_f_acm.ko
  insmod /mnt/system/ko/cvi_usb_f_cvg.ko
  insmod /mnt/system/ko/usb_f_serial.ko
  insmod /mnt/system/ko/usb_f_mass_storage.ko
  insmod /mnt/system/ko/u_ether.ko
  insmod /mnt/system/ko/usb_f_ecm.ko
  insmod /mnt/system/ko/usb_f_eem.ko
  insmod /mnt/system/ko/usb_f_rndis.ko
  insmod /mnt/system/ko/cv183x_usb_gadget.ko
}

case "$1" in
  host)
	insmod /mnt/system/ko/cv183x_usb_drd.ko
	hub_on
	;;
  device)
	hub_off
	inst_mod
	;;
  *)
	echo "Usage: $0 host"
	echo "Usage: $0 device"
	exit 1
esac
exit $?
