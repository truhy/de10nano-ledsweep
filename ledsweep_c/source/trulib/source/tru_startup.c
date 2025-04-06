/*
	MIT License

	Copyright (c) 2023 Truong Hy

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

	Bare-metal C startup initialisations for the Intel Cyclone V SoC (HPS), ARM Cortex-A9.
	My own standalone init functions.

	References:
		- Cyclone V SoC: Cyclone V Hard Processor System Technical Reference Manual
		- MMU          : ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition. Notable refs: B3.5.1 Short-descriptor translation table format descriptors
		- L1 cache     : ARM Cortex™-A9 Technical Reference Manual. Notable refs: Chapter 4.3 Register descriptions
		- L2 cache     : ARM CoreLink™ Level 2 Cache Controller L2C-310 Technical Reference Manual
		- SCU          : ARM Cortex™-A9 MPCore Technical Reference Manual
*/

#include "tru_config.h"

#if defined(TRU_STARTUP) && TRU_STARTUP == 1U

#include "tru_cortex_a9.h"
#include "alt_interrupt.h"
#include "tru_mmu.h"
#include <stdint.h>

#if defined(ALT_INT_PROVISION_VECTOR_SUPPORT) && ALT_INT_PROVISION_VECTOR_SUPPORT == 0U
	// Exception & interrupt handler supporting CMSIS
	void Default_Handler(void);
	void Undef_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
	void SVC_Handler(void)   __attribute__ ((weak, alias("Default_Handler")));
	void PAbt_Handler(void)  __attribute__ ((weak, alias("Default_Handler")));
	void DAbt_Handler(void)  __attribute__ ((weak, alias("Default_Handler")));
	void IRQ_Handler(void)   __attribute__ ((weak, alias("Default_Handler")));
	void FIQ_Handler(void)   __attribute__ ((weak, alias("Default_Handler")));
	void Vectors(void)       __attribute__ ((naked, section("RESET")));

	void Vectors(void){
		__asm__ volatile(
			"LDR pc, =Reset_Handler                              \n"
			"LDR pc, =Undef_Handler                              \n"
			"LDR pc, =SVC_Handler                                \n"
			"LDR pc, =PAbt_Handler                               \n"
			"LDR pc, =DAbt_Handler                               \n"
			"NOP                                                 \n"
			"LDR pc, =IRQ_Handler                                \n"
			"LDR pc, =FIQ_Handler                                \n"
		);
	}

	// Unhandled interrupt handler
	void Default_Handler(void){
		while(1);
	}
#endif

#if defined(TRU_EXIT_TO_UBOOT) && TRU_EXIT_TO_UBOOT == 1U
	#define RESET_ARGS int argc, char *const argv[]
#else
	#define RESET_ARGS void
#endif

// =============
// Reset handler
// =============

