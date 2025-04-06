/**************************************************************************//**
 * @file     mmu_c5soc.c
 * @brief    MMU Configuration
 *           Device c5soc
 * @version  V1.1.0
 * @date     23. November 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Memory map description

	Processor memory map of DE10-Nano processor system (HPS).

	The implemented MMU table is 4096 short descriptor entries of 1MB sections, which translates the six Cyclone V SoC memory regions below.
	Notes:
	- the Peripherals+L3, Boot ROM, SCU+L2 and OCRAM memory regions had to be combined because their sizes do not align to 1MB
	- bottom 1MB region is assumed to be remapped to SDRAM
	+-----------------------------------------------------------------------------------------------------------------+
	| Region                             | Address Range           | MMU table entry attributes                       |
	|-----------------------------------------------------------------------------------------------------------------|
	| Periph+L3, Boot ROM, SCU+L2, OCRAM | 0xFF400000 - 0xFFFFFFFF | Shared device, RW, non-cacheable, shareable      |
	|-----------------------------------------------------------------------------------------------------------------|
	| LW H-to-F                          | 0xFF200000 - 0xFF3FFFFF | Shared device, RW, non-cacheable, shareable      |
	|-----------------------------------------------------------------------------------------------------------------|
	| DAP                                | 0xFF000000 - 0xFF1FFFFF | Shared device, RW, non-cacheable, shareable      |
	|-----------------------------------------------------------------------------------------------------------------|
	| STM                                | 0xFC000000 - 0xFEFFFFFF | Shared device, RW, non-cacheable, shareable      |
	|-----------------------------------------------------------------------------------------------------------------|
	| H-to-F                             | 0xC0000000 - 0xFBFFFFFF | Shared device, RW, non-cacheable, shareable      |
	|-----------------------------------------------------------------------------------------------------------------|
	| 3GB SDRAM                          | 0x00000000 - 0xBFFFFFFF | Normal, RWX, inner & outer-cacheable, shareable  |
	+-----------------------------------------------------------------------------------------------------------------+

	Note, the DE10-Nano only has 1GB of SDRAM populated, but since there is enough table entries, and it wraps around
	to address 0 it is safe to cover the entire 3GB range.

	Below is the ideal MMU table, but it is not easily achievable:
	+-----------------------------------------------------------------------------------------------------------------+
	| Region                    | Address Range           | MMU table entry attributes                                |
	|-----------------------------------------------------------------------------------------------------------------|
	| OCRAM (On-Chip RAM)       | 0xFFFF0000 - 0xFFFFFFFF | Normal, RWX, inner-cacheable, shareable                   |
	|-----------------------------------------------------------------------------------------------------------------|
	| SCU and L2 Registers      | 0xFFFEC000 - 0xFFFEFFFF | Shared device, RW, non-cacheable, shareable               |
	|-----------------------------------------------------------------------------------------------------------------|
	| Boot ROM                  | 0xFFFD0000 - 0xFFFEBFFF | Shared device, RO, non-cacheable, shareable               |
	|-----------------------------------------------------------------------------------------------------------------|
	| Peripherals and L3 GPV    | 0xFF400000 - 0xFFFCFFFF | Shared device, RW, non-cacheable, shareable               |
	|-----------------------------------------------------------------------------------------------------------------|
	| LW H-to-F                 | 0xFF200000 - 0xFF3FFFFF | Shared device, RW, non-cacheable, shareable               |
	|-----------------------------------------------------------------------------------------------------------------|
	| DAP                       | 0xFF000000 - 0xFF1FFFFF | Shared device, RW, non-cacheable, shareable               |
	|-----------------------------------------------------------------------------------------------------------------|
	| STM                       | 0xFC000000 - 0xFEFFFFFF | Shared device, RW, non-cacheable, shareable               |
	|-----------------------------------------------------------------------------------------------------------------|
	| H-to-F                    | 0xC0000000 - 0xFBFFFFFF | Shared device, RW, non-cacheable, shareable               |
	|-----------------------------------------------------------------------------------------------------------------|
	| SDRAM                     | 0x00100000 - 0xBFFFFFFF | Normal, RWX, inner & outer-cacheable, shareable           |
	|-----------------------------------------------------------------------------------------------------------------|
	| When remapped to SDRAM    | 0x00010000 - 0x000FFFFF | Normal, RWX, inner & outer-cacheable, shareable           |
	|-----------------------------------------------------------------------------------------------------------------+
	| When remapped to OCRAM    | 0x00000000 - 0x0000FFFF | Normal, RWX, inner-cacheable, shareable                   |
	| When remapped to Boot ROM | 0x00000000 - 0x0000FFFF | Normal, RO, inner & outer-cacheable, shareable            |
	+-----------------------------------------------------------------------------------------------------------------+

	The Cortex-A9 in Cyclone V SoC has Global Monitors so shareable attribute is supported, and is required for data
	coherence support for AXI bridge mapped regions when accessing cached regions - required when the FPGA is using the
	AXI bridges to access SDRAM.

	There are two table modes
	-------------------------

	L1 only table mode:
	Enabling only L1 translation table supports only 1MB section (or 16MB section if the processor supports it).
	A 1MB section take up 1 table entry in the L1 table
	A 16MB section take up 16 table entries in the L1 table

	L1 + L2 table mode:
	Note, in order to use 64K and 4K pages you need to also enable L2 translation tables.
	A 64K page take up 1 table entry in the L1 table, and 16 table entries in the L2 table
	A 4K page take up 1 table entry in the L1 table, and 1 table entries in the L2 table
	When L2 table is enabled and TTBCR register attribute is set N > 0, then L1 table's input (VA) address is limited to an upper boundary.
	This means swapping is needed, because there is not enough table entries for all the regions, making it very difficult to use.

	References:
		- Cyclone V Hard Processor System Technical Reference Manual
		  Notable sections:
		- Figure 8-4: Address Maps for System Interconnect Masters

		- ARM Architecture v7-A ref manual
		  Notable sections:
		- B3.5.4 Selecting between TTBR0 and TTBR1, Short-descriptor translation table format
		- B4.1.153 TTBCR, Translation Table Base Control Register, VMSA
		- B4.1.154 TTBR0, Translation Table Base Register 0, VMSA
		- B4.1.155 TTBR1, Translation Table Base Register 1, VMSA
*/

