#!/bin/sh

SILENT=0
export LD_LIBRARY_PATH=$(pwd)/../lib:$LD_LIBRARY_PATH

usage() {
    echo "$0 -t [ h264 | h265 ] [-s]"
    echo "    -t test item"
    echo "    -v silent"
}

run_rtsp_server() {
    if [ $SILENT = 1 ]; then
        timeout 10 ./rtsp_server ${TYPE}.json > /dev/null 2>&1
    else
        timeout 10 ./rtsp_server ${TYPE}.json
    fi
}

rtsp_client() {
    if [ $SILENT = 1 ]; then
        timeout 5 ./rtsp_client rtsp://127.0.0.1/stream0 > /dev/null 2>&1
    else
        timeout 5 ./rtsp_client rtsp://127.0.0.1/stream0
    fi
    if [ $? = 0 ]; then
        echo "TEST-PASS"
    else
        echo "TEST-FAIL"
    fi
}

while getopts 'st:' c
do
  case $c in
    s) SILENT=1;;
    t)
        TYPE=$OPTARG
        if [ $TYPE != "h264" -a $TYPE != "h265" ]; then
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

run_rtsp_server &
sleep 2
rtsp_client
