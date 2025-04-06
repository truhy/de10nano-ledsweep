/*
	Created on: 26 Oct 2024
	Author: Truong Hy
*/

#ifndef MMU_C5SOC_H
#define MMU_C5SOC_H

#include "c5soc.h"

// ITTBxx register N values for setting the L1 table size and VA range (see ARM Architecture v7-A ref manual)
// L1 only table mode must use value 0 (N = 0)
// L1 and L2 table mode will use a size split (N > 0), i.e. a value 1 to 7.  The split determines the L1 table size and the input VA boundary (address range) for the two tables
#define TTBCR_N_L1_16K        0x0U
#define TTBCR_N_L1_8K_L2_16K  0x1U
#define TTBCR_N_L1_4K_L2_16K  0x2U
#define TTBCR_N_L1_2K_L2_16K  0x3U
#define TTBCR_N_L1_1K_L2_16K  0x4U
#define TTBCR_N_L1_512_L2_16K 0x5U
#define TTBCR_N_L1_256_L2_16K 0x6U
#define TTBCR_N_L1_128_L2_16K 0x7U

// User settings
#define USE_L1_AND_L2_TABLE 0U
#define USE_TTBCR_N TTBCR_N_L1_8K_L2_16K

// L1 table size + L2 table input VA address boundary
#if(USE_L1_AND_L2_TABLE == 1U)
	// Determine L1 table size and alignment
	#if(USE_TTBCR_N == TTBCR_N_L1_8K_L2_16K)
		#define L1_SIZE 8192
		#define L2_FIRST_VA_ADDR 0x80000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_4K_L2_16K)
		#define L1_SIZE 4096
		#define L2_FIRST_VA_ADDR 0x40000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_2K_L2_16K)
		#define L1_SIZE 2048
		#define L2_FIRST_VA_ADDR 0x20000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_1K_L2_16K)
		#define L1_SIZE 1024
		#define L2_FIRST_VA_ADDR 0x10000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_512_L2_16K)
		#define L1_SIZE 512
		#define L2_FIRST_VA_ADDR 0x08000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_256_L2_16K)
		#define L1_SIZE 256
		#define L2_FIRST_VA_ADDR 0x04000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_128_L2_16K)
		#define L1_SIZE 128
		#define L2_FIRST_VA_ADDR 0x02000000
	#endif
	#define L2_SIZE 16384
#else
	#define L1_SIZE 16384
	#define L2_SIZE 0
#endif

void *mmu_get_ttb_l1(void);

// Multiprocessing Extensions: Invalidate unified TLB by MVA, all ASID
__STATIC_FORCEINLINE void __set_TLBIMVAA(uint32_t value){
	__set_CP(15, 0, value, 8, 7, 3);
}

// Multiprocessing Extensions: Invalidate unified TLB by MVA, all ASID
__STATIC_INLINE void MMU_InvalidateTLBIMVAA(uint32_t value){
	__set_TLBIMVAA(value);
	__DSB();  // Ensure completion of the invalidation
	__ISB();  // Ensure instruction fetch path sees new state
}

static inline void MMU_InvalidateRange(uint32_t *ttb, uint32_t base_address, uint32_t count){
	uint32_t offset = base_address >> 20U;
	uint32_t *entry_addr = ttb;

	// 4 bytes aligned
	entry_addr = entry_addr + offset;

	for(uint32_t i = 0; i < count; i++){
		MMU_InvalidateTLBIMVAA((uint32_t)entry_addr);
		entry_addr++;
	}
}

#if defined(TRU_DMA_BUFFER_NONCACHEABLE) && TRU_DMA_BUFFER_NONCACHEABLE == 1U && defined(TRU_MMU) && TRU_MMU == 1U
	void mmu_create_dma_buffer_table_entries(void);
#endif

#endif
