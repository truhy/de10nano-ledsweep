/*
    MIT License

    Copyright (c) 2023 Truong Hy

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    Version: 20230701

    Supporting code for PIO Core Intel FPGA IP (Parallel IO).

	Notes
	=====

	PIO work very much like GPIO port registers you normally see in microprocessors, e.g. PIC, STM32, etc.

	There are four modes:
		- BiDir, which implements bidirectional I/O using FPGA tristate inout ports
		- InOut, which implements bidirectional I/O using separate FPGA input and output ports
		- Input, which implements input only using FPGA input ports
		- Output, which implements output only using FPGA output ports
	Note, InOut mode is unfortunately named similarly to FPGA inout port, but they are different.

	Note, the BiDir and InOut modes are not the same, BiDir = single bidirectional FPGA ports, and InOut = two separate FPGA ports.
	The direction register is only available when using bidirectional modes.
	The bidirectional modes (BiDir & InOut) are implemented as dual one-way directional states.

	If HPS writes to the PIO data or out register the FPGA can extract the value by reading the PIO output port.
	Note, the HPS value will not appear in the HPS PIO data register.
	Basically:
		- A HPS write to the PIO data (output) or out memory-mapped register will transfer to the FPGA PIO output port
		- A HPS read from the data (input) register which will read the last changed value made by the FPGA
		- A FPGA write to the PIO input port will transfer to the PIO data (input) register
		- A FPGA read from the PIO output port will read the last changed value made by the HPS

	For more details see Intel Embedded Peripherals IP User Guide, PIO Core:
	https://www.intel.com/content/www/us/en/docs/programmable/683130/23-4/pio-core.html
*/

#ifndef FPGA_PIO_INTEL_H
#define FPGA_PIO_INTEL_H

// Generic address offsets
#define PIO_DATA_OFFSET    (0UL * 4UL)
#define PIO_DIR_OFFSET     (1UL * 4UL)
#define PIO_IRQ_MSK_OFFSET (2UL * 4UL)
#define PIO_IRQ_CLR_OFFSET (3UL * 4UL)
// These two only exists in the memory register when the Output Register is ticked in Platform Designer
#define PIO_OUT_SET_OFFSET (4UL * 4UL)
#define PIO_OUT_CLR_OFFSET (5UL * 4UL)

#endif
