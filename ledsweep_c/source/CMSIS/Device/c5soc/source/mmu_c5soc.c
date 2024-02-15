/**************************************************************************//**
 * @file     system_c5soc.c
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

   Processor memory map of DE-10 Nano processor system (HPS).

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
	| When remapped to Boot ROM |                         | Normal, RO, inner & outer-cacheable, shareable            |
	+-----------------------------------------------------------------------------------------------------------------+

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

#include <stdint.h>
#include "c5soc.h"

// L2 table pointers
//-----------------------------------------------------
#define PRIVATE_TABLE_L2_BASE_4k       (0x80504000) //Map 4k Private Address space
#define SYNC_FLAGS_TABLE_L2_BASE_4k    (0x80504C00) //Map 4k Flag synchronization
#define PERIPHERAL_A_TABLE_L2_BASE_64k (0x80504400) //Map 64k Peripheral #1 
#define PERIPHERAL_B_TABLE_L2_BASE_64k (0x80504800) //Map 64k Peripheral #2 

//--------------------- PERIPHERALS -------------------
#define PERIPHERAL_A_FAULT             (0x00000000 + 0x1C000000) 
#define PERIPHERAL_B_FAULT             (0x00100000 + 0x1C000000) 

//--------------------- SYNC FLAGS --------------------
#define FLAG_SYNC                       0xFFFFF000
#define F_SYNC_BASE                     0xFFF00000  //1M aligned

//Import symbols from linker
//extern uint32_t Image$$VECTORS$$Base;
//extern uint32_t Image$$RW_DATA$$Base;
//extern uint32_t Image$$ZI_DATA$$Base;
extern uint32_t Image$$TTB$$ZI$$Base;

// TTB (Translation Table Base) address.
// I've defined TTB A and TTB B sections to allow switching of L1 and L2 assignment
// If L1 table only then TTB A = L1 table, else TTB A = L2 table and TTB B = L1 table
#define TTB_L2_SIZE 0x4000
#define TTB_A_BASE ((uint32_t)&Image$$TTB$$ZI$$Base)
#define TTB_B_BASE (((uint32_t)&Image$$TTB$$ZI$$Base) + TTB_L2_SIZE)

// TTBxx register values for setting the L1 table size and VA range (see ARM Architecture v7-A ref manual)
#define TTBCR_N_L1_16K        0x0
#define TTBCR_N_L1_8K_L2_16K  0x1
#define TTBCR_N_L1_4K_L2_16K  0x2
#define TTBCR_N_L1_2K_L2_16K  0x3
#define TTBCR_N_L1_1K_L2_16K  0x4
#define TTBCR_N_L1_512_L2_16K 0x5
#define TTBCR_N_L1_256_L2_16K 0x6
#define TTBCR_N_L1_128_L2_16K 0x7

// Layout 1: Use only L1 table
void MMU_CreateTranslationTable(void){
	mmu_region_attributes_Type region;
	uint32_t L1_Section_Attrib_Normal_RWX;  // 1MB Section descriptor with attributes: normal, RWX, shared, cacheable
	uint32_t L1_Section_Attrib_Device_RW;   // 1MB Section descriptor with attributes: device, RW, shared, non-cacheable

	region.rg_t = SECTION;
	region.domain = 0x0;
	region.e_t = ECC_DISABLED;
	region.g_t = GLOBAL;
	region.inner_norm_t = WB_WA;
	region.outer_norm_t = WB_WA;
	region.mem_t = NORMAL;
	region.sec_t = NON_SECURE;
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
	//   We will overlap those regions with 1MB sections and we will also have to set memory type to shared device, non-executable
	MMU_TTSection((uint32_t *)TTB_A_BASE, C5SOC_RAM_BASE, 3072U, L1_Section_Attrib_Normal_RWX);   // Define 1MB sections for 3GB SDRAM region
	MMU_TTSection((uint32_t *)TTB_A_BASE, C5SOC_H2F_BASE, 960U, L1_Section_Attrib_Device_RW);     // Define 1MB sections for H2F region
	MMU_TTSection((uint32_t *)TTB_A_BASE, C5SOC_STM_BASE, 48U, L1_Section_Attrib_Device_RW);      // Define 1MB sections for STM region
	MMU_TTSection((uint32_t *)TTB_A_BASE, C5SOC_DAP_BASE, 2U, L1_Section_Attrib_Device_RW);       // Define 1MB sections for DAP region
	MMU_TTSection((uint32_t *)TTB_A_BASE, C5SOC_L2F_BASE, 2U, L1_Section_Attrib_Device_RW);       // Define 1MB sections for L2F region
	MMU_TTSection((uint32_t *)TTB_A_BASE, C5SOC_PERI_L3_BASE, 12U, L1_Section_Attrib_Device_RW);  // Define 1MB sections for the combined regions peripherals/L3, BootROM, SCU/L2 and OCRAM

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
	//__set_TTBR0(TTB_A_BASE | 0x5b);  // Set TTBR0.  Enable level 1 Translation Table
	__set_CP(15, 0, TTB_A_BASE | 0x5b, 2, 0, 0);  // Set TTBR0.  Enable level 1 Translation Table
	//__set_CP(15, 0, TTB_A_BASE | 0x59, 2, 0, 0);  // Set TTBR0.  Enable level 1 Translation Table
	//__set_CP(15, 0, TTBCR_N_L1_16K, 2, 0, 2);  // Set TTBCR.  Select short-descriptor format, use level 1 and level 2 tables
	__ISB();

	// Example of enabling L1 and L2 tables, but due to table size and range limitations, we will not be using it, nor have we defined any table entries above anyway
	// If only Arm allowed table split with independent input VA (Vector Address) index range for L1 and L2 tables
	// See B3.5.4 Selecting between TTBR0 and TTBR1, Short-descriptor translation table format from ARM Architecture v7-A ref manual
	// The L2 table scheme for Cortex A9 is useless for bare-metal, i.e. too much work to handle fault exception and switch in dynamic table entries for pages
	//__set_CP(15, 0, TTB_A_BASE | 0x5b, 2, 0, 0);  // Set TTBR0.  Enable level 1 Translation Table
	//__set_CP(15, 0, TTB_A_BASE | 0x5b, 2, 0, 1);  // Set TTBR1.  Set level 2 Translation Table base address and table walk settings
	//__set_CP(15, 0, TTBCR_N_L1_8K_L2_16K, 2, 0, 2);  // Set TTBCR.  Select short-descriptor format, use level 1 and level 2 tables
	//__ISB();

	// Set up domain access control register
	__set_DACR(1);    // Client access. Accesses are checked against the permission bits in the translation tables, i.e. apply table entry setting
	//__set_DACR(3);  // Manager access. Accesses are not checked against the permission bits in the translation tables, i.e. ignore table entry setting, unrestricted access
	__ISB();
}
