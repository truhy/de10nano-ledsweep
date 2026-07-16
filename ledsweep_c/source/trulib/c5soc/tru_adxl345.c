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

	Version: 20260208
*/

#include "tru_adxl345.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include "tru_iom.h"
#include "c5soc/tru_rstmgr_c5soc.h"
#include "c5soc/tru_i2c_c5soc.h"

// Setup HPS I2C0 controller
void tru_adxl345_i2c_init(uint32_t l4_sp_clk_freq_hz, uint32_t i2c_dev_speed_khz, uint8_t mode10bit, uint16_t dev_addr){
	// Release I2C0 from reset
	TRU_HPS_RSTMGR_PERMODRST_REG->bits.i2c0 = 0;

	// Temporary disable I2C0
	TRU_HPS_I2C0_IC_ENABLE_REG->bits.enable = 0;

	// Setup defaults
	tru_hps_i2c_ic_con_t con = { .val = TRU_HPS_I2C0_IC_CON_REG->val };
	con.bits.master_mode = TRU_HPS_I2C_CON_MASTER_ENABLE;
	con.bits.speed = TRU_HPS_I2C_CON_SPEED_100K;
	con.bits.ic_10bitaddr_slave = TRU_HPS_I2C_CON_ADDR_7BIT;
	con.bits.ic_10bitaddr_master = TRU_HPS_I2C_CON_ADDR_7BIT;
	con.bits.ic_restart_en = TRU_HPS_I2C_CON_RESTART_ENABLE;
	con.bits.ic_slave_disable = TRU_HPS_I2C_CON_SLAVE_DISABLE;
	TRU_HPS_I2C0_IC_CON_REG->val = con.val;

	// Unmask and clear interrupt triggers
	TRU_HPS_I2C0_IC_INTR_MASK_REG->val = TRU_HPS_I2C_INTR_MASK_ENABLE_ALL;
	TRU_HPS_I2C0_IC_CLR_INTR_REG->val = TRU_HPS_I2C_CLR_INTR_ALL;

	// Enable DMA controller support
	//TRU_HPS_I2C0_IC_DMA_CR_REG->bits.rdmae = 1;
	//TRU_HPS_I2C0_IC_DMA_CR_REG->bits.tdmae = 1;

	// Enable I2C0
	TRU_HPS_I2C0_IC_ENABLE_REG->bits.enable = 1;

	// Set desired speed and filter duration
	// The low pass filter (spike filter) duration is set using number of cycles of the l4_sp_clk (default = 100MHz, so 10ns cycle)
	// TODO: Filter duration must be less than the SCL high and low time so perhaps best we calculate and use some fraction of that time,
	// anyway let's go with these fixed values for now
	if(i2c_dev_speed_khz > 100000){
		con.bits.speed = TRU_HPS_I2C_CON_SPEED_400K;
		TRU_HPS_I2C0_IC_FS_SPKLEN_REG->bits.spklen = 2;  // 20ns (at 100MHz)
	}else{
		con.bits.speed = TRU_HPS_I2C_CON_SPEED_100K;
		TRU_HPS_I2C0_IC_FS_SPKLEN_REG->bits.spklen = 8;  // 80ns (at 100MHz)
	}

	// Set low and high count using calculations taken from HWLIB
/*
	uint32_t scl_cnt = l4_sp_clk_freq_hz / (2 * i2c_dev_speed_khz);
	TRU_HPS_I2C0_IC_SS_SCL_LCNT_REG->bits.ic_ss_scl_lcnt = scl_cnt - 1;
	TRU_HPS_I2C0_IC_FS_SCL_LCNT_REG->bits.ic_fs_scl_lcnt = TRU_HPS_I2C0_IC_SS_SCL_LCNT_REG->bits.ic_ss_scl_lcnt;
	TRU_HPS_I2C0_IC_SS_SCL_HCNT_REG->bits.ic_ss_scl_hcnt = scl_cnt - TRU_HPS_I2C0_IC_FS_SPKLEN_REG->bits.spklen - 6;
	TRU_HPS_I2C0_IC_FS_SCL_HCNT_REG->bits.ic_fs_scl_hcnt = TRU_HPS_I2C0_IC_SS_SCL_HCNT_REG->bits.ic_ss_scl_hcnt;
*/

	// Set low and high count using correct calculations from my understanding
	// Corrected equations under Figure 21-12: Impact of SCL Rise Time and Fall Time on Generated SCL:
	// SCL_LOW_TIME = (LCNT + 1) * (1/ic_clk) + SCL_FALL_TIME + SCL_RISE_TIME
	// SCL_HIGH_TIME = (HCNT + IC_FS_SPKLEN + 6) * (1/ic_clk) + SCL_FALL_TIME
	// *Where ic_clk = l4_sp_clk_freq, and with equivalent units
	// The equations in the datasheet should multiply by the cycle (i.e. period), hence 1/ic_clk
	// Rearranging the above equations, adding in equivalent units and rounding up gives us:
	// LCNT = CEIL(SCL_LOW_TIME_NS * (l4_sp_clk_freq_hz / 1000000000)) - 1 - SCL_FALL_TIME_NS + SCL_RISE_TIME_NS
	// HCNT = CEIL(SCL_HIGH_TIME_NS * (l4_sp_clk_freq_hz / 1000000000)) - IC_FS_SPKLEN - 6 - SCL_FALL_TIME_NS
	TRU_HPS_I2C0_IC_SS_SCL_LCNT_REG->bits.ic_ss_scl_lcnt = TRU_ADXL345_I2C_SCL_LOW_TIME_NS * DIV_CEIL(l4_sp_clk_freq_hz, 1000000000) - 1;
	TRU_HPS_I2C0_IC_FS_SCL_LCNT_REG->bits.ic_fs_scl_lcnt = TRU_HPS_I2C0_IC_SS_SCL_LCNT_REG->bits.ic_ss_scl_lcnt;
	TRU_HPS_I2C0_IC_SS_SCL_HCNT_REG->bits.ic_ss_scl_hcnt = TRU_ADXL345_I2C_SCL_HIGH_TIME_NS * DIV_CEIL(l4_sp_clk_freq_hz, 1000000000) - TRU_HPS_I2C0_IC_FS_SPKLEN_REG->bits.spklen - 6 - TRU_ADXL345_I2C_FALL_TIME_NS;
	TRU_HPS_I2C0_IC_FS_SCL_HCNT_REG->bits.ic_fs_scl_hcnt = TRU_HPS_I2C0_IC_SS_SCL_HCNT_REG->bits.ic_ss_scl_hcnt;

	// Set device address
	TRU_HPS_I2C0_IC_TAR_REG->bits.ic_10bitaddr_master = mode10bit;
	TRU_HPS_I2C0_IC_TAR_REG->bits.ic_tar = dev_addr;
}

