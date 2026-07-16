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

	I2C controller (Synopsys I2C controller) low-level support for Cyclone V SoC HPS.
*/

#ifndef TRU_I2C_C5SOC_H
#define TRU_I2C_C5SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include <stdint.h>

// =====================================================================
// Intel Cyclone V SoC FPGA (Synopsys I2C controller) specific registers
// =====================================================================

#define TRU_HPS_I2C_TXFIFO_DEPTH 64
#define TRU_HPS_I2C_RXFIFO_DEPTH 64

#define TRU_HPS_I2C_CON_MASTER_ENABLE   0x1
#define TRU_HPS_I2C_CON_MASTER_DISABLE  0x0
#define TRU_HPS_I2C_CON_SPEED_100K      0x1
#define TRU_HPS_I2C_CON_SPEED_400K      0x2
#define TRU_HPS_I2C_CON_ADDR_7BIT       0x0
#define TRU_HPS_I2C_CON_ADDR_10BIT      0x1
#define TRU_HPS_I2C_CON_RESTART_ENABLE  0x1
#define TRU_HPS_I2C_CON_RESTART_DISABLE 0x0
#define TRU_HPS_I2C_CON_SLAVE_ENABLE    0x0
#define TRU_HPS_I2C_CON_SLAVE_DISABLE   0x1

#define TRU_HPS_I2C_INTR_MASK_ENABLE_ALL 0xfff

#define TRU_HPS_I2C_CLR_INTR_ALL 0xfff

#define TRU_HPS_I2C_DATA_CMD_MASTER_READ       0x1
#define TRU_HPS_I2C_DATA_CMD_MASTER_WRITE      0x0
#define TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_YES    0x1
#define TRU_HPS_I2C_DATA_CMD_ISSUE_STOP_NO     0x0
#define TRU_HPS_I2C_DATA_CMD_ISSUE_RESTART_YES 0x1
#define TRU_HPS_I2C_DATA_CMD_ISSUE_RESTART_NO  0x0

// Generic
#define TRU_HPS_I2C_IC_CON_OFFSET                0x0
#define TRU_HPS_I2C_IC_TAR_OFFSET                0x4
#define TRU_HPS_I2C_IC_SAR_OFFSET                0x8
#define TRU_HPS_I2C_IC_DATA_CMD_OFFSET           0x10
#define TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET        0x14
#define TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET        0x18
#define TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET        0x1c
#define TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET        0x20
#define TRU_HPS_I2C_IC_INTR_STAT_OFFSET          0x2c
#define TRU_HPS_I2C_IC_INTR_MASK_OFFSET          0x30
#define TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET      0x34
#define TRU_HPS_I2C_IC_RX_TL_OFFSET              0x38
#define TRU_HPS_I2C_IC_TX_TL_OFFSET              0x3c
#define TRU_HPS_I2C_IC_CLR_INTR_OFFSET           0x40
#define TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET       0x44
#define TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET        0x48
#define TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET        0x4c
#define TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET         0x50
#define TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET        0x54
#define TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET        0x58
#define TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET       0x5c
#define TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET       0x60
#define TRU_HPS_I2C_IC_CLR_START_DET_OFFSET      0x64
#define TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET       0x68
#define TRU_HPS_I2C_IC_ENABLE_OFFSET             0x6c
#define TRU_HPS_I2C_IC_STATUS_OFFSET             0x70
#define TRU_HPS_I2C_IC_TXFLR_OFFSET              0x74
#define TRU_HPS_I2C_IC_RXFLR_OFFSET              0x78
#define TRU_HPS_I2C_IC_SDA_HOLD_OFFSET           0x7c
#define TRU_HPS_I2C_IC_ABRT_SOURCE_OFFSET        0x80
#define TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET 0x84
#define TRU_HPS_I2C_IC_DMA_CR_OFFSET             0x88
#define TRU_HPS_I2C_IC_DMA_TDLR_OFFSET           0x8c
#define TRU_HPS_I2C_IC_DMA_RDLR_OFFSET           0x90
#define TRU_HPS_I2C_IC_SDA_SETUP_OFFSET          0x94
#define TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET   0x98
#define TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET      0x9c
#define TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET          0xa0
#define TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET       0xf4
#define TRU_HPS_I2C_IC_COMP_VERSION_OFFSET       0xf8
#define TRU_HPS_I2C_IC_COMP_TYPE_OFFSET          0xfc

// HPS I2C0 registers
#define TRU_HPS_I2C0_BASE                       0xffc04000
#define TRU_HPS_I2C0_IC_CON_ADDR                (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CON_OFFSET)
#define TRU_HPS_I2C0_IC_TAR_ADDR                (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_TAR_OFFSET)
#define TRU_HPS_I2C0_IC_SAR_ADDR                (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SAR_OFFSET)
#define TRU_HPS_I2C0_IC_DATA_CMD_ADDR           (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET)
#define TRU_HPS_I2C0_IC_SS_SCL_HCNT_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C0_IC_SS_SCL_LCNT_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C0_IC_FS_SCL_HCNT_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C0_IC_FS_SCL_LCNT_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C0_IC_INTR_STAT_ADDR          (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET)
#define TRU_HPS_I2C0_IC_INTR_MASK_ADDR          (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET)
#define TRU_HPS_I2C0_IC_RAW_INTR_STAT_ADDR      (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET)
#define TRU_HPS_I2C0_IC_RX_TL_ADDR              (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET)
#define TRU_HPS_I2C0_IC_TX_TL_ADDR              (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_INTR_ADDR           (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_RX_UNDER_ADDR       (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_RX_OVER_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_TX_OVER_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_RD_REQ_ADDR         (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_TX_ABRT_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_RX_DONE_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_ACTIVITY_ADDR       (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_STOP_DET_ADDR       (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_START_DET_ADDR      (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET)
#define TRU_HPS_I2C0_IC_CLR_GEN_CALL_ADDR       (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET)
#define TRU_HPS_I2C0_IC_ENABLE_ADDR             (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET)
#define TRU_HPS_I2C0_IC_STATUS_ADDR             (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET)
#define TRU_HPS_I2C0_IC_TXFLR_ADDR              (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET)
#define TRU_HPS_I2C0_IC_RXFLR_ADDR              (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET)
#define TRU_HPS_I2C0_IC_SDA_HOLD_ADDR           (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET)
#define TRU_HPS_I2C0_IC_ABRT_SOURCE_ADDR        (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_ABRT_SOURCE_OFFSET)
#define TRU_HPS_I2C0_IC_SLV_DATA_NACK_ONLY_ADDR (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET)
#define TRU_HPS_I2C0_IC_DMA_CR_ADDR             (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET)
#define TRU_HPS_I2C0_IC_SDA_SETUP_ADDR          (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET)
#define TRU_HPS_I2C0_IC_ACK_GENERAL_CALL_ADDR   (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET)
#define TRU_HPS_I2C0_IC_ENABLE_STATUS_ADDR      (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET)
#define TRU_HPS_I2C0_IC_FS_SPKLEN_ADDR          (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET)
#define TRU_HPS_I2C0_IC_COMP_PARAM_1_ADDR       (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET)
#define TRU_HPS_I2C0_IC_COMP_VERSION_ADDR       (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET)
#define TRU_HPS_I2C0_IC_COMP_TYPE_ADDR          (TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET)

