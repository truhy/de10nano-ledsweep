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

	Version: 20260208

	System low-level support for Cyclone V SoC HPS.
*/

#ifndef TRU_HPS_C5SOC_H
#define TRU_HPS_C5SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#define TRU_HPS_OCRAM_BASE   0xffff0000  // 64kB On-Chip RAM
#define TRU_HPS_PERI_BASE    0xfffec000  // Arm peripheral base
#define TRU_HPS_L2C310_BASE  0xfffef000
#define TRU_HPS_BOOTROM_BASE 0xfffd0000
#define TRU_HPS_PERI_L3_BASE 0xff400000  // Cyclone V SoC peripheral base
#define TRU_HPS_L2F_BASE     0xff200000
#define TRU_HPS_DAP_BASE     0xff000000
#define TRU_HPS_STM_BASE     0xfc000000
#define TRU_HPS_H2F_BASE     0xc0000000
#define TRU_HPS_RAM_BASE     0x00000000

// Cyclone V SoC L2 cache latency (vendor specific)
#define TRU_HPS_L2C310_TAGRAM_LATENCY  0x0
#define TRU_HPS_L2C310_DATARAM_LATENCY 0x10

// Map HPS to Arm defines
#define TRU_PERIPH_BASE            TRU_HPS_PERI_BASE
#define TRU_L2C310_BASE            TRU_HPS_L2C310_BASE
#define TRU_L2C310_TAGRAM_LATENCY  TRU_HPS_L2C310_TAGRAM_LATENCY
#define TRU_L2C310_DATARAM_LATENCY TRU_HPS_L2C310_DATARAM_LATENCY

#endif

#ifdef __cplusplus
}
#endif

#endif
