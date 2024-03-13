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

	Version: 20240203

	Trulib configuration
 */

#ifndef TRU_CONFIG_H
#define TRU_CONFIG_H

// ======================
// General configurations
// ======================

#ifndef TRU_TARGET
	#ifdef CYCLONEV
		#define TRU_TARGET TRU_C5SOC
	#endif
#endif

#ifndef TRU_USE_CMSIS
	#define TRU_USE_CMSIS 1U
#endif

#ifndef TRU_USE_STARTUP
#if(TRU_USE_CMSIS == 0U)
	#define TRU_USE_STARTUP 1U
#else
	#define TRU_USE_STARTUP 0U
#endif
#endif

#ifdef DEBUG
	#ifndef TRU_PRINTF_UART
		#define TRU_PRINTF_UART 1U
	#endif
#else
	#ifndef TRU_PRINTF_UART
		#define TRU_PRINTF_UART 0U
	#endif
#endif

// ======================
// Startup configurations
// ======================

// Settings:
//   L1_CACHE_ENABLE: 0 = disable L1 cache, 1 = disable, invalidate and enable L1 cache, 2 = do nothing
//   L2_CACHE_ENABLE: 0 = disable L2 cache, 1 = disable, invalidate and enable L2 cache, 2 = do nothing

#ifndef TRU_EXIT_TO_UBOOT
	#define TRU_EXIT_TO_UBOOT 0U
#endif

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
