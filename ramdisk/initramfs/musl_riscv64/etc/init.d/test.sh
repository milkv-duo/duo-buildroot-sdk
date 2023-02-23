#!/bin/sh

#9pfs works on qemu specifically
cat /proc/cmdline | grep -E 'qemuci'
if [ $? -eq 0 ]; then
	mkdir -p /root/host_shared
	mount -t 9p -o trans=virtio,version=9p2000.L hostshare /root/host_shared/
	ls -l /root/host_shared/

	if [ ! -f /root/host_shared/9PFS_IS_GREAT ]; then
		echo "9pfs tests failed"
		exit 0
	else
		echo "9pfs works fine"
	fi
fi

cat /proc/cmdline | grep -E 'chipci|qemuci'
if [ $? -ne 0 ]
then
        echo "Skip the ci test"
        exit 0
else
		echo "Lets start the ci test"
fi

sleep 5
#init system time to avoid case settimeofday01 fail
date -s 09:00:00
sleep 10 # Sleep 10 more secs to login

num=`ls -A /etc/init.ci/ | wc -w`

if [ $num == 0 ]; then
	echo "No test cases"
else
	cd /etc/init.ci/
	for i in *; do
		j=${i%_*}
		echo ================== $j test start ==================
		./$i
		if [ "$?" -ne "0" ]; then
			break;
		fi
		echo ================== $j test end ==================
	done
	cd - #This will echo a "/", so, don't bother
fi

echo "check total_result"
if [ -f /usr/lib/csky-ci/total_result ]; then
	echo "csky-ci tests failed"
	cat /usr/lib/csky-ci/total_result
fi

echo "sleep 10"
sleep 10
poweroff
