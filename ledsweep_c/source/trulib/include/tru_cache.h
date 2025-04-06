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

	Version: 20250405
*/

#ifndef TRU_CACHE_H
#define TRU_CACHE_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_TARGET_C5SOC)

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
	#include "RTE_Components.h"   // CMSIS
	#include CMSIS_device_header  // CMSIS
	#include "tru_cache_l2c310.h"
	#include <stdbool.h>
#else
	#include "alt_cache.h"
	#include "tru_cortex_a9.h"
	#include "tru_util_ll.h"
	#include "tru_cache_l2c310.h"
	#include <stdbool.h>
#endif

#define CACHELINE_SIZE L2C310_CACHELINE_SIZE

// ================
// L1 cache related
// ================

#if defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT != 0U

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline bool tru_l1_is_dcache_enabled(void){
	return __get_SCTLR() & SCTLR_C_Msk;
}
#else
static inline bool tru_l1_is_dcache_enabled(void){
	uint32_t sctlr;
	__read_sctlr(sctlr);
	return sctlr & (1U << 2U);
}
#endif

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline void tru_l1_invalidate_branch_all(void){
	L1C_InvalidateBTAC();
}
#else
static inline void tru_l1_invalidate_branch_all(void){
	alt_cache_l1_branch_invalidate();
}
#endif

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline void tru_l1_data_clean_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		L1C_CleanDCacheMVA((void *)addr);
		addr += CACHELINE_SIZE;  // Increment index
	}
	__DSB();  // Ensure completion of the clean
}
#else
static inline void tru_l1_data_clean_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		__write_dccmvac((void *)addr);
		addr += CACHELINE_SIZE;  // Increment index
	}
	__dsb();  // Ensure completion of the clean
}
#endif

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline void tru_l1_data_inv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		L1C_InvalidateDCacheMVA((void *)addr);
		addr += CACHELINE_SIZE;  // Increment index
	}
	__DSB();  // Ensure completion of the invalidate
}
#else
static inline void tru_l1_data_inv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		__write_dcimvac((void *)addr);
		addr += CACHELINE_SIZE;  // Increment index
	}
	__dsb();  // Ensure completion of the invalidate
}
#endif

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline void tru_l1_data_cleaninv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		L1C_CleanInvalidateDCacheMVA((void *)addr);
		addr += CACHELINE_SIZE;  // Increment index
	}
	__DSB();  // Ensure completion
}
#else
static inline void tru_l1_data_cleaninv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		__write_dccimvac((void *)addr);
		addr += CACHELINE_SIZE;  // Increment index
	}
	__dsb();  // Ensure completion
}
#endif

#endif

// ================
// L2 cache related
// ================

#if defined(TRU_L2_CACHE_PRESENT) && TRU_L2_CACHE_PRESENT != 0U

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline bool tru_l2_is_enabled(void){
	return L2C_310->CONTROL & 0x1U;
}
#else
static inline bool tru_l2_is_enabled(void){
	return tru_iom_rd32((uint32_t *)(L2C310_BASE + L2C310_CTRL_OFFSET)) & 0x1U;
}
#endif

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline void tru_l2_data_clean_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		L2C_CleanPa((void *)addr);  // Note, this also issues the L2 sync instruction, which will stall the slave port until completion, that is unless there is a L2 background operation in in progress
		addr += CACHELINE_SIZE;  // Increment index
	}
	__DSB();
}
#else
static inline void tru_l2_data_clean_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		tru_iom_wr32((uint32_t *)(L2C310_BASE + L2C310_CLEAN_PA_OFFSET), addr);
		tru_iom_wr32((uint32_t *)(L2C310_BASE + L2C310_CACHE_SYNC_OFFSET), 0U);
		addr += CACHELINE_SIZE;  // Increment index
	}
	alt_cache_l2_sync();
	__dsb();
}
#endif

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline void tru_l2_data_inv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		L2C_InvPa((void *)addr);  // Note, this also issues the L2 sync instruction, which will stall the slave port until completion, that is unless there is a L2 background operation in in progress
		addr += CACHELINE_SIZE;  // Increment index
	}
	__DSB();
}
#else
static inline void tru_l2_data_inv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		tru_iom_wr32((uint32_t *)(L2C310_BASE + L2C310_INV_PA_OFFSET), addr);
		tru_iom_wr32((uint32_t *)(L2C310_BASE + L2C310_CACHE_SYNC_OFFSET), 0U);
		addr += CACHELINE_SIZE;  // Increment index
	}
	alt_cache_l2_sync();
	__dsb();
}
#endif

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
static inline void tru_l2_data_cleaninv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		L2C_CleanInvPa((void *)addr);  // Note, this also issues the L2 sync instruction, which will stall the slave port until completion, that is unless there is a L2 background operation in in progress
		addr += CACHELINE_SIZE;  // Increment index
	}
	__DSB();
}
#else
static inline void tru_l2_data_cleaninv_range(void *buf, uint32_t len){
	uint32_t limit = (uint32_t)buf + len;
	uint32_t addr = (uint32_t)buf & ~(CACHELINE_SIZE - 1U);

	while(addr < limit){
		tru_iom_wr32((uint32_t *)(L2C310_BASE + L2C310_CLEANINV_PA_OFFSET), addr);
		tru_iom_wr32((uint32_t *)(L2C310_BASE + L2C310_CACHE_SYNC_OFFSET), 0U);
		addr += CACHELINE_SIZE;  // Increment index
	}
	alt_cache_l2_sync();
	__dsb();
}
#endif

#endif

#elif(TRU_TARGET == TRU_TARGET_STM32H7)

#include "stm32h7xx_hal.h"

#define CACHELINE_SIZE __SCB_DCACHE_LINE_SIZE

// ================
// L1 cache related
// ================

#if defined(TRU_L1_CACHE_PRESENT) && TRU_L1_CACHE_PRESENT != 0U

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

#endif

#endif