// Notes:
// - Newlib will initialise the .bss section for us so no need to do it here
// - In Altera's HWLib their vector table is named __intc_interrupt_vector, see alt_interrupt.c
// - In Altera's HWLib their vector table sets _socfpga_main as the reset handler, see alt_interrupt.c
void __attribute__((naked)) Reset_Handler(RESET_ARGS){
	__asm__ volatile(
#if defined(ALT_INT_PROVISION_VECTOR_SUPPORT) && ALT_INT_PROVISION_VECTOR_SUPPORT == 0U
		// Reference our vector and startup
		".set VBAR_TBL, Vectors                             \n"
#else
		// Reference Altera's HWLib vector and startup
		".set VBAR_TBL, __intc_interrupt_vector             \n"  // In Altera's HWLib, their vector table is named __intc_interrupt_vector, located in alt_interrupt.c
		".globl _socfpga_main                               \n"
		"_socfpga_main:                                     \n"  // In Altera's HWLib, their reset handler is named _socfpga_main, located in alt_interrupt.c
#endif

		// =========
		// Constants
		// =========

		// Arm MPCORE peripheral (private memory region) registers.  Only the peripheral base address is vendor specific, the offsets are the same for all other vendors
		".set PERIPH_BASE, 0xfffec000UL                     \n"  // This same address can also be determined from the coprocessor register: MRC p15, 4, r0, c15, c0, 0
		".set SCU_BASE,    (PERIPH_BASE + 0x0000U)          \n"

		// Arm CoreLink™ Level 2 Cache Controller L2C-310 registers.  Only the L2 base register is vendor specific, the offsets are the same for all other vendors
		".set L2_BASE,                0xfffef000UL          \n"
		".set L2_REG1_CTRL,           (L2_BASE + 0x100U)    \n"
		".set L2_REG1_AUX_CTRL,       (L2_BASE + 0x104U)    \n"
		".set L2_REG1_TAGRAM_CTRL,    (L2_BASE + 0x108U)    \n"
		".set L2_REG1_DATARAM_CTRL,   (L2_BASE + 0x10cU)    \n"
		".set L2_REG1_INT_CLR,        (L2_BASE + 0x220U)    \n"
		".set L2_REG15_DBG_CTRL,      (L2_BASE + 0xf40U)    \n"
		".set L2_REG15_PREFETCH_CTRL, (L2_BASE + 0xf60U)    \n"
		".set L2_REG9_D_LOCKDN0,      (L2_BASE + 0x900U)    \n"
		".set L2_REG7_CACHE_SYNC,     (L2_BASE + 0x730U)    \n"
		".set L2_REG7_INV_WAY,        (L2_BASE + 0x77cU)    \n"
		// Latency is vendor specific
		".set L2_TAG_LATENCY,         0x0U                  \n"
		".set L2_DATA_LATENCY,        0x10U                 \n"

		"CPSID if                                           \n"  // Mask interrupts

#if defined(TRU_EXIT_TO_UBOOT) && TRU_EXIT_TO_UBOOT == 1U
		// Save U-Boot argc
		"LDR r3, =uboot_argc                                \n"
		"STR r0, [r3]                                       \n"

		// Save U-Boot argv
		"LDR r3, =uboot_argv                                \n"
		"STR r1, [r3]                                       \n"

		// Save U-Boot return address
		"LDR r3, =uboot_lr                                  \n"
		"STR lr, [r3]                                       \n"

		// Save U-Boot processor mode
		"LDR r3, =uboot_cpsr                                \n"
		"MRS r2, cpsr                                       \n"
		"STR r2, [r3]                                       \n"

		// Save U-Boot stack pointer for each mode
		"CPS #0x11                                          \n"
		"LDR r3, =uboot_fiq_sp                              \n"
		"STR sp, [r3]                                       \n"
		"CPS #0x12                                          \n"
		"LDR r3, =uboot_irq_sp                              \n"
		"STR sp, [r3]                                       \n"
		"CPS #0x13                                          \n"
		"LDR r3, =uboot_svc_sp                              \n"
		"STR sp, [r3]                                       \n"
		"CPS #0x17                                          \n"
		"LDR r3, =uboot_abt_sp                              \n"
		"STR sp, [r3]                                       \n"
		"CPS #0x1B                                          \n"
		"LDR r3, =uboot_und_sp                              \n"
		"STR sp, [r3]                                       \n"
		"CPS #0x1F                                          \n"
		"LDR r3, =uboot_sys_sp                              \n"
		"STR sp, [r3]                                       \n"

		// Save VBAR (vector base address)
		"LDR r3, =uboot_vbar                                \n"
		"MRC p15, 0, r2, c12, c0, 0                         \n"
		"STR r2, [r3]                                       \n"
#endif

		// Put any cores other than 0 to sleep
		/*
		"MRC p15, 0, r0, c0, c0, 5                          \n"  // Read MPIDR
		"ANDS r0, r0, #3                                    \n"
		"goToSleep:                                         \n"
		"WFINE                                              \n"
		"BNE goToSleep                                      \n"
		*/

		// Switch into secure access mode
		"MRC p15, 0, r0, c1, c1, 2                          \n"  // Read NSACR (Non-secure Access Control Register)
		"ORR r0, r0, #(0x3 << 20)                           \n"  // Setup bits to enable access permissions.  Undocumented Altera/Intel Cyclone V SoC vendor specific
		"MCR p15, 0, r0, c1, c1, 2                          \n"  // Write NSACR
		"ISB                                                \n"

#if defined(TRU_CLEAN_CACHE) && TRU_CLEAN_CACHE == 1U
		// Since we are starting from U-Boot which may have the cache enabled,
		// loaded file(s) and some global variables may be cached and stay dirty.
		// Let's make sure that all dirty lines are written back into memory - in
		// case cache settings are changed later on
		"BL clean_l1_dcache_all                             \n"
#endif

		// Turn off caches and MMU
		"MRC p15, 0, r0, c1, c0, 0                          \n"  // Read SCTLR
		"BIC r0, r0, #(0x1 << 13)                           \n"  // Clear V bit 13 to disable hivecs
#if defined(TRU_L1_CACHE) && TRU_L1_CACHE != 2U
		"BIC r0, r0, #(0x1 << 12)                           \n"  // Clear I bit 12 to disable L1 I-cache
		"BIC r0, r0, #(0x1 << 11)                           \n"  // Clear Z bit 11 to disable branch prediction
		"BIC r0, r0, #(0x1 << 2)                            \n"  // Clear C bit 2 to disable L1 D-cache
#endif
#if defined(TRU_MMU) && TRU_MMU != 2U
		"BIC r0, r0, #(0x1 << 0)                            \n"  // Clear M bit 0 to disable MMU
#endif
		"MCR p15, 0, r0, c1, c0, 0                          \n"  // Write SCTLR
		"ISB                                                \n"  // Ensures changes have completed

		// ACTLR
		"MRC p15, 0, r0, c1, c0, 1                          \n"  // Read ACTLR
#if defined(TRU_SMP_COHERENCY) && TRU_SMP_COHERENCY != 2U
		"BIC r0, r0, #(0x1 << 6)                            \n"  // Disable participate in SMP coherency
		"BIC r0, r0, #(0x1 << 0)                            \n"  // Disable maintenance broadcast
#endif
		"BIC r0, r0, #(0x1 << 2)                            \n"  // Disable L1 dside prefetch
		"BIC r0, r0, #(0x1 << 1)                            \n"  // Disable L2 prefetch hint (UNK/WI since r4p1)
		"MCR p15, 0, r0, c1, c0, 1                          \n"  // Write ACTLR
		"ISB                                                \n"

#if defined(TRU_SCU) && TRU_SCU != 2U
		// Disable SCU
		"LDR r0, =SCU_BASE                                  \n"  // Load SCU base register
		"LDR r1, [r0, #0x0]                                 \n"  // Read SCU register
		"BIC r1, r1, #0x1                                   \n"  // Clear bit 0 (The Enable bit)
		"STR r1, [r0, #0x0]                                 \n"  // Write back modified value
#endif

#if defined(TRU_L2_CACHE) && TRU_L2_CACHE != 2U
		// Disable L2 cache
		"LDR r0, =L2_REG1_CTRL                              \n"  // Load L2 control register (reg1_control)
		"MOV r1, #0                                         \n"  // Value with bit cleared on the L2 cache enable bit
		"STR r1, [r0]                                       \n"  // Write

		// Setup L2 prefetch control register
		"LDR r0, =L2_REG15_PREFETCH_CTRL                    \n"
		"LDR r1, [r0, #0x0]                                 \n"  // Read prefetch control register
		"BIC r1, r1, #(0x1 << 29)                           \n"  // Disable L2 instruction prefetch
		"BIC r1, r1, #(0x1 << 28)                           \n"  // Disable L2 data prefetch
		"STR r1, [r0]                                       \n"  // Write back modified value

		// Setup L2 auxiliary control register (Note: this can only be set in secure access mode)
		"LDR r0, =L2_REG1_AUX_CTRL                          \n"
		"LDR r1, [r0]                                       \n"
		"BIC r1, r1, #(0x1 << 21)                           \n"  // Disable L2 parity
		"STR r1, [r0]                                       \n"
#endif

		// Set Vector Base Address Register (VBAR)
		"LDR r0, =VBAR_TBL                                  \n"  // Register the specified vector table
		"MCR p15, 0, r0, c12, c0, 0                         \n"

		// Setup stack for each exception mode
		// Note: When you call HWLib's interrupt init function the stacks will be change to a global variable array, and this setup will be dropped
		"CPS #0x11                                          \n"
		"LDR sp, =__FIQ_STACK_LIMIT                         \n"
		"CPS #0x12                                          \n"
		"LDR sp, =__IRQ_STACK_LIMIT                         \n"
		"CPS #0x13                                          \n"
		"LDR sp, =__SVC_STACK_LIMIT                         \n"
		"CPS #0x17                                          \n"
		"LDR sp, =__ABT_STACK_LIMIT                         \n"
		"CPS #0x1B                                          \n"
		"LDR sp, =__UND_STACK_LIMIT                         \n"
		"CPS #0x1F                                          \n"
		"LDR sp, =__SYS_STACK_LIMIT                         \n"

		// ============================================================================
		// Invalidate.  We must invalidate MMU and L1 cache first before we enable them
		// ============================================================================

#if defined(TRU_MMU) && TRU_MMU == 1U
		// Invalidate MMU
		"MOV r0, #0                                         \n"
		"MCR p15, 0, r0, c8, c7, 0                          \n"  // Invalidate MMU TLBs all (Translation Lookaside Buffers) (TLBIALL)
#endif

#if defined(TRU_L1_CACHE) && TRU_L1_CACHE == 1U
		// Invalidate L1 branch predictor all (BPIALL)
		"MOV r0, #0                                         \n"
		"MCR p15, 0, r0, c7, c5, 6                          \n"
		"DSB                                                \n"
		"ISB                                                \n"

		// Invalidate L1 instruction cache (ICIALLU)
		"MCR p15, 0, r0, c7, c5, 0                          \n"
		"DSB                                                \n"
		"ISB                                                \n"

		// Invalidate L1 data cache
		"BL invalidate_l1_dcache_all                        \n"
		"DSB                                                \n"
		"ISB                                                \n"
#endif

#if defined(TRU_NEON) && TRU_NEON == 1U
		// Enable permission and turn on NEON/VFP (FPU)
		"MRC p15, 0, r0, c1, c0, 2                          \n"  // Read CPACR (Coprocessor Access Control Register)
		"ORR r0, r0, #0x00F00000                            \n"  // Setup bits to enable access to NEON/VFP (Coprocessors 10 and 11)
		"MCR p15, 0, r0, c1, c0, 2                          \n"  // Write CPACR (Coprocessor Access Control Register)
		"ISB                                                \n"  // Ensures CPACR write have completed before continuing

		// Enable NEON
		"VMRS r0, fpexc                                     \n"  // Read FPEXC (Floating-Point Exception Control register)
		"ORR r0, r0, #0x40000000                            \n"  // Setup bits to enable the NEON/VFP (Advanced SIMD and floating-point extensions)
		"VMSR fpexc, r0                                     \n"  // Write FPEXC (Floating-Point Exception Control register)

		// Initialise VFP/NEON registers to 0
		"MOV r0, #0                                         \n"
		"VMOV d0, r0, r0                                    \n"
		"VMOV d1, r0, r0                                    \n"
		"VMOV d2, r0, r0                                    \n"
		"VMOV d3, r0, r0                                    \n"
		"VMOV d4, r0, r0                                    \n"
		"VMOV d5, r0, r0                                    \n"
		"VMOV d6, r0, r0                                    \n"
		"VMOV d7, r0, r0                                    \n"
		"VMOV d8, r0, r0                                    \n"
		"VMOV d9, r0, r0                                    \n"
		"VMOV d10, r0, r0                                   \n"
		"VMOV d11, r0, r0                                   \n"
		"VMOV d12, r0, r0                                   \n"
		"VMOV d13, r0, r0                                   \n"
		"VMOV d14, r0, r0                                   \n"
		"VMOV d15, r0, r0                                   \n"

		// Initialise FPSCR to a known state
		"VMRS r0, fpscr                                     \n"
		"LDR r1, =0x00086060                                \n"
		"AND r0, r0, r1                                     \n"
		"VMSR fpscr, r0                                     \n"
#endif

		// ====================
		// Setup and enable MMU
		// ====================

#if defined(TRU_MMU) && TRU_MMU == 1U
		// Register MMU table
		"LDR r0, =c5soc_mmu_tbl                             \n"  // Load MMU translation table base address
		"ORR r0, r0, #0x5b                                  \n"  // MMU attributes
		"MCR 15, 0, r0, c2, c0, 0                           \n"  // Register level-1 MMU translation table and attributes with the TTBR0 register
		"ISB                                                \n"  // Ensures changes have completed

		// Set MMU domain access
		"MOV r0, #0x1                                       \n"  // Client access. Accesses are checked against the permission bits in the translation tables, i.e. apply table entry setting
		"MCR p15, 0, r0, c3, c0, 0                          \n"  // Write DACR
		"ISB                                                \n"  // Ensures changes have completed

		// Enable MMU
		"MRC p15, 0, r0, c1, c0, 0                          \n"  // Read SCTLR
		"ORR r0, r0, #(0x1 << 29)                           \n"  // Set AFE bit to enable simplified access permissions model
		"BIC r0, r0, #(0x1 << 28)                           \n"  // Clear TRE bit to disable TEX remap
		"BIC r0, r0, #(0x1 << 1)                            \n"  // Clear A bit to disable strict alignment fault checking
		"ORR r0, r0, #(0x1 << 0)                            \n"  // Set M bit 0 to enable MMU
		"MCR p15, 0, r0, c1, c0, 0                          \n"  // Write SCTLR
		"ISB                                                \n"  // Ensures changes have completed
#endif

		// ===============
		// Enable L1 cache
		// ===============

#if defined(TRU_L1_CACHE) && TRU_L1_CACHE == 1U
		// Enable L1 caches
		"MRC p15, 0, r0, c1, c0, 0                          \n"  // Read SCTLR
		"ORR r0, r0, #(0x1 << 12)                           \n"  // Set I bit 12 to enable L1 I-cache
		"ORR r0, r0, #(0x1 << 2)                            \n"  // Set C bit 2 to enable L1 D-cache
		"MCR p15, 0, r0, c1, c0, 0                          \n"  // Write SCTLR
		"ISB                                                \n"  // Ensures changes have completed

		// Enable L1 cache branch prediction
		"MRC p15, 0, r0, c1, c0, 0                          \n"  // Read SCTLR
		"ORR r0, r0, #(0x1 << 11)                           \n"  // Set Z bit 11 to enable branch prediction
		"MCR p15, 0, r0, c1, c0, 0                          \n"  // Write SCTLR
		"ISB                                                \n"  // Ensures changes have completed
#endif

		// ==============================================
		// Initialise L2 cache via the controller L2C-310
		// ==============================================

#if defined(TRU_L2_CACHE) && TRU_L2_CACHE == 1U
		// Write L2 cache tag latency
		"LDR r0, =L2_REG1_TAGRAM_CTRL                       \n"
		"LDR r1, =L2_TAG_LATENCY                            \n"
		"STR r1, [r0]                                       \n"

		// Write L2 cache data latency
		"LDR r0, =L2_REG1_DATARAM_CTRL                      \n"
		"LDR r1, =L2_DATA_LATENCY                           \n"
		"STR r1, [r0]                                       \n"

		// Invalidate L2 cache
		"LDR r0, =L2_REG7_INV_WAY                           \n"
		"MOV r1, #0xffff                                    \n"
		"STR r1, [r0]                                       \n"

		// Poll L2 cache sync until done
		"LDR r0, =L2_REG7_CACHE_SYNC                        \n"
	"_poll_l2_sync:                                         \n"
		"LDR r1, [r0]                                       \n"
		"CMP r1, #0                                         \n"
		"BNE _poll_l2_sync                                  \n"

		// Clear any pending L2 cache interrupts
		"LDR r0, =L2_REG1_INT_CLR                           \n"
		"MOV r1, #0x000001ffUL                              \n"
		"STR r1, [r0]                                       \n"

		// Initialise L2 cache debug
		"LDR r0, =L2_REG15_DBG_CTRL                         \n"
		"MOV r1, #0                                         \n"
		"STR r1, [r0]                                       \n"

		// Initialise L2 cache lock down 0 by way
		"LDR r0, =L2_REG9_D_LOCKDN0                         \n"
		"MOV r1, #0                                         \n"
		"STR r1, [r0]                                       \n"

		// Enable L2 cache prefetch
		//"LDR r0, =L2_REG15_PREFETCH_CTRL                    \n"
		//"LDR r1, [r0, #0x0]                                 \n"  // Read prefetch control register
		//"ORR r1, r1, #(0x1 << 29)                           \n"  // Set instruction prefetch enable
		//"ORR r1, r1, #(0x1 << 28)                           \n"  // Set data prefetch enable
		//"STR r1, [r0]                                       \n"  // Write back modified value

		// Cache sync
		"LDR r0, =L2_REG7_CACHE_SYNC                        \n"
		"MOV r1, #0                                         \n"
		"STR r1, [r0]                                       \n"

		// Enable L2 cache
		"LDR r0, =L2_REG1_CTRL                              \n"  // Load L2 control register (reg1_control)
		"LDR r1, [r0]                                       \n"  // Read the register
		"ORR r1, r1, #1                                     \n"  // Set the L2 cache enable bit
		"STR r1, [r0]                                       \n"  // Enable the L2 cache

		// Cache sync
		"LDR r0, =L2_REG7_CACHE_SYNC                        \n"
		"MOV r1, #0                                         \n"
		"STR r1, [r0]                                       \n"

		// Enable L2 cache prefetch hint
		//"MRC p15, 0, r0, c1, c0, 1                          \n"  // Read ACTLR
		//"ORR r0, r0, #(0x1 << 1)                            \n"  // Enable L2 prefetch hint (UNK/WI since r4p1)
		//"MCR p15, 0, r0, c1, c0, 1                          \n"  // Write ACTLR
#endif

		// =======================================
		// Initialise the SCU (Snoop Control Unit)
		// =======================================

#if defined(TRU_SCU) && TRU_SCU == 1U
		// Invalidate SCU
		"LDR r0, =SCU_BASE                                  \n"  // Load SCU base register
		"LDR r1, =0xffff                                    \n"  // Value to write
		"STR r1, [r0, #0xc]                                 \n"  // Write to SCU Invalidate All register (0xfffec00c)

		// Enable SCU
		"LDR r0, =SCU_BASE                                  \n"  // Load SCU base register
		"LDR r1, [r0, #0x0]                                 \n"  // Read SCU register
		"ORR r1, r1, #0x1                                   \n"  // Set bit 0 (The Enable bit)
		"STR r1, [r0, #0x0]                                 \n"  // Write back modified value
#endif

		// =================================
		// Enable SMP coherency for this CPU
		// =================================

#if defined(TRU_SMP_COHERENCY) && TRU_SMP_COHERENCY == 1U
		// Enable SMP cache coherency support
		"MRC p15, 0, r0, c1, c0, 1                          \n"  // Read ACTLR
		"ORR r0, r0, #(0x1 << 22)                           \n"  // Set bit 22 to enable shared attribute override. Recommended for ACP data coherency from Cyclone V HPS tech ref
		"ORR r0, r0, #(0x1 << 6)                            \n"  // Set bit 6 to participate in SMP coherency
		//"ORR r0, r0, #(0x1 << 2)                            \n"  // Set bit 2 to enable L1 dside prefetch
		"ORR r0, r0, #(0x1 << 0)                            \n"  // Set bit 0 to enable maintenance broadcast
		"MCR p15, 0, r0, c1, c0, 1                          \n"  // Write ACTLR
#endif

#if defined(TRU_DMA_BUFFER_NONCACHEABLE) && TRU_DMA_BUFFER_NONCACHEABLE == 1U && defined(TRU_MMU) && TRU_MMU == 1U
		"BL tru_mmu_create_dma_buffer_table_entries         \n"
#endif

		"CPSIE if                                           \n"  // Unmask interrupts

		"B _mainCRTStartup                                  \n"  // Call C Run-Time library startup from newlib or libc, which will later call our main().  Alternatively, for newlib call BL _start (alias of the same function)
		//"BL _mainCRTStartup                                 \n"  // Call C Run-Time library startup from newlib or libc, which will later call our main().  Alternatively, for newlib call BL _start (alias of the same function)
		// We don't expect the above to return
		//"_infinity_loop:                                    \n"  // Catch unexpected main() return
		//"B _infinity_loop                                   \n"
	);
}

