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

du -sh $SYSTEM_DIR/* |sort -rh
du -sh $SYSTEM_DIR/mnt/* |sort -rh
du -sh $SYSTEM_DIR/mnt/system/* |sort -rh
du -sh $SYSTEM_DIR/mnt/system/lib/*  |sort -rh
du -sh $SYSTEM_DIR/mnt/system/data/install/* |sort -rh
du -sh $SYSTEM_DIR/usr/* |sort -rh

if [ "$SDK_VER" = "64bit" ]
then
	#del glibc
	rm -rf $SYSTEM_DIR/lib/{libudev*,libuuid*,libtinfo*,libselinux*,libreadline*,libnss_nisplus*, \
		libnss_nis*,libnss_hesiod*,libnss_db*,libnss_compat*}
	rm -rf $SYSTEM_DIR/lib/{libiperf*,libffi*,libexpat*,libdevmapper*,libblkid*,ld-linux-armhf*,arm-linux-gnueabihf}
	rm -rf $SYSTEM_DIR/lib/libstdc++.so.6.0.22-gdb.py
	#del ai lib
	rm -rf $SYSTEM_DIR/mnt/system/lib/{libopencv*,libav*,libjson*,libsqlite*,libswre*}
	#del mw lib

	#del /bin , /sbin
	rm -rf $SYSTEM_DIR/sbin/{fsck.fat,ldconfig,ifconfig}
	rm -rf $SYSTEM_DIR/bin/{bash,findmnt,format.sh,mkfs.fat,reboot_recovery,sgdisk,veritysetup.static}
	#del /usr/bin , /usr/sbin
	rm -rf $SYSTEM_DIR/usr/sbin/{ubi*,tftpd,telnetd,sshd,svlogd,setfont,sendmail,rtcwake,remove-sfhell, \
		rsyslogd,readahead,rdev,powertop,popmaildir,ntpd,nbd-client,nandwrite,nanddump,lpd,ifplugd, \
		httpd,ftpd,flash_erase,fakeidentd,dhcprelay,chpasswd,chat,brctl,blkid,add-shell}
	rm -rf $SYSTEM_DIR/usr/bin/{beep,blkdiscard,bzip2,cal,chpst,comm,cryptpw,cvi_pinmux,dpkg,dpkg-deb, \
		expand,expand.sh,fgconsole,ftpget,ftpput,gator.sh,gatord_acc,gatord_avg,groups,hd,hostapd,hostapd_cli, \
		i2c_read,i2c_write,lpq,lpr,man,mtr,mtr-packet,nc,netperf,nmeter,nsenter,pgrep,pip,pip3,pip3.7,pkill, \
		pscan,pstree,pwdx,rpm2cpio,runsv,runsvdir,rx,script,setuidgid,showkey,shuf,smemcap,softlimit,split, \
		ssl_client,sum,sv,tac,taskset,tcpsvd,timeout,traceroute6,ttysize,udpsvd,unexpand,unshare,users, \
		pmap,dumpleases,envdir,envuidgid,volname,wall,whois,wifi.sh}
	rm -rf $SYSTEM_DIR/usr/{libexec,local/bin/s*,local/bin/ntp*}
	rm -rf $SYSTEM_DIR/usr/lib/{libestr.so*,libfastjson.so*,liblogging-stdlog.so*,share}
	rm -rf $SYSTEM_DIR/usr/share/{doc,lintian}
	#add ln file
	pushd $SYSTEM_DIR/sbin
	ln -s /bin/busybox ifconfig
	popd
	pushd $SYSTEM_DIR/lib
	ln -s ld-linux-aarch64.so.1 ld-2.23.so
	ln -s libanl.so.1 libanl-2.23.so
	ln -s libc.so.6 libc-2.23.so
	ln -s libcrypt.so.1 libcrypt-2.23.so
	ln -s libdl.so.2 libdl-2.23.so
	ln -s libm.so.6 libm-2.23.so
	ln -s libnsl.so.1 libnsl-2.23.so
	ln -s libpthread.so.0 libpthread-2.23.so
	ln -s libresolv.so.2 libresolv-2.23.so
	ln -s librt.so.1 librt-2.23.so
	ln -s libstdc++.so.6 libstdc++.so.6.0.22
	ln -s libutil.so.1 libutil-2.23.so
	popd
fi
