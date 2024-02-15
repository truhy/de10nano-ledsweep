#!/bin/bash

set -e
function cleanup {
	rc=$?
	# If error and shell is child level 1 then stay in shell
	if [ $rc -ne 0 ] && [ $SHLVL -eq 1 ]; then exec $SHELL; else exit $rc; fi
}
trap cleanup EXIT

if [ -z "${BM_HOME_PATH+x}" ]; then
	chmod +x ../scripts-env/env-linux.sh
	source ../scripts-env/env-linux.sh
fi

cd "$BM_HOME_PATH"

# Program .sof to the FPGA
# Parameters: -c 1 = selects J-TAG cable number 1, @2 is referring to device index on the J-TAG chain (1 = HPS SoC CPU, 2 = Cyclone V FPGA)
quartus_pgm -m jtag -c 1 -o "p;$BM_SRC_PATH1/bsp/$FPGA_PROGRAM_NAME.sof@2"