// Reads using burst mode (multiple I2C reads), making better use of the FIFO
// Read data using HPS I2C0 controller
void tru_adxl345_i2c_read_bm(void *buf, uint32_t len, uint32_t reg_addr_start){
	uint8_t *buf8 = buf;
	tru_hps_i2c_ic_data_cmd_t data_cmd = { .val = 0 };
	uint8_t txremain;
	uint8_t rxremain;

	// Send write command and register address
	while(TRU_HPS_I2C0_IC_STATUS_REG->bits.tfnf == 0);  // Ensure TXFIFO is not full
	data_cmd.bits.cmd = TRU_HPS_I2C_DATA_CMD_MASTER_WRITE;
	data_cmd.bits.dat = reg_addr_start;
	data_cmd.bits.restart = TRU_HPS_I2C_DATA_CMD_ISSUE_RESTART_YES;
	data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_NO;
	TRU_HPS_I2C0_IC_DATA_CMD_REG->val = data_cmd.val;

	// Set common read values
	data_cmd.bits.cmd = TRU_HPS_I2C_DATA_CMD_MASTER_READ;
	data_cmd.bits.dat = 0;
	data_cmd.bits.restart = TRU_HPS_I2C_DATA_CMD_ISSUE_RESTART_NO;
	data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_NO;
	while(len){
		// Note, we assume the TXFIFO and RXFIFO is empty
		// The strategy is to make use of the FIFOs by filling them up first
		// then read the data out in blocks.  Although not used here, it would
		// benefit with DMA transfers

		// Calculate limit
		txremain = (len < TRU_HPS_I2C_RXFIFO_DEPTH) ? len : TRU_HPS_I2C_RXFIFO_DEPTH;
		rxremain = txremain;

		// Send read commands to fill up the RXFIFO
		while(txremain){
			while(TRU_HPS_I2C0_IC_STATUS_REG->bits.tfnf == 0);  // Ensure TXFIFO is not full
			len--;
			if(len == 0) data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_YES;
			TRU_HPS_I2C0_IC_DATA_CMD_REG->val = data_cmd.val;
			txremain--;
		}

		// Now we read the received data from the RXFIFO
		while(rxremain){
			while(TRU_HPS_I2C0_IC_STATUS_REG->bits.rfne == 0);  // Wait till we have data
			buf8[0] = TRU_HPS_I2C0_IC_DATA_CMD_REG->bits.dat;  // Store the received data
			buf8++;
			rxremain--;
		}
	}
}

