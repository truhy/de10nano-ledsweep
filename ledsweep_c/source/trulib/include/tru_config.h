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

	Version: 20250405

	Trulib configuration
*/

#ifndef TRU_CONFIG_H
#define TRU_CONFIG_H

#include "tru_options.h"
#include "tru_user_config.h"

// ==================================================
// Apply final config settings if not already defined
// ==================================================

#ifndef TRU_TARGET
	#if defined(TRU_CFG_TARGET)
		#define TRU_TARGET TRU_CFG_TARGET
	#elif defined(soc_cv_av)
		#define TRU_TARGET TRU_TARGET_C5SOC
	#elif defined(STM32H753xx)
		#define TRU_TARGET TRU_TARGET_STM32H7
	#else
		#error "TRU_CFG_TARGET or TRU_TARGET define not set!"
	#endif
#endif

#if TRU_TARGET == TRU_TARGET_C5SOC
	#define TRU_TARGET_TYPE uint32_t
	#define TRU_L1_CACHE_PRESENT      1U
	#define TRU_L2_CACHE_PRESENT      1U
	#define TRU_MMU_PRESENT           1U
	#define TRU_SMP_COHERENCY_PRESENT 1U
	#define TRU_SCU_PRESENT           1U
	#define TRU_NEON_PRESENT          1U
#elif TRU_TARGET == TRU_TARGET_STM32H7
	#define TRU_TARGET_TYPE uint32_t
	#define TRU_L1_CACHE_PRESENT 1U
#else
	#error "TRU_TARGET define has an unsupported value!"
#endif

#ifndef TRU_CPU_FAMILY
	#if defined(TRU_CFG_CPU_FAMILY)
		#define TRU_CPU_FAMILY TRU_CFG_CPU_FAMILY
	#elif(TRU_TARGET == TRU_TARGET_C5SOC)
		#define TRU_CPU_FAMILY TRU_CPU_FAMILY_CORTEXA9
	#elif(TRU_TARGET == TRU_TARGET_STM32H7)
		#define TRU_CPU_FAMILY TRU_CPU_FAMILY_CORTEXM7
	#else
		#error "TRU_CFG_CPU_FAMILY or TRU_CPU_FAMILY define not set!"
	#endif
#endif

// Use CMSIS for startup and CPU stuff
#if !defined(TRU_CMSIS) && defined(TRU_CFG_CMSIS)
	#define TRU_CMSIS TRU_CFG_CMSIS
#endif

// This is to support FreeRTOS with CMSIS, set to 1 when using FreeRTOS, else set to 0
#if !defined(TRU_CMSIS_WEAK_IRQH) && defined(TRU_CFG_CMSIS_WEAK_IRQH)
	#define TRU_CMSIS_WEAK_IRQH TRU_CFG_CMSIS_WEAK_IRQH
#endif

// Use my replacement startup for HWLIB
#if !defined(TRU_STARTUP) && defined(TRU_CFG_STARTUP)
	#define TRU_STARTUP TRU_CFG_STARTUP
#endif

#if !defined(TRU_EXIT_TO_UBOOT) && defined(TRU_CFG_EXIT_TO_UBOOT)
	#define TRU_EXIT_TO_UBOOT TRU_CFG_EXIT_TO_UBOOT
#endif

#ifdef SEMIHOSTING
	#define TRU_PRINT_UART0 0U
	#define TRU_PRINT_UART1 0U
#endif

#if !defined(TRU_PRINT_UART0) && defined(TRU_CFG_PRINT_UART0)
	#define TRU_PRINT_UART0 TRU_CFG_PRINT_UART0
#elif  !defined(TRU_PRINT_UART1) && defined(TRU_CFG_PRINT_UART1)
	#define TRU_PRINT_UART1 TRU_CFG_PRINT_UART1
#endif

#if !defined(TRU_LOG) && defined(TRU_CFG_LOG)
	#define TRU_LOG TRU_CFG_LOG
#endif

