#!/bin/bash

SYSTEM_DIR=$1
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libz*
rm -f $SYSTEM_DIR/mnt/system/lib/libz*

rm -f $SYSTEM_DIR/mnt/system/usr/lib/3rd/libcvi*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/3rd/libmad*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/3rd/libmp3*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/3rd/libopencv*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libopencv*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcvi_rtsp.so
rm -f $SYSTEM_DIR/mnt/system/data/install/lib/libcvi_rtsp.so
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcvikernel.so
rm -f $SYSTEM_DIR/mnt/system/usr/lib/*.a
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libgst*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libg*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/gstreamer-1.0/libgst*
rm -f $SYSTEM_DIR/mnt/system/data/install/lib/libcrypto.so*
rm -f $SYSTEM_DIR/mnt/system/data/install/lib/libssl.so*
rm -f $SYSTEM_DIR/mnt/system/data/install/lib/libcvi_protobuf.so*
rm -f $SYSTEM_DIR/mnt/system/data/install/lib/libprotobuf-lite.so*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcviai*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcvi_ispd.so*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libraw_replay.so*
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcvi_ive_tpu.so*
rm -rf $SYSTEM_DIR/mnt/system/usr/lib/gio
rm -rf $SYSTEM_DIR/mnt/system/usr/lib/glib*
rm -rf $SYSTEM_DIR/mnt/system/usr/lib/gstreamer-1.0*
rm -rf $SYSTEM_DIR/mnt/system/usr/libexec*
rm -rf $SYSTEM_DIR/mnt/system/usr/bin
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcvimath.so
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcviruntime.so
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcnpy.so
rm -f $SYSTEM_DIR/mnt/system/usr/lib/libcipher.so

rm -f $SYSTEM_DIR/mnt/system/lib/libcipher.so*
rm -f $SYSTEM_DIR/mnt/system/lib/libcvi_ispd.so*
rm -f $SYSTEM_DIR/mnt/system/lib/libraw_replay.so*
rm -f $SYSTEM_DIR/mnt/system/lib/libmad.so*
rm -f $SYSTEM_DIR/mnt/system/lib/libmp3*
rm -f $SYSTEM_DIR/mnt/system/lib/libnanomsg*

#del 3rdparty lib
#del thttpd/libwebsockets lib
rm -f $SYSTEM_DIR/mnt/system/lib/libthttpd*
rm -f $SYSTEM_DIR/mnt/system/lib/libwebsocket*
if [ $SDK_VER = "uclibc" ]
then
    #del opencv lib
    rm -f $SYSTEM_DIR/mnt/system/lib/libopencv*
    #del ffmpeg lib
    rm -f $SYSTEM_DIR/mnt/system/lib/libav*
    #save /mnt/system/lib/ openssl lib; need by ntpdate/wpa_supplicant
else
    #glibc ramdisk(rootfs/common_arm/usr/lib/) has libcrypto.so and libssl.so
    #del openssl
    rm -f $SYSTEM_DIR/mnt/system/lib/libssl*
    rm -f $SYSTEM_DIR/mnt/system/lib/libcrypto*
fi

rm -rf $SYSTEM_DIR/etc/init.d/S23ntp
rm -rf $SYSTEM_DIR/bin/ntpd

du -sh $SYSTEM_DIR/* |sort -rh
du -sh $SYSTEM_DIR/mnt/* |sort -rh
du -sh $SYSTEM_DIR/mnt/system/* |sort -rh
du -sh $SYSTEM_DIR/mnt/system/lib/*  |sort -rh
du -sh $SYSTEM_DIR/mnt/system/data/install/* |sort -rh
du -sh $SYSTEM_DIR/usr/* |sort -rh
