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

	Version: 20251207

	Cyclone V SoC HPS Clock Manager.
*/

#include "tru_c5soc_hps_clkmgr_ll.h"

#if(TRU_TARGET == TRU_TARGET_C5SOC)

#include "tru_util_ll.h"

tru_hps_clk_t get_mpu_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t mpu_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		//.c = ((tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C0) & 0x1ff) + 1) * 2,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C0) & 0x1ff) + 1,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C0) & 0x1ff) + 1,
		.fref = clk_in / mpu_base_clk.n,
		.fvco = mpu_base_clk.fref * mpu_base_clk.m,
		.fout = mpu_base_clk.fvco / (mpu_base_clk.c * mpu_base_clk.k)
	};

	return mpu_base_clk;
}

tru_hps_clk_t get_main_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t main_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		//.c = ((tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C1) & 0x1ff) + 1) * 4,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C1) & 0x1ff) + 1,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C1) & 0x1ff) + 1,
		.fref = clk_in / main_base_clk.n,
		.fvco = main_base_clk.fref * main_base_clk.m,
		.fout = main_base_clk.fvco / (main_base_clk.c * main_base_clk.k)
	};

	return main_base_clk;
}

tru_hps_clk_t get_dbg_base_clk(float clk_in){

	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t dbg_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		//.c = ((tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C2) & 0x1ff) + 1) * 4,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C2) & 0x1ff) + 1,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C2) & 0x1ff) + 1,
		.fref = clk_in / dbg_base_clk.n,
		.fvco = dbg_base_clk.fref * dbg_base_clk.m,
		.fout = dbg_base_clk.fvco / (dbg_base_clk.c * dbg_base_clk.k)
	};

	return dbg_base_clk;
}

tru_hps_clk_t get_main_qspi_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t main_qspi_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C3) & 0x1ff) + 1,
		.k = 1,
		.fref = clk_in / main_qspi_base_clk.n,
		.fvco = main_qspi_base_clk.fref * main_qspi_base_clk.m,
		.fout = main_qspi_base_clk.fvco / (main_qspi_base_clk.c * main_qspi_base_clk.k)
	};

	return main_qspi_base_clk;
}

tru_hps_clk_t get_main_nand_sdmmc_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t main_nand_sdmmc_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C4) & 0x1ff) + 1,
		.k = 1,
		.fref = clk_in / main_nand_sdmmc_base_clk.n,
		.fvco = main_nand_sdmmc_base_clk.fref * main_nand_sdmmc_base_clk.m,
		.fout = main_nand_sdmmc_base_clk.fvco / (main_nand_sdmmc_base_clk.c * main_nand_sdmmc_base_clk.k)
	};

	return main_nand_sdmmc_base_clk;
}

tru_hps_clk_t get_cfg_h2f_user0_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t cfg_h2f_user0_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_C5) & 0x1ff) + 1,
		.k = 1,
		.fref = clk_in / cfg_h2f_user0_base_clk.n,
		.fvco = cfg_h2f_user0_base_clk.fref * cfg_h2f_user0_base_clk.m,
		.fout = cfg_h2f_user0_base_clk.fvco / (cfg_h2f_user0_base_clk.c * cfg_h2f_user0_base_clk.k)
	};

	return cfg_h2f_user0_base_clk;
}

tru_hps_clk_t get_emac0_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t emac0_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_C0) & 0x1ff) + 1,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C0) & 0x1ff) + 1,
		.fref = clk_in / emac0_base_clk.n,
		.fvco = emac0_base_clk.fref * emac0_base_clk.m,
		.fout = emac0_base_clk.fvco / (emac0_base_clk.c * emac0_base_clk.k)
	};

	return emac0_base_clk;
}

tru_hps_clk_t get_emac1_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t emac1_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_C1) & 0x1ff) + 1,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C1) & 0x1ff) + 1,
		.fref = clk_in / emac1_base_clk.n,
		.fvco = emac1_base_clk.fref * emac1_base_clk.m,
		.fout = emac1_base_clk.fvco / (emac1_base_clk.c * emac1_base_clk.k)
	};

	return emac1_base_clk;
}