// L1 Cache info and restrictions about architecture of the caches (CCSIR register):
// Write-Through support *not* available
// Write-Back support available.
// Read allocation support available.
// Write allocation support available.

// Note: You should use the Shareable attribute carefully.
// For cores without coherency logic (such as SCU) marking a region as shareable forces the processor to not cache that region regardless of the inner cache settings.
// Cortex-A versions of RTX use LDREX/STREX instructions relying on Local monitors. Local monitors will be used only when the region gets cached, regions that are not cached will use the Global Monitor.
// Some Cortex-A implementations do not include Global Monitors, so wrongly setting the attribute Shareable may cause STREX to fail.
   
// Recall: When the Shareable attribute is applied to a memory region that is not Write-Back, Normal memory, data held in this region is treated as Non-cacheable.
// When SMP bit = 0, Inner WB/WA Cacheable Shareable attributes are treated as Non-cacheable.
// When SMP bit = 1, Inner WB/WA Cacheable Shareable attributes are treated as Cacheable.
   
// Following MMU configuration is expected
// SCTLR.AFE == 1 (Simplified access permissions model - AP[2:1] define access permissions, AP[0] is an access flag)
// SCTLR.TRE == 0 (TEX remap disabled, so memory type and attributes are described directly by bits in the descriptor)
// Domain 0 is always the Client domain
// Descriptors should place all memory in domain 0

#include "mmu_c5soc.h"
#include <stdint.h>

