/**************************************************************************//**
 * @file     c5soc.h
 * @brief    CMSIS-Core(A) Device Header File for Device <Device>
 *
 * @version  V1.0.1
 * @date     18. July 2023
 ******************************************************************************/
/*
 * Copyright (c) 2009-2023 Arm Limited. All rights reserved.
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

#ifndef C5SOC_H
#define C5SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

/* ========================================================================= */
/* ============           Interrupt Number Definition           ============ */
/* ========================================================================= */

typedef enum IRQn
{
/* ================     Cortex-A Specific Interrupt Numbers  =============== */

  /* Software Generated Interrupts */
  SGI0_IRQn                          =  0,  /* Software Generated Interrupt  0 */
  SGI1_IRQn                          =  1,  /* Software Generated Interrupt  1 */
  SGI2_IRQn                          =  2,  /* Software Generated Interrupt  2 */
  SGI3_IRQn                          =  3,  /* Software Generated Interrupt  3 */
  SGI4_IRQn                          =  4,  /* Software Generated Interrupt  4 */
  SGI5_IRQn                          =  5,  /* Software Generated Interrupt  5 */
  SGI6_IRQn                          =  6,  /* Software Generated Interrupt  6 */
  SGI7_IRQn                          =  7,  /* Software Generated Interrupt  7 */
  SGI8_IRQn                          =  8,  /* Software Generated Interrupt  8 */
  SGI9_IRQn                          =  9,  /* Software Generated Interrupt  9 */
  SGI10_IRQn                         = 10,  /* Software Generated Interrupt 10 */
  SGI11_IRQn                         = 11,  /* Software Generated Interrupt 11 */
  SGI12_IRQn                         = 12,  /* Software Generated Interrupt 12 */
  SGI13_IRQn                         = 13,  /* Software Generated Interrupt 13 */
  SGI14_IRQn                         = 14,  /* Software Generated Interrupt 14 */
  SGI15_IRQn                         = 15,  /* Software Generated Interrupt 15 */

  /* Private Peripheral Interrupts */
  VirtualMaintenanceInterrupt_IRQn   = 25,  /* Virtual Maintenance Interrupt */
  HypervisorTimer_IRQn               = 26,  /* Hypervisor Timer Interrupt */
  VirtualTimer_IRQn                  = 27,  /* Virtual Timer Interrupt */
  Legacy_nFIQ_IRQn                   = 28,  /* Legacy nFIQ Interrupt */
  SecurePhyTimer_IRQn                = 29,  /* Secure Physical Timer Interrupt */
  NonSecurePhyTimer_IRQn             = 30,  /* Non-Secure Physical Timer Interrupt */
  Legacy_nIRQ_IRQn                   = 31,  /* Legacy nIRQ Interrupt */

  /* Shared Peripheral Interrupts */
  C5SOC_F2H0_IRQn = 72,
  C5SOC_F2H1_IRQn = 73,
  C5SOC_F2H2_IRQn = 74,
  C5SOC_F2H3_IRQn = 75,
  C5SOC_F2H4_IRQn = 76,
  C5SOC_F2H5_IRQn = 77,
  C5SOC_F2H6_IRQn = 78,
  C5SOC_F2H7_IRQn = 79,
  C5SOC_F2H8_IRQn = 80,
  C5SOC_F2H9_IRQn = 81,
  C5SOC_F2H10_IRQn = 82,
  C5SOC_F2H11_IRQn = 83,
  C5SOC_F2H12_IRQn = 84,
  C5SOC_F2H13_IRQn = 85,
  C5SOC_F2H14_IRQn = 86,
  C5SOC_F2H15_IRQn = 87,
  C5SOC_F2H16_IRQn = 88,
  C5SOC_F2H17_IRQn = 89,
  C5SOC_F2H18_IRQn = 90,
  C5SOC_F2H19_IRQn = 91,
  C5SOC_F2H20_IRQn = 92,
  C5SOC_F2H21_IRQn = 93,
  C5SOC_F2H22_IRQn = 94,
  C5SOC_F2H23_IRQn = 95,
  C5SOC_F2H24_IRQn = 96,
  C5SOC_F2H25_IRQn = 97,
  C5SOC_F2H26_IRQn = 98,
  C5SOC_F2H27_IRQn = 99,
  C5SOC_F2H28_IRQn = 100,
  C5SOC_F2H29_IRQn = 101,
  C5SOC_F2H30_IRQn = 102,
  C5SOC_F2H31_IRQn = 103,
  C5SOC_F2H32_IRQn = 104,
  C5SOC_F2H33_IRQn = 105,
  C5SOC_F2H34_IRQn = 106,
  C5SOC_F2H35_IRQn = 107,
  C5SOC_F2H36_IRQn = 108,
  C5SOC_F2H37_IRQn = 109,
  C5SOC_F2H38_IRQn = 110,
  C5SOC_F2H39_IRQn = 111,
  C5SOC_F2H40_IRQn = 112,
  C5SOC_F2H41_IRQn = 113,
  C5SOC_F2H42_IRQn = 114,
  C5SOC_F2H43_IRQn = 115,
  C5SOC_F2H44_IRQn = 116,
  C5SOC_F2H45_IRQn = 117,
  C5SOC_F2H46_IRQn = 118,
  C5SOC_F2H47_IRQn = 119,
  C5SOC_F2H48_IRQn = 120,
  C5SOC_F2H49_IRQn = 121,
  C5SOC_F2H50_IRQn = 122,
  C5SOC_F2H51_IRQn = 123,
  C5SOC_F2H52_IRQn = 124,
  C5SOC_F2H53_IRQn = 125,
  C5SOC_F2H54_IRQn = 126,
  C5SOC_F2H55_IRQn = 127,
  C5SOC_F2H56_IRQn = 128,
  C5SOC_F2H57_IRQn = 129,
  C5SOC_F2H58_IRQn = 130,
  C5SOC_F2H59_IRQn = 131,
  C5SOC_F2H60_IRQn = 132,
  C5SOC_F2H61_IRQn = 133,
  C5SOC_F2H62_IRQn = 134,
  C5SOC_F2H63_IRQn = 135,
  C5SOC_DMA0_IRQn = 136,
  C5SOC_DMA1_IRQn = 137,
  C5SOC_DMA2_IRQn = 138,
  C5SOC_DMA3_IRQn = 139,
  C5SOC_DMA4_IRQn = 140,
  C5SOC_DMA5_IRQn = 141,
  C5SOC_DMA6_IRQn = 142,
  C5SOC_DMA7_IRQn = 143,
  C5SOC_DMA_IRQ_ABORT_IRQn = 144,
  C5SOC_DMA_ECC_CORRECTED_IRQ_IRQn = 145,
  C5SOC_DMA_ECC_UNCORRECTED_IRQ_IRQn = 146,
  C5SOC_EMAC0_IRQ_IRQn = 147,
  C5SOC_EMAC0_TX_ECC_CORRECTED_IRQ_IRQn = 148,
  C5SOC_EMAC0_TX_ECC_UNCORRECTED_IRQ_IRQn = 149,
  C5SOC_EMAC0_RX_ECC_CORRECTED_IRQ_IRQn = 150,
  C5SOC_EMAC0_RX_ECC_UNCORRECTED_IRQ_IRQn = 151,
  C5SOC_EMAC1_IRQ_IRQn = 152,
  C5SOC_EMAC1_TX_ECC_CORRECTED_IRQ_IRQn = 153,
  C5SOC_EMAC1_TX_ECC_UNCORRECTED_IRQ_IRQn = 154,
  C5SOC_EMAC1_RX_ECC_CORRECTED_IRQ_IRQn = 155,
  C5SOC_EMAC1_RX_ECC_UNCORRECTED_IRQ_IRQn = 156,
  C5SOC_USB0_IRQ_IRQn = 157,
  C5SOC_USB0_ECC_CORRECTED_IRQn = 158,
  C5SOC_USB0_ECC_UNCORRECTED_IRQn = 159,
  C5SOC_USB1_IRQ_IRQn = 160,
  C5SOC_USB1_ECC_CORRECTED_IRQn = 161,
  C5SOC_USB1_ECC_UNCORRECTED_IRQn = 162,
  C5SOC_CAN0_STS_IRQ_IRQn = 163,
  C5SOC_CAN0_MO_IRQ_IRQn = 164,
  C5SOC_CAN0_ECC_CORRECTED_IRQ_IRQn = 165,
  C5SOC_CAN0_ECC_UNCORRECTED_IRQ_IRQn = 166,
  C5SOC_CAN1_STS_IRQ_IRQn = 167,
  C5SOC_CAN1_MO_IRQ_IRQn = 168,
  C5SOC_CAN1_ECC_CORRECTED_IRQ_IRQn = 169,
  C5SOC_CAN1_ECC_UNCORRECTED_IRQ_IRQn = 170,
  C5SOC_SDMMC_IRQ_IRQn = 171,
  C5SOC_SDMMC_PORTA_ECC_CORRECTED_IRQn = 172,
  C5SOC_SDMMC_PORTA_ECC_UNCORRECTED_IRQn = 173,
  C5SOC_SDMMC_PORTB_ECC_CORRECTED_IRQn = 174,
  C5SOC_SDMMC_PORTB_ECC_UNCORRECTED_IRQn = 175,
  C5SOC_NAND_IRQ_IRQn = 176,
  C5SOC_NANDR_ECC_CORRECTED_IRQ_IRQn = 177,
  C5SOC_NANDR_ECC_UNCORRECTED_IRQ_IRQn = 178,
  C5SOC_NANDW_ECC_CORRECTED_IRQ_IRQn = 179,
  C5SOC_NANDW_ECC_UNCORRECTED_IRQ_IRQn = 180,
  C5SOC_NANDE_ECC_CORRECTED_IRQ_IRQn = 181,
  C5SOC_NANDE_ECC_UNCORRECTED_IRQ_IRQn = 182,
  C5SOC_QSPI_IRQ_IRQn = 183,
  C5SOC_QSPI_ECC_CORRECTED_IRQ_IRQn = 184,
  C5SOC_QSPI_ECC_UNCORRECTED_IRQ_IRQn = 185,
  C5SOC_SPI0_IRQ_IRQn = 186,
  C5SOC_SPI1_IRQ_IRQn = 187,
  C5SOC_SPI2_IRQ_IRQn = 188,
  C5SOC_SPI3_IRQ_IRQn = 189,
  C5SOC_I2C0_IRQ_IRQn = 190,
  C5SOC_I2C1_IRQ_IRQn = 191,
  C5SOC_I2C2_IRQ_IRQn = 192,
  C5SOC_I2C3_IRQ_IRQn = 193,
  C5SOC_UART0_IRQn = 194,
  C5SOC_UART1_IRQn = 195,
  C5SOC_GPIO0_IRQn = 196,
  C5SOC_GPIO1_IRQn = 197,
  C5SOC_GPIO2_IRQn = 198,
  C5SOC_TIMER_L4SP_0_IRQ_IRQn = 199,
  C5SOC_TIMER_L4SP_1_IRQ_IRQn = 200,
  C5SOC_TIMER_OSC1_0_IRQ_IRQn = 201,
  C5SOC_TIMER_OSC1_1_IRQ_IRQn = 202,
  C5SOC_WDOG0_IRQ_IRQn = 203,
  C5SOC_WDOG1_IRQ_IRQn = 204,
  C5SOC_CLKMGR_IRQ_IRQn = 205,
  C5SOC_MPUWAKEUP_IRQ_IRQn = 206,
  C5SOC_FPGA_MAN_IRQ_IRQn = 207,
  C5SOC_NCTIIRQ0_IRQn = 208,
  C5SOC_NCTIIRQ1_IRQn = 209,
  C5SOC_RAM_ECC_CORRECTED_IRQ_IRQn = 210,
  C5SOC_RAM_ECC_UNCORRECTED_IRQ_IRQn = 211
} IRQn_Type;