#if !defined(TRU_LOG_RN) && defined(TRU_CFG_LOG_RN)
	// 1U == Enables insertion of '\r' for each '\n' character
	#define TRU_LOG_RN TRU_CFG_LOG_RN
#endif

#if !defined(TRU_LOG_LOC) && defined(TRU_CFG_LOG_LOC)
	#define TRU_LOG_LOC TRU_CFG_LOG_LOC
#endif

// Tells this library to use non-cacheable memory region for DMA buffers
#if !defined(TRU_DMA_BUFFER_NONCACHEABLE) && defined(TRU_CFG_DMA_BUFFER_NONCACHEABLE)
	#define TRU_DMA_BUFFER_NONCACHEABLE TRU_CFG_DMA_BUFFER_NONCACHEABLE
#endif

#if !defined(TRU_USB_LOG_INIT) && defined(TRU_CFG_USB_LOG_INIT)
	#define TRU_USB_LOG_INIT TRU_CFG_USB_LOG_INIT
#endif
#if !defined(TRU_USB_LOG_EPENA) && defined(TRU_CFG_USB_LOG_EPENA)
	#define TRU_USB_LOG_EPENA TRU_CFG_USB_LOG_EPENA
#endif
#if !defined(TRU_USB_LOG_INTR) && defined(TRU_CFG_USB_LOG_INTR)
	#define TRU_USB_LOG_INTR TRU_CFG_USB_LOG_INTR
#endif
#if !defined(TRU_USB_LOG_SETUP_BYTES) && defined(TRU_CFG_USB_LOG_SETUP_BYTES)
	#define TRU_USB_LOG_SETUP_BYTES TRU_CFG_USB_LOG_SETUP_BYTES
#endif
#if !defined(TRU_USB_LOG_SETUP_TEXT) && defined(TRU_CFG_USB_LOG_SETUP_TEXT)
	#define TRU_USB_LOG_SETUP_TEXT TRU_CFG_USB_LOG_SETUP_TEXT
#endif
#if !defined(TRU_USB_LOG_XPROGRESS) && defined(TRU_CFG_USB_LOG_XPROGRESS)
	#define TRU_USB_LOG_XPROGRESS TRU_CFG_USB_LOG_XPROGRESS
#endif
#if !defined(TRU_USB_LOG_DIEPTSIZ) && defined(TRU_CFG_USB_LOG_DIEPTSIZ)
	#define TRU_USB_LOG_DIEPTSIZ TRU_CFG_USB_LOG_DIEPTSIZ
#endif
#if !defined(TRU_USB_LOG_DOEPTSIZ) && defined(TRU_CFG_USB_LOG_DOEPTSIZ)
	#define TRU_USB_LOG_DOEPTSIZ TRU_CFG_USB_LOG_DOEPTSIZ
#endif
#if !defined(TRU_USB_LOG_CALLBACK) && defined(TRU_CFG_USB_LOG_CALLBACK)
	#define TRU_USB_LOG_CALLBACK TRU_CFG_USB_LOG_CALLBACK
#endif

// ======================
// Startup configurations
// ======================

// Settings:
//   L1_CACHE_ENABLE: 0 = disable L1 cache, 1 = disable, invalidate and enable L1 cache, 2 = do nothing
//   L2_CACHE_ENABLE: 0 = disable L2 cache, 1 = disable, invalidate and enable L2 cache, 2 = do nothing

