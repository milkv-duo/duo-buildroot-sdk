#!/bin/bash

logfile="UTIL/os/linux/Module.symvers"

if [ -e $logfile ]; then
 cp -f MODULE/os/linux/Module.symvers NETIF/os/linux/.
fi
echo

exit 0