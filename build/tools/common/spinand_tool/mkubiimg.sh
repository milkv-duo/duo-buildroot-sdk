#!/bin/bash

function usage ()
{
	echo "Usage: ${selfname}  Chip Pagesize  Blocksize  Dir  Size  Tool_path Res"
	echo "  Chip            Chip name. "
	echo "  Pagesize        NAND page size. 2k/4k/8k."
	echo "  Blocksize       NAND block size. 128k/256k/1M "
	echo "  Dir             The directory you want to make ubifs"
	echo "  Size            This ubifs partition size in NAND. 48M, ... 50M"
	echo "  UBI Tool        The path of mkfs.ubifs and ubinize"
	echo "  Res             Reserve ubiimg and ubifs both (1:Yes 0:No(default))"
	echo ""
	echo "Example:"
	echo "  ${selfname} hi35xx 2k 128k osdrv/pub/rootfs 50M osdrv/pub/bin/pc 0"
	echo ""
	exit 0
}
###############################################################################

function run ()
{
	local cmd=$1
	echo "${cmd}"
	msg=$(eval "${cmd}"); result=$?
	echo ${msg}
	[ ${result} == 0 ] || exit ${result}
}
###############################################################################

function hstrtol ()
{
	local hstr=$1
	local zoom=1
	local result=$(echo "${hstr}" | awk '{printf "%d",$0}')

	if [ "$(echo ${hstr} | grep '[Gg]')" == "${hstr}" ]; then
		zoom=1073741824
	elif [ "$(echo ${hstr} | grep '[Mm]')" == "${hstr}" ]; then
		zoom=1048576
	elif [ "$(echo ${hstr} | grep '[Kk]')" == "${hstr}" ]; then
		zoom=1024
	fi

	echo $((${result} * ${zoom}))
}

selfname=$(basename $0)

if [ $# != 7 ] && [ $# != 6 ]; then
	usage;
fi

hpagesize=${2}
pagesize=$(hstrtol ${hpagesize})
hblocksize=${3}
blocksize=$(hstrtol ${hblocksize})
rootdir=$(echo $(echo "${4} " | sed 's/\/ //'))
#rootfs=${rootdir##*/}
rootfs=rootfs
hpartsize=${5}
partsize=$(hstrtol ${hpartsize})
chip=${1}

if [ ! -d ${rootdir} ]; then
	echo "Directory ${rootdir} not exist."
	exit 1;
fi

LEB=$((${blocksize} - ${pagesize} * 2))
MAX_LEB_CNT=$((${partsize} / ${blocksize}))
###############################################################################

ubiimg=${rootfs}_${chip}_${hpagesize}_${hblocksize}_${hpartsize}.ubiimg
ubifsimg=${rootfs}_${chip}_${hpagesize}_${hblocksize}_${hpartsize}.ubifs
ubicfg=${rootfs}_${chip}_${hpagesize}_${hblocksize}_${hpartsize}.ubicfg

MKUBIFS=$(echo $(echo "${6} " | sed 's/\/ //'))/mkfs.ubifs
MKUBI=$(echo $(echo "${6} " | sed 's/\/ //'))/ubinize
chmod +x ${MKUBIFS}
chmod +x ${MKUBI}

run "${MKUBIFS} -F -d ${rootdir} -m ${pagesize} -o ${ubiimg} -e ${LEB} -c ${MAX_LEB_CNT}"

{
	echo "[ubifs-volumn]"
	echo "mode=ubi"
	echo "image=${ubiimg}"
	echo "vol_id=0"
	echo "vol_type=dynamic"
	echo "vol_alignment=1"
	echo "vol_name=ubifs"
	echo "vol_flags=autoresize"
	echo ""

} > ${ubicfg}

run "${MKUBI} -o ${ubifsimg} -m ${pagesize} -p ${blocksize} ${ubicfg}"


echo "--------- ${ubifsimg} is prepared !!"
if [ ! -n "${7}" ] || [ ${7} = 0 ]; then
	rm -f ${ubiimg} ${ubicfg}
	exit 1;
fi
echo "--------- ${ubiimg} is prepared !!"
echo "--------- ${ubicfg} is prepared !!"