tru_hps_clk_t get_peri_qspi_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t periph_qspi_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_C2) & 0x1ff) + 1,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C2) & 0x1ff) + 1,
		.fref = clk_in / periph_qspi_base_clk.n,
		.fvco = periph_qspi_base_clk.fref * periph_qspi_base_clk.m,
		.fout = periph_qspi_base_clk.fvco / (periph_qspi_base_clk.c * periph_qspi_base_clk.k)
	};

	return periph_qspi_base_clk;
}

tru_hps_clk_t get_peri_nand_sdmmc_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t periph_nand_sdmmc_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_C3) & 0x1ff) + 1,
		.k = 1,
		.fref = clk_in / periph_nand_sdmmc_base_clk.n,
		.fvco = periph_nand_sdmmc_base_clk.fref * periph_nand_sdmmc_base_clk.m,
		.fout = periph_nand_sdmmc_base_clk.fvco / (periph_nand_sdmmc_base_clk.c * periph_nand_sdmmc_base_clk.k)
	};

	return periph_nand_sdmmc_base_clk;
}

tru_hps_clk_t get_peri_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t periph_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_C4) & 0x1ff) + 1,
		.k = 1,
		.fref = clk_in / periph_base_clk.n,
		.fvco = periph_base_clk.fref * periph_base_clk.m,
		.fout = periph_base_clk.fvco / (periph_base_clk.c * periph_base_clk.k)
	};

	return periph_base_clk;
}

tru_hps_clk_t get_h2f_user1_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;

	tru_hps_clk_t h2f_user1_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_C5) & 0x1ff) + 1,
		.k = 1,
		.fref = clk_in / h2f_user1_base_clk.n,
		.fvco = h2f_user1_base_clk.fref * h2f_user1_base_clk.m,
		.fout = h2f_user1_base_clk.fvco / (h2f_user1_base_clk.c * h2f_user1_base_clk.k)
	};

	return h2f_user1_base_clk;
}

tru_hps_clk_t get_ddr_dqs_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;
	uint32_t settings = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_C0);

	tru_hps_clk_t ddr_dqs_base_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (settings & 0x1ff) + 1,
		.phase = (settings >> 9 & 0xfff) * 45,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C0) & 0x1ff) + 1,
		.fref = clk_in / ddr_dqs_base_clk.n,
		.fvco = ddr_dqs_base_clk.fref * ddr_dqs_base_clk.m,
		.fout = ddr_dqs_base_clk.fvco / (ddr_dqs_base_clk.c * ddr_dqs_base_clk.k)
	};

	return ddr_dqs_base_clk;
}

tru_hps_clk_t get_ddr_2x_dqs_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;
	uint32_t settings = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_C1);

	tru_hps_clk_t ddr_2x_dqs_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (settings & 0x1ff) + 1,
		.phase = (settings >> 9 & 0xfff) * 45,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C1) & 0x1ff) + 1,
		.fref = clk_in / ddr_2x_dqs_clk.n,
		.fvco = ddr_2x_dqs_clk.fref * ddr_2x_dqs_clk.m,
		.fout = ddr_2x_dqs_clk.fvco / (ddr_2x_dqs_clk.c * ddr_2x_dqs_clk.k)
	};

	return ddr_2x_dqs_clk;
}

tru_hps_clk_t get_ddr_dq_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;
	uint32_t settings = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_C2);

	tru_hps_clk_t ddr_dq_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (settings & 0x1ff) + 1,
		.phase = (settings >> 9 & 0xfff) * 45,
		.k = (tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_ALTERA_K_C2) & 0x1ff) + 1,
		.fref = clk_in / ddr_dq_clk.n,
		.fvco = ddr_dq_clk.fref * ddr_dq_clk.m,
		.fout = ddr_dq_clk.fvco / (ddr_dq_clk.c * ddr_dq_clk.k)
	};

	return ddr_dq_clk;
}