// =============================
// Override newlib _stack_init()
// =============================

// This makes newlib setup only the system stack
void _stack_init(void){
}

// ===============
// Cache functions
// ===============

// Unsigned integer log2(n) and rounded up to next integer function
uint32_t uint_log2_roundup(uint32_t n){
	if(n < 2U) return 0U;  // First 2 sequence is zero

	uint32_t t = n;
	uint32_t e = 0U;
	// Keep dividing by 2 until 0 or 1, the count will be the result
	while(t > 1U){
		t >>= 1U;
		e++;
	}

	if(n & 1U) e++;  // Roundup if is odd

	return e;
}

void invalidate_l1_dcache(uint32_t cl_imp){
	uint32_t ccsidr;
	uint32_t num_sets;
	uint32_t num_ways;
	uint32_t pos_way;
	uint32_t log2_num_ways;
	uint32_t log2_linesize;
	uint32_t index;

	__write_csselr(cl_imp << 1U);  // Write CSSELR (Cache Size SELection Register)
	__read_ccsidr(ccsidr);  // Read CCSIDR (Cache Size IDentification Register)

	num_ways = ((ccsidr & 0x00001FF8U) >> 3U) + 1U;
	log2_num_ways = uint_log2_roundup(num_ways);
	if(log2_num_ways > 32U) return;  // Error

	num_sets = ((ccsidr & 0x0FFFE000U) >> 13U) + 1U;
	log2_linesize = (ccsidr & 0x00000007U) + 4U;

	pos_way = 32U - log2_num_ways;
	for(uint32_t way = 0; way < num_ways; way++){
		for(uint32_t set = 0; set < num_sets; set++){
			index = (cl_imp << 1U) | ((num_sets - set - 1) << log2_linesize) | ((num_ways - way - 1) << pos_way);
			__write_dcisw(index);  // Write DCISW (Invalidate Data Cache Line (using Index))
		}
	}

	__dmb();
}