#if defined(TRU_EXIT_TO_UBOOT) && TRU_EXIT_TO_UBOOT == 1U
	#include "tru_etu.h"

	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#if (defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U) || (defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U)
			#if !defined(TRU_CLEAN_CACHE) && defined(TRU_CFG_CLEAN_CACHE)
				#define TRU_CLEAN_CACHE TRU_CFG_CLEAN_CACHE
			#else
				#define TRU_CLEAN_CACHE 0U
			#endif
		#endif
		#if defined(TRU_MMU_PRESENT) && TRU_MMU_PRESENT == 1U
			#if !defined(TRU_MMU) && defined(TRU_CFG_MMU)
				#define TRU_MMU TRU_CFG_MMU
			#else
				#define TRU_MMU 2U
			#endif
		#endif
		#if defined(TRU_SMP_COHERENCY_PRESENT) && TRU_SMP_COHERENCY_PRESENT == 1U
			#if !defined(TRU_SMP_COHERENCY) && defined(TRU_CFG_SMP_COHERENCY)
				#define TRU_SMP_COHERENCY TRU_CFG_SMP_COHERENCY
			#else
				#define TRU_SMP_COHERENCY 2U
			#endif
		#endif
		#if defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U
			#if !defined(TRU_L1_CACHE) && defined(TRU_CFG_L1_CACHE)
				#define TRU_L1_CACHE TRU_CFG_L1_CACHE
			#else
				#define TRU_L1_CACHE 2U
			#endif
		#endif
		#if defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U
			#if !defined(TRU_L2_CACHE) && defined(TRU_CFG_L2_CACHE)
				#define TRU_L2_CACHE TRU_CFG_L2_CACHE
			#else
				#define TRU_L2_CACHE 2U
			#endif
		#endif
		#if defined(TRU_SCU_PRESENT) && TRU_SCU_PRESENT == 1U
			#if !defined(TRU_SCU) && defined(TRU_CFG_SCU)
				#define TRU_SCU TRU_CFG_SCU
			#else
				#define TRU_SCU 2U
			#endif
		#endif
	#else
		#if (defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U) || (defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U)
			#if !defined(TRU_CLEAN_CACHE) && defined(TRU_CFG_CLEAN_CACHE)
				#define TRU_CLEAN_CACHE TRU_CFG_CLEAN_CACHE
			#else
				#define TRU_CLEAN_CACHE 0U
			#endif
		#endif
		#if defined(TRU_MMU_PRESENT) && TRU_MMU_PRESENT == 1U
			#if !defined(TRU_MMU) && defined(TRU_CFG_MMU)
				#define TRU_MMU TRU_CFG_MMU
			#else
				#define TRU_MMU 2U
			#endif
		#endif
		#if defined(TRU_SMP_COHERENCY_PRESENT) && TRU_SMP_COHERENCY_PRESENT == 1U
			#if !defined(TRU_SMP_COHERENCY) && defined(TRU_CFG_SMP_COHERENCY)
				#define TRU_SMP_COHERENCY TRU_CFG_SMP_COHERENCY
			#else
				#define TRU_SMP_COHERENCY 2U
			#endif
		#endif
		#if defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U
			#if !defined(TRU_L1_CACHE) && defined(TRU_CFG_L1_CACHE)
				#define TRU_L1_CACHE TRU_CFG_L1_CACHE
			#else
				#define TRU_L1_CACHE 2U
			#endif
		#endif
		#if defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U
			#if !defined(TRU_L2_CACHE) && defined(TRU_CFG_L2_CACHE)
				#define TRU_L2_CACHE TRU_CFG_L2_CACHE
			#else
				#define TRU_L2_CACHE 2U
			#endif
		#endif
		#if defined(TRU_SCU_PRESENT) && TRU_SCU_PRESENT == 1U
			#if !defined(TRU_SCU) && defined(TRU_CFG_SCU)
				#define TRU_SCU TRU_CFG_SCU
			#else
				#define TRU_SCU 2U
			#endif
		#endif
	#endif
