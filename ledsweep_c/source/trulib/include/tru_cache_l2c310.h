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

	Version: 20250119

	Arm CoreLink™ Level 2 Cache Controller L2C-310.
*/

#ifndef TRU_CAHCE_L2C310_H
#define TRU_CAHCE_L2C310_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_TARGET_C5SOC)

#define L2C310_BASE                 0xfffef000UL
#define L2C310_CTRL_OFFSET          0x100U
#define L2C310_AUX_CTRL_OFFSET      0x104U
#define L2C310_TAGRAM_OFFSET        0x108U
#define L2C310_DATARAM_OFFSET       0x10cU
#define L2C310_INT_CLR_OFFSET       0x220U
#define L2C310_CACHE_SYNC_OFFSET    0x730U
#define L2C310_INV_PA_OFFSET        0x770U
#define L2C310_CLEAN_PA_OFFSET      0x7b0U
#define L2C310_CLEANINV_PA_OFFSET   0x7f0U
#define L2C310_D_LOCKDN0_OFFSET     0x900U
#define L2C310_DBG_CTRL_OFFSET      0xf40U
#define L2C310_PREFETCH_CTRL_OFFSET 0xf60U

#define L2C310_CACHELINE_SIZE 32U

// Cyclone V SoC latency (vendor specific)
#define L2C310_TAGRAM_LATENCY  0x0U
#define L2C310_DATARAM_LATENCY 0x10U

#endif

#endif