void invalidate_l1_dcache_all(void){
	uint32_t clidr;
	uint32_t cl_imp;  // Cache level implementation

	__read_clidr(clidr);  // Read CLIDR (Cache Level ID Register)

	// Iterate processor cache levels.  Note: Arm Cortex-A9 only implements level 1 cache,
	// so we only need to check the first level, but going through all so that code is generic.
	// Level 2 here refers to the cache implemented internally within the processor, not to be confused with the level 2 cache by the separate controller (L2C-310).
	for(uint32_t i = 0U; i < 7U; i++){
		cl_imp = (clidr >> 3U * i) & 0x7UL;  // Get cache level implementation

		// Is data cache implemented at this level?
		if((cl_imp >= 2U) && (cl_imp <= 4U)){
			invalidate_l1_dcache(cl_imp);
		}
	}
}

void clean_l1_dcache(uint32_t cl_imp){
	uint32_t sctlr;
	uint32_t ccsidr;
	uint32_t num_sets;
	uint32_t num_ways;
	uint32_t pos_way;
	uint32_t log2_num_ways;
	uint32_t log2_linesize;
	uint32_t index;

	__read_sctlr(sctlr);
	if((sctlr & (1 << 2)) == 0) return;  // Exit if L1 d-cache is off

	__write_csselr(cl_imp << 1U);  // Write CSSELR (Cache Size SELection Register)
	__read_ccsidr(ccsidr);  // Read CCSIDR (Cache Size IDentification Register)

	num_ways = ((ccsidr & 0x00001FF8U) >> 3U) + 1U;
	log2_num_ways = uint_log2_roundup(num_ways);
	if(log2_num_ways > 32U) return;  // Error

	num_sets = ((ccsidr & 0x0FFFE000U) >> 13U) + 1U;
	log2_linesize = (ccsidr & 0x00000007U) + 2U + 2U;

	pos_way = 32U - log2_num_ways;
	for(uint32_t way = 0; way < num_ways; way++){
		for(uint32_t set = 0; set < num_sets; set++){
			index = (cl_imp << 1U) | ((num_sets - set - 1) << log2_linesize) | ((num_ways - way - 1) << pos_way);
			__write_dccsw(index);  // Write DCCSW (Clean Data Cache Line (using Index))
		}
	}

	__dmb();
}

