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

	Version: 20260606

	Useful notes:
	Clean = Writes the dirty cache lines into memory and leaves the cache lines as it is (i.e. flags remain as is).
	Invalidate = Deletes the dirty cache lines (i.e. flags), note, this does not write the dirty cache lines into memory.
	Clean+invalidate = Writes the dirty cache lines into memory and deletes the dirty cache lines.  This is also known as flush.
*/

#ifndef TRU_CACHE_H
#define TRU_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

	#include "RTE_Components.h"   // CMSIS
	#include CMSIS_device_header  // CMSIS

	#include "arm/tru_cache_l2c310.h"

	#include <stdbool.h>

	#define TRU_CACHELINE_SIZE TRU_L2C310_CACHELINE_SIZE

	// ================
	// L1 cache related
	// ================

	#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT != 0

		static inline bool tru_l1_is_dcache_enabled(void){
			return __get_SCTLR() & SCTLR_C_Msk;
		}

		static inline void tru_l1_invalidate_branch_all(void){
			L1C_InvalidateBTAC();
		}

		static inline void tru_l1_data_clean_range(void *buf, uint32_t len){
			uint32_t limit = (uint32_t)buf + len;
			uint32_t addr = (uint32_t)buf & ~(TRU_CACHELINE_SIZE - 1);

			while(addr < limit){
				L1C_CleanDCacheMVA((void *)addr);
				addr += TRU_CACHELINE_SIZE;  // Increment index
			}
			__DSB();  // Ensure completion of the clean
		}

		static inline void tru_l1_data_inv_range(void *buf, uint32_t len){
			uint32_t limit = (uint32_t)buf + len;
			uint32_t addr = (uint32_t)buf & ~(TRU_CACHELINE_SIZE - 1);

			while(addr < limit){
				L1C_InvalidateDCacheMVA((void *)addr);
				addr += TRU_CACHELINE_SIZE;  // Increment index
			}
			__DSB();  // Ensure completion of the invalidate
		}

		static inline void tru_l1_data_cleaninv_range(void *buf, uint32_t len){
			uint32_t limit = (uint32_t)buf + len;
			uint32_t addr = (uint32_t)buf & ~(TRU_CACHELINE_SIZE - 1);

			while(addr < limit){
				L1C_CleanInvalidateDCacheMVA((void *)addr);
				addr += TRU_CACHELINE_SIZE;  // Increment index
			}
			__DSB();  // Ensure completion
		}

	#endif

	// ================
	// L2 cache related
	// ================

	#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT != 0

		static inline bool tru_l2_is_enabled(void){
			return L2C_310->CONTROL & 0x1;
		}

		static inline void tru_l2_data_clean_range(void *buf, uint32_t len){
			uint32_t limit = (uint32_t)buf + len;
			uint32_t addr = (uint32_t)buf & ~(TRU_CACHELINE_SIZE - 1);

			while(addr < limit){
				L2C_CleanPa((void *)addr);  // Note, this also issues the L2 sync instruction, which will stall the slave port until completion, that is unless there is a L2 background operation in in progress
				addr += TRU_CACHELINE_SIZE;  // Increment index
			}
			__DSB();
		}

		static inline void tru_l2_data_inv_range(void *buf, uint32_t len){
			uint32_t limit = (uint32_t)buf + len;
			uint32_t addr = (uint32_t)buf & ~(TRU_CACHELINE_SIZE - 1);

			while(addr < limit){
				L2C_InvPa((void *)addr);  // Note, this also issues the L2 sync instruction, which will stall the slave port until completion, that is unless there is a L2 background operation in in progress
				addr += TRU_CACHELINE_SIZE;  // Increment index
			}
			__DSB();
		}

		static inline void tru_l2_data_cleaninv_range(void *buf, uint32_t len){
			uint32_t limit = (uint32_t)buf + len;
			uint32_t addr = (uint32_t)buf & ~(TRU_CACHELINE_SIZE - 1);

			while(addr < limit){
				L2C_CleanInvPa((void *)addr);  // Note, this also issues the L2 sync instruction, which will stall the slave port until completion, that is unless there is a L2 background operation in in progress
				addr += TRU_CACHELINE_SIZE;  // Increment index
			}
			__DSB();
		}

	#endif

#elif defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_STM32H7

	#include "stm32h7xx_hal.h"
	#include <stdbool.h>

	#define TRU_CACHELINE_SIZE __SCB_DCACHE_LINE_SIZE

	// ================
	// L1 cache related
	// ================

	#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT != 0

		static inline bool tru_l1_is_dcache_enabled(void){
			return SCB->CCR & SCB_CCR_DC_Msk;
		}

		static inline void tru_l1_data_clean_range(void *buf, uint32_t len){
			SCB_CleanDCache_by_Addr(buf, len);
		}

		static inline void tru_l1_data_inv_range(void *buf, uint32_t len){
			SCB_InvalidateDCache_by_Addr(buf, len);
		}

		static inline void tru_l1_data_cleaninv_range(void *buf, uint32_t len){
			SCB_CleanInvalidateDCache_by_Addr(buf, len);
		}

	#endif

#elif defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_SG2002

	#if defined(TRU_CFG_CPU) && TRU_CFG_CPU == TRU_OPT_CPU_RISCV_C906

		#include "c906/cache.h"

		#define TRU_CACHELINE_SIZE C906_L1_CACHE_LINE_SIZE

		// ================
		// L1 cache related
		// ================

		#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT != 0

			static inline bool tru_l1_is_dcache_enabled(void){
				return l1_is_dcache_enabled();
			}

			static inline void tru_l1_data_clean_range(void *buf, uint32_t len){
				l1_clean_dcache_range_thead(buf, len);
			}

			static inline void tru_l1_data_inv_range(void *buf, uint32_t len){
				l1_inv_dcache_range_thead(buf, len);
			}

			static inline void tru_l1_data_cleaninv_range(void *buf, uint32_t len){
				l1_flush_dcache_range_thead(buf, len);
			}

		#endif

	#elif defined(TRU_CFG_CPU) && TRU_CFG_CPU == TRU_OPT_CPU_CORTEX_A53

		// TODO

	#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