// HPS I2C1 registers
#define TRU_HPS_I2C1_BASE                       0xffc05000
#define TRU_HPS_I2C1_IC_CON_ADDR                (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CON_OFFSET)
#define TRU_HPS_I2C1_IC_TAR_ADDR                (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_TAR_OFFSET)
#define TRU_HPS_I2C1_IC_SAR_ADDR                (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SAR_OFFSET)
#define TRU_HPS_I2C1_IC_DATA_CMD_ADDR           (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET)
#define TRU_HPS_I2C1_IC_SS_SCL_HCNT_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C1_IC_SS_SCL_LCNT_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C1_IC_FS_SCL_HCNT_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C1_IC_FS_SCL_LCNT_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C1_IC_INTR_STAT_ADDR          (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET)
#define TRU_HPS_I2C1_IC_INTR_MASK_ADDR          (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET)
#define TRU_HPS_I2C1_IC_RAW_INTR_STAT_ADDR      (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET)
#define TRU_HPS_I2C1_IC_RX_TL_ADDR              (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET)
#define TRU_HPS_I2C1_IC_TX_TL_ADDR              (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_INTR_ADDR           (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_RX_UNDER_ADDR       (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_RX_OVER_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_TX_OVER_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_RD_REQ_ADDR         (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_TX_ABRT_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_RX_DONE_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_ACTIVITY_ADDR       (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_STOP_DET_ADDR       (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_START_DET_ADDR      (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET)
#define TRU_HPS_I2C1_IC_CLR_GEN_CALL_ADDR       (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET)
#define TRU_HPS_I2C1_IC_ENABLE_ADDR             (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET)
#define TRU_HPS_I2C1_IC_STATUS_ADDR             (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET)
#define TRU_HPS_I2C1_IC_TXFLR_ADDR              (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET)
#define TRU_HPS_I2C1_IC_RXFLR_ADDR              (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET)
#define TRU_HPS_I2C1_IC_SDA_HOLD_ADDR           (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET)
#define TRU_HPS_I2C1_IC_ABRT_SOURCE_ADDR        (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_ABRT_SOURCE_OFFSET)
#define TRU_HPS_I2C1_IC_SLV_DATA_NACK_ONLY_ADDR (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET)
#define TRU_HPS_I2C1_IC_DMA_CR_ADDR             (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET)
#define TRU_HPS_I2C1_IC_SDA_SETUP_ADDR          (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET)
#define TRU_HPS_I2C1_IC_ACK_GENERAL_CALL_ADDR   (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET)
#define TRU_HPS_I2C1_IC_ENABLE_STATUS_ADDR      (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET)
#define TRU_HPS_I2C1_IC_FS_SPKLEN_ADDR          (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET)
#define TRU_HPS_I2C1_IC_COMP_PARAM_1_ADDR       (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET)
#define TRU_HPS_I2C1_IC_COMP_VERSION_ADDR       (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET)
#define TRU_HPS_I2C1_IC_COMP_TYPE_ADDR          (TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET)

// HPS I2C2 registers
#define TRU_HPS_I2C2_BASE                       0xffc06000
#define TRU_HPS_I2C2_IC_CON_ADDR                (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CON_OFFSET)
#define TRU_HPS_I2C2_IC_TAR_ADDR                (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_TAR_OFFSET)
#define TRU_HPS_I2C2_IC_SAR_ADDR                (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SAR_OFFSET)
#define TRU_HPS_I2C2_IC_DATA_CMD_ADDR           (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET)
#define TRU_HPS_I2C2_IC_SS_SCL_HCNT_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C2_IC_SS_SCL_LCNT_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C2_IC_FS_SCL_HCNT_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C2_IC_FS_SCL_LCNT_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C2_IC_INTR_STAT_ADDR          (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET)
#define TRU_HPS_I2C2_IC_INTR_MASK_ADDR          (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET)
#define TRU_HPS_I2C2_IC_RAW_INTR_STAT_ADDR      (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET)
#define TRU_HPS_I2C2_IC_RX_TL_ADDR              (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET)
#define TRU_HPS_I2C2_IC_TX_TL_ADDR              (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_INTR_ADDR           (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_RX_UNDER_ADDR       (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_RX_OVER_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_TX_OVER_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_RD_REQ_ADDR         (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_TX_ABRT_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_RX_DONE_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_ACTIVITY_ADDR       (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_STOP_DET_ADDR       (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_START_DET_ADDR      (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET)
#define TRU_HPS_I2C2_IC_CLR_GEN_CALL_ADDR       (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET)
#define TRU_HPS_I2C2_IC_ENABLE_ADDR             (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET)
#define TRU_HPS_I2C2_IC_STATUS_ADDR             (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET)
#define TRU_HPS_I2C2_IC_TXFLR_ADDR              (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET)
#define TRU_HPS_I2C2_IC_RXFLR_ADDR              (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET)
#define TRU_HPS_I2C2_IC_SDA_HOLD_ADDR           (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET)
#define TRU_HPS_I2C2_IC_ABRT_SOURCE_ADDR        (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_ABRT_SOURCE_OFFSET)
#define TRU_HPS_I2C2_IC_SLV_DATA_NACK_ONLY_ADDR (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET)
#define TRU_HPS_I2C2_IC_DMA_CR_ADDR             (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET)
#define TRU_HPS_I2C2_IC_SDA_SETUP_ADDR          (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET)
#define TRU_HPS_I2C2_IC_ACK_GENERAL_CALL_ADDR   (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET)
#define TRU_HPS_I2C2_IC_ENABLE_STATUS_ADDR      (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET)
#define TRU_HPS_I2C2_IC_FS_SPKLEN_ADDR          (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET)
#define TRU_HPS_I2C2_IC_COMP_PARAM_1_ADDR       (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET)
#define TRU_HPS_I2C2_IC_COMP_VERSION_ADDR       (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET)
#define TRU_HPS_I2C2_IC_COMP_TYPE_ADDR          (TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET)

