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

	Arm CoreLink™ Level 2 Cache Controller L2C-310 registers.
*/

#ifndef TRU_CACHE_L2C310_H
#define TRU_CACHE_L2C310_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CPU) && TRU_CFG_CPU == TRU_OPT_CPU_CORTEXA9

#define TRU_L2C310_CTRL_OFFSET          0x100
#define TRU_L2C310_AUX_CTRL_OFFSET      0x104
#define TRU_L2C310_TAGRAM_OFFSET        0x108
#define TRU_L2C310_DATARAM_OFFSET       0x10c
#define TRU_L2C310_INT_CLR_OFFSET       0x220
#define TRU_L2C310_CACHE_SYNC_OFFSET    0x730
#define TRU_L2C310_INV_PA_OFFSET        0x770
#define TRU_L2C310_CLEAN_PA_OFFSET      0x7b0
#define TRU_L2C310_CLEANINV_PA_OFFSET   0x7f0
#define TRU_L2C310_D_LOCKDN0_OFFSET     0x900
#define TRU_L2C310_DBG_CTRL_OFFSET      0xf40
#define TRU_L2C310_PREFETCH_CTRL_OFFSET 0xf60

#define TRU_L2C310_CACHELINE_SIZE 32

#endif

#ifdef __cplusplus
}
#endif

#endif
