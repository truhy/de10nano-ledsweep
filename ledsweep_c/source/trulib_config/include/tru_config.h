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

	Version: 20240316

	Trulib configuration
 */

#ifndef TRU_CONFIG_H
#define TRU_CONFIG_H
// =============
// User settings
// =============

#define TRU_USER_TARGET            TRU_C5SOC
#define TRU_USER_USE_CMSIS         1U
#define TRU_USER_STARTUP           0U
#define TRU_USER_EXIT_TO_UBOOT     0U
#define TRU_USER_DEBUG_PRINT_LEVEL 1U
#define TRU_USER_PRINTF_UART       1U
#define TRU_USER_DEBUG_PRINT_R_NL  1U
#define TRU_USER_NEON_ENABLE       1U

// ===============================
// Apply user or override settings
// ===============================

#ifdef CYCLONEV
	#define TRU_TARGET TRU_C5SOC
#else
#ifndef TRU_TARGET
	#define TRU_TARGET TRU_USER_TARGET
#endif
#endif

#ifndef TRU_USE_CMSIS
	#define TRU_USE_CMSIS TRU_USER_USE_CMSIS
#endif

#ifndef TRU_STARTUP
	#define TRU_STARTUP TRU_USER_STARTUP
#endif

#ifndef TRU_EXIT_TO_UBOOT
	#define TRU_EXIT_TO_UBOOT TRU_USER_EXIT_TO_UBOOT
#endif

#ifndef TRU_DEBUG_PRINT_LEVEL
	#define TRU_DEBUG_PRINT_LEVEL TRU_USER_DEBUG_PRINT_LEVEL
#endif

#ifndef TRU_PRINTF_UART
	#define TRU_PRINTF_UART TRU_USER_PRINTF_UART
#endif

#ifdef SEMIHOSTING
	#define TRU_DEBUG_PRINT_R_NL 0U
#else
	// 1U == Enables insertion of '\r' for each '\n' character
	#define TRU_DEBUG_PRINT_R_NL TRU_USER_DEBUG_PRINT_R_NL
#endif

// ======================
// Startup configurations
// ======================

// Settings:
//   L1_CACHE_ENABLE: 0 = disable L1 cache, 1 = disable, invalidate and enable L1 cache, 2 = do nothing
//   L2_CACHE_ENABLE: 0 = disable L2 cache, 1 = disable, invalidate and enable L2 cache, 2 = do nothing

#if(TRU_EXIT_TO_UBOOT)
	#include "etu.h"

	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU_ENABLE
			#define TRU_MMU_ENABLE 2U
		#endif
		#ifndef TRU_SMP_COHERENCY_ENABLE
			#define TRU_SMP_COHERENCY_ENABLE 2U
		#endif
		#ifndef TRU_L1_CACHE_ENABLE
			#define TRU_L1_CACHE_ENABLE 2U
		#endif
		#ifndef TRU_L2_CACHE_ENABLE
			#define TRU_L2_CACHE_ENABLE 2U
		#endif
		#ifndef TRU_SCU_ENABLE
			#define TRU_SCU_ENABLE 2U
		#endif
	#else
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU_ENABLE
			#define TRU_MMU_ENABLE 2U
		#endif
		#ifndef TRU_SMP_COHERENCY_ENABLE
			#define TRU_SMP_COHERENCY_ENABLE 2U
		#endif
		#ifndef TRU_L1_CACHE_ENABLE
			#define TRU_L1_CACHE_ENABLE 2U
		#endif
		#ifndef TRU_L2_CACHE_ENABLE
			#define TRU_L2_CACHE_ENABLE 2U
		#endif
		#ifndef TRU_SCU_ENABLE
			#define TRU_SCU_ENABLE 2U
		#endif
	#endif
#else
	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU_ENABLE
			#define TRU_MMU_ENABLE 1U
		#endif
		#ifndef TRU_SMP_COHERENCY_ENABLE
			#define TRU_SMP_COHERENCY_ENABLE 0U
		#endif
		#ifndef TRU_L1_CACHE_ENABLE
			#define TRU_L1_CACHE_ENABLE 0U
		#endif
		#ifndef TRU_L2_CACHE_ENABLE
			#define TRU_L2_CACHE_ENABLE 0U
		#endif
		#ifndef TRU_SCU_ENABLE
			#define TRU_SCU_ENABLE 0U
		#endif
	#else
		#ifndef TRU_CLEAN_CACHE
			#define TRU_CLEAN_CACHE 1U
		#endif
		#ifndef TRU_MMU_ENABLE
			#define TRU_MMU_ENABLE 1U
		#endif
		#ifndef TRU_SMP_COHERENCY_ENABLE
			#define TRU_SMP_COHERENCY_ENABLE 1U
		#endif
		#ifndef TRU_L1_CACHE_ENABLE
			#define TRU_L1_CACHE_ENABLE 1U
		#endif
		#ifndef TRU_L2_CACHE_ENABLE
			#define TRU_L2_CACHE_ENABLE 1U
		#endif
		#ifndef TRU_SCU_ENABLE
			#define TRU_SCU_ENABLE 1U
		#endif
	#endif
#endif

// This should match with your compiler/linker flag
#ifndef TRU_NEON_ENABLE
	#define TRU_NEON_ENABLE 1U
#endif

#endif
