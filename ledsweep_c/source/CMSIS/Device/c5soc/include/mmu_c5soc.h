/*
	Created on: 26 Oct 2024
	Author: Truong Hy
*/

#ifndef MMU_C5SOC_H
#define MMU_C5SOC_H

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
		#define L1_SIZE_ALIGNMENT 8192
		#define L2_FIRST_VA_ADDR 0x80000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_4K_L2_16K)
		#define L1_SIZE_ALIGNMENT 4096
		#define L2_FIRST_VA_ADDR 0x40000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_2K_L2_16K)
		#define L1_SIZE_ALIGNMENT 2048
		#define L2_FIRST_VA_ADDR 0x20000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_1K_L2_16K)
		#define L1_SIZE_ALIGNMENT 1024
		#define L2_FIRST_VA_ADDR 0x10000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_512_L2_16K)
		#define L1_SIZE_ALIGNMENT 512
		#define L2_FIRST_VA_ADDR 0x08000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_256_L2_16K)
		#define L1_SIZE_ALIGNMENT 256
		#define L2_FIRST_VA_ADDR 0x04000000
	#elif(USE_TTBCR_N == TTBCR_N_L1_128_L2_16K)
		#define L1_SIZE_ALIGNMENT 128
		#define L2_FIRST_VA_ADDR 0x02000000
	#endif
#else
	#define L1_SIZE_ALIGNMENT 16384
#endif

void *get_mmu_ttb(void);

#endif
