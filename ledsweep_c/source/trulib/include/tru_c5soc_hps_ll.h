/*
	MIT License

	Copyright (c) 2024 Truong Hy

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

	Version: 20241020

	Low-level code for Cyclone V SoC HPS.
*/

#ifndef TRU_C5SOC_HPS_LL_H
#define TRU_C5SOC_HPS_LL_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_C5SOC)

#include <stdint.h>

// Reset Manager Register
#define TRU_HPS_RSTMGR_BASE 0xffd05000UL

// Peripheral Module Reset Register
#define TRU_HPS_RSTMGR_PERMODRST               (TRU_HPS_RSTMGR_BASE + 0x14U)
#define TRU_HPS_RSTMGR_PERMODRST_GPIO0_POS     25U
#define TRU_HPS_RSTMGR_PERMODRST_GPIO1_POS     26U
#define TRU_HPS_RSTMGR_PERMODRST_GPIO2_POS     27U
#define TRU_HPS_RSTMGR_PERMODRST_GPIO0_SET_MSK (1U << TRU_HPS_RSTMGR_PERMODRST_GPIO0_POS)
#define TRU_HPS_RSTMGR_PERMODRST_GPIO1_SET_MSK (1U << TRU_HPS_RSTMGR_PERMODRST_GPIO1_POS)
#define TRU_HPS_RSTMGR_PERMODRST_GPIO2_SET_MSK (1U << TRU_HPS_RSTMGR_PERMODRST_GPIO2_POS)

typedef union{
  struct{
	  volatile uint32_t emac0:1;
	  volatile uint32_t emac1:1;
	  volatile uint32_t usb0:1;
	  volatile uint32_t usb1:1;
	  volatile uint32_t nand:1;
	  volatile uint32_t qspi:1;
	  volatile uint32_t l4wd0:1;
	  volatile uint32_t l4wd1:1;
	  volatile uint32_t osc1timer0:1;
	  volatile uint32_t osc1timer1:1;
	  volatile uint32_t sptimer0:1;
	  volatile uint32_t sptimer1:1;
	  volatile uint32_t i2c0:1;
	  volatile uint32_t i2c1:1;
	  volatile uint32_t i2c2:1;
	  volatile uint32_t i2c3:1;
	  volatile uint32_t uart0:1;
	  volatile uint32_t uart1:1;
	  volatile uint32_t spim0:1;
	  volatile uint32_t spim1:1;
	  volatile uint32_t spis0:1;
	  volatile uint32_t spis1:1;
	  volatile uint32_t sdmmc:1;
	  volatile uint32_t can0:1;
	  volatile uint32_t can1:1;
	  volatile uint32_t gpio0:1;
	  volatile uint32_t gpio1:1;
	  volatile uint32_t gpio2:1;
	  volatile uint32_t dma:1;
	  volatile uint32_t sdr:1;
	  uint32_t reserved:2;
  }bits;
  volatile uint32_t word;
}tru_hps_rstmgr_permodrst_t;

// Reset Manager register as type representation
#define TRU_HPS_RSTMGR_PERMODRST_REG ((tru_hps_rstmgr_permodrst_t *)TRU_HPS_RSTMGR_PERMODRST)

#define TRU_HPS_OCRAM_BASE   0xFFFF0000UL  // 64kB On-Chip RAM
#define TRU_HPS_SCU_L2_BASE  0xFFFEC000UL
#define TRU_HPS_BOOTROM_BASE 0xFFFD0000UL
#define TRU_HPS_PERI_L3_BASE 0xFF400000UL
#define TRU_HPS_L2F_BASE     0xFF200000UL
#define TRU_HPS_DAP_BASE     0xFF000000UL
#define TRU_HPS_STM_BASE     0xFC000000UL
#define TRU_HPS_H2F_BASE     0xC0000000UL
#define TRU_HPS_RAM_BASE     0x00000000UL

#endif

#endif
