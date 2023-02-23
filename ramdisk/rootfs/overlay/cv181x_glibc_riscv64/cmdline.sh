#!/bin/sh

# read the kernel options. we need surve things like:
#  acpi_osi="!Windows 2006" xen-pciback.hide=(01:00.0)
set -- $(busybox cat /proc/cmdline)

myopts="root"

for opt; do
	case "$opt" in
	s|single|1)
		SINGLEMODE=yes
		continue
		;;
	esac

	for i in $myopts; do
		case "$opt" in
		$i=*)	eval "KOPT_${i}"='${opt#*=}';;
		$i)	eval "KOPT_${i}=yes";;
		no$i)	eval "KOPT_${i}=no";;
		esac
	done
done

echo "${KOPT_root}"