// Arm components, but with Cyclone V SoC assigned base address
#define L2C_310_BASE               0xFFFEF000UL  // L2 cache register base address
#define GIC_DISTRIBUTOR_BASE       0xFFFED000UL  // GIC distributor register base address
#define TIMER_BASE                 0xFFFEC600UL  // Private high resolution timer register base address
#define GIC_INTERFACE_BASE         0xFFFEC100UL  // GIC CPU interface register base address

/* ========================================================================= */
/* ============      Processor and Core Peripheral Section      ============ */
/* ========================================================================= */

/* ================ Start of section using anonymous unions ================ */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __CORTEX_A              9U       /* Cortex-A9 Core */
#define __CA_REV                0x0000U  /* Core revision r0p0 */

#define __FPU_PRESENT           1U       /* Set to 1 if FPU is present */
#define __GIC_PRESENT           1U       /* Set to 1 if GIC is present */
#define __TIM_PRESENT           1U       /* Set to 1 if TIM is present */
#define __L2C_PRESENT           1U       /* Set to 1 if L2C is present */

#include <core_ca.h>                         /* Processor and core peripherals */
#include "system_c5soc.h"                    /* System Header */

/* ========================================================================= */
/* ============       Device Specific Peripheral Section        ============ */
/* ========================================================================= */



