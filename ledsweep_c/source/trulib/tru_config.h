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

	Version: 20260707

	Trulib final configuration.
*/

#ifndef TRU_CONFIG_H
#define TRU_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_options.h"
#include "tru_user_config.h"

// ==================================================
// Apply final config settings if not already defined
// ==================================================

#ifndef TRU_CFG_BOARD
	#if defined(TRU_UCFG_BOARD)
		#define TRU_CFG_BOARD TRU_UCFG_BOARD
	#else
		#define TRU_CFG_BOARD TRU_OPT_BOARD_CUSTOM
	#endif
#endif

#if TRU_CFG_BOARD == TRU_OPT_BOARD_C5SOC_CUSTOM
	#if !defined(TRU_CFG_CHIPSET)
		#define TRU_CFG_CHIPSET TRU_OPT_CHIPSET_C5SOC
	#endif
	#if !defined(TRU_CFG_CPU)
		#define TRU_CFG_CPU TRU_OPT_CPU_CORTEXA9
	#endif
#elif TRU_CFG_BOARD == TRU_OPT_BOARD_DE10NANO
	#if !defined(TRU_CFG_CHIPSET)
		#define TRU_CFG_CHIPSET TRU_OPT_CHIPSET_C5SOC
	#endif
	#if !defined(TRU_CFG_CPU)
		#define TRU_CFG_CPU TRU_OPT_CPU_CORTEXA9
	#endif
#elif TRU_CFG_BOARD == TRU_OPT_BOARD_STM32H7_CUSTOM
	#if !defined(TRU_CFG_CHIPSET)
		#define TRU_CFG_CHIPSET TRU_OPT_CHIPSET_STM32H7
	#endif
#elif TRU_CFG_BOARD == TRU_OPT_BOARD_NUCLEO144_753ZI
	#if !defined(TRU_CFG_CHIPSET)
		#define TRU_CFG_CHIPSET TRU_OPT_CHIPSET_STM32H7
	#endif
	#if !defined(TRU_CFG_CPU)
		#define TRU_CFG_CPU TRU_OPT_CPU_CORTEXM7
	#endif
#elif TRU_CFG_BOARD == TRU_OPT_BOARD_MILKV_DUO256M
	#if !defined(TRU_CFG_CHIPSET)
		#define TRU_CFG_CHIPSET TRU_OPT_CHIPSET_SG2002
	#endif
#endif

#ifndef TRU_CFG_CHIPSET
	#if defined(TRU_UCFG_CHIPSET)
		#define TRU_CFG_CHIPSET TRU_UCFG_CHIPSET
	#elif defined(soc_cv_av)
		#define TRU_CFG_CHIPSET TRU_OPT_CHIPSET_C5SOC
	#elif defined(STM32H753xx)
		#define TRU_CFG_CHIPSET TRU_OPT_CHIPSET_STM32H7
	#else
		#error "TRU_UCFG_CHIPSET or TRU_CFG_CHIPSET define not set!"
	#endif
#endif

#ifndef TRU_CFG_CPU
	#if defined(TRU_UCFG_CPU)
		#define TRU_CFG_CPU TRU_UCFG_CPU
	#else
		#error "TRU_UCFG_CPU or TRU_CFG_CPU define not set!"
	#endif
#endif

#if TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC
	#define TRU_CFG_L1C_PRESENT           1
	#define TRU_CFG_L2C_PRESENT           1
	#define TRU_CFG_MMU_PRESENT           1
	#define TRU_CFG_SMP_COHERENCY_PRESENT 1
	#define TRU_CFG_SCU_PRESENT           1
#elif TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_STM32H7
	#define TRU_CFG_L1C_PRESENT 1
#elif TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_SG2002
	#if TRU_CFG_CPU == TRU_OPT_CPU_RISCV_C906
		#define TRU_CFG_L1C_PRESENT 1
		#define TRU_CFG_MMU_PRESENT 1
	#elif TRU_CFG_CPU == TRU_OPT_CPU_CORTEX_A53
		#define TRU_CFG_L1C_PRESENT 1
		#define TRU_CFG_MMU_PRESENT 1
	#endif
#endif

#if !defined(TRU_CFG_FPU) && defined(TRU_UCFG_FPU)
	#define TRU_CFG_FPU TRU_UCFG_FPU