#else
	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#if (defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U) || (defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U)
			#if !defined(TRU_CLEAN_CACHE) && defined(TRU_CFG_CLEAN_CACHE)
				#define TRU_CLEAN_CACHE TRU_CFG_CLEAN_CACHE
			#else
				#define TRU_CLEAN_CACHE 0U
			#endif
		#endif
		#if defined(TRU_MMU_PRESENT) && TRU_MMU_PRESENT == 1U
			#if !defined(TRU_MMU) && defined(TRU_CFG_MMU)
				#define TRU_MMU TRU_CFG_MMU
			#else
				#define TRU_MMU 1U
			#endif
		#endif
		#if defined(TRU_SMP_COHERENCY_PRESENT) && TRU_SMP_COHERENCY_PRESENT == 1U
			#if !defined(TRU_SMP_COHERENCY) && defined(TRU_CFG_SMP_COHERENCY)
				#define TRU_SMP_COHERENCY TRU_CFG_SMP_COHERENCY
			#else
				#define TRU_SMP_COHERENCY 0U
			#endif
		#endif
		#if defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U
			#if !defined(TRU_L1_CACHE) && defined(TRU_CFG_L1_CACHE)
				#define TRU_L1_CACHE TRU_CFG_L1_CACHE
			#else
				#define TRU_L1_CACHE 0U
			#endif
		#endif
		#if defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U
			#if !defined(TRU_L2_CACHE) && defined(TRU_CFG_L2_CACHE)
				#define TRU_L2_CACHE TRU_CFG_L2_CACHE
			#else
				#define TRU_L2_CACHE 0U
			#endif
		#endif
		#if defined(TRU_SCU_PRESENT) && TRU_SCU_PRESENT == 1U
			#if !defined(TRU_SCU) && defined(TRU_CFG_SCU)
				#define TRU_SCU TRU_CFG_SCU
			#else
				#define TRU_SCU 0U
			#endif
		#endif
	#else
		#if (defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U) || (defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U)
			#if !defined(TRU_CLEAN_CACHE) && defined(TRU_CFG_CLEAN_CACHE)
				#define TRU_CLEAN_CACHE TRU_CFG_CLEAN_CACHE
			#else
				#define TRU_CLEAN_CACHE 0U
			#endif
		#endif
		#if defined(TRU_MMU_PRESENT) && TRU_MMU_PRESENT == 1U
			#if !defined(TRU_MMU) && defined(TRU_CFG_MMU)
				#define TRU_MMU TRU_CFG_MMU
			#else
				#define TRU_MMU 1U
			#endif
		#endif
		#if defined(TRU_SMP_COHERENCY_PRESENT) && TRU_SMP_COHERENCY_PRESENT == 1U
			#if !defined(TRU_SMP_COHERENCY) && defined(TRU_CFG_SMP_COHERENCY)
				#define TRU_SMP_COHERENCY TRU_CFG_SMP_COHERENCY
			#else
				#define TRU_SMP_COHERENCY 1U
			#endif
		#endif
		#if defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT == 1U
			#if !defined(TRU_L1_CACHE) && defined(TRU_CFG_L1_CACHE)
				#define TRU_L1_CACHE TRU_CFG_L1_CACHE
			#else
				#define TRU_L1_CACHE 1U
			#endif
		#endif
		#if defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT == 1U
			#if !defined(TRU_L2_CACHE) && defined(TRU_CFG_L2_CACHE)
				#define TRU_L2_CACHE TRU_CFG_L2_CACHE
			#else
				#define TRU_L2_CACHE 1U
			#endif
		#endif
		#if defined(TRU_SCU_PRESENT) && TRU_SCU_PRESENT == 1U
			#if !defined(TRU_SCU) && defined(TRU_CFG_SCU)
				#define TRU_SCU TRU_CFG_SCU
			#else
				#define TRU_SCU 1U
			#endif
		#endif
	#endif
#endif

// This should match with your compiler/linker flag
#if defined(TRU_NEON_PRESENT) && TRU_NEON_PRESENT == 1U
	#if !defined(TRU_NEON) && defined(TRU_CFG_NEON)
		#define TRU_NEON TRU_CFG_NEON
	#endif
#endif

// Indicates unaligned byte access is supported
#if !defined(TRU_UNALIGNED_ACCESS) && defined(TRU_CFG_UNALIGNED_ACCESS)
	#define TRU_UNALIGNED_ACCESS TRU_CFG_UNALIGNED_ACCESS
#endif

#endif