void clean_l1_dcache_all(void){
	uint32_t clidr;
	uint32_t cl_imp;  // Cache level implementation

	__read_clidr(clidr);  // Read CLIDR (Cache Level ID Register)

	// Iterate processor cache levels.  Note: Arm Cortex-A9 only implements level 1 cache,
	// so we only need to check the first level, but going through all so that code is generic.
	// Level 2 here refers to the cache implemented internally within the processor, not to be confused with the level 2 cache by the separate controller (L2C-310).
	for(uint32_t i = 0U; i < 7U; i++){
		cl_imp = (clidr >> 3U * i) & 0x7UL;  // Get cache level implementation

		// Is data cache implemented at this level?
		if((cl_imp >= 2U) && (cl_imp <= 4U)){
			clean_l1_dcache(cl_imp);
		}
	}
}

// ===========================================================================================================================================
// MMU table setup
// For details see these documents:
//   Arm Cortex-A9 Technical Reference Manual.  Notable refs: Ch8 System Interconnect
//   ARM Architecture Reference Manual ARMv7-A and ARMv7-R edition. Notable refs: B3.5.1 Short-descriptor translation table format descriptors
// ===========================================================================================================================================

// The implemented MMU table is 4096 short descriptor entries of 1MB sections, which translates the six Cyclone V SoC memory regions below.
// Notes:
// - the Peripherals+L3, Boot ROM, SCU+L2 and OCRAM memory regions had to be combined because their sizes do not align to 1MB
// - bottom 1MB region is assumed to be remapped to SDRAM
// +-----------------------------------------------------------------------------------------------------------------+
// | Region                             | Address Range           | MMU table entry attributes                       |
// |-----------------------------------------------------------------------------------------------------------------|
// | Periph+L3, Boot ROM, SCU+L2, OCRAM | 0xFF400000 - 0xFFFFFFFF | Shared device, RW, non-cacheable, shareable      |
// |-----------------------------------------------------------------------------------------------------------------|
// | LW H-to-F                          | 0xFF200000 - 0xFF3FFFFF | Shared device, RW, non-cacheable, shareable      |
// |-----------------------------------------------------------------------------------------------------------------|
// | DAP                                | 0xFF000000 - 0xFF1FFFFF | Shared device, RW, non-cacheable, shareable      |
// |-----------------------------------------------------------------------------------------------------------------|
// | STM                                | 0xFC000000 - 0xFEFFFFFF | Shared device, RW, non-cacheable, shareable      |
// |-----------------------------------------------------------------------------------------------------------------|
// | H-to-F                             | 0xC0000000 - 0xFBFFFFFF | Shared device, RW, non-cacheable, shareable      |
// |-----------------------------------------------------------------------------------------------------------------|
// | 3GB SDRAM                          | 0x00000000 - 0xBFFFFFFF | Normal, RWX, inner & outer-cacheable, shareable  |
// +-----------------------------------------------------------------------------------------------------------------+