#if defined(__ICCARM__)
	#define MMU_L1_SECTION _Pragma("location=\"mmu_ttb_l1_entries\"")
#else
	#define MMU_L1_SECTION __attribute__((section("mmu_ttb_l1_entries")))
#endif

MMU_L1_SECTION uint8_t mmu_ttb_l1[L1_SIZE];

#if defined(TRU_DMA_BUFFER_NONCACHEABLE) && TRU_DMA_BUFFER_NONCACHEABLE == 1U && defined(TRU_MMU) && TRU_MMU == 1U
	extern uint32_t __dma_buffer_start;  // Reference external symbol name from the linker file
	extern uint32_t __dma_buffer_end;  // Reference external symbol name from the linker file
#endif

void *mmu_get_ttb_l1(void){
	return mmu_ttb_l1;
}

#if(USE_L1_AND_L2_TABLE == 0U)
	// Use L1 translation table only
	void MMU_CreateTranslationTable(void){
		mmu_region_attributes_Type region;
		uint32_t L1_Section_Attrib_Normal_RWX;  // 1MB Section descriptor with attributes: normal, RWX, shared, cacheable
		uint32_t L1_Section_Attrib_Device_RW;   // 1MB Section descriptor with attributes: device, RW, shared, non-cacheable

		region.rg_t = SECTION;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = WB_WA;  // Inner = L1 cache
		region.outer_norm_t = WB_WA;  // Outer = L2 cache
		region.mem_t = NORMAL;
		region.sec_t = SECURE;
		region.xn_t = EXECUTE;
		region.priv_t = RW;
		region.user_t = RW;
		region.sh_t = SHARED;
		MMU_GetSectionDescriptor(&L1_Section_Attrib_Normal_RWX, region);

		region.rg_t = SECTION;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = NON_CACHEABLE;
		region.outer_norm_t = NON_CACHEABLE;
		region.mem_t = SHARED_DEVICE;
		region.sec_t = SECURE;
		region.xn_t = NON_EXECUTE;
		region.priv_t = RW;
		region.user_t = RW;
		region.sh_t = SHARED;
		MMU_GetSectionDescriptor(&L1_Section_Attrib_Device_RW, region);

		// Fill MMU level 1 table with entries
		// ===================================
		// We will use level 1 table size of 16KB and no level 2 table.
		// This configuration has these limitations:
		//   L1 max entries = 16384 / 4 = 4096
		//   Due to 1MB granularity (size and alignment), it is not possible to have separate sections for these mis-aligned regions: peripherals/L3, BootROM, SCU/L2 and OCRAM
		MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_RAM_BASE, 3072U, L1_Section_Attrib_Normal_RWX);   // Define 1MB sections for 3GB SDRAM region
		MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_H2F_BASE, 960U, L1_Section_Attrib_Device_RW);     // Define 1MB sections for H2F region
		MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_STM_BASE, 48U, L1_Section_Attrib_Device_RW);      // Define 1MB sections for STM region
		MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_DAP_BASE, 2U, L1_Section_Attrib_Device_RW);       // Define 1MB sections for DAP region
		MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_L2F_BASE, 2U, L1_Section_Attrib_Device_RW);       // Define 1MB sections for L2F region
		MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_PERI_L3_BASE, 12U, L1_Section_Attrib_Device_RW);  // Define 1MB sections for the combined regions peripherals/L3, BootROM, SCU/L2 and OCRAM
		// -----------------------
		// Total L1 entries = 4096

		/* Set location of level 1 page table.  Bit assignments:
				31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
				13:7  - 0x0
				6     - IRGN[0]      (See below #1)
				5     - NOS          (0 = Non-shared, 1 = Shared)
				4:3   - RGN          (See below #2)
				2     - IMP          (Implementation Defined)
				1     - S            (0 = Non-shared, 1 = Shared)
				0     - C or IRGN[1] (See below #1)
			Note #1
				Without Multiprocessing Extensions:
					bit 0 = C =
						0 Inner Non-cacheable.
						1 Inner Cacheable.
				With Multiprocessing Extensions:
					bits 6 & 0 = IRGN[1:0] =
						0b00 Normal memory, Inner Non-cacheable.
						0b01 Normal memory, Inner Write-Back Write-Allocate Cacheable.
						0b10 Normal memory, Inner Write-Through Cacheable.
						0b11 Normal memory, Inner Write-Back no Write-Allocate Cacheable.
			Note #2
				RGN =
					0b00 Normal memory, Outer Non-cacheable.
					0b01 Normal memory, Outer Write-Back Write-Allocate Cacheable.
					0b10 Normal memory, Outer Write-Through Cacheable.
					0b11 Normal memory, Outer Write-Back no Write-Allocate Cacheable. */

		// Enable L1 translation table
		//__set_TTBR0((uint32_t)mmu_ttb_l1 | 0x5b);  // Set TTBR0.  Set level 1 translation table base address and table walk settings
		__set_CP(15, 0, (uint32_t)mmu_ttb_l1 | 0x5b, 2, 0, 0);  // Set TTBR0.  Set level 1 translation table base address and table walk settings
		__ISB();

		// Set up domain access control register
		__set_DACR(1);    // Client access. Accesses are checked against the permission bits in the translation table, i.e. apply permission from table settings
		//__set_DACR(3);  // Manager access. Accesses are not checked against the permission bits in the translation table, i.e. ignore permission from table settings, unrestricted access
		__ISB();
	}
