#!/bin/bash
#OUTPUT_DIR=/home/loveklss/work/bm1682/install/soc_bm1880_asic_2/
IMAGES_DIR=./images

#cp $OUTPUT_DIR/recovery.itb $IMAGES_DIR/
#cp $OUTPUT_DIR/emmcboot.itb $IMAGES_DIR/
#cp $OUTPUT_DIR/fip.bin $IMAGES_DIR/
#cp $OUTPUT_DIR/rootfs $IMAGES_DIR/ -rf

clear
cat << EOF
--------------------
Usage:
	Select images to create a update image. Option with GPT and FIP image. If you only
	want to updating certain partition in current board environment, do not 'select GPT as y/Y',
	it will crash datas if do so.

	You'd better refer to doc below before you starting.
	https://info.bitmain.vip:8443/display/AICSW/eMMC+burning+scheme

General Description:
	GPT partition table details:
	| M  |   M    |  M   |  M   | M  |   O   | M  |
	| p1 |   p2   |  p3  |  p4  | p5 |   p6  | p7 |
	|128M|  256M  |  c1  |  c1  | c1 |   c1  | c1 |
	|BOOT|RECOVERY|ROOTFS|SYSTEM|MISC|FACTORY|DATA|

Note:
M: Manufactor partition
O: Optional partition
p[x]: Relative to linux /dev/mmcblk0p[x]
c1: The size of partition is belong to EMMC capacity, has different size when using 8G, 16G, 32G respectively
--------------------
EOF

function make_partition
{
	if [ $? -eq 0 ]; then
		./mk_gpt -p ./ -d $IMAGES_DIR/gpt.img
	else
		echo "Compile error,please check!"
	fi
}

function make_package
{
	if [ $? -eq 0 ]; then
		./crt_p -p ./ -o bm_update.img
	else
		echo "Compile error,please check!"
	fi
}

function get_resp()
{
	select=$1
	pattern=$2

	while [ true ]
	do
		read -p "Select (default $select): "

		if [ -z $REPLY ]; then
			result=$select
			break
		else
			if [ `expr match $REPLY "[Y|y|N|n]"` -ne 0 ]; then
				result=`echo $REPLY | tr '[A-Z]' '[a-z]'`
				break
			fi
			if [ `expr match $REPLY "$pattern"` -ne 0 ]; then
				result=$REPLY
				break
			fi
		fi
	done

	echo "$result"
}

package_xml=./package.xml
update_all=y

gpt=n
capacity=1
fip=n
boot=n
recovery=n
rootfs=n
system=n
misc=n
factory=n
data=n


if [ "$1" == "auto" ]; then
	update_all="y"
else
	echo -e "\nUpdate all (y: yes, n: no)"
	update_all=$(get_resp $update_all)
fi

if [ $update_all == "y" ]; then
	capacity=1
	gpt=y
	fip=y
	boot=y
	recovery=y
	rootfs=y
	system=y
	misc=y
	factory=n
	data=y
else
	echo -e "\nGPT (y: yes, n: no)"
	gpt=$(get_resp $gpt)

	echo -e "\nFIP (y: yes, n: no)"
	fip=$(get_resp $fip)

	echo -e "\nboot (y: yes, n: no)"
	boot=$(get_resp $boot)

	echo -e "\nrecovery (y: yes, n: no)"
	recovery=$(get_resp $recovery)

	echo -e "\nrootfs (y: yes, n: no)"
	rootfs=$(get_resp $rootfs)

	echo -e "\nsystem (y: yes, n: no)"
	system=$(get_resp $system)

	echo -e "\nmisc (y: yes, n: no)"
	misc=$(get_resp $misc)

	echo -e "\nfactory (y: yes, n: no)"
	factory=$(get_resp $factory)

	echo -e "\ndata (y: yes, n: no)"
	data=$(get_resp $data)
fi

echo -e "\nSTORAGE CAPACITY (1: 8G, 2: 16G, 4: 32G)"
echo -e "\033[31m Be sure the real capacity your board using!!! \033[0m"
if [ "$1" == "auto" ]; then
	capacity=1
else
	capacity=$(get_resp $capacity "[1|2|4]")
fi
let capacity=$capacity*8

echo "<header time=\"$(date "+%F-%H-%M-%S")\">" > $package_xml
cp ./partition${capacity}G.xml ./partition.xml
if [ $gpt == "y" ]; then
	misc=y
	make_partition
	echo "make partition completed"
	if [ $? -ne 0 ]; then
		echo "check 1"
		exit -1
	fi
	gpt=$IMAGES_DIR/gpt.img
	if [ ! -e $gpt ]; then
		echo -e "\033[31m$gpt not exist, please copy it to 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $gpt | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		echo "check 2"
		exit -1
	fi
	echo " <item name=\"GPT\" path=\"$gpt\" md5=\"$md5\" />" >> $package_xml

fi

