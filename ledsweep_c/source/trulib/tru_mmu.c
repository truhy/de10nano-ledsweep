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

	Version: 20260208
*/

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include "tru_mmu.h"
#include "tru_cache.h"

#include "RTE_Components.h"   // CMSIS
#include CMSIS_device_header  // CMSIS

#include <stdint.h>

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
		uint32_t noncache_num_sections = (mem_size % 1048576) ? mem_size / 1048576 + 1 : mem_size / 1048576;  // Calc number of 1MB MMU sections rounding up
		uint32_t *mmu_ttb_l1 = mmu_get_ttb_l1();

		MMU_GetSectionDescriptor(&L1_Section_Attrib_NonCache_RWX, region);  // Fill section attribute variable
		MMU_TTSection(mmu_ttb_l1, (uint32_t)start_addr, noncache_num_sections, DESCRIPTOR_FAULT);  // Replace the old translation table entry with an invalid (faulting) entry
		// Clean not required with the Multiprocessing Extensions
		//uint32_t offset = (uint32_t)stream0.xfer_addr >> 20;
		//tru_l1_data_clean_range(mmu_ttb_l1 + offset, 4 * noncache_num_sections);
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