tru_hps_clk_t get_h2f_user2_base_clk(float clk_in){
	uint32_t vco = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_VCO);  // Read VCO register
	uint32_t denom = vco >> 16 & 0x3f;
	uint32_t numer = vco >> 3 & 0xfff;
	uint32_t settings = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_SDRAMPLL_C5);

	tru_hps_clk_t h2f_user2_clk = {
		.n = denom + 1,
		.m = numer + 1,
		.c = (settings & 0x1ff) + 1,
		.phase = (settings >> 9 & 0xfff) * 45,
		.k = 1,
		.fref = clk_in / h2f_user2_clk.n,
		.fvco = h2f_user2_clk.fref * h2f_user2_clk.m,
		.fout = h2f_user2_clk.fvco / (h2f_user2_clk.c * h2f_user2_clk.k)
	};

	return h2f_user2_clk;
}

tru_hps_clk_t get_mpu_peri_clk(float clk_in){
	tru_hps_clk_t mpu_periph_clk = get_mpu_base_clk(clk_in);
	mpu_periph_clk.fout = mpu_periph_clk.fout / 4;
	return mpu_periph_clk;
}

tru_hps_clk_t get_mpu_l2_ram_clk(float clk_in){
	tru_hps_clk_t mpu_l2_ram_clk = get_mpu_base_clk(clk_in);
	mpu_l2_ram_clk.fout = mpu_l2_ram_clk.fout / 2;
	return mpu_l2_ram_clk;
}

tru_hps_clk_t get_l4_main_clk(float clk_in){
	return get_main_base_clk(clk_in);
}

tru_hps_clk_t get_l3_main_clk(float clk_in){
	return get_main_base_clk(clk_in);
}

tru_hps_clk_t get_dbg_at_clk(float clk_in){
	uint32_t dbgatclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_DBGDIV) & 0x3;
	dbgatclkdiv = (dbgatclkdiv == 0) ? 1 : 2 << (dbgatclkdiv - 1);

	tru_hps_clk_t dbg_at_clk = get_dbg_base_clk(clk_in);
	dbg_at_clk.fout = dbg_at_clk.fout / dbgatclkdiv;

	return dbg_at_clk;
}

tru_hps_clk_t get_dbg_clk(float clk_in){
	uint32_t div = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_DBGDIV);

	uint32_t dbgatclkdiv = div & 0x3;
	dbgatclkdiv = (dbgatclkdiv == 0) ? 1 : 2 << (dbgatclkdiv - 1);

	uint32_t dbgclkdiv = div >> 2 & 0x3;
	dbgclkdiv = (dbgclkdiv == 0) ? 1 : 2 << (dbgclkdiv - 1);
	dbgatclkdiv = (dbgatclkdiv == 0) ? 1 : 2 << (dbgatclkdiv - 1);

	tru_hps_clk_t dbg_clk = get_dbg_base_clk(clk_in);
	dbg_clk.fout = dbg_clk.fout / dbgatclkdiv / dbgclkdiv;

	return dbg_clk;
}

tru_hps_clk_t get_dbg_trace_clk(float clk_in){
	uint32_t traceclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_TRACEDIV) & 0x7;
	traceclkdiv = (traceclkdiv == 0) ? 1 : 2 << (traceclkdiv - 1);

	tru_hps_clk_t dbg_trace_clk = get_dbg_base_clk(clk_in);
	dbg_trace_clk.fout = dbg_trace_clk.fout / traceclkdiv;

	return dbg_trace_clk;
}

tru_hps_clk_t get_dbg_timer_clk(float clk_in){
	return get_dbg_base_clk(clk_in);
}

tru_hps_clk_t get_cfg_clk(float clk_in){
	return get_cfg_h2f_user0_base_clk(clk_in);
}

tru_hps_clk_t get_h2f_user0_clk(float clk_in){
	return get_cfg_h2f_user0_base_clk(clk_in);
}

tru_hps_clk_t get_l3_mp_clk(float clk_in){
	uint32_t l3mpclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_MAINDIV) & 0x3;
	l3mpclkdiv = (l3mpclkdiv == 0) ? 1 : 2 << (l3mpclkdiv - 1);

	tru_hps_clk_t l3_mp_clk = get_main_base_clk(clk_in);
	l3_mp_clk.fout = l3_mp_clk.fout / l3mpclkdiv;

	return l3_mp_clk;
}

