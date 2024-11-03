#!/bin/bash

set -e
function cleanup {
	rc=$?
	# If error and shell is child level 1 then stay in shell
	if [ $rc -ne 0 ] && [ $SHLVL -eq 1 ]; then exec $SHELL; else exit $rc; fi
}
trap cleanup EXIT

if [ -z "${APP_HOME_PATH+x}" ]; then
	chmod +x ../scripts-env/env-linux.sh
	source ../scripts-env/env-linux.sh
fi

cd "$APP_HOME_PATH"

SOFFILE="../$FPGA_SRC_PATH/output_files/$FPGA_PROGRAM_NAME.sof"
if [ -f $SOFFILE ]; then
	cp -f $SOFFILE "$APP_SRC_PATH1/bsp/$FPGA_PROGRAM_NAME.sof"
fi
