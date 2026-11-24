/*
	MIT License

	Copyright (c) 2026 Truong Hy

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

	Version: 20260607

	USB 2.0 OTG controller low-level support for Milk V Duo 256M (SG2002).
	This is actually a Synopsys DWC2 USB 2.0 OTG controller.
 */

#ifndef TRU_DWC2_SG2002_H
#define TRU_DWC2_SG2002_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_SG2002

#include "sg2002/system.h"

// ====================
// Hardware limitations
// ====================

#define TRU_DWC2_DEV_MAX_TXFIFO_PACKETS 7

// ========
// Hardware
// ========

#define TRU_USB_U0_BASE USB_BASE

#endif

#ifdef __cplusplus
}
#endif

#endif
