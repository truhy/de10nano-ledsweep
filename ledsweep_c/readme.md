# Ledsweep software for the Terasic DE10-Nano development board

## Overview

A bare-metal C program that demonstrates the processor system (HPS) working with FPGA logic.

This is the software part of the ledsweep, programmed to work with the FPGA logic design counterpart.  It animates the 8x FPGA LEDs (0 to 7) by sweeping them from side to side.  Pressing the FPGA tactile keys (0 and 1) will increase and decrease the animation speed.
		
The FPGA logic design sets up the L2F bridge so that the HPS can control the LEDs by using the L2F memory map registers, and also sets up the FPGA keys so they will trigger a GIC interrupt and is forwarded to the HPS side.

The correct run order is:
1. configure the FPGA first using the bitstream file ledsweep.sof or ledsweep.rbf
2. and then execute this program

## Build requirements

Minimum to build the C sources:
- GNU C/C++ cross toolchain for ARM processors supporting Cortex-A9
- newlib library.  This is usually included with the above toolchain

### Using make script to build
Included is my makefile script (actually a set of scripts) to build the C sources and SD card image.

To build the C sources using the script under Windows, you will need a Windows port of GNU make with some shell facilities, a good solution is xPack's Build Tools.

### Using "Eclipse IDE for Embedded C/C++ Developers" to build
To build the C sources using Eclipse, you can open with the included project file for Eclipse IDE, but you may need to setup other things first as described in my guide.

### Building the SD card image and U-Boot sources
To build these under Windows you will need to use WSL2 or Linux under a VM.  See the makefile or my guide for more information.

## Guide

You can find the guide on my website:
[https://truhy.co.uk](https://truhy.co.uk)

## Notes

If debug print is enabled, then to view the serial messages, start a standard serial com terminal software (e.g. PuTTY) and set it to use the relevant com port with the following settings: 115200 baud, 8 data bits, no parity and 1 stop bit.