#else
	#if TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC && ((defined(__VFP_FP__) && !defined(__SOFTFP__)) || defined(__TARGET_FPU_VFP) || defined(__ARMVFP__))
		#define TRU_CFG_FPU 1
	#elif TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_SG2002 && defined(__riscv_d)
		#define TRU_CFG_FPU 1
	#elif TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_STM32H7 && ((defined(__VFP_FP__) && !defined(__SOFTFP__)) || defined(__TARGET_FPU_VFP) || defined(__ARMVFP__))
		#define TRU_CFG_FPU 1
	#else
		#define TRU_CFG_FPU 0
	#endif
#endif

#if !defined(TRU_CFG_VPU) && defined(TRU_UCFG_VPU)
	#define TRU_CFG_VPU TRU_UCFG_VPU
#else
	#if TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_SG2002 && defined(__riscv_xtheadvector)
		#define TRU_CFG_VPU 1
	#else
		#define TRU_CFG_VPU 0
	#endif
#endif

#ifndef TRU_CFG_COMPILER_C906THEAD
	#if defined(TRU_UCFG_COMPILER_C906THEAD)
		#define TRU_CFG_COMPILER_C906THEAD TRU_UCFG_COMPILER_C906THEAD
	#endif
#endif

#ifndef TRU_CFG_COMPILER_C906CSR
	#if defined(TRU_UCFG_COMPILER_C906CSR)
		#define TRU_CFG_COMPILER_C906CSR TRU_UCFG_COMPILER_C906CSR
	#endif
#endif

#if !defined(TRU_CFG_VECT_DIRECT) && defined(TRU_UCFG_VECT_DIRECT)
	#define TRU_CFG_VECT_DIRECT TRU_UCFG_VECT_DIRECT
#else
	#define TRU_CFG_VECT_DIRECT 0
#endif

#ifndef TRU_CFG_ZEROFILL_BSS
	#if defined(TRU_UCFG_ZEROFILL_BSS)
		#define TRU_CFG_ZEROFILL_BSS TRU_UCFG_ZEROFILL_BSS
	#endif
#endif

#ifndef TRU_CFG_FLASH_PRESENT
	#if defined(TRU_UCFG_FLASH_PRESENT)
		#define TRU_CFG_FLASH_PRESENT TRU_UCFG_FLASH_PRESENT
	#endif
#endif

#if !defined(TRU_CFG_FREERTOS)
	#if defined(TRU_UCFG_FREERTOS)
		#define TRU_CFG_FREERTOS TRU_UCFG_FREERTOS
	#else
		#define TRU_CFG_FREERTOS 0
	#endif
#endif

#if defined(TRU_CFG_FREERTOS) && TRU_CFG_FREERTOS == 1
	#define TRU_CFG_CMSIS_WEAK_IRQH 1
#endif

// This is to support FreeRTOS with CMSIS, set to 1 when using FreeRTOS, else set to 0
#if !defined(TRU_CFG_CMSIS_WEAK_IRQH)
	#if defined(TRU_UCFG_CMSIS_WEAK_IRQH)
		#define TRU_CFG_CMSIS_WEAK_IRQH TRU_UCFG_CMSIS_WEAK_IRQH
	#else
		#define TRU_CFG_CMSIS_WEAK_IRQH 0
	#endif
#endif

#if !defined(TRU_CFG_EXIT_TO_UBOOT) && defined(TRU_UCFG_EXIT_TO_UBOOT)
	#define TRU_CFG_EXIT_TO_UBOOT TRU_UCFG_EXIT_TO_UBOOT
#endif

#ifdef SEMIHOSTING
	TRU_CFG_SYSCALL_IO TRU_OPT_SYSCALL_IO_SEMIHOSTING
#elif !defined(TRU_CFG_SYSCALL_IO) && defined(TRU_UCFG_SYSCALL_IO)
	#define TRU_CFG_SYSCALL_IO TRU_UCFG_SYSCALL_IO
#endif

#if !defined(TRU_CFG_LOG) && defined(TRU_UCFG_LOG)
	#define TRU_CFG_LOG TRU_UCFG_LOG
#endif

#if !defined(TRU_CFG_LOG_RN) && defined(TRU_UCFG_LOG_RN)
	// 1 == Enables insertion of '\r' for each '\n' character
	#define TRU_CFG_LOG_RN TRU_UCFG_LOG_RN
#endif

#if !defined(TRU_CFG_LOG_LOC) && defined(TRU_UCFG_LOG_LOC)
	#define TRU_CFG_LOG_LOC TRU_UCFG_LOG_LOC
