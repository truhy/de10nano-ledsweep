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

	USB 2.0 OTG controller low-level support for STMicroelectronics STM32H7.
	This is actually a Synopsys DWC2 USB 2.0 OTG controller.
 */

#ifndef TRU_DWC2_STM32H7_H
#define TRU_DWC2_STM32H7_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_STM32H7

// ====================
// Hardware limitations
// ====================

// In slave mode (manual FIFO transfer mode) there is a hardware limitation that appears to be related to in internal
// fifo queue counter. Only a maximum of n packets can be pushed into the TXFIFO at once (i.e. together with DIEPTSIZx).
// This becomes a problem in full-speed mode because the speed is slow enough where host is not pulling the data off
// fast enough from the queue.  In high-speed mode we don't normally see a problem because it is fast.
// Note, this does not apply to DMA modes
// Some STM32 reference manuals describes an internal counter, for instance see STM32H446RE RM0390, page 1245:
// "Every time a packet is written into the transmit FIFO by the application, the transfer size for that endpoint is
// decremented by the packet size. The data is fetched from the memory by the application, until the transfer size
// for the endpoint becomes 0. After writing the data into the FIFO, the “number of packets in FIFO” count is incremented
// (this is a 3-bit count, internally maintained by the core for each IN endpoint transmit FIFO. The maximum number of
// packets maintained by the core at any time in an IN endpoint FIFO is eight). For zero-length packets, a separate flag
// is set for each FIFO, without any data in the FIFO."
#define TRU_DWC2_DEV_MAX_TXFIFO_PACKETS 7

// ========
// Hardware
// ========

#define TRU_USB_U0_BASE 0x40040000  // STM32H7 USB OTG HS1 (alias USB1 OTG HS) controller is renamed as USB0 controller for convenience
#define TRU_USB_U1_BASE 0x40080000  // STM32H7 USB OTG HS2 (alias USB2 OTG_FS) controller is renamed as USB1 controller for convenience

#endif

#ifdef __cplusplus
}
#endif

#endif