// HPS I2C3 registers
#define TRU_HPS_I2C3_BASE                       0xffc07000
#define TRU_HPS_I2C3_IC_CON_ADDR                (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CON_OFFSET)
#define TRU_HPS_I2C3_IC_TAR_ADDR                (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_TAR_OFFSET)
#define TRU_HPS_I2C3_IC_SAR_ADDR                (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SAR_OFFSET)
#define TRU_HPS_I2C3_IC_DATA_CMD_ADDR           (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET)
#define TRU_HPS_I2C3_IC_SS_SCL_HCNT_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C3_IC_SS_SCL_LCNT_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C3_IC_FS_SCL_HCNT_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET)
#define TRU_HPS_I2C3_IC_FS_SCL_LCNT_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET)
#define TRU_HPS_I2C3_IC_INTR_STAT_ADDR          (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET)
#define TRU_HPS_I2C3_IC_INTR_MASK_ADDR          (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET)
#define TRU_HPS_I2C3_IC_RAW_INTR_STAT_ADDR      (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET)
#define TRU_HPS_I2C3_IC_RX_TL_ADDR              (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET)
#define TRU_HPS_I2C3_IC_TX_TL_ADDR              (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_INTR_ADDR           (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_RX_UNDER_ADDR       (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_RX_OVER_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_TX_OVER_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_RD_REQ_ADDR         (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_TX_ABRT_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_RX_DONE_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_ACTIVITY_ADDR       (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_STOP_DET_ADDR       (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_START_DET_ADDR      (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET)
#define TRU_HPS_I2C3_IC_CLR_GEN_CALL_ADDR       (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET)
#define TRU_HPS_I2C3_IC_ENABLE_ADDR             (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET)
#define TRU_HPS_I2C3_IC_STATUS_ADDR             (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET)
#define TRU_HPS_I2C3_IC_TXFLR_ADDR              (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET)
#define TRU_HPS_I2C3_IC_RXFLR_ADDR              (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET)
#define TRU_HPS_I2C3_IC_SDA_HOLD_ADDR           (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET)
#define TRU_HPS_I2C3_IC_ABRT_SOURCE_ADDR        (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_ABRT_SOURCE_OFFSET)
#define TRU_HPS_I2C3_IC_SLV_DATA_NACK_ONLY_ADDR (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET)
#define TRU_HPS_I2C3_IC_DMA_CR_ADDR             (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET)
#define TRU_HPS_I2C3_IC_SDA_SETUP_ADDR          (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET)
#define TRU_HPS_I2C3_IC_ACK_GENERAL_CALL_ADDR   (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET)
#define TRU_HPS_I2C3_IC_ENABLE_STATUS_ADDR      (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET)
#define TRU_HPS_I2C3_IC_FS_SPKLEN_ADDR          (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET)
#define TRU_HPS_I2C3_IC_COMP_PARAM_1_ADDR       (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET)
#define TRU_HPS_I2C3_IC_COMP_VERSION_ADDR       (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET)
#define TRU_HPS_I2C3_IC_COMP_TYPE_ADDR          (TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET)

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t master_mode         :1;
		volatile uint32_t speed               :2;
		volatile uint32_t ic_10bitaddr_slave  :1;
		volatile uint32_t ic_10bitaddr_master :1;
		volatile uint32_t ic_restart_en       :1;
		volatile uint32_t ic_slave_disable    :1;
		volatile uint32_t res7_31             :25;
    }bits;
}tru_hps_i2c_ic_con_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t master_mode         :1;
		uint32_t speed               :2;
		uint32_t ic_10bitaddr_slave  :1;
		uint32_t ic_10bitaddr_master :1;
		uint32_t ic_restart_en       :1;
		uint32_t ic_slave_disable    :1;
		uint32_t res7_31             :25;
	}bits;
}tru_hps_i2c_ic_con_t;

#define TRU_HPS_I2C_IC_CON_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_con_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CON_OFFSET))
#define TRU_HPS_I2C0_IC_CON_REG               ((volatile tru_hps_i2c_ic_con_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CON_OFFSET))
#define TRU_HPS_I2C1_IC_CON_REG               ((volatile tru_hps_i2c_ic_con_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CON_OFFSET))
#define TRU_HPS_I2C2_IC_CON_REG               ((volatile tru_hps_i2c_ic_con_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CON_OFFSET))
#define TRU_HPS_I2C3_IC_CON_REG               ((volatile tru_hps_i2c_ic_con_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CON_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_tar              :10;
		volatile uint32_t gc_or_start         :1;
		volatile uint32_t special             :1;
		volatile uint32_t ic_10bitaddr_master :1;
		volatile uint32_t res13_31            :19;
    }bits;
}tru_hps_i2c_ic_tar_reg_t;

#define TRU_HPS_I2C_IC_TAR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_tar_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_TAR_OFFSET))
#define TRU_HPS_I2C0_IC_TAR_REG               ((volatile tru_hps_i2c_ic_tar_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_TAR_OFFSET))
#define TRU_HPS_I2C1_IC_TAR_REG               ((volatile tru_hps_i2c_ic_tar_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_TAR_OFFSET))
#define TRU_HPS_I2C2_IC_TAR_REG               ((volatile tru_hps_i2c_ic_tar_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_TAR_OFFSET))
#define TRU_HPS_I2C3_IC_TAR_REG               ((volatile tru_hps_i2c_ic_tar_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_TAR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_sar   :10;
		volatile uint32_t res10_31 :22;
    }bits;
}tru_hps_i2c_ic_sar_reg_t;

#define TRU_HPS_I2C_IC_SAR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_sar_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_SAR_OFFSET))
#define TRU_HPS_I2C0_IC_SAR_REG               ((volatile tru_hps_i2c_ic_sar_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SAR_OFFSET))
#define TRU_HPS_I2C1_IC_SAR_REG               ((volatile tru_hps_i2c_ic_sar_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SAR_OFFSET))
#define TRU_HPS_I2C2_IC_SAR_REG               ((volatile tru_hps_i2c_ic_sar_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SAR_OFFSET))
#define TRU_HPS_I2C3_IC_SAR_REG               ((volatile tru_hps_i2c_ic_sar_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SAR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t dat      :8;
		volatile uint32_t cmd      :1;
		volatile uint32_t stop     :1;
		volatile uint32_t restart  :1;
		volatile uint32_t res11_31 :21;
    }bits;
}tru_hps_i2c_ic_data_cmd_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t dat      :8;
		uint32_t cmd      :1;
		uint32_t stop     :1;
		uint32_t restart  :1;
		uint32_t res11_31 :21;
    }bits;
}tru_hps_i2c_ic_data_cmd_t;

