#!/bin/bash

logfile="UTIL/os/linux/Module.symvers"

if [ -e $logfile ]; then
 cp -f UTIL/os/linux/Module.symvers MODULE/os/linux/.
fi
echo

exit 0