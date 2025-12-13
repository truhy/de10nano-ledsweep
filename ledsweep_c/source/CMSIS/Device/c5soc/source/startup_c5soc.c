/******************************************************************************
 * @file     startup_c5soc.c
 * @brief    CMSIS Device System Source File for Arm Cortex-A9 Device Series
 * @version  V1.0.1
 * @date     10. January 2021
 ******************************************************************************/
/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

#include <c5soc.h>
#include <core_ca.h>

#if defined(TRU_EXIT_TO_UBOOT) && TRU_EXIT_TO_UBOOT == 1U
  #define RESET_ARGS int argc, char *const argv[]
#else
  #define RESET_ARGS void
#endif

/*----------------------------------------------------------------------------
  Definitions
 *----------------------------------------------------------------------------*/
#define USR_MODE 0x10U            // User mode
#define FIQ_MODE 0x11U            // Fast Interrupt Request mode
#define IRQ_MODE 0x12U            // Interrupt Request mode
#define SVC_MODE 0x13U            // Supervisor mode
#define ABT_MODE 0x17U            // Abort mode
#define UND_MODE 0x1BU            // Undefined Instruction mode
#define SYS_MODE 0x1FU            // System mode

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Vectors        (void)       __attribute__ ((naked, section("RESET")));
void Reset_Handler  (RESET_ARGS) __attribute__ ((naked));
void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
void Undef_Handler (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void PAbt_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void DAbt_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void IRQ_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void FIQ_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector Table
 *----------------------------------------------------------------------------*/
void Vectors(void) {
  __ASM volatile(
  "LDR    PC, =Reset_Handler                        \n"
  "LDR    PC, =Undef_Handler                        \n"
  "LDR    PC, =SVC_Handler                          \n"
  "LDR    PC, =PAbt_Handler                         \n"
  "LDR    PC, =DAbt_Handler                         \n"
  "NOP                                              \n"
  "LDR    PC, =IRQ_Handler                          \n"
  "LDR    PC, =FIQ_Handler                          \n"
  );
}

void CleanCache(void){
	L1C_CleanDCacheAll();
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(RESET_ARGS) {
  __ASM volatile(

  // Mask interrupts
  "CPSID   if                                      \n"

#if defined(TRU_EXIT_TO_UBOOT) && TRU_EXIT_TO_UBOOT == 1U
  // Save U-Boot argc
  "LDR r3, =uboot_argc                             \n"
  "STR r0, [r3]                                    \n"

  // Save U-Boot argv
  "LDR r3, =uboot_argv                             \n"
  "STR r1, [r3]                                    \n"

  // Save U-Boot return address
  "LDR r3, =uboot_lr                               \n"
  "STR lr, [r3]                                    \n"

  // Save U-Boot processor mode
  "LDR r3, =uboot_cpsr                             \n"
  "MRS r2, cpsr                                    \n"
  "STR r2, [r3]                                    \n"

  // Save U-Boot stack pointer for each mode
  "CPS #0x11                                       \n"
  "LDR r3, =uboot_fiq_sp                           \n"
  "STR sp, [r3]                                    \n"
  "CPS #0x12                                       \n"
  "LDR r3, =uboot_irq_sp                           \n"
  "STR sp, [r3]                                    \n"
  "CPS #0x13                                       \n"
  "LDR r3, =uboot_svc_sp                           \n"
  "STR sp, [r3]                                    \n"
  "CPS #0x17                                       \n"
  "LDR r3, =uboot_abt_sp                           \n"
  "STR sp, [r3]                                    \n"
  "CPS #0x1B                                       \n"
  "LDR r3, =uboot_und_sp                           \n"
  "STR sp, [r3]                                    \n"
  "CPS #0x1F                                       \n"
  "LDR r3, =uboot_sys_sp                           \n"
  "STR sp, [r3]                                    \n"

  // Save VBAR (vector base address)
  "LDR r3, =uboot_vbar                             \n"
  "MRC p15, 0, r2, c12, c0, 0                      \n"
  "STR r2, [r3]                                    \n"
#endif

  // Put any cores other than 0 to sleep
	/*
  "MRC     p15, 0, R0, c0, c0, 5                   \n"  // Read MPIDR
  "ANDS    R0, R0, #3                              \n"
  "goToSleep:                                      \n"
  "WFINE                                           \n"
  "BNE     goToSleep                               \n"
	*/

#if defined(TRU_CLEAN_CACHE) && TRU_CLEAN_CACHE == 1U
  // Clean D Cache if it is enabled
  // Since we are starting from U-Boot which may have the cache enabled,
  // loaded file(s) and some global variables may be cached and stay dirty.
  // Let's make sure that all dirty lines are written back into memory, in
  // case cache settings are changed later on
  "MRC     p15, 0, r0, c1, c0, 0                   \n"  // Read CP15 System Control register
  "AND     r0, r0, #4                              \n"  // Apply L1 D Cache bit mask
  "CMP     r0, #0                                  \n"  // Is L1 D Cache enabled?
  "BEQ     _disable_l1                             \n"  // Skip clean
  "BL      CleanCache                              \n"  // Clean L1 D Cache
#endif

"_disable_l1:                                      \n"
  // Reset SCTLR Settings
  "MRC     p15, 0, R0, c1, c0, 0                   \n"  // Read CP15 System Control register
#if defined(TRU_L1_CACHE) && TRU_L1_CACHE != 2U
  "BIC     R0, R0, #(0x1 << 12)                    \n"  // Clear I bit 12 to disable I Cache
  "BIC     R0, R0, #(0x1 << 11)                    \n"  // Clear Z bit 11 to disable branch prediction
  "BIC     R0, R0, #(0x1 <<  2)                    \n"  // Clear C bit  2 to disable D Cache
#endif
#if defined(TRU_MMU) && TRU_MMU != 2U
  "BIC     R0, R0, #0x1                            \n"  // Clear M bit  0 to disable MMU
#endif
  "BIC     R0, R0, #(0x1 << 13)                    \n"  // Clear V bit 13 to disable hivecs
  "MCR     p15, 0, R0, c1, c0, 0                   \n"  // Write value back to CP15 System Control register
  "ISB                                             \n"  // Ensures writes have completed

  // Configure ACTLR
  "MRC     p15, 0, r0, c1, c0, 1                   \n"  // Read CP15 Auxiliary Control Register
  "ORR     r0, r0, #(1 << 1)                       \n"  // Enable L2 prefetch hint (UNK/WI since r4p1)
  "MCR     p15, 0, r0, c1, c0, 1                   \n"  // Write CP15 Auxiliary Control Register

  // Configure access permissions (switch into secure access mode)
  "MRC    p15, 0, r0, c1, c1, 2                    \n"  // Read from NSACR (Non-secure Access Control Register)
  "ORR    r0, r0, #(0x3 << 20)                     \n"  // Setup bits to enable access permissions.  Undocumented Altera/Intel Cyclone V SoC vendor specific
  "MCR    p15, 0, r0, c1, c1, 2                    \n"  // Write to NSACR
  "ISB                                             \n"  // Ensures writes have completed

  // Set Vector Base Address Register (VBAR) to point to this application's vector table
  "LDR    R0, =Vectors                             \n"
  "MCR    p15, 0, R0, c12, c0, 0                   \n"

  // Setup Stack for each exceptional mode
  "CPS    #0x11                                    \n"
  "LDR    SP, =Image$$FIQ_STACK$$ZI$$Limit         \n"
  "CPS    #0x12                                    \n"
  "LDR    SP, =Image$$IRQ_STACK$$ZI$$Limit         \n"
  "CPS    #0x13                                    \n"
  "LDR    SP, =Image$$SVC_STACK$$ZI$$Limit         \n"
  "CPS    #0x17                                    \n"
  "LDR    SP, =Image$$ABT_STACK$$ZI$$Limit         \n"
  "CPS    #0x1B                                    \n"
  "LDR    SP, =Image$$UND_STACK$$ZI$$Limit         \n"
  "CPS    #0x1F                                    \n"
  "LDR    SP, =Image$$SYS_STACK$$ZI$$Limit         \n"

  // Call SystemInit
  "BL     SystemInit                               \n"

#if defined(TRU_SCU) && TRU_SCU == 1U
  // =======================================
  // Initialise the SCU (Snoop Control Unit)
  // =======================================

  // Invalidate SCU
  "LDR    r0, =0xfffec000UL                        \n"  // Load SCU base register
  "LDR    r1, =0xffff                              \n"  // Value to write
  "STR    r1, [r0, #0xc]                           \n"  // Write to SCU Invalidate All register (0xfffec00c)

  // Enable SCU
  "LDR    r0, =0xfffec000UL                        \n"  // Load SCU base register
  "LDR    r1, [r0, #0x0]                           \n"  // Read SCU register
  "ORR    r1, r1, #0x1                             \n"  // Set bit 0 (The Enable bit)
  "STR    r1, [r0, #0x0]                           \n"  // Write back modified value
#endif

#if defined(TRU_SMP_COHERENCY) && TRU_SMP_COHERENCY == 1U
  // Enable SMP cache coherency support
  "MRC    p15, 0, r0, c1, c0, 1                    \n"  // Read ACTLR
  "ORR    r0, r0, #(0x1 << 22)                     \n"  // Set bit 22 to enable shared attribute override. Recommended for ACP data coherency from Cyclone V HPS tech ref
  "ORR    r0, r0, #(0x1 << 6)                      \n"  // Set bit 6 to participate in SMP coherency
  //"ORR    r0, r0, #(0x1 << 2)                      \n"  // Set bit 2 to enable L1 dside prefetch
  "ORR    r0, r0, #(0x1 << 0)                      \n"  // Set bit 0 to enable maintenance broadcast
  "MCR    p15, 0, r0, c1, c0, 1                    \n"  // Write ACTLR
#endif

  // Unmask interrupts
  "CPSIE  if                                       \n"

  // Call newlib start
  "BL     _start                                   \n"
  );
}

//#if(TRU_EXIT_TO_UBOOT)
  // =============================
  // Override newlib _stack_init()
  // =============================

  // This makes newlib setup only the system stack
  void _stack_init(void){
  }
//#endif

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void) {
  while(1);
}
