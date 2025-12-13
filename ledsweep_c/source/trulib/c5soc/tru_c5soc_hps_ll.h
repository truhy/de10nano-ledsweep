/*
	MIT License

	Copyright (c) 2024 Truong Hy

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

	Version: 20251206

	Low-level code for Cyclone V SoC HPS.
*/

#ifndef TRU_C5SOC_HPS_LL_H
#define TRU_C5SOC_HPS_LL_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_TARGET_C5SOC)

#define TRU_HPS_OCRAM_BASE   0xffff0000UL  // 64kB On-Chip RAM
#define TRU_HPS_PERI_BASE    0xfffec000UL
#define TRU_HPS_L2C310_BASE  0xfffef000UL
#define TRU_HPS_BOOTROM_BASE 0xfffd0000UL
#define TRU_HPS_PERI_L3_BASE 0xff400000UL
#define TRU_HPS_L2F_BASE     0xff200000UL
#define TRU_HPS_DAP_BASE     0xff000000UL
#define TRU_HPS_STM_BASE     0xfc000000UL
#define TRU_HPS_H2F_BASE     0xc0000000UL
#define TRU_HPS_RAM_BASE     0x00000000UL

// Cyclone V SoC L2 cache latency (vendor specific)
#define TRU_HPS_L2C310_TAGRAM_LATENCY  0x0U
#define TRU_HPS_L2C310_DATARAM_LATENCY 0x10U

#endif

#endif