#else
	#if defined(__ICCARM__)
		#define MMU_L2_SECTION _Pragma("location=\"mmu_ttb_l2_entries\"")
	#else
		#define MMU_L2_SECTION __attribute__((section("mmu_ttb_l2_entries")))
	#endif

	MMU_L2_SECTION uint8_t mmu_ttb_l2[16384];

	// WARNING: Due to the L1 + L2 table restrictions this does not cover the required memory regions and is incomplete, i.e. not usable really!!
	// This only serve as a sample how to
	// Use L1 + L2 translation table
	void MMU_CreateTranslationTable(void){
		mmu_region_attributes_Type region;
		uint32_t L1_Section_Attrib_Normal_RWX;  // 1MB Section descriptor with attributes: normal, RWX, shared, cacheable
		uint32_t L1_Section_Attrib_Device_RW;   // 1MB Section descriptor with attributes: device, RW, shared, non-cacheable
		uint32_t L1_64k_Attrib_Normal_RWX;      // 64K page descriptor with attributes: normal, RWX, shared, cacheable
		uint32_t L1_64k_Attrib_Device_RW;       // 64K page descriptor with attributes: device, RW, shared, non-cacheable
		uint32_t L1_4k_Attrib_Device_RW;        // 4K page descriptor with attributes: device, RW, shared, non-cacheable
		uint32_t L1_4k_Attrib_Device_R;         // 4K page descriptor with attributes: device, R, shared, non-cacheable
		uint32_t L2_64k_Attrib_Normal_RWX;      // 64K page descriptor with attributes: normal, RWX, shared, cacheable
		uint32_t L2_64k_Attrib_Device_RW;       // 64K page descriptor with attributes: device, RW, shared, non-cacheable
		uint32_t L2_4k_Attrib_Device_RW;        // 4K page descriptor with attributes: device, RW, shared, non-cacheable
		uint32_t L2_4k_Attrib_Device_R;         // 4K page descriptor with attributes: device, R, shared, non-cacheable

		region.rg_t = SECTION;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = WB_WA;  // Inner = L1 cache
		region.outer_norm_t = WB_WA;  // Outer = L2 cache
		region.mem_t = NORMAL;
		region.sec_t = SECURE;
		region.xn_t = EXECUTE;
		region.priv_t = RW;
		region.user_t = RW;
		region.sh_t = SHARED;
		MMU_GetSectionDescriptor(&L1_Section_Attrib_Normal_RWX, region);

		region.rg_t = SECTION;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = NON_CACHEABLE;
		region.outer_norm_t = NON_CACHEABLE;
		region.mem_t = SHARED_DEVICE;
		region.sec_t = SECURE;
		region.xn_t = NON_EXECUTE;
		region.priv_t = RW;
		region.user_t = RW;
		region.sh_t = SHARED;
		MMU_GetSectionDescriptor(&L1_Section_Attrib_Device_RW, region);

		region.rg_t = PAGE_64k;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = WB_WA;
		region.outer_norm_t = WB_WA;
		region.mem_t = NORMAL;
		region.sec_t = SECURE;
		region.xn_t = EXECUTE;
		region.priv_t = RW;
		region.user_t = RW;
		region.sh_t = SHARED;
		MMU_GetPageDescriptor(&L1_64k_Attrib_Normal_RWX, &L2_64k_Attrib_Normal_RWX, region);

		region.rg_t = PAGE_64k;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = NON_CACHEABLE;
		region.outer_norm_t = NON_CACHEABLE;
		region.mem_t = SHARED_DEVICE;
		region.sec_t = SECURE;
		region.xn_t = NON_EXECUTE;
		region.priv_t = RW;
		region.user_t = RW;
		region.sh_t = SHARED;
		MMU_GetPageDescriptor(&L1_64k_Attrib_Device_RW, &L2_64k_Attrib_Device_RW, region);

		region.rg_t = PAGE_4k;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = NON_CACHEABLE;
		region.outer_norm_t = NON_CACHEABLE;
		region.mem_t = SHARED_DEVICE;
		region.sec_t = SECURE;
		region.xn_t = NON_EXECUTE;
		region.priv_t = RW;
		region.user_t = RW;
		region.sh_t = SHARED;
		MMU_GetPageDescriptor(&L1_4k_Attrib_Device_RW, &L2_4k_Attrib_Device_RW, region);

		region.rg_t = PAGE_4k;
		region.domain = 0x0;
		region.e_t = ECC_DISABLED;
		region.g_t = GLOBAL;
		region.inner_norm_t = NON_CACHEABLE;
		region.outer_norm_t = NON_CACHEABLE;
		region.mem_t = SHARED_DEVICE;
		region.sec_t = SECURE;
		region.xn_t = NON_EXECUTE;
		region.priv_t = READ;
		region.user_t = READ;
		region.sh_t = SHARED;
		MMU_GetPageDescriptor(&L1_4k_Attrib_Device_R, &L1_4k_Attrib_Device_R, region);

		// Fill MMU level 1 and level 2 table with entries
		// ===============================================
		// This is configured for use with level 1 table size of 8KB and level 2 table size of 16KB.
		// This configuration has these limitations:
		//   L1 max table entries = 8196 / 4  = 2048
		//   L2 max table entries = 16384 / 4 = 4096
		//   L1 input addresses must be below the first L2 VA address, in this case = 0x80000000, i.e. section entry addresses must be below this
		//
		// Sizes:
		//   1M section take up 1 L1 table entry
		//   16MB super section take up 16 L1 table entries is optional so the processor may not support it
		//   4KB page take up 1 L1 table entry and 1 L2 table entry
		//   16KB page take up 1 L1 table entry and 16 L2 table entry
		//
		// Note 64K pages is actually treated as a block of 16 * 4K pages, so it will take up 16 entries inside the MMU table
		// The reason for using it is because the 64K block is faster to process than 16 * 4K pages
		//
		// Due to the limitations of the level 1 and level 2 table scheme we don't have enough table entries for all memory regions

		// First initialise all L1 entries with fault entries
		MMU_TTSection((uint32_t *)mmu_ttb_l1, 0U, 2048U, DESCRIPTOR_FAULT);

		// Set up 1MB sections
		// Note: memoory regions H2F, STM, DAP, L2F are all below L2 VA boundary address so they cannot use 4k or 64k pages
		// Problem we don't have enough MMU table entries to enable these!!
		MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_RAM_BASE, 1024U, L1_Section_Attrib_Normal_RWX);     // Define 1MB sections for 1GB SDRAM region
		//MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_H2F_BASE, 960U, L1_Section_Attrib_Device_RW);     // Define 1MB sections for H2F region
		//MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_STM_BASE, 48U, L1_Section_Attrib_Device_RW);      // Define 1MB sections for STM region
		//MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_DAP_BASE, 2U, L1_Section_Attrib_Device_RW);       // Define 1MB sections for DAP region
		//MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_L2F_BASE, 2U, L1_Section_Attrib_Device_RW);       // Define 1MB sections for L2F region
		//MMU_TTSection((uint32_t *)mmu_ttb_l1, C5SOC_PERI_L3_BASE, 11U, L1_Section_Attrib_Device_RW);  // Define 1MB sections for regions covering most part of peripherals/L3
		// ----------------------------------------------------------------------------------------------------
		// Total L1 entries used if including uncommented ones = 2047, then the remaining L1 entry is 1 - ouch!
		// Total L1 entries used = 1024

		// Set up 4k + 64k pages
		MMU_TTPage64k((uint32_t *)mmu_ttb_l1, C5SOC_PERI_L3_BASE + 0x00b00000UL, 13U, L1_64k_Attrib_Device_RW, (uint32_t *)mmu_ttb_l2, L2_64k_Attrib_Device_RW);  // Define 64k pages for peripherals/L3 GPV region part 2
		MMU_TTPage4k((uint32_t *)mmu_ttb_l1, C5SOC_BOOTROM_BASE, 28U, L1_4k_Attrib_Device_R, (uint32_t *)mmu_ttb_l2, L2_4k_Attrib_Device_R);                      // Define 4k pages for BootROM
		MMU_TTPage4k((uint32_t *)mmu_ttb_l1, C5SOC_SCU_L2_BASE, 4U, L1_4k_Attrib_Device_RW, (uint32_t *)mmu_ttb_l2, L2_4k_Attrib_Device_RW);                      // Define 4k pages for SCU
		MMU_TTPage64k((uint32_t *)mmu_ttb_l1, C5SOC_OCRAM_BASE, 1U, L1_64k_Attrib_Normal_RWX, (uint32_t *)mmu_ttb_l2, L2_64k_Attrib_Normal_RWX);                  // Define 64k pages for OCRAM
		// ---------------------------------------------------
		// Total L1 entries used = 13    + 28 + 4 + 1    = 46
		// Total L2 entries used = 13*16 + 28 + 4 + 1*16 = 256

		// Final total of entries used
		// ---------------------------
		// Total L1 entries used = 1024 + 46 = 1070
		// Total L2 entries used = 256

		// It is not possible to enable the uncommented entries
		// ----------------------------------------------------
		// Total L1 entries used = 2047 + 46 = 2093 (INVALID!!  Must be 2048 or less)
		// Total L2 entries used = 256

		/* Set location of level 1 page table.  Bit assignments:
				31:14 - Translation table base addr (31:14-TTBCR.N, TTBCR.N is 0 out of reset)
				13:7  - 0x0
				6     - IRGN[0]      (See below #1)
				5     - NOS          (0 = Non-shared, 1 = Shared)
				4:3   - RGN          (See below #2)
				2     - IMP          (Implementation Defined)
				1     - S            (0 = Non-shared, 1 = Shared)
				0     - C or IRGN[1] (See below #1)
			Note #1
				Without Multiprocessing Extensions:
					bit 0 = C =
						0 Inner Non-cacheable.
						1 Inner Cacheable.
				With Multiprocessing Extensions:
					bits 6 & 0 = IRGN[1:0] =
						0b00 Normal memory, Inner Non-cacheable.
						0b01 Normal memory, Inner Write-Back Write-Allocate Cacheable.
						0b10 Normal memory, Inner Write-Through Cacheable.
						0b11 Normal memory, Inner Write-Back no Write-Allocate Cacheable.
			Note #2
				RGN =
					0b00 Normal memory, Outer Non-cacheable.
					0b01 Normal memory, Outer Write-Back Write-Allocate Cacheable.
					0b10 Normal memory, Outer Write-Through Cacheable.
					0b11 Normal memory, Outer Write-Back no Write-Allocate Cacheable. */

		// Enable L1 and L2 tables
		// See B3.5.4 Selecting between TTBR0 and TTBR1, Short-descriptor translation table format from ARM Architecture v7-A ref manual
		__set_CP(15, 0, (uint32_t)mmu_ttb_l1 | 0x5b, 2, 0, 0);  // Set TTBR0.  Set level 1 translation table base address and table walk settings
		__set_CP(15, 0, (uint32_t)mmu_ttb_l2 | 0x5b, 2, 0, 1);  // Set TTBR1.  Set level 2 translation Table base address and table walk settings
		__set_CP(15, 0, USE_TTBCR_N, 2, 0, 2);  // Set TTBCR.  Select short-descriptor format, use level 1 and level 2 translation tables
		__ISB();

		// Set up domain access control register
		__set_DACR(1);    // Client access. Accesses are checked against the permission bits in the translation table, i.e. apply permission from table settings
		//__set_DACR(3);  // Manager access. Accesses are not checked against the permission bits in the translation table, i.e. ignore permission from table settings, unrestricted access
		__ISB();
	}