/* ========================================================================= */
/* ============                       TMR                       ============ */
/* ========================================================================= */

typedef struct
{
  __IOM uint32_t  TimerLoad;                 /* Offset: 0x004 (R/W) Load Register */
  __IM  uint32_t  TimerValue;                /* Offset: 0x008 (R/ ) Counter Current Value Register */
  __IOM uint32_t  TimerControl;              /* Offset: 0x00C (R/W) Control Register */
  __OM  uint32_t  TimerIntClr;               /* Offset: 0x010 ( /W) Interrupt Clear Register */
  __IM  uint32_t  TimerRIS;                  /* Offset: 0x014 (R/ ) Raw Interrupt Status Register */
  __IM  uint32_t  TimerMIS;                  /* Offset: 0x018 (R/ ) Masked Interrupt Status Register */
  __IM  uint32_t  RESERVED[1];
  __IOM uint32_t  TimerBGLoad;               /* Offset: 0x020 (R/W) Background Load Register */
} c5soc_TMR_TypeDef;

/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/* ========================================================================= */
/* ============     Device Specific Peripheral Address Map      ============ */
/* ========================================================================= */

// Cyclone V SoC components
#define C5SOC_OCRAM_BASE           0xFFFF0000UL  // 64kB On-Chip RAM
#define C5SOC_SCU_L2_BASE          0xFFFEC000UL
#define C5SOC_BOOTROM_BASE         0xFFFD0000UL
#define C5SOC_PERI_L3_BASE         0xFF400000UL
#define C5SOC_L2F_BASE             0xFF200000UL
#define C5SOC_DAP_BASE             0xFF000000UL
#define C5SOC_STM_BASE             0xFC000000UL
#define C5SOC_H2F_BASE             0xC0000000UL
#define C5SOC_RAM_BASE             0x00000000UL  // 3GB SDRAM.  Only bottom 1GB DDR-3 SDRAM is populated on DE10-Nano

/* ========================================================================= */
/* ============             Peripheral declaration              ============ */
/* ========================================================================= */



#ifdef __cplusplus
}
#endif

#endif  /* C5SOC_H */