tru_hps_clk_t get_l3_sp_clk(float clk_in){
	uint32_t div = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_MAINDIV);

	uint32_t l3mpclkdiv =div & 0x3;
	l3mpclkdiv = (l3mpclkdiv == 0) ? 1 : 2 << (l3mpclkdiv - 1);

	uint32_t l3spclkdiv = div >> 2 & 0x3;
	l3spclkdiv = (l3spclkdiv == 0) ? 1 : 2 << (l3spclkdiv - 1);

	tru_hps_clk_t l3_sp_clk = get_main_base_clk(clk_in);
	l3_sp_clk.fout = l3_sp_clk.fout / l3mpclkdiv / l3spclkdiv;

	return l3_sp_clk;
}

tru_hps_clk_t get_l4_mp_clk(float clk_in){
	uint32_t l4mpclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_MAINDIV) >> 4 & 0x7;
	l4mpclkdiv = (l4mpclkdiv == 0) ? 1 : 2 << (l4mpclkdiv - 1);

	uint32_t l4src = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_L4SRC);

	tru_hps_clk_t l4_mp_clk;
	l4_mp_clk = (l4src & 0x1) ? get_peri_base_clk(clk_in) : get_main_base_clk(clk_in);
	l4_mp_clk.fout = l4_mp_clk.fout / l4mpclkdiv;

	return l4_mp_clk;
}

tru_hps_clk_t get_l4_sp_clk(float clk_in){
	uint32_t l4spclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_MAINDIV) >> 7 & 0x7;
	l4spclkdiv = (l4spclkdiv == 0) ? 1 : 2 << (l4spclkdiv - 1);

	uint32_t l4src = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_MAINPLL_L4SRC);

	tru_hps_clk_t l4_sp_clk;
	l4_sp_clk = (l4src >> 1 & 0x1) ? get_peri_base_clk(clk_in) : get_main_base_clk(clk_in);
	l4_sp_clk.fout = l4_sp_clk.fout / l4spclkdiv;

	return l4_sp_clk;
}

tru_hps_clk_t get_emac0_clk(float clk_in){
	return get_emac0_base_clk(clk_in);
}

tru_hps_clk_t get_emac1_clk(float clk_in){
	return get_emac1_base_clk(clk_in);
}

tru_hps_clk_t get_usb_mp_clk(float clk_in){
	uint32_t usbclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_DIV) & 0x7;
	usbclkdiv = (usbclkdiv == 0) ? 1 : 2 << (usbclkdiv - 1);

	tru_hps_clk_t usb_mp_clk = get_peri_base_clk(clk_in);
	usb_mp_clk.fout = usb_mp_clk.fout / usbclkdiv;

	return usb_mp_clk;
}

tru_hps_clk_t get_spi_m_clk(float clk_in){
	uint32_t spimclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_DIV) >> 3 & 0x7;
	spimclkdiv = (spimclkdiv == 0) ? 1 : 2 << (spimclkdiv - 1);

	tru_hps_clk_t spi_m_clk = get_peri_base_clk(clk_in);
	spi_m_clk.fout = spi_m_clk.fout / spimclkdiv;

	return spi_m_clk;
}

tru_hps_clk_t get_can0_clk(float clk_in){
	uint32_t can0clkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_DIV) >> 6 & 0x7;
	can0clkdiv = (can0clkdiv == 0) ? 1 : 2 << (can0clkdiv - 1);

	tru_hps_clk_t can0_clk = get_peri_base_clk(clk_in);
	can0_clk.fout = can0_clk.fout / can0clkdiv;

	return can0_clk;
}

tru_hps_clk_t get_can1_clk(float clk_in){
	uint32_t can1clkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_DIV) >> 9 & 0x7;
	can1clkdiv = (can1clkdiv == 0) ? 1 : 2 << (can1clkdiv - 1);

	tru_hps_clk_t can1_clk = get_peri_base_clk(clk_in);
	can1_clk.fout = can1_clk.fout / can1clkdiv;

	return can1_clk;
}

