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

	Version: 20260208

	UART controller (Synopsys UART controller) low-level support for Cyclone V
	SoC HPS.
*/

#ifndef TRU_UART_C5SOC_H
#define TRU_UART_C5SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include <stdint.h>

#define TRU_HPS_UART_RBR_THR_DLL_OFFSET 0x0
#define TRU_HPS_UART_LSR_OFFSET         0x14
#define TRU_HPS_UART_SFE_OFFSET         0x98
#define TRU_HPS_UART_STET_OFFSET        0xa0

#define TRU_HPS_UART_LSR_DR_SETMSK      (1 << 0)
#define TRU_HPS_UART_LSR_TE_SETMSK      (1 << 6)
#define TRU_HPS_UART_LSR_THRE_SETMSK    (1 << 5)

#define TRU_HPS_UART_LCR_DLS_SETMSK 0x03  // Data Length Select mask
#define TRU_HPS_UART_LCR_DLS_5      0x00  // 5 bit character length
#define TRU_HPS_UART_LCR_DLS_6      0x01  // 6 bit character length
#define TRU_HPS_UART_LCR_DLS_7      0x02  // 7 bit character length
#define TRU_HPS_UART_LCR_DLS_8      0x03  // 8 bit character length
#define TRU_HPS_UART_LCR_STB        0x04  // Number of STop Bits, off=1, on=1.5 or 2)
#define TRU_HPS_UART_LCR_PEN        0x08  // Parity ENeble
#define TRU_HPS_UART_LCR_EPS        0x10  // Even Parity Select
#define TRU_HPS_UART_LCR_STKP       0x20  // STicK Parity
#define TRU_HPS_UART_LCR_BCB        0x40  // Break Control Bit
#define TRU_HPS_UART_LCR_DLAB       0x80  // Divisor Latch Access Bit

// HPS UART0 registers
#define TRU_HPS_UART0_BASE              0xffc02000
#define TRU_HPS_UART0_RBR_THR_DLL_ADDR  (TRU_HPS_UART0_BASE + TRU_HPS_UART_RBR_THR_DLL_OFFSET)
#define TRU_HPS_UART0_LSR_ADDR          (TRU_HPS_UART0_BASE + TRU_HPS_UART_LSR_OFFSET)
#define TRU_HPS_UART0_SFE_ADDR          (TRU_HPS_UART0_BASE + TRU_HPS_UART_SFE_OFFSET)
#define TRU_HPS_UART0_STET_ADDR         (TRU_HPS_UART0_BASE + TRU_HPS_UART_STET_OFFSET)

// HPS UART1 registers
#define TRU_HPS_UART1_BASE              0xffc03000
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
}tru_hps_uart_reg_t;

// UART registers as type representation
#define TRU_HPS_UART0_REG ((volatile tru_hps_uart_reg_t *const)TRU_HPS_UART0_BASE)
#define TRU_HPS_UART1_REG ((volatile tru_hps_uart_reg_t *const)TRU_HPS_UART1_BASE)
#define TRU_HPS_UART_REG(base_addr) ((volatile tru_hps_uart_reg_t *const)base_addr)

void tru_hps_uart_init(uintptr_t uart_base, uint32_t baudrate);
void tru_hps_uart_wait_empty(uintptr_t uart_base);
void tru_hps_uart_wait_ready(uintptr_t uart_base, char fifo_th_en);
void tru_hps_uart_wait_data_avail(uintptr_t uart_base);
int tru_hps_uart_getc(uintptr_t uart_base);
void tru_hps_uart_putc(uintptr_t uart_base, uint8_t ch);
void tru_hps_uart_puts(uintptr_t uart_base, char *str);
void tru_hps_uart_putns(uintptr_t uart_base, const char *str, uint32_t len);
void tru_hps_uart_put_nibhex(uintptr_t uart_base, unsigned char nibble);
void tru_hps_uart_put_inthex(uintptr_t uart_base, int num, unsigned int bits);

#endif

#ifdef __cplusplus
}
#endif

#endif
