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

	Version: 20250315
*/

#include "tru_mmu.h"
#include "tru_cache.h"

#if defined(TRU_CMSIS) && TRU_CMSIS == 0U

extern uint32_t __mmu_ttb_l1_entries_start;  // Reference external symbol name from the linker file
//extern uint8_t *c5soc_mmu_tbl;

void *mmu_get_ttb_l1(void){
	//return c5soc_mmu_tbl;
	return &__mmu_ttb_l1_entries_start;
}

static void tru_mmu_set_ttb_section_entries(const ALT_MMU_MEM_REGION_t *region, uint32_t noncache_num_sections){
	uintptr_t pa = (uintptr_t)region->pa;
	uintptr_t va = (uintptr_t)region->va;
	uint32_t desc = alt_mmu_va_space_gen_section(pa, region);
	uint32_t* ttb1 = &__mmu_ttb_l1_entries_start;

	for(uint32_t i = 0; i < noncache_num_sections; i++){
		ttb1[va >> 20] = desc;
		va += ALT_MMU_SECTION_SIZE;
		pa += ALT_MMU_SECTION_SIZE;
	}
}

// Change MMU table section entry for a memory range to non-cacheable
void tru_mmu_set_noncacheable_section(void *start_addr, uint32_t mem_size){
	if(mem_size){
		ALT_MMU_MEM_REGION_t region_fault = {
			.va         = start_addr,
			.pa         = start_addr,
			.size       = mem_size,
			.access     = ALT_MMU_AP_FULL_ACCESS,
			.attributes = ALT_MMU_ATTR_FAULT,
			.shareable  = ALT_MMU_TTB_S_SHAREABLE,
			.execute    = ALT_MMU_TTB_XN_DISABLE,
			.security   = ALT_MMU_TTB_NS_SECURE
		};
		ALT_MMU_MEM_REGION_t region = {
			.va         = start_addr,
			.pa         = start_addr,
			.size       = mem_size,
			.access     = ALT_MMU_AP_FULL_ACCESS,
			.attributes = ALT_MMU_ATTR_NC,
			.shareable  = ALT_MMU_TTB_S_SHAREABLE,
			.execute    = ALT_MMU_TTB_XN_DISABLE,
			.security   = ALT_MMU_TTB_NS_SECURE
		};
		uint32_t noncache_num_sections = (mem_size % 1048576UL) ? mem_size / 1048576UL + 1 : mem_size / 1048576UL;  // Calc number of 1MB MMU sections rounding up

		tru_mmu_set_ttb_section_entries(&region_fault, noncache_num_sections);
		__dsb();
		tru_mmu_inv_range(&__mmu_ttb_l1_entries_start, (uint32_t)start_addr, noncache_num_sections);  // Invalidate TLB entries by MVA with Multiprocessing Extension support
		tru_l1_invalidate_branch_all();
		__dsb();
		__isb();
		tru_mmu_set_ttb_section_entries(&region, noncache_num_sections);
		__dsb();
	}
}

#if defined(TRU_DMA_BUFFER_NONCACHEABLE) && TRU_DMA_BUFFER_NONCACHEABLE == 1U && defined(TRU_MMU) && TRU_MMU == 1U
	extern uint32_t __dma_buffer_start;  // Reference external symbol name from the linker file
	extern uint32_t __dma_buffer_end;  // Reference external symbol name from the linker file

	// Note: this assumes the MMU table is using L1 entries with 1MB sections
	void tru_mmu_create_dma_buffer_table_entries(void){
		uint32_t dma_buffer_size = &__dma_buffer_end - &__dma_buffer_start;
		tru_mmu_set_noncacheable_section(&__dma_buffer_start, dma_buffer_size);
	}
#endif

#else

#include "RTE_Components.h"   // CMSIS
#include CMSIS_device_header  // CMSIS

// Change MMU table section entry for a memory range to non-cacheable
void tru_mmu_set_noncacheable_section(void *start_addr, uint32_t mem_size){
	if(mem_size){
		mmu_region_attributes_Type region = {
			.rg_t = SECTION,
			.domain = 0x0,
			.e_t = ECC_DISABLED,
			.g_t = GLOBAL,
			.inner_norm_t = NON_CACHEABLE,  // L1 cache
			.outer_norm_t = NON_CACHEABLE,  // L2 cache
			.mem_t = NORMAL,
			.sec_t = SECURE,
			.xn_t = EXECUTE,
			.priv_t = RW,
			.user_t = RW,
			.sh_t = SHARED
		};
		uint32_t L1_Section_Attrib_NonCache_RWX;  // Section attribute variable
		uint32_t noncache_num_sections = (mem_size % 1048576UL) ? mem_size / 1048576UL + 1 : mem_size / 1048576UL;  // Calc number of 1MB MMU sections rounding up
		uint32_t *mmu_ttb_l1 = mmu_get_ttb_l1();

		MMU_GetSectionDescriptor(&L1_Section_Attrib_NonCache_RWX, region);  // Fill section attribute variable
		MMU_TTSection(mmu_ttb_l1, (uint32_t)start_addr, noncache_num_sections, DESCRIPTOR_FAULT);  // Replace the old translation table entry with an invalid (faulting) entry
		// Clean not required with the Multiprocessing Extensions
		//uint32_t offset = (uint32_t)stream0.xfer_addr >> 20U;
		//tru_l1_data_clean_range(mmu_ttb_l1 + offset, 4U * noncache_num_sections);
		__DSB();  // Ensure faulting entry is visible
		MMU_InvalidateRange(mmu_ttb_l1, (uint32_t)start_addr, noncache_num_sections);  // Invalidate TLB entries by MVA with Multiprocessing Extension support
		__set_BPIALL(0);  // Invalidate entire branch predictor array
		__DSB();  // Ensure completion of the invalidate branch predictor operation
		__ISB();  // Ensure changes visible to instruction fetch
		MMU_TTSection(mmu_ttb_l1, (uint32_t)start_addr, noncache_num_sections, L1_Section_Attrib_NonCache_RWX);  // Write MMU table 1MB section entries that are non-cacheable
		__DSB();  // Ensure the new entry is visible
	}
}

#endif
