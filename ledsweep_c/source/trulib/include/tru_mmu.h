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
*/

#ifndef TRU_MMU_H
#define TRU_MMU_H

#include "tru_config.h"
#include <stdint.h>

#if(TRU_TARGET == TRU_TARGET_C5SOC)

#if defined(TRU_CMSIS) && TRU_CMSIS == 0U

#include "tru_cortex_a9.h"
#include "alt_mmu.h"
#include "alt_cache.h"

#if defined(TRU_DMA_BUFFER_NONCACHEABLE) && TRU_DMA_BUFFER_NONCACHEABLE == 1U && defined(TRU_MMU) && TRU_MMU == 1U
void tru_mmu_create_dma_buffer_table_entries(void);
#endif

// Multiprocessing Extensions: Invalidate unified TLB by MVA, all ASID
static inline void tru_mmu_inv_tlb_imvaa(uint32_t value){
	__write_tlbimvaa(value);
	__dsb();  // Ensure completion of the invalidation
	__isb();  // Ensure instruction fetch path sees new state
}

static inline void tru_mmu_inv_range(uint32_t *ttb, uint32_t base_address, uint32_t count){
	uint32_t offset = base_address >> 20U;
	uint32_t *entry_addr = ttb;

	// 4 bytes aligned
	entry_addr = entry_addr + offset;

	for(uint32_t i = 0; i < count; i++){
		tru_mmu_inv_tlb_imvaa((uint32_t)entry_addr);
		entry_addr++;
	}
}

static __inline uint32_t alt_mmu_va_space_gen_section(uintptr_t pa, const ALT_MMU_MEM_REGION_t * mem){
	int tex = (mem->attributes >> 4) & 0x7;
	int c   = (mem->attributes >> 1) & 0x1;
	int b   = (mem->attributes >> 0) & 0x1;

	if (mem->attributes == ALT_MMU_ATTR_FAULT)
	{
		return 0;
	}

	return
		  ALT_MMU_TTB1_TYPE_SET(0x2)
		| ALT_MMU_TTB1_SECTION_B_SET(b)
		| ALT_MMU_TTB1_SECTION_C_SET(c)
		| ALT_MMU_TTB1_SECTION_XN_SET(mem->execute)
		| ALT_MMU_TTB1_SECTION_DOMAIN_SET(0)
		| ALT_MMU_TTB1_SECTION_AP_SET(mem->access)
		| ALT_MMU_TTB1_SECTION_TEX_SET(tex)
		| ALT_MMU_TTB1_SECTION_S_SET(mem->shareable)
		| ALT_MMU_TTB1_SECTION_NG_SET(0)
		| ALT_MMU_TTB1_SECTION_NS_SET(mem->security)
		| ALT_MMU_TTB1_SECTION_BASE_ADDR_SET(pa >> 20);
}

#endif

#endif

void tru_mmu_set_noncacheable_section(void *start_addr, uint32_t mem_size);

#endif