#endif

#if !defined(TRU_CFG_NONCACHEABLE_SECTION) && defined(TRU_UCFG_NONCACHEABLE_SECTION)
	#define TRU_CFG_NONCACHEABLE_SECTION TRU_UCFG_NONCACHEABLE_SECTION
#endif

#if !defined(TRU_CFG_USB_LOG_INIT) && defined(TRU_UCFG_USB_LOG_INIT)
	#define TRU_CFG_USB_LOG_INIT TRU_UCFG_USB_LOG_INIT
#endif
#if !defined(TRU_CFG_USB_LOG_EPENA) && defined(TRU_UCFG_USB_LOG_EPENA)
	#define TRU_CFG_USB_LOG_EPENA TRU_UCFG_USB_LOG_EPENA
#endif
#if !defined(TRU_CFG_USB_LOG_INTR) && defined(TRU_UCFG_USB_LOG_INTR)
	#define TRU_CFG_USB_LOG_INTR TRU_UCFG_USB_LOG_INTR
#endif
#if !defined(TRU_CFG_USB_LOG_SETUP_BYTES) && defined(TRU_UCFG_USB_LOG_SETUP_BYTES)
	#define TRU_CFG_USB_LOG_SETUP_BYTES TRU_UCFG_USB_LOG_SETUP_BYTES
#endif
#if !defined(TRU_CFG_USB_LOG_SETUP_TEXT) && defined(TRU_UCFG_USB_LOG_SETUP_TEXT)
	#define TRU_CFG_USB_LOG_SETUP_TEXT TRU_UCFG_USB_LOG_SETUP_TEXT
#endif
#if !defined(TRU_CFG_USB_LOG_XPROGRESS) && defined(TRU_UCFG_USB_LOG_XPROGRESS)
	#define TRU_CFG_USB_LOG_XPROGRESS TRU_UCFG_USB_LOG_XPROGRESS
#endif
#if !defined(TRU_CFG_USB_LOG_DIEPTSIZ) && defined(TRU_UCFG_USB_LOG_DIEPTSIZ)
	#define TRU_CFG_USB_LOG_DIEPTSIZ TRU_UCFG_USB_LOG_DIEPTSIZ
#endif
#if !defined(TRU_CFG_USB_LOG_DOEPTSIZ) && defined(TRU_UCFG_USB_LOG_DOEPTSIZ)
	#define TRU_CFG_USB_LOG_DOEPTSIZ TRU_UCFG_USB_LOG_DOEPTSIZ
#endif
#if !defined(TRU_CFG_USB_LOG_CALLBACK) && defined(TRU_UCFG_USB_LOG_CALLBACK)
	#define TRU_CFG_USB_LOG_CALLBACK TRU_UCFG_USB_LOG_CALLBACK
#endif
#if !defined(TRU_CFG_USB_LOG_UAC_FB) && defined(TRU_UCFG_USB_LOG_UAC_FB)
	#define TRU_CFG_USB_LOG_UAC_FB TRU_UCFG_USB_LOG_UAC_FB
#endif

// ======================
// Startup configurations
// ======================

// Settings:
//   L1_CACHE_ENABLE: 0 = disable L1 cache, 1 = disable, invalidate and enable L1 cache, 2 = do nothing
//   L2_CACHE_ENABLE: 0 = disable L2 cache, 1 = disable, invalidate and enable L2 cache, 2 = do nothing

