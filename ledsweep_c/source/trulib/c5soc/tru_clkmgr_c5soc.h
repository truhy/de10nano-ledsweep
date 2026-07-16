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

	Clock Manager low-level support for Cyclone V SoC HPS.
*/

#ifndef TRU_CLKMGR_C5SOC_H
#define TRU_CLKMGR_C5SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include <stdint.h>

// Clock Manager base address
#define TRU_HPS_CLKMGR_BASE 0xffd04000

// Clock Manager Main PLL registers
#define TRU_HPS_CLKMGR_MAINPLL_VCO      (TRU_HPS_CLKMGR_BASE + 0x40)
#define TRU_HPS_CLKMGR_MAINPLL_MISC     (TRU_HPS_CLKMGR_BASE + 0x44)
#define TRU_HPS_CLKMGR_MAINPLL_C0       (TRU_HPS_CLKMGR_BASE + 0x48)
#define TRU_HPS_CLKMGR_MAINPLL_C1       (TRU_HPS_CLKMGR_BASE + 0x4c)
#define TRU_HPS_CLKMGR_MAINPLL_C2       (TRU_HPS_CLKMGR_BASE + 0x50)
#define TRU_HPS_CLKMGR_MAINPLL_C3       (TRU_HPS_CLKMGR_BASE + 0x54)
#define TRU_HPS_CLKMGR_MAINPLL_C4       (TRU_HPS_CLKMGR_BASE + 0x58)
#define TRU_HPS_CLKMGR_MAINPLL_C5       (TRU_HPS_CLKMGR_BASE + 0x5c)
#define TRU_HPS_CLKMGR_MAINPLL_EN       (TRU_HPS_CLKMGR_BASE + 0x60)
#define TRU_HPS_CLKMGR_MAINPLL_MAINDIV  (TRU_HPS_CLKMGR_BASE + 0x64)
#define TRU_HPS_CLKMGR_MAINPLL_DBGDIV   (TRU_HPS_CLKMGR_BASE + 0x68)
#define TRU_HPS_CLKMGR_MAINPLL_TRACEDIV (TRU_HPS_CLKMGR_BASE + 0x6c)
#define TRU_HPS_CLKMGR_MAINPLL_L4SRC    (TRU_HPS_CLKMGR_BASE + 0x70)
#define TRU_HPS_CLKMGR_MAINPLL_STAT     (TRU_HPS_CLKMGR_BASE + 0x74)

// Clock Manager Peripheral PLL registers
#define TRU_HPS_CLKMGR_PERIPLL_VCO     (TRU_HPS_CLKMGR_BASE + 0x80)
#define TRU_HPS_CLKMGR_PERIPLL_MISC    (TRU_HPS_CLKMGR_BASE + 0x84)
#define TRU_HPS_CLKMGR_PERIPLL_C0      (TRU_HPS_CLKMGR_BASE + 0x88)
#define TRU_HPS_CLKMGR_PERIPLL_C1      (TRU_HPS_CLKMGR_BASE + 0x8c)
#define TRU_HPS_CLKMGR_PERIPLL_C2      (TRU_HPS_CLKMGR_BASE + 0x90)
#define TRU_HPS_CLKMGR_PERIPLL_C3      (TRU_HPS_CLKMGR_BASE + 0x94)
#define TRU_HPS_CLKMGR_PERIPLL_C4      (TRU_HPS_CLKMGR_BASE + 0x98)
#define TRU_HPS_CLKMGR_PERIPLL_C5      (TRU_HPS_CLKMGR_BASE + 0x9c)
#define TRU_HPS_CLKMGR_PERIPLL_EN      (TRU_HPS_CLKMGR_BASE + 0xa0)
#define TRU_HPS_CLKMGR_PERIPLL_DIV     (TRU_HPS_CLKMGR_BASE + 0xa4)
#define TRU_HPS_CLKMGR_PERIPLL_GPIODIV (TRU_HPS_CLKMGR_BASE + 0xa8)
#define TRU_HPS_CLKMGR_PERIPLL_SRC     (TRU_HPS_CLKMGR_BASE + 0xac)
#define TRU_HPS_CLKMGR_PERIPLL_STAT    (TRU_HPS_CLKMGR_BASE + 0xb0)

// Clock Manager SDRAM PLL registers
#define TRU_HPS_CLKMGR_SDRAMPLL_VCO  (TRU_HPS_CLKMGR_BASE + 0xc0)
#define TRU_HPS_CLKMGR_SDRAMPLL_CTRL (TRU_HPS_CLKMGR_BASE + 0xc4)
#define TRU_HPS_CLKMGR_SDRAMPLL_C0   (TRU_HPS_CLKMGR_BASE + 0xc8)
#define TRU_HPS_CLKMGR_SDRAMPLL_C1   (TRU_HPS_CLKMGR_BASE + 0xcc)
#define TRU_HPS_CLKMGR_SDRAMPLL_C2   (TRU_HPS_CLKMGR_BASE + 0xd0)
#define TRU_HPS_CLKMGR_SDRAMPLL_C5   (TRU_HPS_CLKMGR_BASE + 0xd4)
#define TRU_HPS_CLKMGR_SDRAMPLL_EN   (TRU_HPS_CLKMGR_BASE + 0xd8)
#define TRU_HPS_CLKMGR_SDRAMPLL_STAT (TRU_HPS_CLKMGR_BASE + 0xdc)

