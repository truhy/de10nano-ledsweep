/*
	MIT License

	Copyright (c) 2022 Truong Hy

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

	USB 2.0 OTG controller low-level support for Altera Cyclone V SOC HPS.
	This is actually a Synopsys DWC2 USB 2.0 OTG controller.
 */

#ifndef TRU_USB_DWC2_C5SOC_H
#define TRU_USB_DWC2_C5SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include "c5soc/tru_rstmgr_c5soc.h"

// ====================
// Hardware limitations
// ====================

// There is a bug in Altera Cyclone V SoC DWC2 version 2.93a, the minimum RX FIFO is 119 words, i.e. 4*119 = 476 bytes
// In USB peripheral device mode, setting the RX FIFO size less than this will cause enumeration to fail, specifically the EP0 status out (ZLP) never completes, i.e. keeps NAKing and no transfer complete interrupt
#define TRU_DWC2_DEV_MIN_RXFIFO_WORDSIZE 119

#define TRU_DWC2_DEV_MAX_TXFIFO_PACKETS 7

// ========
// Hardware
// ========

#define TRU_USB_U0_BASE 0xffb00000  // USB0 controller
#define TRU_USB_U1_BASE 0xffb40000  // USB1 controller
#define TRU_USB_U0_IRQ 157
#define TRU_USB_U1_IRQ 160

#endif

#ifdef __cplusplus
}
#endif

#endif
