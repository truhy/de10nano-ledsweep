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

	Version: 20240211

	Vendor specific low-level UART functions.
*/

#ifndef TRU_UART_LL_H
#define TRU_UART_LL_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_C5SOC)

#include "tru_util_ll.h"
#include <stdint.h>

// ======================================================================
// Intel Cyclone V SoC FPGA (Synopsys UART controller) specific registers
// ======================================================================

// UART base registers
#define TRU_UART0_BASE_ADDR         0xffc02000UL
// UART register offsets
#define TRU_UART_RBR_THR_DLL_OFFSET 0x0UL
#define TRU_UART_LSR_OFFSET         0x14UL
#define TRU_UART_SFE_OFFSET         0x98UL
#define TRU_UART_STET_OFFSET        0xa0UL
// UART0 registers
#define TRU_UART0_RBR_THR_DLL_ADDR  0xffc02000UL
#define TRU_UART0_LSR_ADDR          0xffc02014UL
#define TRU_UART0_SFE_ADDR          0xffc02098UL
#define TRU_UART0_STET_ADDR         0xffc020a0UL

void tru_uart_ll_wait_empty(uint32_t uart_base_addr);
void tru_uart_ll_write_str(uint32_t uart_base_addr, const char *str, uint32_t len);
void tru_uart_ll_write_char(uint32_t uart_base_addr, const char c);
void tru_uart_ll_write_hex_nibble(uint32_t uart_base_addr, unsigned char nibble);
void tru_uart_ll_write_inthex(uint32_t uart_base_addr, int num, unsigned int bits);

#endif

#endif
