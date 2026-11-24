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

	USB 2.0 OTG controller low-level support.
	A central file that includes all manufacturer port headers and defines.
 */

#ifndef TRU_DWC2_PORT_H
#define TRU_DWC2_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_dwc2_c5soc.h"
#include "tru_dwc2_stm32h7.h"
#include "tru_dwc2_sg2002.h"

// ====================
// Hardware limitations
// ====================

// Manufacturer default defines.  You can override these defines by defining it in the manufacturer port file

#if !defined(TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED)
	#define TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED 1
#endif

// ============
// User options
// ============

// A FIFO slave mode setting for OUTDONE & SETUPDONE status: 1 = handle in the GRXFLVL (receive full level) interrupt, 0 = handle in XFRCOMPL interrupt
#if !defined(TRU_DWC2_PC_HANDLE_DONE_IN_RXLVL)
	#define TRU_DWC2_PC_HANDLE_DONE_IN_RXLVL 0
#endif

// Update dxepctl register oddeven bit method: 0 = by eonum register bit, 1 = by frame number
#if !defined(TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN)
	#define TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN 0
#endif

#ifdef __cplusplus
}
#endif

#endif