#if defined(TRU_CFG_EXIT_TO_UBOOT) && TRU_CFG_EXIT_TO_UBOOT == 1
	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#if (defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1) || (defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1)
			#if !defined(TRU_CFG_CLEAN_CACHE) && defined(TRU_UCFG_CLEAN_CACHE)
				#define TRU_CFG_CLEAN_CACHE TRU_UCFG_CLEAN_CACHE
			#else
				#define TRU_CFG_CLEAN_CACHE 0
			#endif
		#endif
		#if defined(TRU_CFG_MMU_PRESENT) && TRU_CFG_MMU_PRESENT == 1
			#if !defined(TRU_CFG_MMU) && defined(TRU_UCFG_MMU)
				#define TRU_CFG_MMU TRU_UCFG_MMU
			#else
				#define TRU_CFG_MMU 2
			#endif
		#endif
		#if defined(TRU_CFG_SMP_COHERENCY_PRESENT) && TRU_CFG_SMP_COHERENCY_PRESENT == 1
			#if !defined(TRU_CFG_SMP_COHERENCY) && defined(TRU_UCFG_SMP_COHERENCY)
				#define TRU_CFG_SMP_COHERENCY TRU_UCFG_SMP_COHERENCY
			#else
				#define TRU_CFG_SMP_COHERENCY 2
			#endif
		#endif
		#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
			#if !defined(TRU_CFG_L1C) && defined(TRU_UCFG_L1C)
				#define TRU_CFG_L1C TRU_UCFG_L1C
			#else
				#define TRU_CFG_L1C 2
			#endif
		#endif
		#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
			#if !defined(TRU_CFG_L2C) && defined(TRU_UCFG_L2_CACHE)
				#define TRU_CFG_L2C TRU_UCFG_L2_CACHE
			#else
				#define TRU_CFG_L2C 2
			#endif
		#endif
		#if defined(TRU_CFG_SCU_PRESENT) && TRU_CFG_SCU_PRESENT == 1
			#if !defined(TRU_CFG_SCU) && defined(TRU_UCFG_SCU)
				#define TRU_CFG_SCU TRU_UCFG_SCU
			#else
				#define TRU_CFG_SCU 2
			#endif
		#endif
	#else
		#if (defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1) || (defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1)
			#if !defined(TRU_CFG_CLEAN_CACHE) && defined(TRU_UCFG_CLEAN_CACHE)
				#define TRU_CFG_CLEAN_CACHE TRU_UCFG_CLEAN_CACHE
			#else
				#define TRU_CFG_CLEAN_CACHE 0
			#endif
		#endif
		#if defined(TRU_CFG_MMU_PRESENT) && TRU_CFG_MMU_PRESENT == 1
			#if !defined(TRU_CFG_MMU) && defined(TRU_UCFG_MMU)
				#define TRU_CFG_MMU TRU_UCFG_MMU
			#else
				#define TRU_CFG_MMU 2
			#endif
		#endif
		#if defined(TRU_CFG_SMP_COHERENCY_PRESENT) && TRU_CFG_SMP_COHERENCY_PRESENT == 1
			#if !defined(TRU_CFG_SMP_COHERENCY) && defined(TRU_UCFG_SMP_COHERENCY)
				#define TRU_CFG_SMP_COHERENCY TRU_UCFG_SMP_COHERENCY
			#else
				#define TRU_CFG_SMP_COHERENCY 2
			#endif
		#endif
		#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
			#if !defined(TRU_CFG_L1C) && defined(TRU_UCFG_L1C)
				#define TRU_CFG_L1C TRU_UCFG_L1C
			#else
				#define TRU_CFG_L1C 2
			#endif
		#endif
		#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
			#if !defined(TRU_CFG_L2C) && defined(TRU_UCFG_L2_CACHE)
				#define TRU_CFG_L2C TRU_UCFG_L2_CACHE
			#else
				#define TRU_CFG_L2C 2
			#endif
		#endif
		#if defined(TRU_CFG_SCU_PRESENT) && TRU_CFG_SCU_PRESENT == 1
			#if !defined(TRU_CFG_SCU) && defined(TRU_UCFG_SCU)
				#define TRU_CFG_SCU TRU_UCFG_SCU
			#else
				#define TRU_CFG_SCU 2
			#endif
		#endif
	#endif