// Note, the DE10-Nano only has 1GB of SDRAM populated, but since there is enough table entries, and it wrap to
// address 0 it is safe to cover the entire 3GB range.

// Below is the ideal MMU table, but it is not easily achievable:
// +-----------------------------------------------------------------------------------------------------------------+
// | Region                    | Address Range           | MMU table entry attributes                                |
// |-----------------------------------------------------------------------------------------------------------------|
// | OCRAM (On-Chip RAM)       | 0xFFFF0000 - 0xFFFFFFFF | Normal, RWX, inner-cacheable, shareable                   |
// |-----------------------------------------------------------------------------------------------------------------|
// | SCU and L2 Registers      | 0xFFFEC000 - 0xFFFEFFFF | Shared device, RW, non-cacheable, shareable               |
// |-----------------------------------------------------------------------------------------------------------------|
// | Boot ROM                  | 0xFFFD0000 - 0xFFFEBFFF | Shared device, RO, non-cacheable, shareable               |
// |-----------------------------------------------------------------------------------------------------------------|
// | Peripherals and L3 GPV    | 0xFF400000 - 0xFFFCFFFF | Shared device, RW, non-cacheable, shareable               |
// |-----------------------------------------------------------------------------------------------------------------|
// | LW H-to-F                 | 0xFF200000 - 0xFF3FFFFF | Shared device, RW, non-cacheable, shareable               |
// |-----------------------------------------------------------------------------------------------------------------|
// | DAP                       | 0xFF000000 - 0xFF1FFFFF | Shared device, RW, non-cacheable, shareable               |
// |-----------------------------------------------------------------------------------------------------------------|
// | STM                       | 0xFC000000 - 0xFEFFFFFF | Shared device, RW, non-cacheable, shareable               |
// |-----------------------------------------------------------------------------------------------------------------|
// | H-to-F                    | 0xC0000000 - 0xFBFFFFFF | Shared device, RW, non-cacheable, shareable               |
// |-----------------------------------------------------------------------------------------------------------------|
// | SDRAM                     | 0x00100000 - 0xBFFFFFFF | Normal, RWX, inner & outer-cacheable, shareable           |
// |-----------------------------------------------------------------------------------------------------------------|
// | When remapped to SDRAM    | 0x00010000 - 0x000FFFFF | Normal, RWX, inner & outer-cacheable, shareable           |
// |-----------------------------------------------------------------------------------------------------------------+
// | When remapped to OCRAM    | 0x00000000 - 0x0000FFFF | Normal, RWX, inner-cacheable, shareable                   |
// | When remapped to Boot ROM |                         | Normal, RO, inner & outer-cacheable, shareable            |
// +-----------------------------------------------------------------------------------------------------------------+

