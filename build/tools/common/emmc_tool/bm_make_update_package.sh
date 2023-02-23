#!/bin/bash

PROGNAME=$(basename $0)

set -e

EMMC_DEV_NUM=0
SDCARD_DEV_NUM=1
SDCARD_PARTTION_NUM=1

LOAD_COMMAND=fatload #default fatload

SECTOR_BYTES=512
CHUNK_SIZE=524288

BOOT_PART_OFFSET=8192
SCRIPT_ADDR=0x310000000
IN_ADDR=0x311000000
OUT_ADDR=0x321000000

RECOVERY_DIR=recovery_files
PARTITION_FILE=partition32G.xml
BOOTCMD_SCRIPT=boot
MOUNT_DIR=tmp

ERASE_PARTITION=false

SOURCE_FILES_PATH=

# help function
function usage()
{
	echo >&2
	echo "${PROGNAME}: $@" >&2
	echo >&2
	echo "Usage: ${PROGNAME} <type> <partition file> <source tgz files path>" >&2
	echo >&2
	echo "	type:" >&2
	echo "	sdcard		build sdcard update img" >&2
	echo "	tftp		build tftp update img" >&2
	echo >&2
	echo "	partition file:" >&2
	echo "	specified a partition file" >&2
	echo >&2
	echo >&2
	echo "	source tgz files path:" >&2
	echo "	specified your .tgz files path" >&2
	echo >&2
	exit 1
}

