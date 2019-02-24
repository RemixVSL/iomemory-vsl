#!/bin/sh

# Sets PREV_KERNELVER, PREV_KERNEL_SRC
CONF="./check_target_kernel.conf"

# kfio_config.h
KFIO_CONFIG_H="include/fio/port/linux/kfio_config.h"

if [ -e "$CONF" ] ; then
	. "$CONF"
else
	PREV_KERNELVER=""
	PREV_KERNEL_SRC=""
fi

CUR_KERNELVER="$1"
CUR_KERNEL_SRC="$2"

RET=0
if [ "$CUR_KERNELVER" != "$PREV_KERNELVER" ] ; then
	RET=1
	if [ "$PREV_KERNELVER" != "" ] ; then
		echo -n "KERNELVER "
	fi
fi
if [ "$CUR_KERNEL_SRC" != "$PREV_KERNEL_SRC" ] ; then
	RET=1
	if [ "$PREV_KERNEL_SRC" != "" ] ; then
		echo -n "KERNEL_SRC "
	fi
fi
echo "PREV_KERNELVER=\"$CUR_KERNELVER\"" > "$CONF"
echo "PREV_KERNEL_SRC=\"$CUR_KERNEL_SRC\"" >> "$CONF"

exit $RET
