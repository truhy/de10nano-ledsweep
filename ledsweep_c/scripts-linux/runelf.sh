#!/bin/bash

set -e
function cleanup {
	rc=$?
	# If error and shell is child level 1 then stay in shell
	if [ $rc -ne 0 ] && [ $SHLVL -eq 1 ]; then exec $SHELL; else exit $rc; fi
}
trap cleanup EXIT

if [ -z "${SCRIPT_PATH+x}" ]; then
	chmod +x ../scripts-env/env-linux.sh
	source ../scripts-env/env-linux.sh
fi

cd $SCRIPT_PATH

# Determine build from input argument
if [ $1 = "debug" ]; then
	app1_elf="Debug/$APP_PROGRAM_NAME1".elf
	if [ ! -f $app1_elf ]; then app1_elf="source/Debug/$APP_PROGRAM_NAME1".elf fi
	ubootspl="$APP_SRC_PATH1/bsp/u-boot-spl-nocache"
else
	app1_elf="Release/$APP_PROGRAM_NAME1".elf
	if [ ! -f $app1_elf ]; then app1_elf="source/Release/$APP_PROGRAM_NAME1".elf fi
	ubootspl="$APP_SRC_PATH1/bsp/u-boot-spl-nocache"
fi

# Find elf entry point
user_entry=$(arm-none-eabi-readelf -h $app1_elf | grep "Entry point" | cut -d : -f 2)
#user_entry=0x00100040

# Reset SoC HPS, load and execute U-Boot SPL elf
openocd -f interface/altera-usb-blaster2.cfg -f target/altera_fpgasoc_de.cfg -c "init; halt; c5_reset; halt; c5_spl $ubootspl; sleep 200; halt; arm core_state arm; load_image $app1_elf; resume $user_entry; shutdown"