tru_hps_clk_t get_gpio_db_clk(float clk_in){
	uint32_t gpiodbclkdiv = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_GPIODIV) & 0xffffff;

	tru_hps_clk_t gpio_db_clk = get_peri_base_clk(clk_in);
	gpio_db_clk.fout = gpio_db_clk.fout / gpiodbclkdiv;

	return gpio_db_clk;
}

tru_hps_clk_t get_h2f_user1_clk(float clk_in){
	return get_h2f_user1_base_clk(clk_in);
}

tru_hps_clk_t get_sdmmc_clk(float clk_in){
	uint32_t src = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_SRC);

	tru_hps_clk_t sdmmc_clk = {
		.n = 0,
		.m = 0,
		.c = 0,
		.phase = 0,
		.k = 0,
		.fref = 0,
		.fvco = 0,
		.fout = 0
	};
	switch(src & 0x3){
		case 1: sdmmc_clk = get_main_nand_sdmmc_base_clk(clk_in); break;    // Base clock from Main PLL
		case 2: sdmmc_clk = get_peri_nand_sdmmc_base_clk(clk_in); break;  // Base clock from Peripheral PLL
		case 0:                                                             // Base clock from f2h_periph_ref_clk, which is set inside FPGA Platform Designer so you would need to extract from the handoff file
		default: // Unknown values, we will return zeroes
	}

	return sdmmc_clk;
}

tru_hps_clk_t get_nand_x_clk(float clk_in){
	uint32_t src = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_SRC);

	tru_hps_clk_t nand_x_clk = {
		.n = 0,
		.m = 0,
		.c = 0,
		.phase = 0,
		.k = 0,
		.fref = 0,
		.fvco = 0,
		.fout = 0
	};
	switch(src >> 2 & 0x3){
		case 1: nand_x_clk = get_main_nand_sdmmc_base_clk(clk_in); break;    // Base clock from Main PLL
		case 2: nand_x_clk = get_peri_nand_sdmmc_base_clk(clk_in); break;  // Base clock from Peripheral PLL
		case 0:                                                              // Base clock from f2h_periph_ref_clk, which is set inside FPGA Platform Designer so you would need to extract from the handoff file
		default: // Unknown values, we will return zeroes
	}

	return nand_x_clk;
}

tru_hps_clk_t get_nand_clk(float clk_in){
	tru_hps_clk_t nand_clk = get_nand_x_clk(clk_in);
	nand_clk.fout = nand_clk.fout / 4;

	return nand_clk;
}

tru_hps_clk_t get_qspi_clk(float clk_in){
	uint32_t src = tru_iom_rd32((uint32_t *)TRU_HPS_CLKMGR_PERIPLL_SRC);

	tru_hps_clk_t qspi_clk = {
		.n = 0,
		.m = 0,
		.c = 0,
		.phase = 0,
		.k = 0,
		.fref = 0,
		.fvco = 0,
		.fout = 0
	};
	switch(src >> 4 & 0x3){
		case 1: qspi_clk = get_main_nand_sdmmc_base_clk(clk_in); break;    // Base clock from Main PLL
		case 2: qspi_clk = get_peri_nand_sdmmc_base_clk(clk_in); break;  // Base clock from Peripheral PLL
		case 0:                                                              // Base clock from f2h_periph_ref_clk, which is set inside FPGA Platform Designer so you would need to extract from the handoff file
		default: // Unknown values, we will return zeroes
	}

	return qspi_clk;
}

tru_hps_clk_t get_ddr_dqs_clk(float clk_in){
	return get_ddr_dqs_base_clk(clk_in);
}

tru_hps_clk_t get_ddr_2x_dqs_clk(float clk_in){
	return get_ddr_2x_dqs_base_clk(clk_in);
}

tru_hps_clk_t get_ddr_dq_clk(float clk_in){
	return get_ddr_dq_base_clk(clk_in);
}

tru_hps_clk_t get_h2f_user2_clk(float clk_in){
	return get_h2f_user2_base_clk(clk_in);
}

#endif