#define TRU_HPS_I2C_IC_DATA_CMD_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_data_cmd_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_DATA_CMD_OFFSET))
#define TRU_HPS_I2C0_IC_DATA_CMD_REG               ((volatile tru_hps_i2c_ic_data_cmd_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET))
#define TRU_HPS_I2C1_IC_DATA_CMD_REG               ((volatile tru_hps_i2c_ic_data_cmd_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET))
#define TRU_HPS_I2C2_IC_DATA_CMD_REG               ((volatile tru_hps_i2c_ic_data_cmd_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET))
#define TRU_HPS_I2C3_IC_DATA_CMD_REG               ((volatile tru_hps_i2c_ic_data_cmd_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_DATA_CMD_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_ss_scl_hcnt :16;
		volatile uint32_t res16_31       :16;
    }bits;
}tru_hps_i2c_ic_ss_scl_hcnt_reg_t;

#define TRU_HPS_I2C_IC_SS_SCL_HCNT_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_ss_scl_hcnt_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C0_IC_SS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C1_IC_SS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C2_IC_SS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C3_IC_SS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SS_SCL_HCNT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_ss_scl_lcnt :16;
		volatile uint32_t res16_31       :16;
    }bits;
}tru_hps_i2c_ic_ss_scl_lcnt_reg_t;

#define TRU_HPS_I2C_IC_SS_SCL_LCNT_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_ss_scl_lcnt_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C0_IC_SS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C1_IC_SS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C2_IC_SS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C3_IC_SS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_ss_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SS_SCL_LCNT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_fs_scl_hcnt :16;
		volatile uint32_t res16_31       :16;
    }bits;
}tru_hps_i2c_ic_fs_scl_hcnt_reg_t;

#define TRU_HPS_I2C_IC_FS_SCL_HCNT_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_fs_scl_hcnt_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C0_IC_FS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C1_IC_FS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C2_IC_FS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET))
#define TRU_HPS_I2C3_IC_FS_SCL_HCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_hcnt_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_FS_SCL_HCNT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_fs_scl_lcnt :16;
		volatile uint32_t res16_31       :16;
    }bits;
}tru_hps_i2c_ic_fs_scl_lcnt_reg_t;

#define TRU_HPS_I2C_IC_FS_SCL_LCNT_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_fs_scl_lcnt_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C0_IC_FS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C1_IC_FS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C2_IC_FS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET))
#define TRU_HPS_I2C3_IC_FS_SCL_LCNT_REG               ((volatile tru_hps_i2c_ic_fs_scl_lcnt_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_FS_SCL_LCNT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t r_rx_under  :1;
		volatile uint32_t r_rx_over   :1;
		volatile uint32_t r_rx_full   :1;
		volatile uint32_t r_tx_over   :1;
		volatile uint32_t r_tx_empty  :1;
		volatile uint32_t r_rd_req    :1;
		volatile uint32_t r_tx_abrt   :1;
		volatile uint32_t r_rx_done   :1;
		volatile uint32_t r_activity  :1;
		volatile uint32_t r_stop_det  :1;
		volatile uint32_t r_start_det :1;
		volatile uint32_t r_gen_call  :1;
		volatile uint32_t res12_31    :20;
    }bits;
}tru_hps_i2c_ic_intr_stat_reg_t;

#define TRU_HPS_I2C_IC_INTR_STAT_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_intr_stat_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_INTR_STAT_OFFSET))
#define TRU_HPS_I2C0_IC_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_intr_stat_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET))
#define TRU_HPS_I2C1_IC_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_intr_stat_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET))
#define TRU_HPS_I2C2_IC_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_intr_stat_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET))
#define TRU_HPS_I2C3_IC_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_intr_stat_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_INTR_STAT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t m_rx_under  :1;
		volatile uint32_t m_rx_over   :1;
		volatile uint32_t m_rx_full   :1;
		volatile uint32_t m_tx_over   :1;
		volatile uint32_t m_tx_empty  :1;
		volatile uint32_t m_rd_req    :1;
		volatile uint32_t m_tx_abrt   :1;
		volatile uint32_t m_rx_done   :1;
		volatile uint32_t m_activity  :1;
		volatile uint32_t m_stop_det  :1;
		volatile uint32_t m_start_det :1;
		volatile uint32_t m_gen_call  :1;
		volatile uint32_t res12_31    :20;
    }bits;
}tru_hps_i2c_ic_intr_mask_reg_t;

#define TRU_HPS_I2C_IC_INTR_MASK_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_intr_mask_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_INTR_MASK_OFFSET))
#define TRU_HPS_I2C0_IC_INTR_MASK_REG               ((volatile tru_hps_i2c_ic_intr_mask_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET))
#define TRU_HPS_I2C1_IC_INTR_MASK_REG               ((volatile tru_hps_i2c_ic_intr_mask_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET))
#define TRU_HPS_I2C2_IC_INTR_MASK_REG               ((volatile tru_hps_i2c_ic_intr_mask_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET))
#define TRU_HPS_I2C3_IC_INTR_MASK_REG               ((volatile tru_hps_i2c_ic_intr_mask_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_INTR_MASK_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t r_rx_under  :1;
		volatile uint32_t r_rx_over   :1;
		volatile uint32_t r_rx_full   :1;
		volatile uint32_t r_tx_over   :1;
		volatile uint32_t r_tx_empty  :1;
		volatile uint32_t r_rd_req    :1;
		volatile uint32_t r_tx_abrt   :1;
		volatile uint32_t r_rx_done   :1;
		volatile uint32_t r_activity  :1;
		volatile uint32_t r_stop_det  :1;
		volatile uint32_t r_start_det :1;
		volatile uint32_t r_gen_call  :1;
		volatile uint32_t res12_31    :20;
    }bits;
}tru_hps_i2c_ic_raw_intr_stat_reg_t;