// Read data using HPS I2C0 controller
void tru_adxl345_i2c_read(void *buf, uint32_t len, uint32_t reg_addr_start){
	uint8_t *buf8 = buf;
	tru_hps_i2c_ic_data_cmd_t data_cmd = { .val = 0 };

	// Send write command and register address
	while(TRU_HPS_I2C0_IC_STATUS_REG->bits.tfnf == 0);  // Ensure TXFIFO is not full
	data_cmd.bits.cmd = TRU_HPS_I2C_DATA_CMD_MASTER_WRITE;
	data_cmd.bits.dat = reg_addr_start;
	data_cmd.bits.restart = TRU_HPS_I2C_DATA_CMD_ISSUE_RESTART_YES;
	data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_NO;
	TRU_HPS_I2C0_IC_DATA_CMD_REG->val = data_cmd.val;

	// Set common read values
	data_cmd.bits.cmd = TRU_HPS_I2C_DATA_CMD_MASTER_READ;
	data_cmd.bits.dat = 0;
	data_cmd.bits.restart = TRU_HPS_I2C_DATA_CMD_ISSUE_RESTART_NO;
	data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_NO;
	while(len){
		// Send read command
		while(TRU_HPS_I2C0_IC_STATUS_REG->bits.tfnf == 0);  // Ensure TXFIFO is not full
		len--;
		if(len == 0) data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_YES;
		TRU_HPS_I2C0_IC_DATA_CMD_REG->val = data_cmd.val;

		// Store the received data
		while(TRU_HPS_I2C0_IC_STATUS_REG->bits.rfne == 0);  // Wait till we have data
		buf8[0] = TRU_HPS_I2C0_IC_DATA_CMD_REG->bits.dat;
		buf8++;
	}
}

// Write data using HPS I2C0 controller
void tru_adxl345_i2c_write(void *buf, uint32_t len, uint32_t reg_addr_start){
	uint8_t *buf8 = buf;
	tru_hps_i2c_ic_data_cmd_t data_cmd = { .val = 0 };

	// Send write command and register address
	while(TRU_HPS_I2C0_IC_STATUS_REG->bits.tfnf == 0);  // Ensure TXFIFO is not full
	data_cmd.bits.cmd = TRU_HPS_I2C_DATA_CMD_MASTER_WRITE;
	data_cmd.bits.dat = reg_addr_start;
	data_cmd.bits.restart = TRU_HPS_I2C_DATA_CMD_ISSUE_RESTART_NO;
	data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_NO;
	TRU_HPS_I2C0_IC_DATA_CMD_REG->val = data_cmd.val;

	while(len){
		// Send data
		while(TRU_HPS_I2C0_IC_STATUS_REG->bits.tfnf == 0);  // Ensure TXFIFO is not full
		len--;
		data_cmd.bits.dat = buf8[0];
		if(len == 0) data_cmd.bits.stop = TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_YES;
		TRU_HPS_I2C0_IC_DATA_CMD_REG->val = data_cmd.val;
		buf8++;
	}
}

void tru_adxl345_i2c_stop_flush_fifo(void){
	uint8_t buf[1];

	TRU_ADXL345_POWER_CTL_PTR(buf)->val = 0;
	TRU_ADXL345_POWER_CTL_PTR(buf)->bits.measure = 0;
	tru_adxl345_i2c_write(buf, 1, TRU_ADXL345_POWER_CTL_ADDR);

	TRU_ADXL345_FIFO_CTL_PTR(buf)->val = 0;
	TRU_ADXL345_FIFO_CTL_PTR(buf)->bits.fifomode = TRU_ADXL345_FIFOMODE_BYPASS;
	tru_adxl345_i2c_write(buf, 1, TRU_ADXL345_FIFO_CTL_ADDR);
}

#endif