// Clock Manager Altera Group registers
#define TRU_HPS_CLKMGR_ALTERA_K_C0 (TRU_HPS_CLKMGR_BASE + 0xe0)
#define TRU_HPS_CLKMGR_ALTERA_K_C1 (TRU_HPS_CLKMGR_BASE + 0xe4)
#define TRU_HPS_CLKMGR_ALTERA_K_C2 (TRU_HPS_CLKMGR_BASE + 0xe8)

typedef struct{
	uint32_t n;
	uint32_t m;
	uint32_t c;
	uint32_t k;
	uint32_t phase;
	float fref;
	float fvco;
	float fout;
}tru_hps_clk_t;

// Main PLL base clocks (Hz)
tru_hps_clk_t get_mpu_base_clk(float clk_in);
tru_hps_clk_t get_main_base_clk(float clk_in);
tru_hps_clk_t get_dbg_base_clk(float clk_in);
tru_hps_clk_t get_main_qspi_base_clk(float clk_in);
tru_hps_clk_t get_main_nand_sdmmc_base_clk(float clk_in);
tru_hps_clk_t get_cfg_h2f_user0_base_clk(float clk_in);

// Peripheral PLL base clocks (Hz)
tru_hps_clk_t get_emac0_base_clk(float clk_in);
tru_hps_clk_t get_emac1_base_clk(float clk_in);
tru_hps_clk_t get_peri_qspi_base_clk(float clk_in);
tru_hps_clk_t get_peri_nand_sdmmc_base_clk(float clk_in);
tru_hps_clk_t get_peri_base_clk(float clk_in);

// SDRAM PLL base clocks (Hz)
tru_hps_clk_t get_h2f_user1_base_clk(float clk_in);
tru_hps_clk_t get_ddr_dqs_base_clk(float clk_in);
tru_hps_clk_t get_ddr_2x_dqs_base_clk(float clk_in);
tru_hps_clk_t get_ddr_dq_base_clk(float clk_in);
tru_hps_clk_t get_h2f_user2_base_clk(float clk_in);

// Main PLL assigned clocks (Hz)
tru_hps_clk_t get_mpu_peri_clk(float clk_in);
tru_hps_clk_t get_mpu_l2_ram_clk(float clk_in);
tru_hps_clk_t get_l4_main_clk(float clk_in);
tru_hps_clk_t get_l3_main_clk(float clk_in);
tru_hps_clk_t get_dbg_at_clk(float clk_in);
tru_hps_clk_t get_dbg_clk(float clk_in);
tru_hps_clk_t get_dbg_trace_clk(float clk_in);
tru_hps_clk_t get_dbg_timer_clk(float clk_in);
tru_hps_clk_t get_cfg_clk(float clk_in);
tru_hps_clk_t get_h2f_user0_clk(float clk_in);
tru_hps_clk_t get_l3_mp_clk(float clk_in);
tru_hps_clk_t get_l3_sp_clk(float clk_in);
tru_hps_clk_t get_l4_mp_clk(float clk_in);
tru_hps_clk_t get_l4_sp_clk(float clk_in);

// Peripheral PLL assigned clocks (Hz)
tru_hps_clk_t get_emac0_clk(float clk_in);
tru_hps_clk_t get_emac1_clk(float clk_in);
tru_hps_clk_t get_usb_mp_clk(float clk_in);
tru_hps_clk_t get_spi_m_clk(float clk_in);
tru_hps_clk_t get_can0_clk(float clk_in);
tru_hps_clk_t get_can1_clk(float clk_in);
tru_hps_clk_t get_gpio_db_clk(float clk_in);
tru_hps_clk_t get_h2f_user1_clk(float clk_in);

// Flash controller clocks (Hz)
tru_hps_clk_t get_sdmmc_clk(float clk_in);
tru_hps_clk_t get_nand_x_clk(float clk_in);
tru_hps_clk_t get_nand_clk(float clk_in);
tru_hps_clk_t get_qspi_clk(float clk_in);

// SDRAM PLL assigned clocks (Hz)
tru_hps_clk_t get_ddr_dqs_clk(float clk_in);
tru_hps_clk_t get_ddr_2x_dqs_clk(float clk_in);
tru_hps_clk_t get_ddr_dq_clk(float clk_in);
tru_hps_clk_t get_h2f_user2_clk(float clk_in);

#endif

#ifdef __cplusplus
}
#endif

#endif
