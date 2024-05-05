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

	Version: 20240505

	Low-level code for Cyclone V SoC HPS UART controller.
*/

#ifndef TRU_C5SOC_HPS_UART_LL_H
#define TRU_C5SOC_HPS_UART_LL_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_C5SOC)

#include "tru_util_ll.h"
#include <stdint.h>

// ======================================================================
// Intel Cyclone V SoC FPGA (Synopsys UART controller) specific registers
// ======================================================================

// HPS UART generic
#define TRU_HPS_UART_RBR_THR_DLL_OFFSET 0x0U
#define TRU_HPS_UART_LSR_OFFSET         0x14U
#define TRU_HPS_UART_SFE_OFFSET         0x98U
#define TRU_HPS_UART_STET_OFFSET        0xa0U
#define TRU_HPS_UART_LSR_TEMT_SET_MSK   0x00000040UL
#define TRU_HPS_UART_LSR_THRE_SET_MSK   0x00000020UL

// HPS UART0 registers
#define TRU_HPS_UART0_BASE              0xffc02000UL
#define TRU_HPS_UART0_RBR_THR_DLL_ADDR  (TRU_HPS_UART0_BASE + TRU_HPS_UART_RBR_THR_DLL_OFFSET)
#define TRU_HPS_UART0_LSR_ADDR          (TRU_HPS_UART0_BASE + TRU_HPS_UART_LSR_OFFSET)
#define TRU_HPS_UART0_SFE_ADDR          (TRU_HPS_UART0_BASE + TRU_HPS_UART_SFE_OFFSET)
#define TRU_HPS_UART0_STET_ADDR         (TRU_HPS_UART0_BASE + TRU_HPS_UART_STET_OFFSET)

// HPS UART1 registers
#define TRU_HPS_UART1_BASE              0xffc03000UL
#define TRU_HPS_UART1_RBR_THR_DLL_ADDR  (TRU_HPS_UART1_BASE + TRU_HPS_UART_RBR_THR_DLL_OFFSET)
#define TRU_HPS_UART1_LSR_ADDR          (TRU_HPS_UART1_BASE + TRU_HPS_UART_LSR_OFFSET)
#define TRU_HPS_UART1_SFE_ADDR          (TRU_HPS_UART1_BASE + TRU_HPS_UART_SFE_OFFSET)
#define TRU_HPS_UART1_STET_ADDR         (TRU_HPS_UART1_BASE + TRU_HPS_UART_STET_OFFSET)

typedef struct{
	volatile uint32_t rbr_thr_dll;
	volatile uint32_t ier_dlh;
	volatile uint32_t iir_fcr;  // Has dual functionality: read = iir, write = fcr
	volatile uint32_t lcr;
	volatile uint32_t mcr;
	volatile uint32_t lsr;
	volatile uint32_t msr;
	volatile uint32_t scr;
	volatile uint32_t reserved[4];
	volatile uint32_t srbr;
	volatile uint32_t sthr;
	volatile uint32_t reserved2[14];
	volatile uint32_t far;
	volatile uint32_t tfr;
	volatile uint32_t RFW;
	volatile uint32_t usr;
	volatile uint32_t tfl;
	volatile uint32_t rfl;
	volatile uint32_t srr;
	volatile uint32_t srts;
	volatile uint32_t sbcr;
	volatile uint32_t sdmam;
	volatile uint32_t sfe;
	volatile uint32_t srt;
	volatile uint32_t stet;
	volatile uint32_t htx;
	volatile uint32_t dmasa;
	volatile uint32_t reserved3[18];
	volatile uint32_t cpr;
	volatile uint32_t ucv;
	volatile uint32_t ctr;
}tru_hps_uart_t;

// UART registers as type representation
#define TRU_HPS_UART0_REG ((tru_hps_uart_t *)TRU_HPS_UART0_BASE)
#define TRU_HPS_UART1_REG ((tru_hps_uart_t *)TRU_HPS_UART1_BASE)
#define TRU_HPS_UART_REG(base_addr) ((tru_hps_uart_t *)base_addr)

void tru_hps_uart_ll_wait_empty(TRU_TARGET_TYPE *uart_base);
void tru_hps_uart_ll_write_str(TRU_TARGET_TYPE *uart_base, const char *str, uint32_t len);
void tru_hps_uart_ll_write_char(TRU_TARGET_TYPE *uart_base, const char c);
void tru_hps_uart_ll_write_hex_nibble(TRU_TARGET_TYPE *uart_base, unsigned char nibble);
void tru_hps_uart_ll_write_inthex(TRU_TARGET_TYPE *uart_base, int num, unsigned int bits);

#endif

#endif