#define TRU_HPS_I2C_IC_RAW_INTR_STAT_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_raw_intr_stat_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET))
#define TRU_HPS_I2C0_IC_RAW_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_raw_intr_stat_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET))
#define TRU_HPS_I2C1_IC_RAW_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_raw_intr_stat_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET))
#define TRU_HPS_I2C2_IC_RAW_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_raw_intr_stat_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET))
#define TRU_HPS_I2C3_IC_RAW_INTR_STAT_REG               ((volatile tru_hps_i2c_ic_raw_intr_stat_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_RAW_INTR_STAT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t rx_tl   :8;
		volatile uint32_t res8_31 :24;
    }bits;
}tru_hps_i2c_ic_rx_tl_reg_t;

#define TRU_HPS_I2C_IC_RX_TL_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_rx_tl_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_RX_TL_OFFSET))
#define TRU_HPS_I2C0_IC_RX_TL_REG               ((volatile tru_hps_i2c_ic_rx_tl_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET))
#define TRU_HPS_I2C1_IC_RX_TL_REG               ((volatile tru_hps_i2c_ic_rx_tl_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET))
#define TRU_HPS_I2C2_IC_RX_TL_REG               ((volatile tru_hps_i2c_ic_rx_tl_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET))
#define TRU_HPS_I2C3_IC_RX_TL_REG               ((volatile tru_hps_i2c_ic_rx_tl_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_RX_TL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t tx_tl   :8;
		volatile uint32_t res8_31 :24;
    }bits;
}tru_hps_i2c_ic_tx_tl_reg_t;

#define TRU_HPS_I2C_IC_TX_TL_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_tx_tl_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_TX_TL_OFFSET))
#define TRU_HPS_I2C0_IC_TX_TL_REG               ((volatile tru_hps_i2c_ic_tx_tl_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET))
#define TRU_HPS_I2C1_IC_TX_TL_REG               ((volatile tru_hps_i2c_ic_tx_tl_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET))
#define TRU_HPS_I2C2_IC_TX_TL_REG               ((volatile tru_hps_i2c_ic_tx_tl_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET))
#define TRU_HPS_I2C3_IC_TX_TL_REG               ((volatile tru_hps_i2c_ic_tx_tl_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_TX_TL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_intr :1;
		volatile uint32_t res1_31  :31;
    }bits;
}tru_hps_i2c_ic_clr_intr_reg_t;

#define TRU_HPS_I2C_IC_CLR_INTR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_intr_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_INTR_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_INTR_REG               ((volatile tru_hps_i2c_ic_clr_intr_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_INTR_REG               ((volatile tru_hps_i2c_ic_clr_intr_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_INTR_REG               ((volatile tru_hps_i2c_ic_clr_intr_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_INTR_REG               ((volatile tru_hps_i2c_ic_clr_intr_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_INTR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_rx_under :1;
		volatile uint32_t res1_31      :31;
    }bits;
}tru_hps_i2c_ic_clr_rx_under_reg_t;

#define TRU_HPS_I2C_IC_CLR_RX_UNDER_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_rx_under_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_RX_UNDER_REG               ((volatile tru_hps_i2c_ic_clr_rx_under_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_RX_UNDER_REG               ((volatile tru_hps_i2c_ic_clr_rx_under_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_RX_UNDER_REG               ((volatile tru_hps_i2c_ic_clr_rx_under_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_RX_UNDER_REG               ((volatile tru_hps_i2c_ic_clr_rx_under_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RX_UNDER_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_rx_over :1;
		volatile uint32_t res1_31     :31;
    }bits;
}tru_hps_i2c_ic_clr_rx_over_reg_t;

#define TRU_HPS_I2C_IC_CLR_RX_OVER_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_rx_over_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_RX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_rx_over_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_RX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_rx_over_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_RX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_rx_over_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_RX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_rx_over_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RX_OVER_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_tx_over :1;
		volatile uint32_t res1_31     :31;
    }bits;
}tru_hps_i2c_ic_clr_tx_over_reg_t;

#define TRU_HPS_I2C_IC_CLR_TX_OVER_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_tx_over_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_TX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_tx_over_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_TX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_tx_over_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_TX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_tx_over_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_TX_OVER_REG               ((volatile tru_hps_i2c_ic_clr_tx_over_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_TX_OVER_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_rd_req :1;
		volatile uint32_t res1_31    :31;
    }bits;
}tru_hps_i2c_ic_clr_rd_req_reg_t;

#define TRU_HPS_I2C_IC_CLR_RD_REQ_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_rd_req_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_RD_REQ_REG               ((volatile tru_hps_i2c_ic_clr_rd_req_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_RD_REQ_REG               ((volatile tru_hps_i2c_ic_clr_rd_req_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_RD_REQ_REG               ((volatile tru_hps_i2c_ic_clr_rd_req_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_RD_REQ_REG               ((volatile tru_hps_i2c_ic_clr_rd_req_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RD_REQ_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_tx_abrt :1;
		volatile uint32_t res1_31     :31;
    }bits;
}tru_hps_i2c_ic_clr_tx_abrt_reg_t;

#define TRU_HPS_I2C_IC_CLR_TX_ABRT_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_tx_abrt_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_TX_ABRT_REG               ((volatile tru_hps_i2c_ic_clr_tx_abrt_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_TX_ABRT_REG               ((volatile tru_hps_i2c_ic_clr_tx_abrt_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_TX_ABRT_REG               ((volatile tru_hps_i2c_ic_clr_tx_abrt_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_TX_ABRT_REG               ((volatile tru_hps_i2c_ic_clr_tx_abrt_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_TX_ABRT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_rx_done :1;
		volatile uint32_t res1_31     :31;
    }bits;
}tru_hps_i2c_ic_clr_rx_done_reg_t;

#define TRU_HPS_I2C_IC_CLR_RX_DONE_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_rx_done_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_RX_DONE_REG               ((volatile tru_hps_i2c_ic_clr_rx_done_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_RX_DONE_REG               ((volatile tru_hps_i2c_ic_clr_rx_done_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_RX_DONE_REG               ((volatile tru_hps_i2c_ic_clr_rx_done_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_RX_DONE_REG               ((volatile tru_hps_i2c_ic_clr_rx_done_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_RX_DONE_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_activity :1;
		volatile uint32_t res1_31      :31;
    }bits;
}tru_hps_i2c_ic_clr_activity_reg_t;

#define TRU_HPS_I2C_IC_CLR_ACTIVITY_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_activity_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_ACTIVITY_REG               ((volatile tru_hps_i2c_ic_clr_activity_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_ACTIVITY_REG               ((volatile tru_hps_i2c_ic_clr_activity_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_ACTIVITY_REG               ((volatile tru_hps_i2c_ic_clr_activity_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_ACTIVITY_REG               ((volatile tru_hps_i2c_ic_clr_activity_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_ACTIVITY_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_stop_det :1;
		volatile uint32_t res1_31      :31;
    }bits;
}tru_hps_i2c_ic_clr_stop_det_reg_t;

