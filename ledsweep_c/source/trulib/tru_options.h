/*
	MIT License

	Copyright (c) 2025 Truong Hy

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

	Version: 20260208

	Configuration options.
*/

#ifndef TRU_OPTIONS_H
#define TRU_OPTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRU_OPT_CHIPSET_C5SOC   0
#define TRU_OPT_CHIPSET_STM32H7 1
#define TRU_OPT_CHIPSET_SG2002  2

#define TRU_OPT_CPU_CORTEXA9   0
#define TRU_OPT_CPU_CORTEXM7   1
#define TRU_OPT_CPU_CORTEX_A53 2
#define TRU_OPT_CPU_RISCV_C906 3
#define TRU_OPT_CPU_8051       4

// Development board
#define TRU_OPT_BOARD_CUSTOM          0
#define TRU_OPT_BOARD_C5SOC_CUSTOM    1
#define TRU_OPT_BOARD_DE10NANO        2
#define TRU_OPT_BOARD_STM32H7_CUSTOM  3
#define TRU_OPT_BOARD_NUCLEO144_753ZI 4
#define TRU_OPT_BOARD_MILKV_DUO256M   5

#define TRU_OPT_SYSCALL_IO_SWV         0
#define TRU_OPT_SYSCALL_IO_SEMIHOSTING 1
#define TRU_OPT_SYSCALL_IO_UART0       2
#define TRU_OPT_SYSCALL_IO_UART1       3

#ifdef __cplusplus
}
#endif

#endif