if [ $fip == "y" ]; then
	fip=$IMAGES_DIR/fip.bin
	if [ ! -e $fip ]; then
		echo -e "\033[31m$fip not exist, please copy it to 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $fip | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi
	echo " <item name=\"FIP\" path=\"$fip\" md5=\"$md5\" />" >> $package_xml
fi

if [ $boot == "y" ]; then
	boot=$IMAGES_DIR/emmcboot.itb
	if [ ! -e $boot ]; then
		echo -e "\033[31m$boot not exist, please copy it to 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $boot | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi

	echo " <item name=\"BOOT\" path=\"$boot\" md5=\"$md5\" />" >> $package_xml
fi

if [ $recovery == "y" ]; then
	recovery=$IMAGES_DIR/recovery.itb
	if [ ! -e $recovery ]; then
		echo -e "\033[31m$recovery not exist, please copy it to 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $recovery | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi

	echo " <item name=\"RECOVERY\" path=\"$recovery\" md5=\"$md5\" />" >> $package_xml
fi

if [ $rootfs == "y" ]; then
	rootfs=$IMAGES_DIR/rootfs.img
	p_size=`sed -n '/ROOTFS/p' partition.xml | awk -F '[ ]+' '{print $3}' | cut -f 2 -d '=' | sed 's/\"//g'`"K"
	make_ext4fs -s -l $p_size $rootfs $IMAGES_DIR/rootfs/
	if [ $? -ne 0 ]; then
		echo -e "\033[31m make_ext4fs command not existed, please install: \nsudo apt-get install android-tools-fsutils \033[0m"
		exit -1
	fi
	if [ ! -e $rootfs ]; then
		echo -e "\033[31m$rootfs not exist, please copy 'rootfs' into 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $rootfs | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi

	echo " <item name=\"ROOTFS\" path=\"$rootfs\" md5=\"$md5\" />" >> $package_xml
fi

if [ $system == "y" ]; then
	system=$IMAGES_DIR/system.img
	p_size=`sed -n '/SYSTEM/p' partition.xml | awk -F '[ ]+' '{print $3}' | cut -f 2 -d '=' | sed 's/\"//g'`"K"
	make_ext4fs -s -l $p_size $system $IMAGES_DIR/system/
	if [ $? -ne 0 ]; then
		echo -e "\033[31m make_ext4fs command not existed, please install: \nsudo apt-get install android-tools-fsutils \033[0m"
		exit -1
	fi
	if [ ! -e $system ]; then
		echo -e "\033[31m$system not exist, please copy 'system' into 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $system | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi
	echo " <item name=\"SYSTEM\" path=\"$system\" md5=\"$md5\" />" >> $package_xml
fi

if [ $misc == "y" ]; then
	misc=$IMAGES_DIR/misc.img
	echo "boot-recovery /usr/bin/expand.sh" > $misc
	p_size=`sed -n '/MISC/p' partition.xml | awk -F '[ ]+' '{print $3}' | cut -f 2 -d '=' | sed 's/\"//g'`"K"
	md5=$(md5sum $misc | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi
	echo " <item name=\"MISC\" path=\"$misc\" md5=\"$md5\" />" >> $package_xml
fi

if [ $factory == "y" ]; then
	factory=$IMAGES_DIR/factory.img
	p_size=`sed -n '/FACTORY/p' partition.xml | awk -F '[ ]+' '{print $3}' | cut -f 2 -d '=' | sed 's/\"//g'`"K"
	make_ext4fs -s -l $p_size $factory $IMAGES_DIR/factory/
	if [ $? -ne 0 ]; then
		echo -e "\033[31m make_ext4fs command not existed, please install: \nsudo apt-get install android-tools-fsutils \033[0m"
		exit -1
	fi
	if [ ! -e $factory ]; then
		echo -e "\033[31m$factory not exist, please copy 'factory' into 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $factory | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi
	echo " <item name=\"FACTORY\" path=\"$factory\" md5=\"$md5\" />" >> $package_xml
fi

if [ $data == "y" ]; then
	data=$IMAGES_DIR/data.img
	p_size=`sed -n '/DATA/p' partition.xml | awk -F '[ ]+' '{print $3}' | cut -f 2 -d '=' | sed 's/\"//g'`"K"
	make_ext4fs -s -l $p_size $data $IMAGES_DIR/data/
	if [ $? -ne 0 ]; then
		echo -e "\033[31m make_ext4fs command not existed, please install: \nsudo apt-get install android-tools-fsutils \033[0m"
		exit -1
	fi
	if [ ! -e $data ]; then
		echo -e "\033[31m$data not exist, please copy 'data' into 'images' folder \033[0m"
		exit -1
	fi
	md5=$(md5sum $data | cut -d ' ' -f 1 )
	if [ $? -ne 0 ]; then
		exit -1
	fi
	echo " <item name=\"DATA\" path=\"$data\" md5=\"$md5\" />" >> $package_xml
fi

echo "</header>" >> $package_xml

make_package

echo ""
echo -e "\033[32mGPT TABLE: \033[0m"
while read line
do
	echo -e "\033[32m $line \033[0m"
done < partition.xml

