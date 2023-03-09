#!/bin/bash

function copyBinToOutFolder()
{
	echo $1 $2
	# mPATH=$( getBinPath )
	# mSENSOR=$( getSensor )
	# mLENS=$( getLens )
	mPATH=${CHIP_ARCH,,}/src/$2
	mPRE_BIN_NAME=$2

	if [ "$mPATH" != "" ]; then
		sdrBin=${mPATH}/${mPRE_BIN_NAME}_sdr.bin
		wdrBin=${mPATH}/${mPRE_BIN_NAME}_wdr.bin
		sdr_irBin=${mPATH}/${mPRE_BIN_NAME}_sdr_ir.bin
		outPath=$1

		mkdir -p ${outPath}

		if [ -f "${wdrBin}" ]; then
			cp ${wdrBin} ${outPath}/cvi_wdr_bin
		fi

		if [ -f "${sdrBin}" ]; then
			cp ${sdrBin} ${outPath}/cvi_sdr_bin
		fi

		if [ -f "${sdr_irBin}" ]; then
			cp ${sdr_irBin} ${outPath}/cvi_sdr_ir_bin
		fi
	fi
}

copyBinToOutFolder $1 $2
