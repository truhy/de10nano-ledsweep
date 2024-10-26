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

	Version: 20241021

	Trulib configuration
*/

#ifndef TRU_CONFIG_H
#define TRU_CONFIG_H

// ==============
// Config options
// ==============

#define TRU_CFG_TARGET          TRU_C5SOC
#define TRU_CFG_CMSIS           1U
#define TRU_CFG_CMSIS_WEAK_IRQH 0U
#define TRU_CFG_STARTUP         0U
#define TRU_CFG_EXIT_TO_UBOOT   0U
#define TRU_CFG_NEON            1U
#define TRU_CFG_PRINT_UART      1U
#define TRU_CFG_LOG             1U
#define TRU_CFG_LOG_RN          1U
#define TRU_CFG_LOG_LOC         0U

// ==============================================
// Apply config to options if not already defined
// ==============================================

#ifdef CYCLONEV
	#define TRU_TARGET TRU_C5SOC
#else
#ifndef TRU_TARGET
	#define TRU_TARGET TRU_CFG_TARGET
#endif
#endif

#ifndef TRU_CMSIS
	#define TRU_CMSIS TRU_CFG_CMSIS
#endif

#ifndef TRU_CMSIS_WEAK_IRQH
	#define TRU_CMSIS_WEAK_IRQH TRU_CFG_CMSIS_WEAK_IRQH
#endif

#ifndef TRU_STARTUP
	#define TRU_STARTUP TRU_CFG_STARTUP
#endif

#ifndef TRU_EXIT_TO_UBOOT
	#define TRU_EXIT_TO_UBOOT TRU_CFG_EXIT_TO_UBOOT
#endif

#ifdef SEMIHOSTING
	#define TRU_PRINT_UART 0U
#endif

#ifndef TRU_PRINT_UART
	#define TRU_PRINT_UART TRU_CFG_PRINT_UART
#endif

#ifndef TRU_LOG
	#define TRU_LOG TRU_CFG_LOG
#endif

#ifndef TRU_LOG_RN
	// 1U == Enables insertion of '\r' for each '\n' character
	#define TRU_LOG_RN TRU_CFG_LOG_RN
#endif

#ifndef TRU_LOG_LOC
	#define TRU_LOG_LOC TRU_CFG_LOG_LOC
#endif

// ======================
// Startup configurations
// ======================

// Settings:
//   L1_CACHE_ENABLE: 0 = disable L1 cache, 1 = disable, invalidate and enable L1 cache, 2 = do nothing
//   L2_CACHE_ENABLE: 0 = disable L2 cache, 1 = disable, invalidate and enable L2 cache, 2 = do nothing

#if(TRU_EXIT_TO_UBOOT)
	#include "tru_etu.h"

	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU
			#define TRU_MMU 2U
		#endif
		#ifndef TRU_SMP_COHERENCY
			#define TRU_SMP_COHERENCY 2U
		#endif
		#ifndef TRU_L1_CACHE
			#define TRU_L1_CACHE 2U
		#endif
		#ifndef TRU_L2_CACHE
			#define TRU_L2_CACHE 2U
		#endif
		#ifndef TRU_SCU
			#define TRU_SCU 2U
		#endif
	#else
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU
			#define TRU_MMU 2U
		#endif
		#ifndef TRU_SMP_COHERENCY
			#define TRU_SMP_COHERENCY 2U
		#endif
		#ifndef TRU_L1_CACHE
			#define TRU_L1_CACHE 2U
		#endif
		#ifndef TRU_L2_CACHE
			#define TRU_L2_CACHE 2U
		#endif
		#ifndef TRU_SCU
			#define TRU_SCU 2U
		#endif
	#endif
#else
	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU
			#define TRU_MMU 1U
		#endif
		#ifndef TRU_SMP_COHERENCY
			#define TRU_SMP_COHERENCY 0U
		#endif
		#ifndef TRU_L1_CACHE
			#define TRU_L1_CACHE 0U
		#endif
		#ifndef TRU_L2_CACHE
			#define TRU_L2_CACHE 0U
		#endif
		#ifndef TRU_SCU
			#define TRU_SCU 0U
		#endif
	#else
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU
			#define TRU_MMU 1U
		#endif
		#ifndef TRU_SMP_COHERENCY
			#define TRU_SMP_COHERENCY 1U
		#endif
		#ifndef TRU_L1_CACHE
			#define TRU_L1_CACHE 1U
		#endif
		#ifndef TRU_L2_CACHE
			#define TRU_L2_CACHE 1U
		#endif
		#ifndef TRU_SCU
			#define TRU_SCU 1U
		#endif
	#endif
#endif

// This should match with your compiler/linker flag
#ifndef TRU_NEON
	#define TRU_NEON TRU_CFG_NEON
#endif

#endif