#endif

#if defined(TRU_DMA_BUFFER_NONCACHEABLE) && TRU_DMA_BUFFER_NONCACHEABLE == 1U && defined(TRU_MMU) && TRU_MMU == 1U
	// Note: this assumes the MMU table is using L1 entries with 1MB sections
	void mmu_create_dma_buffer_table_entries(void){
		uint32_t dma_buffer_size = &__dma_buffer_end - &__dma_buffer_start;

		if(dma_buffer_size){
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
			uint32_t noncache_num_sections = (dma_buffer_size % 1048576UL) ? dma_buffer_size / 1048576UL + 1 : dma_buffer_size / 1048576UL;  // Calc number of 1MB MMU sections rounding up
			uint32_t *mmu_ttb_l1 = mmu_get_ttb_l1();

			MMU_GetSectionDescriptor(&L1_Section_Attrib_NonCache_RWX, region);  // Fill section attribute variable
			MMU_TTSection(mmu_ttb_l1, (uint32_t)&__dma_buffer_start, noncache_num_sections, DESCRIPTOR_FAULT);  // Replace the old translation table entry with an invalid (faulting) entry
			// Clean not required with the Multiprocessing Extensions
			//uint32_t offset = (uint32_t)stream0.xfer_addr >> 20U;
			//tru_l1_data_clean_range(mmu_ttb_l1 + offset, 4U * noncache_num_sections);
			__DSB();  // Ensure faulting entry is visible
			MMU_InvalidateRange(mmu_ttb_l1, (uint32_t)&__dma_buffer_start, noncache_num_sections);  // Invalidate TLB entries by MVA with Multiprocessing Extension support
			__set_BPIALL(0);  // Invalidate entire branch predictor array
			__DSB();  // Ensure completion of the invalidate branch predictor operation
			__ISB();  // Ensure changes visible to instruction fetch
			MMU_TTSection(mmu_ttb_l1, (uint32_t)&__dma_buffer_start, noncache_num_sections, L1_Section_Attrib_NonCache_RWX);  // Write MMU table 1MB section entries that are non-cacheable
			__DSB();  // Ensure the new entry is visible
		}
	}
#endif