#else
	// We do not want cache in DEBUG mode
	#ifdef DEBUG
		#if (defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1) || (defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1)
			#if !defined(TRU_CFG_CLEAN_CACHE) && defined(TRU_UCFG_CLEAN_CACHE)
				#define TRU_CFG_CLEAN_CACHE TRU_UCFG_CLEAN_CACHE
			#else
				#define TRU_CFG_CLEAN_CACHE 0
			#endif
		#endif
		#if defined(TRU_CFG_MMU_PRESENT) && TRU_CFG_MMU_PRESENT == 1
			#if !defined(TRU_CFG_MMU) && defined(TRU_UCFG_MMU)
				#define TRU_CFG_MMU TRU_UCFG_MMU
			#else
				#define TRU_CFG_MMU 1
			#endif
		#endif
		#if defined(TRU_CFG_SMP_COHERENCY_PRESENT) && TRU_CFG_SMP_COHERENCY_PRESENT == 1
			#if !defined(TRU_CFG_SMP_COHERENCY) && defined(TRU_UCFG_SMP_COHERENCY)
				#define TRU_CFG_SMP_COHERENCY TRU_UCFG_SMP_COHERENCY
			#else
				#define TRU_CFG_SMP_COHERENCY 0
			#endif
		#endif
		#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
			#if !defined(TRU_CFG_L1C) && defined(TRU_UCFG_L1C)
				#define TRU_CFG_L1C TRU_UCFG_L1C
			#else
				#define TRU_CFG_L1C 0
			#endif
		#endif
		#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
			#if !defined(TRU_CFG_L2C) && defined(TRU_UCFG_L2_CACHE)
				#define TRU_CFG_L2C TRU_UCFG_L2_CACHE
			#else
				#define TRU_CFG_L2C 0
			#endif
		#endif
		#if defined(TRU_CFG_SCU_PRESENT) && TRU_CFG_SCU_PRESENT == 1
			#if !defined(TRU_CFG_SCU) && defined(TRU_UCFG_SCU)
				#define TRU_CFG_SCU TRU_UCFG_SCU
			#else
				#define TRU_CFG_SCU 0
			#endif
		#endif
	#else
		#if (defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1) || (defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1)
			#if !defined(TRU_CFG_CLEAN_CACHE) && defined(TRU_UCFG_CLEAN_CACHE)
				#define TRU_CFG_CLEAN_CACHE TRU_UCFG_CLEAN_CACHE
			#else
				#define TRU_CFG_CLEAN_CACHE 0
			#endif
		#endif
		#if defined(TRU_CFG_MMU_PRESENT) && TRU_CFG_MMU_PRESENT == 1
			#if !defined(TRU_CFG_MMU) && defined(TRU_UCFG_MMU)
				#define TRU_CFG_MMU TRU_UCFG_MMU
			#else
				#define TRU_CFG_MMU 1
			#endif
		#endif
		#if defined(TRU_CFG_SMP_COHERENCY_PRESENT) && TRU_CFG_SMP_COHERENCY_PRESENT == 1
			#if !defined(TRU_CFG_SMP_COHERENCY) && defined(TRU_UCFG_SMP_COHERENCY)
				#define TRU_CFG_SMP_COHERENCY TRU_UCFG_SMP_COHERENCY
			#else
				#define TRU_CFG_SMP_COHERENCY 1
			#endif
		#endif
		#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
			#if !defined(TRU_CFG_L1C) && defined(TRU_UCFG_L1C)
				#define TRU_CFG_L1C TRU_UCFG_L1C
			#else
				#define TRU_CFG_L1C 1
			#endif
		#endif
		#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
			#if !defined(TRU_CFG_L2C) && defined(TRU_UCFG_L2_CACHE)
				#define TRU_CFG_L2C TRU_UCFG_L2_CACHE
			#else
				#define TRU_CFG_L2C 1
			#endif
		#endif
		#if defined(TRU_CFG_SCU_PRESENT) && TRU_CFG_SCU_PRESENT == 1
			#if !defined(TRU_CFG_SCU) && defined(TRU_UCFG_SCU)
				#define TRU_CFG_SCU TRU_UCFG_SCU
			#else
				#define TRU_CFG_SCU 1
			#endif
		#endif
	#endif
#endif

// Indicates unaligned byte access is supported
#if !defined(TRU_CFG_UNALIGNED_ACCESS) && defined(TRU_UCFG_UNALIGNED_ACCESS)
	#define TRU_CFG_UNALIGNED_ACCESS TRU_UCFG_UNALIGNED_ACCESS
#endif

#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 1
	#if defined(__ICCARM__)
		#define NONCACHEABLE_SECTION \
			_Pragma("location=\".noncacheable_section\"")
	#else
		#define NONCACHEABLE_SECTION \
			__attribute__((section(".noncacheable_section")))
	#endif
#else
	#define NONCACHEABLE_SECTION
#endif

#ifndef TRU_CFG_BOARD_HEADER
	#if defined(TRU_UCFG_BOARD_HEADER)
		#define TRU_CFG_BOARD_HEADER TRU_UCFG_BOARD_HEADER
	#else
		#define TRU_CFG_BOARD_HEADER ""
	#endif
#endif

#ifndef __ASSEMBLER__
#include TRU_CFG_BOARD_HEADER
#endif

#ifdef __cplusplus
}
#endif

#endif
