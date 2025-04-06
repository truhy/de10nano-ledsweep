/******************************************************************************
 * @file     system_c5soc.c
 * @brief    CMSIS Device System Source File for Arm Cortex-A9 Device Series
 * @version  V1.0.1
 * @date     13. February 2019
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
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

#include <stdint.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "irq_ctrl.h"

#define SYSTEM_CLOCK 800000000UL

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;

/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate(){
  SystemCoreClock = SYSTEM_CLOCK;
}

/*----------------------------------------------------------------------------
  System Initialization
 *----------------------------------------------------------------------------*/
void SystemInit(){
/* do not use global variables because this function is called before
   reaching pre-main. RW section may be overwritten afterwards.          */

#if defined(TRU_MMU) && TRU_MMU == 1U
  // Invalidate entire Unified TLB
  __set_TLBIALL(0);
#endif

#if defined(TRU_L1_CACHE) && TRU_L1_CACHE == 1U
  // Invalidate entire branch predictor array
  __set_BPIALL(0);
  __DSB();
  __ISB();

  // Invalidate instruction cache and flush branch target cache
  __set_ICIALLU(0);
  __DSB();
  __ISB();

  // Invalidate data cache
  L1C_InvalidateDCacheAll();
#endif

#if defined(TRU_NEON) && TRU_NEON == 1U && __FPU_PRESENT == 1U && __FPU_USED == 1U
  __FPU_Enable();
#endif

#if defined(TRU_MMU) && TRU_MMU == 1U
  MMU_CreateTranslationTable();
#if defined(TRU_DMA_BUFFER_NONCACHEABLE) && TRU_DMA_BUFFER_NONCACHEABLE == 1U
  mmu_create_dma_buffer_table_entries();
#endif
  MMU_Enable();
#endif

#if defined(TRU_L1_CACHE) && TRU_L1_CACHE == 1U
  // Enable L1 caches
  L1C_EnableCaches();
  L1C_EnableBTAC();
#endif

#if defined(TRU_L2_CACHE) && TRU_L2_CACHE == 1U && __L2C_PRESENT == 1U
  //L2C_310->AUX_CNT = L2C_310->AUX_CNT & ~(1U << 29U | 1U << 28U);  // Disable L2 instruction and data prefetch
  //L2C_310->AUX_CNT = L2C_310->AUX_CNT & ~(1U << 21U);  // Disable L2 parity

  // Set data RAM latency
  __IOM uint32_t *L2C_310_REG1_DATA_RAM_CNT = (__IOM uint32_t *)L2C_310_BASE + 0x10cU;
  *L2C_310_REG1_DATA_RAM_CNT = (*L2C_310_REG1_DATA_RAM_CNT & ~0x777U) | 0x10U;  // Read access set to 2 cycles of latency (value taken from Intel/Altera HWLib)

  L2C_Enable();
#endif

  IRQ_Initialize();  // Initialise the IRQ system, e.g. user interrupt handler table and GIC system
}