#define TRU_HPS_I2C_IC_CLR_STOP_DET_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_stop_det_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_STOP_DET_REG               ((volatile tru_hps_i2c_ic_clr_stop_det_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_STOP_DET_REG               ((volatile tru_hps_i2c_ic_clr_stop_det_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_STOP_DET_REG               ((volatile tru_hps_i2c_ic_clr_stop_det_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_STOP_DET_REG               ((volatile tru_hps_i2c_ic_clr_stop_det_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_STOP_DET_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_start_det :1;
		volatile uint32_t res1_31      :31;
    }bits;
}tru_hps_i2c_ic_clr_start_det_reg_t;

#define TRU_HPS_I2C_IC_CLR_START_DET_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_start_det_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_START_DET_REG               ((volatile tru_hps_i2c_ic_clr_start_det_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_START_DET_REG               ((volatile tru_hps_i2c_ic_clr_start_det_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_START_DET_REG               ((volatile tru_hps_i2c_ic_clr_start_det_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_START_DET_REG               ((volatile tru_hps_i2c_ic_clr_start_det_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_START_DET_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t clr_gen_call :1;
		volatile uint32_t res1_31      :31;
    }bits;
}tru_hps_i2c_ic_clr_gen_call_reg_t;

#define TRU_HPS_I2C_IC_CLR_GEN_CALL_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_clr_gen_call_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET))
#define TRU_HPS_I2C0_IC_CLR_GEN_CALL_REG               ((volatile tru_hps_i2c_ic_clr_gen_call_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET))
#define TRU_HPS_I2C1_IC_CLR_GEN_CALL_REG               ((volatile tru_hps_i2c_ic_clr_gen_call_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET))
#define TRU_HPS_I2C2_IC_CLR_GEN_CALL_REG               ((volatile tru_hps_i2c_ic_clr_gen_call_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET))
#define TRU_HPS_I2C3_IC_CLR_GEN_CALL_REG               ((volatile tru_hps_i2c_ic_clr_gen_call_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_CLR_GEN_CALL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t enable  :1;
		volatile uint32_t txabort :1;
		volatile uint32_t res2_31 :30;
    }bits;
}tru_hps_i2c_ic_enable_reg_t;

#define TRU_HPS_I2C_IC_ENABLE_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_enable_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_ENABLE_OFFSET))
#define TRU_HPS_I2C0_IC_ENABLE_REG               ((volatile tru_hps_i2c_ic_enable_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET))
#define TRU_HPS_I2C1_IC_ENABLE_REG               ((volatile tru_hps_i2c_ic_enable_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET))
#define TRU_HPS_I2C2_IC_ENABLE_REG               ((volatile tru_hps_i2c_ic_enable_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET))
#define TRU_HPS_I2C3_IC_ENABLE_REG               ((volatile tru_hps_i2c_ic_enable_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_ENABLE_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t activity     :1;
		volatile uint32_t tfnf         :1;
		volatile uint32_t tfe          :1;
		volatile uint32_t rfne         :1;
		volatile uint32_t rff          :1;
		volatile uint32_t mst_activity :1;
		volatile uint32_t slv_activity :1;
		volatile uint32_t res7_31      :25;
    }bits;
}tru_hps_i2c_ic_status_reg_t;

#define TRU_HPS_I2C_IC_STATUS_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_status_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_STATUS_OFFSET))
#define TRU_HPS_I2C0_IC_STATUS_REG               ((volatile tru_hps_i2c_ic_status_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET))
#define TRU_HPS_I2C1_IC_STATUS_REG               ((volatile tru_hps_i2c_ic_status_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET))
#define TRU_HPS_I2C2_IC_STATUS_REG               ((volatile tru_hps_i2c_ic_status_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET))
#define TRU_HPS_I2C3_IC_STATUS_REG               ((volatile tru_hps_i2c_ic_status_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_STATUS_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t txflr   :7;
		volatile uint32_t res7_31 :25;
    }bits;
}tru_hps_i2c_ic_txflr_reg_t;

#define TRU_HPS_I2C_IC_TXFLR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_txflr_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_TXFLR_OFFSET))
#define TRU_HPS_I2C0_IC_TXFLR_REG               ((volatile tru_hps_i2c_ic_txflr_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET))
#define TRU_HPS_I2C1_IC_TXFLR_REG               ((volatile tru_hps_i2c_ic_txflr_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET))
#define TRU_HPS_I2C2_IC_TXFLR_REG               ((volatile tru_hps_i2c_ic_txflr_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET))
#define TRU_HPS_I2C3_IC_TXFLR_REG               ((volatile tru_hps_i2c_ic_txflr_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_TXFLR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t rxflr   :7;
		volatile uint32_t res7_31 :25;
    }bits;
}tru_hps_i2c_ic_rxflr_reg_t;

#define TRU_HPS_I2C_IC_RXFLR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_rxflr_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_RXFLR_OFFSET))
#define TRU_HPS_I2C0_IC_RXFLR_REG               ((volatile tru_hps_i2c_ic_rxflr_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET))
#define TRU_HPS_I2C1_IC_RXFLR_REG               ((volatile tru_hps_i2c_ic_rxflr_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET))
#define TRU_HPS_I2C2_IC_RXFLR_REG               ((volatile tru_hps_i2c_ic_rxflr_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET))
#define TRU_HPS_I2C3_IC_RXFLR_REG               ((volatile tru_hps_i2c_ic_rxflr_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_RXFLR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_sda_hold :16;
		volatile uint32_t res16_31    :16;
    }bits;
}tru_hps_i2c_ic_sda_hold_reg_t;