__asm__(
	// =========
	// Constants
	// =========

	// Memory region Privileged eXecute-Never bit
	".set MMU_SHORT_PXN_NONEXECUTE,            0x1U              \n"  // Bit 0 = 1. Non-execute
	".set MMU_SHORT_PXN_EXECUTE,               0x0U              \n"  // Bit 0 = 0
	// Memory region eXecute-Never bit
	".set MMU_SHORT_XN_NONEXECUTE,             0x10U             \n"  // Bit 4 = 1. Non-execute
	".set MMU_SHORT_XN_EXECUTE,                0x00U             \n"  // Bit 4 = 0
	// Memory region domain number
	".set MMU_SHORT_DOMAIN_ZERO,               0x000U            \n"  // Bit 8, 7, 6, 5. Domain number
	// Memory region and cache type
	// When TEX[2] == 0 & TRE == 0
	".set MMU_SHORT_TEXCB_STRONGLY_ORDERED,    0x0000U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Strongly-ordered
	".set MMU_SHORT_TEXCB_SHAREABLE_DEV,       0x0004U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Shareable Device
	".set MMU_SHORT_TEXCB_NORMAL_OWT_IWT,      0x0008U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WT, Inner WT
	".set MMU_SHORT_TEXCB_NORMAL_OWB_IWB,      0x000cU           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB, Inner WB
	".set MMU_SHORT_TEXCB_NORMAL,              0x1000U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, non-cacheable
	".set MMU_SHORT_TEXCB_VENDOR,              0x1008U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Vendor implementation defined
	".set MMU_SHORT_TEXCB_NORMAL_OWBWA_IWBWA,  0x100cU           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB + WA, Inner WB + WA
	".set MMU_SHORT_TEXCB_NONSHAREABLE_DEV,    0x2000U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Non-shareable Device
	// When TEX[2] == 1 & TRE == 0
	".set MMU_SHORT_TEXCB2_NORMAL,             0x4000U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, non-cacheable
	".set MMU_SHORT_TEXCB2_NORMAL_IWBWA,       0x4004U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Inner WB + WA
	".set MMU_SHORT_TEXCB2_NORMAL_IWT,         0x4008U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Inner WT
	".set MMU_SHORT_TEXCB2_NORMAL_IWB,         0x400cU           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Inner WB
	".set MMU_SHORT_TEXCB2_NORMAL_OWBWA,       0x5000U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB + WA
	".set MMU_SHORT_TEXCB2_NORMAL_OWBWA_IWBWA, 0x5004U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB + WA, Inner WB + WA
	".set MMU_SHORT_TEXCB2_NORMAL_OWBWA_IWT,   0x5008U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB + WA, Inner WT
	".set MMU_SHORT_TEXCB2_NORMAL_OWBWA_IWB,   0x500cU           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB + WA, Inner WB
	".set MMU_SHORT_TEXCB2_NORMAL_OWT,         0x6000U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WT
	".set MMU_SHORT_TEXCB2_NORMAL_OWT_IWBWA,   0x6004U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WT, Inner WB + WA
	".set MMU_SHORT_TEXCB2_NORMAL_OWT_IWT,     0x6008U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WT, Inner WT
	".set MMU_SHORT_TEXCB2_NORMAL_OWT_IWB,     0x600cU           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WT, Inner WB
	".set MMU_SHORT_TEXCB2_NORMAL_OWB,         0x7000U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB
	".set MMU_SHORT_TEXCB2_NORMAL_OWB_IWBWA,   0x7004U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB, Inner WB + WA
	".set MMU_SHORT_TEXCB2_NORMAL_OWB_IWT,     0x7008U           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB, Inner WT
	".set MMU_SHORT_TEXCB2_NORMAL_OWB_IWB,     0x700cU           \n"  // Bits 14, 13, 12, 3, 2. Memory type = Normal, Outer WB, Inner WB
	// Memory region access permission
	".set MMU_SHORT_AP_NONE,                   0x0000U           \n"  // Bits 15, 11, 10. Access Permission = No access
	".set MMU_SHORT_AP_RW_PL1,                 0x0400U           \n"  // Bits 15, 11, 10. Access Permission = RW at level 1
	".set MMU_SHORT_AP_RW_PL1_R_PL0,           0x0800U           \n"  // Bits 15, 11, 10. Access Permission = RW at level 1 and R at level 0
	".set MMU_SHORT_AP_RW_ANY,                 0x0c00U           \n"  // Bits 15, 11, 10. Access Permission = RW at level 1 and level 0
	".set MMU_SHORT_AP_RESERVED,               0x8000U           \n"  // Bits 15, 11, 10. Access Permission = Reserved
	".set MMU_SHORT_AP_R_PL1,                  0x8400U           \n"  // Bits 15, 11, 10. Access Permission = R at level 1
	".set MMU_SHORT_AP_R_PL1_R_PL0_DEP,        0x8800U           \n"  // Bits 15, 11, 10. Access Permission = R at level 1 and level 0 deprecated, use below instead
	".set MMU_SHORT_AP_R_ANY,                  0x8c00U           \n"  // Bits 15, 11, 10. Access Permission = R at level 1 and level 0
	// Memory region shareable bit for normal memory type
	".set MMU_SHORT_S_SHAREABLE,               0x10000UL         \n"  // Bit 16 = 1. Shareable
	".set MMU_SHORT_S_NONSHAREABLE,            0x00000UL         \n"  // Bit 16 = 0. Non-shareable
	// Memory region non-global bit
	".set MMU_SHORT_NG_GLOBAL,                 0x00000UL         \n"  // Bit 17 = 0. NG bit = Global
	".set MMU_SHORT_NG_NONGLOBAL,              0x20000UL         \n"  // Bit 17 = 1. NG bit = Non-global
	// Memory descriptor is 1MB Section type
	".set MMU_SHORT_SECTION,                   0x00002UL         \n"  // Bits 18, 0, 1
	// Memory region non-secure bit
	".set MMU_SHORT_NS_SECURE,                 0x00000UL         \n"  // Bit 19 = 0. NS bit = Secure
	".set MMU_SHORT_NS_NONSECURE,              0x80000UL         \n"  // Bit 19 = 1. NS bit = Non-secure
	// Section address
	".set MMU_SECTION_ADDR,                    0x000U            \n"  // A 12 bit section address which occupies bits 31 to 20 for an MMU table short descriptor

	// ================
	// Inline MMU table
	// ================

	// This table is placed into the specified section defined in the
	// linker file.  Since we explicitly specified a custom section, we should set
	// the section flag, we set the section as "a" = allocatable.

	".section mmu_ttb_l1_entries, \"a\"                          \n"
	".globl c5soc_mmu_tbl                                        \n"
	"c5soc_mmu_tbl:                                              \n"
		// Use repeat directive to create multiple MMU table entries for the 3GB DDR-3 SDRAM memory region
		".rept 3072                                              \n"
			".word MMU_SECTION_ADDR |"
			"      MMU_SHORT_DOMAIN_ZERO |"
			"      MMU_SHORT_TEXCB2_NORMAL_OWBWA_IWBWA |"
			"      MMU_SHORT_AP_RW_ANY |"
			"      MMU_SHORT_S_SHAREABLE |"
			"      MMU_SHORT_NG_GLOBAL |"
			"      MMU_SHORT_SECTION |"
			"      MMU_SHORT_NS_SECURE                           \n"
			".set MMU_SECTION_ADDR, MMU_SECTION_ADDR + 0x100000UL\n"
		".endr                                                   \n"

		// Use repeat directive to create multiple MMU table entries for the H2F bridge memory region
		".rept 960                                               \n"
			".word MMU_SECTION_ADDR |"
			"      MMU_SHORT_XN_NONEXECUTE |"
			"      MMU_SHORT_DOMAIN_ZERO |"
			"      MMU_SHORT_TEXCB_SHAREABLE_DEV |"
			"      MMU_SHORT_AP_RW_ANY |"
			"      MMU_SHORT_S_SHAREABLE |"
			"      MMU_SHORT_NG_GLOBAL |"
			"      MMU_SHORT_SECTION |"
			"      MMU_SHORT_NS_SECURE                           \n"
			".set MMU_SECTION_ADDR, MMU_SECTION_ADDR + 0x100000UL\n"
		".endr                                                   \n"

		// Use repeat directive to create multiple MMU table entries for the STM memory region
		".rept 48                                                \n"
			".word MMU_SECTION_ADDR |"
			"      MMU_SHORT_XN_NONEXECUTE |"
			"      MMU_SHORT_DOMAIN_ZERO |"
			"      MMU_SHORT_TEXCB_SHAREABLE_DEV |"
			"      MMU_SHORT_AP_RW_ANY |"
			"      MMU_SHORT_S_SHAREABLE |"
			"      MMU_SHORT_NG_GLOBAL |"
			"      MMU_SHORT_SECTION |"
			"      MMU_SHORT_NS_SECURE                           \n"
			".set MMU_SECTION_ADDR, MMU_SECTION_ADDR + 0x100000UL\n"
		".endr                                                   \n"

		// Use repeat directive to create multiple MMU table entries for the DAP memory region
		".rept 2                                                 \n"
			".word MMU_SECTION_ADDR |"
			"      MMU_SHORT_XN_NONEXECUTE |"
			"      MMU_SHORT_DOMAIN_ZERO |"
			"      MMU_SHORT_TEXCB_SHAREABLE_DEV |"
			"      MMU_SHORT_AP_RW_ANY |"
			"      MMU_SHORT_S_SHAREABLE |"
			"      MMU_SHORT_NG_GLOBAL |"
			"      MMU_SHORT_SECTION |"
			"      MMU_SHORT_NS_SECURE                           \n"
			".set MMU_SECTION_ADDR, MMU_SECTION_ADDR + 0x100000UL\n"
		".endr                                                   \n"

		// Use repeat directive to create multiple MMU table entries for the L2F bridge memory region
		".rept 2                                                 \n"
			".word MMU_SECTION_ADDR |"
			"      MMU_SHORT_XN_NONEXECUTE |"
			"      MMU_SHORT_DOMAIN_ZERO |"
			"      MMU_SHORT_TEXCB_SHAREABLE_DEV |"
			"      MMU_SHORT_AP_RW_ANY |"
			"      MMU_SHORT_S_SHAREABLE |"
			"      MMU_SHORT_NG_GLOBAL |"
			"      MMU_SHORT_SECTION |"
			"      MMU_SHORT_NS_SECURE                           \n"
			".set MMU_SECTION_ADDR, MMU_SECTION_ADDR + 0x100000UL\n"
		".endr                                                   \n"

		// Use repeat directive to create multiple MMU table entries for the peripherals/L3, BootROM, SCU/L2 and OCRAM memory region
		".rept 12                                                \n"
			".word MMU_SECTION_ADDR |"
			"      MMU_SHORT_XN_NONEXECUTE |"
			"      MMU_SHORT_DOMAIN_ZERO |"
			"      MMU_SHORT_TEXCB_SHAREABLE_DEV |"
			"      MMU_SHORT_AP_RW_ANY |"
			"      MMU_SHORT_S_SHAREABLE |"
			"      MMU_SHORT_NG_GLOBAL |"
			"      MMU_SHORT_SECTION |"
			"      MMU_SHORT_NS_SECURE                           \n"
			".set MMU_SECTION_ADDR, MMU_SECTION_ADDR + 0x100000UL\n"
		".endr                                                   \n"
);

#endif
