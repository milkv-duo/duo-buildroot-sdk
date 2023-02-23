CLASS=acm
VID=0x30b1
PID=0x1003
MSC_PID=0x1008
RNDIS_PID=0x1009
MANUFACTURER="Cvitek"
PRODUCT="USB Com Port"
PRODUCT_RNDIS="RNDIS"
SERIAL="0123456789"
MSC_FILE=$3
CVI_DIR=/tmp/usb
CVI_GADGET=$CVI_DIR/usb_gadget/cvitek
CVI_FUNC=$CVI_GADGET/functions
FUNC_NUM=0
MAX_EP_NUM=4
TMP_NUM=0
EP_IN=0
EP_OUT=0

case "$2" in
  acm)
	CLASS=acm
	;;
  msc)
	CLASS=mass_storage
	PID=$MSC_PID
	;;
  cvg)
	CLASS=cvg
	;;
  rndis)
	CLASS=rndis
	PID=$RNDIS_PID
	PRODUCT=$PRODUCT_RNDIS
	;;
  *)
	if [ "$1" = "probe" ] ; then
	  echo "Usage: $0 probe {acm|msc|cvg|rndis}"
	  exit 1
	fi
esac

calc_func() {
  FUNC_NUM=$(ls $CVI_GADGET/functions -l | grep ^d | wc -l)
  echo "$FUNC_NUM file(s)"
}

res_check() {
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "acm*" | wc -l)
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  TMP_NUM=$(($TMP_NUM * 2))
  EP_IN=$(($EP_IN+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "mass_storage*" | wc -l)
  EP_IN=$(($EP_IN+$TMP_NUM))
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "cvg*" | wc -l)
  EP_IN=$(($EP_IN+$TMP_NUM))
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  TMP_NUM=$(find $CVI_GADGET/functions/ -name "rndis*" | wc -l)
  EP_OUT=$(($EP_OUT+$TMP_NUM))
  TMP_NUM=$(($TMP_NUM * 2))
  EP_IN=$(($EP_IN+$TMP_NUM))
  if [ "$CLASS" = "acm" ] ; then
    EP_IN=$(($EP_IN+2))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "mass_storage" ] ; then
    EP_IN=$(($EP_IN+1))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "cvg" ] ; then
    EP_IN=$(($EP_IN+1))
    EP_OUT=$(($EP_OUT+1))
  fi
  if [ "$CLASS" = "rndis" ] ; then
    EP_IN=$(($EP_IN+2))
    EP_OUT=$(($EP_OUT+1))
  fi
  echo "$EP_IN in ep"
  echo "$EP_OUT out ep"
  if [ $EP_IN -gt $MAX_EP_NUM ]; then
    echo "reach maximum resource"
    exit 1
  fi
  if [ $EP_OUT -gt $MAX_EP_NUM ]; then
    echo "reach maximum resource"
    exit 1
  fi
}

probe() {
  if [ ! -d $CVI_DIR ]; then
    mkdir $CVI_DIR
  fi
  if [ ! -d $CVI_DIR/usb_gadget ]; then
    # Enale USB ConfigFS
    mount none $CVI_DIR -t configfs
    # Create gadget dev
    mkdir $CVI_GADGET
    # Set the VID and PID
    echo $VID >$CVI_GADGET/idVendor
    echo $PID >$CVI_GADGET/idProduct
    # Set the product information string
    mkdir $CVI_GADGET/strings/0x409
    echo $MANUFACTURER>$CVI_GADGET/strings/0x409/manufacturer
    echo $PRODUCT>$CVI_GADGET/strings/0x409/product
    echo $SERIAL>$CVI_GADGET/strings/0x409/serialnumber
    # Set the USB configuration
    mkdir $CVI_GADGET/configs/c.1
    mkdir $CVI_GADGET/configs/c.1/strings/0x409
    echo "config1">$CVI_GADGET/configs/c.1/strings/0x409/configuration
    # Set the MaxPower of USB descriptor
    echo 120 >$CVI_GADGET/configs/c.1/MaxPower
  fi
  # get current functions number
  calc_func
  # assign the class code for composite device
  if [ ! $FUNC_NUM -eq 0 ]; then
    echo 0xEF >$CVI_GADGET/bDeviceClass
    echo 0x02 >$CVI_GADGET/bDeviceSubClass
    echo 0x01 >$CVI_GADGET/bDeviceProtocol
  fi
  # resource check
  res_check
  # create the desired function
  mkdir $CVI_GADGET/functions/$CLASS.usb$FUNC_NUM
  if [ "$CLASS" = "mass_storage" ] ; then
    echo $MSC_FILE >$CVI_GADGET/functions/$CLASS.usb$FUNC_NUM/lun.0/file
  fi
  if [ "$CLASS" = "rndis" ] ; then
    #OS STRING
    echo 1 >$CVI_GADGET/os_desc/use
    echo 0xcd >$CVI_GADGET/os_desc/b_vendor_code
    echo MSFT100 >$CVI_GADGET/os_desc/qw_sign
    #COMPATIBLE ID
    echo RNDIS >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/compatible_id
    #MAKE c.1 THE ONE ASSOCIATED WITH OS DESCRIPTORS
    ln -s $CVI_GADGET/configs/c.1 $CVI_GADGET/os_desc
    #MAKE "Icons" EXTENDED PROPERTY
    mkdir $CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Icons
    echo 2 >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Icons/type
    echo "%SystemRoot%\\system32\\shell32.dll,-233" >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Icons/data
    #MAKE "Label" EXTENDED PROPERTY
    mkdir $CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Label
    echo 1 >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Label/type
    echo "XYZ Device" >$CVI_FUNC/rndis.usb$FUNC_NUM/os_desc/interface.rndis/Label/data
  fi

}

start() {
  # link this function to the configuration
  calc_func
  if [ $FUNC_NUM -eq 0 ]; then
    echo "Functions Empty!"
    exit 1
  fi
  for i in `seq 0 $(($FUNC_NUM-1))`;
  do
    find $CVI_GADGET/functions/ -name "*.usb$i" | xargs -I % ln -s % $CVI_GADGET/configs/c.1
  done
  # Start the gadget driver
  echo 40e0000.cvi-usb-dev >$CVI_GADGET/UDC
}

stop() {
  echo "" >$CVI_GADGET/UDC
  rm $CVI_GADGET/configs/c.*/*.usb*
  rmdir $CVI_GADGET/configs/c.*/strings/0x409/
  tmp_dirs=$(find $CVI_GADGET/os_desc/c.* -type d)
  if [ -n tmp_dirs ]; then
    echo "remove os_desc!"
    rm -rf $CVI_GADGET/os_desc/c.*/
    find $CVI_GADGET/functions/ -name Icons | xargs rmdir
    find $CVI_GADGET/functions/ -name Label | xargs rmdir
  fi
  rmdir $CVI_GADGET/configs/c.*/
  rmdir $CVI_GADGET/functions/*
  rmdir $CVI_GADGET/strings/0x409/
  rmdir $CVI_GADGET
  umount $CVI_DIR
  rmdir $CVI_DIR
}

case "$1" in
  start)
	start
	;;
  stop)
	stop
	;;
  probe)
	probe
	;;
  *)
	echo "Usage: $0 probe {acm|msc|cvg} {file (msc)}"
	echo "Usage: $0 start"
	echo "Usage: $0 stop"
	exit 1
esac
exit $?