#define TRU_HPS_I2C_IC_SDA_HOLD_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_sda_hold_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET))
#define TRU_HPS_I2C0_IC_SDA_HOLD_REG               ((volatile tru_hps_i2c_ic_sda_hold_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET))
#define TRU_HPS_I2C1_IC_SDA_HOLD_REG               ((volatile tru_hps_i2c_ic_sda_hold_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET))
#define TRU_HPS_I2C2_IC_SDA_HOLD_REG               ((volatile tru_hps_i2c_ic_sda_hold_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET))
#define TRU_HPS_I2C3_IC_SDA_HOLD_REG               ((volatile tru_hps_i2c_ic_sda_hold_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SDA_HOLD_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t abrt_7b_addr_noack   :1;
		volatile uint32_t abrt_10addr1_noack   :1;
		volatile uint32_t abrt_10addr2_noack   :1;
		volatile uint32_t abrt_txdata_noack    :1;
		volatile uint32_t abrt_gcall_noack     :1;
		volatile uint32_t abrt_gcall_read      :1;
		volatile uint32_t abrt_hs_ackdet       :1;
		volatile uint32_t abrt_sbyte_ackdet    :1;
		volatile uint32_t abrt_hs_norstrt      :1;
		volatile uint32_t abrt_sbyte_norstrt   :1;
		volatile uint32_t abrt_10b_rd_norstrt  :1;
		volatile uint32_t abrt_master_dis      :1;
		volatile uint32_t arb_lost             :1;
		volatile uint32_t abrt_slvflush_txfifo :1;
		volatile uint32_t abrt_slv_arblost     :1;
		volatile uint32_t abrt_slvrd_intx      :1;
		volatile uint32_t abrt_user_abrt       :1;
		volatile uint32_t res17_22             :6;
		volatile uint32_t tx_flush_cnt         :9;
    }bits;
}tru_hps_i2c_ic_tx_abrt_source_reg_t;

#define TRU_HPS_I2C_IC_TX_ABRT_SOURCE_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_tx_abrt_source_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_TX_ABRT_SOURCE_OFFSET))
#define TRU_HPS_I2C0_IC_TX_ABRT_SOURCE_REG               ((volatile tru_hps_i2c_ic_tx_abrt_source_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_TX_ABRT_SOURCE_OFFSET))
#define TRU_HPS_I2C1_IC_TX_ABRT_SOURCE_REG               ((volatile tru_hps_i2c_ic_tx_abrt_source_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_TX_ABRT_SOURCE_OFFSET))
#define TRU_HPS_I2C2_IC_TX_ABRT_SOURCE_REG               ((volatile tru_hps_i2c_ic_tx_abrt_source_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_TX_ABRT_SOURCE_OFFSET))
#define TRU_HPS_I2C3_IC_TX_ABRT_SOURCE_REG               ((volatile tru_hps_i2c_ic_tx_abrt_source_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_TX_ABRT_SOURCE_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t nack    :1;
		volatile uint32_t res1_31 :31;
    }bits;
}tru_hps_i2c_ic_slv_data_nack_only_reg_t;

#define TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_slv_data_nack_only_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET))
#define TRU_HPS_I2C0_IC_SLV_DATA_NACK_ONLY_REG               ((volatile tru_hps_i2c_ic_slv_data_nack_only_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET))
#define TRU_HPS_I2C1_IC_SLV_DATA_NACK_ONLY_REG               ((volatile tru_hps_i2c_ic_slv_data_nack_only_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET))
#define TRU_HPS_I2C2_IC_SLV_DATA_NACK_ONLY_REG               ((volatile tru_hps_i2c_ic_slv_data_nack_only_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET))
#define TRU_HPS_I2C3_IC_SLV_DATA_NACK_ONLY_REG               ((volatile tru_hps_i2c_ic_slv_data_nack_only_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SLV_DATA_NACK_ONLY_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t rdmae   :1;
		volatile uint32_t tdmae   :1;
		volatile uint32_t res2_31 :30;
    }bits;
}tru_hps_i2c_ic_dma_cr_reg_t;

#define TRU_HPS_I2C_IC_DMA_CR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_dma_cr_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_DMA_CR_OFFSET))
#define TRU_HPS_I2C0_IC_DMA_CR_REG               ((volatile tru_hps_i2c_ic_dma_cr_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET))
#define TRU_HPS_I2C1_IC_DMA_CR_REG               ((volatile tru_hps_i2c_ic_dma_cr_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET))
#define TRU_HPS_I2C2_IC_DMA_CR_REG               ((volatile tru_hps_i2c_ic_dma_cr_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET))
#define TRU_HPS_I2C3_IC_DMA_CR_REG               ((volatile tru_hps_i2c_ic_dma_cr_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_DMA_CR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t dmatdl  :6;
		volatile uint32_t res6_31 :26;
    }bits;
}tru_hps_i2c_ic_dma_tdlr_reg_t;

#define TRU_HPS_I2C_IC_DMA_TDLR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_dma_tdlr_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_DMA_TDLR_OFFSET))
#define TRU_HPS_I2C0_IC_DMA_TDLR_REG               ((volatile tru_hps_i2c_ic_dma_tdlr_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_DMA_TDLR_OFFSET))
#define TRU_HPS_I2C1_IC_DMA_TDLR_REG               ((volatile tru_hps_i2c_ic_dma_tdlr_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_DMA_TDLR_OFFSET))
#define TRU_HPS_I2C2_IC_DMA_TDLR_REG               ((volatile tru_hps_i2c_ic_dma_tdlr_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_DMA_TDLR_OFFSET))
#define TRU_HPS_I2C3_IC_DMA_TDLR_REG               ((volatile tru_hps_i2c_ic_dma_tdlr_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_DMA_TDLR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t dmardl  :6;
		volatile uint32_t res6_31 :26;
    }bits;
}tru_hps_i2c_ic_dma_rdlr_reg_t;

#define TRU_HPS_I2C_IC_DMA_RDLR_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_dma_rdlr_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_DMA_RDLR_OFFSET))
#define TRU_HPS_I2C0_IC_DMA_RDLR_REG               ((volatile tru_hps_i2c_ic_dma_rdlr_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_DMA_RDLR_OFFSET))
#define TRU_HPS_I2C1_IC_DMA_RDLR_REG               ((volatile tru_hps_i2c_ic_dma_rdlr_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_DMA_RDLR_OFFSET))
#define TRU_HPS_I2C2_IC_DMA_RDLR_REG               ((volatile tru_hps_i2c_ic_dma_rdlr_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_DMA_RDLR_OFFSET))
#define TRU_HPS_I2C3_IC_DMA_RDLR_REG               ((volatile tru_hps_i2c_ic_dma_rdlr_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_DMA_RDLR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t sda_setup :8;
		volatile uint32_t res8_31   :24;
    }bits;
}tru_hps_i2c_ic_sda_setup_reg_t;