function cleanup()
{
	for i in $(seq 0 $[${#LABELS[@]}-1]); do
		if [ -f $RECOVERY_DIR/${LABELS[$i]} ]; then
			rm -f $RECOVERY_DIR/${LABELS[$i]}
		fi
		if mountpoint -q $RECOVERY_DIR/$MOUNT_DIR-$i; then
			sudo umount $RECOVERY_DIR/$MOUNT_DIR-$i
		fi
		rm -rf $RECOVERY_DIR/$MOUNT_DIR-*
	done
}

trap cleanup EXIT ERR SIGHUP SIGINT SIGQUIT SIGTERM

function panic()
{
	if [ $# -gt 0 ]; then
		echo "" >&2
		echo "${PROGNAME}: $@" >&2
		echo "" >&2
	fi

	echo build error, bye.

	exit 1
}

function file_validate()
{
	local file

	file=$(eval echo \$1)
	[ -f ${file} ] || panic "$i file \"$file\" does not exist"
	[ -r ${file} ] || panic "$i \"$file\" is not readable"
	[ -s ${file} ] || panic "$i \"$file\" is empty"
}

function suser() {
	echo
	echo To continue, superuser credentials are required.
	sudo -k || panic "failed to kill superuser privilege"
	sudo -v || panic "failed to get superuser privilege"
}

function compute_round_quotient()
{
	local total_size=$1
	local unit_size=$2

	[ ${unit_size} -gt 0 ] || panic "bad unit_size ${unit_size} in compute_round_quotient()"
	expr \( ${total_size} + ${unit_size} - 1 \) / ${unit_size}
}

function file_bytes() {
	local filename=$1

	stat --dereference --format="%s" ${filename} ||
	panic "unable to stat \"${filename}\""
}

function script_update()
{
	echo "$@" >> ${CURRENT_SCRIPT}
}

function get_current_path()
{
	echo $(cd "$(dirname "$1")" && pwd)
}

function parseargs()
{
	[ $# -lt 1 ] && usage "no update type specified"

	UPTYPE=$1

	if [ "${UPTYPE}" = sdcard ]; then
		LOAD_COMMAND="load mmc $SDCARD_DEV_NUM:$SDCARD_PARTTION_NUM"
	elif [ "${UPTYPE}" = tftp ]; then
		LOAD_COMMAND=tftp
	else
		usage "no valideupdate type specified"
	fi

	shift
	file_validate $1
	PARTITION_FILE=$1

	shift
	SOURCE_FILES_PATH=$(get_current_path $1)/$(basename $1)
	RECOVERY_DIR=${SOURCE_FILES_PATH}/${UPTYPE}
}

function parse_partition_xml()
{
	local offset=${BOOT_PART_OFFSET}

	#find total disk size in kb
	TOTAL_SIZE=($(grep -Po "physical_partition size_in_kb=\".+\"" ${PARTITION_FILE} | awk -F\" '{print $2}'))

	#find partions label size, and flag
	LABELS=($( grep -Po "label=\".+\"" ${PARTITION_FILE} | awk -F\" '{print $2}'))

	PART_SIZE_IN_KB=($(grep -Po "size_in_kb=\".+\"" ${PARTITION_FILE} | awk -F\" '{print $2}'))
	PART_SIZE_IN_KB=(${PART_SIZE_IN_KB[@]:1})

	for i in ${!PART_SIZE_IN_KB[*]}; do
		PART_SIZE_IN_SECTOR[$i]=$(expr ${PART_SIZE_IN_KB[$i]} \* 1024 / ${SECTOR_BYTES})
	done

	P_FLAG=($(grep -Po "readonly=\".+\"" ${PARTITION_FILE} | awk -F\" '{print $2}'))

	for i in $(seq 0 $[${#LABELS[@]}-1]); do
		PART_OFFSET[$i]=$offset
		offset=$(expr ${offset} + ${PART_SIZE_IN_SECTOR[$i]})
	done

	END_OFFSET=$offset

	for i in $(seq 0 $[${#LABELS[@]}-1]); do
		PART_COMPRESS_FILE_NAME[$i]=${SOURCE_FILES_PATH}/$(echo ${LABELS[$i],,}).tgz
	done
}

function compile_script() {
	local scr_name=$1
	local description="$1"

	mkimage -A arm64 -O linux -T script -C none -a 0 -e 0 -n "${description}" \
		-d ${CURRENT_SCRIPT} ${RECOVERY_DIR}/$scr_name.scr ||
	panic "failed to compile image for \"${CURRENT_SCRIPT}\""
}

function create_partition_script_done() {
	compile_script $(basename ${CURRENT_SCRIPT} .cmd)
	CURRENT_SCRIPT=${RECOVERY_DIR}/${BOOTCMD_SCRIPT}.cmd
}

function compress_and_update_partition_script()
{
	local filename=$1;
	local hex_disk_offset=$(printf "0x%08x" $2)
	local bytes=$(file_bytes ${filename});
	local hex_bytes=$(printf "0x%08x" ${bytes})
	local size=$(compute_round_quotient ${bytes} ${SECTOR_BYTES})
	local hex_size=$(printf "0x%08x" ${size})

	gzip ${filename}

	script_update "${LOAD_COMMAND} ${IN_ADDR}  $(basename  ${filename}.gz)"
	script_update "unzip ${IN_ADDR} ${OUT_ADDR} ${hex_bytes}"
	script_update "mmc write ${OUT_ADDR} ${hex_disk_offset} ${hex_size}"
	script_update "echo"
	script_update ""
}

function create_partition_script()
{
	local sub=$1; shift
	local description="$@"
	local sub_script=${RECOVERY_DIR}/${BOOTCMD_SCRIPT}-$(echo ${sub,,}).cmd

	# Add commands to the top-level script
	script_update "# ${description}"
	script_update "${LOAD_COMMAND} ${SCRIPT_ADDR} $(basename ${sub_script} .cmd).scr"
	script_update "source ${SCRIPT_ADDR}"
	script_update ""

	# Switch to the sub-script file
	CURRENT_SCRIPT=${sub_script}
	cp /dev/null ${CURRENT_SCRIPT}
	script_update "# ${description}"
	script_update ""
}

function create_main_script()
{
	CURRENT_SCRIPT=${RECOVERY_DIR}/${BOOTCMD_SCRIPT}.cmd
	cp /dev/null ${CURRENT_SCRIPT}

	script_update "# bitmain ${UPTYPE} recovery U-Boot script"
	script_update "# Created $(date)"
	script_update ""
	script_update "mmc dev $EMMC_DEV_NUM"
	script_update ""
	if [  ERASE_PARTITION = true ]; then
		script_update "echo erase mmc chip"
		script_update "mmc erase 0# $(echo "obase=16;${END_OFFSET}" | bc)"
		script_update ""
	fi
}

function split_and_compress_img()
{
	local part_number=$1
	local part_name=${RECOVERY_DIR}/$2
	local part_offset=${PART_OFFSET[${part_number}]}
	local offset=0
	local size
	local chunk_size=${CHUNK_SIZE}
	local count=1;
	local limit
	local desc="Partition ${part_number} (${LABELS[${part_number}]})"

	if [ ! -e "${part_name}" ]; then
		echo "Skipping partition ${part_number}"
		return
	fi

	size=$(du -b ${part_name} | awk '{print $1}')
	size=$(expr \( ${size} + ${SECTOR_BYTES} - 1 \) / ${SECTOR_BYTES})
	limit=$(compute_round_quotient ${size} ${chunk_size})

	create_partition_script $(basename ${part_name}) "${desc}"

	while true; do
		local filename=${part_name}.${count}-of-${limit};
		local disk_offset=$(expr ${part_offset} + ${offset})

		if [ ${size} -lt ${chunk_size} ]; then
			chunk_size=${size}
		fi

		dd status=none if=${part_name} of=${filename} \
			bs=${SECTOR_BYTES} skip=${offset} count=${chunk_size}

		compress_and_update_partition_script ${filename} ${disk_offset}

		count=$(expr ${count} + 1)
		offset=$(expr ${offset} + ${chunk_size})

		size=$(expr ${size} - ${chunk_size}) || break
	done

	create_partition_script_done
}

function do_gen_partition_subimg()
{
	dd if=/dev/zero of=$RECOVERY_DIR/$1 bs=${SECTOR_BYTES} count=${PART_SIZE_IN_SECTOR[$2]}

	if [ $3 -eq 1 ]; then
		mkfs.fat $RECOVERY_DIR/$1
	elif [ $3 -eq 2 ]; then
		mkfs.ext4 $RECOVERY_DIR/$1
	else
		echo $1 partition has no filesystem
	fi

        if [ $3 -eq 1 -o $3 -eq 2 ]; then
		if [ -f ${PART_COMPRESS_FILE_NAME[$2]} ]; then
			mkdir -p $RECOVERY_DIR/$MOUNT_DIR-$2
			sudo mount $RECOVERY_DIR/$1 $RECOVERY_DIR/$MOUNT_DIR-$2
			if [ $3 -eq 1 ]; then
				sudo tar -xzf ${PART_COMPRESS_FILE_NAME[$2]} --no-same-owner -C $RECOVERY_DIR/$MOUNT_DIR-$2
			else
	                      	sudo tar -xzf ${PART_COMPRESS_FILE_NAME[$2]} -C $RECOVERY_DIR/$MOUNT_DIR-$2
			fi
			sudo umount $RECOVERY_DIR/$MOUNT_DIR-$2
		else
			echo $1 may be an empty parition.
		fi
	fi

	if [ $4 -eq 1 ]; then
		e2fsck -f -p $RECOVERY_DIR/$1
		resize2fs -M $RECOVERY_DIR/$1
	fi
}

function gen_partition_img()
{
	local part_number=$1
	local part_name=$2

	case ${part_name} in
	BOOT)
		do_gen_partition_subimg $part_name $part_number 1 0
		;;
	RECOVERY)
		do_gen_partition_subimg $part_name $part_number 1 0
		;;
	MISC)
		do_gen_partition_subimg $part_name $part_number 0 0
		;;
	ROOTFS)
		do_gen_partition_subimg $part_name $part_number 2 1
		;;
	ROOTFS_RW)
		do_gen_partition_subimg $part_name $part_number 2 1
		;;
	SYSTEM)
		do_gen_partition_subimg $part_name $part_number 2 1
		;;
	FACTORY)
		do_gen_partition_subimg $part_name $part_number 2 1
		;;
	DATA)
		do_gen_partition_subimg $part_name $part_number 2 1
		;;
	*)
		#usage "invalid partition ${part_name}"
		;;
	esac
}

function make_partition_imgs()
{
	for i in $(seq 0 $[${#LABELS[@]}-1]); do (
		gen_partition_img $i ${LABELS[$i]}
	) done

	for i in $(seq 0 $[${#LABELS[@]}-1]); do (
		split_and_compress_img $i ${LABELS[$i]}
	) done

	wait
}

function make_gpt_img()
{
	local disk_offset=0
	local filename=${RECOVERY_DIR}/gpt
	local desc="mmc gpt file"

	./mk_gpt -p ${PARTITION_FILE} -d ${filename} 1 >/dev/null || true

	create_partition_script $(basename ${filename}) "${desc}"

	compress_and_update_partition_script ${filename} ${disk_offset}
	create_partition_script_done
}

function make_spi_flash_img()
{
	cp ${SOURCE_FILES_PATH}/fip.bin ${RECOVERY_DIR}
	touch ${RECOVERY_DIR}/boot
	script_update "# FIP in eMMC BOOT1"
	script_update "${LOAD_COMMAND} ${IN_ADDR} fip.bin"
	script_update "mmc dev $EMMC_DEV_NUM 1"
	script_update "mmc write ${IN_ADDR} 0x0 0x800"
	script_update "mmc write ${IN_ADDR} 0x800 0x800"
	script_update "mmc write ${IN_ADDR} 0x1000 0x800"
	script_update "mmc write ${IN_ADDR} 0x1800 0x800"
	script_update "mmc dev $EMMC_DEV_NUM 0"
	script_update ""

	if [ ! -e ${SOURCE_FILES_PATH}/spi_flash.bin ]; then
		echo "no SPI flash image"
	else
                cp ${SOURCE_FILES_PATH}/spi_flash.bin ${RECOVERY_DIR}

	        script_update "# SPI flash image"
	        script_update "${LOAD_COMMAND} ${IN_ADDR} spi_flash.bin"
		script_update "if test \$? -eq 0; then"
	        script_update "sf probe"
		script_update "sf update ${IN_ADDR} 0x0 0x100000"
	        script_update "else"
	        script_update "echo skip SPI flash update."
	        script_update "fi"
	        script_update ""
	fi
}

function all_work_done()
{
	script_update "echo eMMC update done, reboot board."
	script_update "if test \"\$reset_after\" = \"1\"; then reset; fi;"
	script_update "while true; do; echo \"Please remove the installation medium, then reboot\"; sleep 0.5; done;"
	compile_script $(basename ${CURRENT_SCRIPT} .cmd)

	echo build success.
}

function init()
{
	echo generating update files

	if [ -d ${RECOVERY_DIR} ];then
		rm -rf ${RECOVERY_DIR}

	fi

	mkdir -p ${RECOVERY_DIR} || panic "unable to create \"${RECOVERY_DIR}\""

	parse_partition_xml
	create_main_script
}

parseargs "$@"

echo start building ...
suser
init
make_spi_flash_img
make_gpt_img
make_partition_imgs
all_work_done
