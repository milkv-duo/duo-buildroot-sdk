#!/bin/sh

export LD_LIBRARY_PATH=$(pwd)/../lib:$(pwd)/../aisdk_lib/lib:$LD_LIBRARY_PATH

SILENT=0

usage() {
    echo "$0 -t [ unittest | ai | all ] [-s]"
    echo "    -t test item"
    echo "    -v silent"
}

while getopts 'st:' c
do
  case $c in
    s) SILENT=1;;
    t)
        TYPE=$OPTARG
        if [ $TYPE != "all" -a $TYPE != "ai" -a $TYPE != "unittest" ]; then
            usage
            exit 1
        fi
        ;;
     *)
        usage
        exit 1
  esac
done

if [ "$TYPE" = "" ]; then
    usage
    exit 1
fi

if [ $TYPE = "unittest" -o $TYPE = "all" ]; then
    echo "CVI_RTSP UNITTEST TEST"
    if [ $SILENT = 1 ]; then
        ./test_unittest > /dev/null 2>&1
    else
        ./test_unittest
    fi

    if [ $? = 0 ]; then
        echo "TEST-PASS"
    else
        echo "TEST-FAIL"
    fi
fi

if [ $TYPE = "ai" -o $TYPE = "all" ]; then
    echo "CVI_RTSP AISDK_LIB TEST"
    if [ $SILENT = 1 ]; then
        ./test_aisdk_lib > /dev/null 2>&1
    else
        ./test_aisdk_lib
    fi
    if [ $? = 0 ]; then
        echo "TEST-PASS"
    else
        echo "TEST-FAIL"
    fi
fi