#define TRU_HPS_I2C_IC_SDA_SETUP_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_sda_setup_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET))
#define TRU_HPS_I2C0_IC_SDA_SETUP_REG               ((volatile tru_hps_i2c_ic_sda_setup_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET))
#define TRU_HPS_I2C1_IC_SDA_SETUP_REG               ((volatile tru_hps_i2c_ic_sda_setup_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET))
#define TRU_HPS_I2C2_IC_SDA_SETUP_REG               ((volatile tru_hps_i2c_ic_sda_setup_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET))
#define TRU_HPS_I2C3_IC_SDA_SETUP_REG               ((volatile tru_hps_i2c_ic_sda_setup_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_SDA_SETUP_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ack_gen_call :1;
		volatile uint32_t res1_31      :31;
    }bits;
}tru_hps_i2c_ic_ack_general_call_reg_t;

#define TRU_HPS_I2C_IC_ACK_GENERAL_CALL_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_ack_general_call_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET))
#define TRU_HPS_I2C0_IC_ACK_GENERAL_CALL_REG               ((volatile tru_hps_i2c_ic_ack_general_call_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET))
#define TRU_HPS_I2C1_IC_ACK_GENERAL_CALL_REG               ((volatile tru_hps_i2c_ic_ack_general_call_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET))
#define TRU_HPS_I2C2_IC_ACK_GENERAL_CALL_REG               ((volatile tru_hps_i2c_ic_ack_general_call_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET))
#define TRU_HPS_I2C3_IC_ACK_GENERAL_CALL_REG               ((volatile tru_hps_i2c_ic_ack_general_call_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_ACK_GENERAL_CALL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_en                   :1;
		volatile uint32_t slv_disabled_while_busy :1;
		volatile uint32_t slv_rx_data_lost        :1;
		volatile uint32_t res3_31                 :29;
    }bits;
}tru_hps_i2c_ic_enable_status_reg_t;

#define TRU_HPS_I2C_IC_ENABLE_STATUS_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_enable_status_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET))
#define TRU_HPS_I2C0_IC_ENABLE_STATUS_REG               ((volatile tru_hps_i2c_ic_enable_status_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET))
#define TRU_HPS_I2C1_IC_ENABLE_STATUS_REG               ((volatile tru_hps_i2c_ic_enable_status_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET))
#define TRU_HPS_I2C2_IC_ENABLE_STATUS_REG               ((volatile tru_hps_i2c_ic_enable_status_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET))
#define TRU_HPS_I2C3_IC_ENABLE_STATUS_REG               ((volatile tru_hps_i2c_ic_enable_status_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_ENABLE_STATUS_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t spklen  :8;
		volatile uint32_t res8_31 :24;
    }bits;
}tru_hps_i2c_ic_fs_spklen_reg_t;

#define TRU_HPS_I2C_IC_FS_SPKLEN_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_fs_spklen_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET))
#define TRU_HPS_I2C0_IC_FS_SPKLEN_REG               ((volatile tru_hps_i2c_ic_fs_spklen_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET))
#define TRU_HPS_I2C1_IC_FS_SPKLEN_REG               ((volatile tru_hps_i2c_ic_fs_spklen_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET))
#define TRU_HPS_I2C2_IC_FS_SPKLEN_REG               ((volatile tru_hps_i2c_ic_fs_spklen_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET))
#define TRU_HPS_I2C3_IC_FS_SPKLEN_REG               ((volatile tru_hps_i2c_ic_fs_spklen_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_FS_SPKLEN_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t apb_data_width     :2;
		volatile uint32_t max_speed_mode     :2;
		volatile uint32_t hc_count_values    :1;
		volatile uint32_t intr_io            :1;
		volatile uint32_t has_dma            :1;
		volatile uint32_t add_encoded_params :1;
		volatile uint32_t rx_buffer_depth    :8;
		volatile uint32_t tx_buffer_depth    :8;
		volatile uint32_t res24_31           :8;
    }bits;
}tru_hps_i2c_ic_comp_param_1_reg_t;

#define TRU_HPS_I2C_IC_COMP_PARAM_1_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_comp_param_1_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET))
#define TRU_HPS_I2C0_IC_COMP_PARAM_1_REG               ((volatile tru_hps_i2c_ic_comp_param_1_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET))
#define TRU_HPS_I2C1_IC_COMP_PARAM_1_REG               ((volatile tru_hps_i2c_ic_comp_param_1_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET))
#define TRU_HPS_I2C2_IC_COMP_PARAM_1_REG               ((volatile tru_hps_i2c_ic_comp_param_1_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET))
#define TRU_HPS_I2C3_IC_COMP_PARAM_1_REG               ((volatile tru_hps_i2c_ic_comp_param_1_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_COMP_PARAM_1_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_comp_version      :16;
		volatile uint32_t ic_comp_version_copy :16;
    }bits;
}tru_hps_i2c_ic_comp_version_reg_t;

#define TRU_HPS_I2C_IC_COMP_VERSION_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_comp_version_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET))
#define TRU_HPS_I2C0_IC_COMP_VERSION_REG               ((volatile tru_hps_i2c_ic_comp_version_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET))
#define TRU_HPS_I2C1_IC_COMP_VERSION_REG               ((volatile tru_hps_i2c_ic_comp_version_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET))
#define TRU_HPS_I2C2_IC_COMP_VERSION_REG               ((volatile tru_hps_i2c_ic_comp_version_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET))
#define TRU_HPS_I2C3_IC_COMP_VERSION_REG               ((volatile tru_hps_i2c_ic_comp_version_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_COMP_VERSION_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ic_comp_type      :16;
		volatile uint32_t ic_comp_type_copy :16;
    }bits;
}tru_hps_i2c_ic_comp_type_reg_t;

#define TRU_HPS_I2C_IC_COMP_TYPE_REG(usb_base_addr) ((volatile tru_hps_i2c_ic_comp_type_reg_t *const)((char *)usb_base_addr + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET))
#define TRU_HPS_I2C0_IC_COMP_TYPE_REG               ((volatile tru_hps_i2c_ic_comp_type_reg_t *const)((char *)TRU_HPS_I2C0_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET))
#define TRU_HPS_I2C1_IC_COMP_TYPE_REG               ((volatile tru_hps_i2c_ic_comp_type_reg_t *const)((char *)TRU_HPS_I2C1_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET))
#define TRU_HPS_I2C2_IC_COMP_TYPE_REG               ((volatile tru_hps_i2c_ic_comp_type_reg_t *const)((char *)TRU_HPS_I2C2_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET))
#define TRU_HPS_I2C3_IC_COMP_TYPE_REG               ((volatile tru_hps_i2c_ic_comp_type_reg_t *const)((char *)TRU_HPS_I2C3_BASE + TRU_HPS_I2C_IC_COMP_TYPE_OFFSET))

#endif

#ifdef __cplusplus
}
#endif

#endif
