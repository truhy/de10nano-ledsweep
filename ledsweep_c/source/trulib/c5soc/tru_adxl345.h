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

#ifndef TRU_ADXL345_H
#define TRU_ADXL345_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include <stdint.h>

// Table 12. I2C Timing values from the ADXL345 datasheet
#define TRU_ADXL345_I2C_SPEED_KHZ        400000
#define TRU_ADXL345_I2C_SCL_HIGH_TIME_NS 600
#define TRU_ADXL345_I2C_SCL_LOW_TIME_NS  1300
#define TRU_ADXL345_I2C_RISE_TIME_NS     300
#define TRU_ADXL345_I2C_FALL_TIME_NS     300
#define TRU_ADXL345_I2C_DEV_ADDR         0x53

#define TRU_ADXL345_FIFO_DEPTH   32

#define TRU_ADXL345_RATE_0P10_HZ 0
#define TRU_ADXL345_RATE_0P20_HZ 1
#define TRU_ADXL345_RATE_0P39_HZ 2
#define TRU_ADXL345_RATE_0P78_HZ 3
#define TRU_ADXL345_RATE_1P56_HZ 4
#define TRU_ADXL345_RATE_3P13_HZ 5
#define TRU_ADXL345_RATE_6P25_HZ 6
#define TRU_ADXL345_RATE_12P5_HZ 7
#define TRU_ADXL345_RATE_25_HZ   8
#define TRU_ADXL345_RATE_50_HZ   9
#define TRU_ADXL345_RATE_100_HZ  10
#define TRU_ADXL345_RATE_200_HZ  11
#define TRU_ADXL345_RATE_400_HZ  12
#define TRU_ADXL345_RATE_800_HZ  13
#define TRU_ADXL345_RATE_1600_HZ 14
#define TRU_ADXL345_RATE_3200_HZ 15

#define TRU_ADXL345_WAKEUP_8_HZ 0
#define TRU_ADXL345_WAKEUP_4_HZ 1
#define TRU_ADXL345_WAKEUP_2_HZ 2
#define TRU_ADXL345_WAKEUP_1_HZ 3

#define TRU_ADXL345_RANGE_2G  0
#define TRU_ADXL345_RANGE_4G  1
#define TRU_ADXL345_RANGE_8G  2
#define TRU_ADXL345_RANGE_16G 3

#define TRU_ADXL345_FIFOMODE_BYPASS  0
#define TRU_ADXL345_FIFOMODE_FIFO    1
#define TRU_ADXL345_FIFOMODE_STREAM  2
#define TRU_ADXL345_FIFOMODE_TRIGGER 3

#define TRU_ADXL345_DEVID_ADDR          0x00
#define TRU_ADXL345_THRESH_TAP_ADDR     0x1d
#define TRU_ADXL345_OFSX_ADDR           0x1e
#define TRU_ADXL345_OFSY_ADDR           0x1f
#define TRU_ADXL345_OFSZ_ADDR           0x20
#define TRU_ADXL345_DUR_ADDR            0x21
#define TRU_ADXL345_LATENT_ADDR         0x22
#define TRU_ADXL345_WINDOW_ADDR         0x23
#define TRU_ADXL345_THRESH_ACT_ADDR     0x24
#define TRU_ADXL345_THRESH_INACT_ADDR   0x25
#define TRU_ADXL345_TIME_INACT_ADDR     0x26
#define TRU_ADXL345_ACT_INACT_CTL_ADDR  0x27
#define TRU_ADXL345_THRESH_FF_ADDR      0x28
#define TRU_ADXL345_TIME_FF_ADDR        0x29
#define TRU_ADXL345_TAP_AXES_ADDR       0x2a
#define TRU_ADXL345_ACT_TAP_STATUS_ADDR 0x2b
#define TRU_ADXL345_BW_RATE_ADDR        0x2c
#define TRU_ADXL345_POWER_CTL_ADDR      0x2d
#define TRU_ADXL345_INT_ENABLE_ADDR     0x2e
#define TRU_ADXL345_INT_MAP_ADDR        0x2f
#define TRU_ADXL345_INT_SOURCE_ADDR     0x30
#define TRU_ADXL345_DATA_FORMAT_ADDR    0x31
#define TRU_ADXL345_DATAX0_ADDR         0x32
#define TRU_ADXL345_DATAX1_ADDR         0x33
#define TRU_ADXL345_DATAY0_ADDR         0x34
#define TRU_ADXL345_DATAY1_ADDR         0x35
#define TRU_ADXL345_DATAZ0_ADDR         0x36
#define TRU_ADXL345_DATAZ1_ADDR         0x37
#define TRU_ADXL345_FIFO_CTL_ADDR       0x38
#define TRU_ADXL345_FIFO_STATUS_ADDR    0x39

typedef union{
	uint8_t val;
	struct{
		uint8_t inact_z_en :1;
		uint8_t inact_y_en :1;
		uint8_t inact_x_en :1;
		uint8_t inact_acdc :1;
		uint8_t act_z_en   :1;
		uint8_t act_y_en   :1;
		uint8_t act_x_en   :1;
		uint8_t act_acdc   :1;
	}bits;
}tru_adxl345_act_inact_ctl_t;

#define TRU_ADXL345_ACT_INACT_PTR(ptr) ((tru_adxl345_act_inact_ctl_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t tap_z_en :1;
		uint8_t tap_y_en :1;
		uint8_t tap_x_en :1;
		uint8_t suppress :1;
		uint8_t zero4_7  :4;
	}bits;
}tru_adxl345_tap_axes_t;

#define TRU_ADXL345_TAP_AXES_PTR(ptr) ((tru_adxl345_tap_axes_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t tap_z_src :1;
		uint8_t tap_y_src :1;
		uint8_t tap_x_src :1;
		uint8_t asleep    :1;
		uint8_t act_z_src :1;
		uint8_t act_y_src :1;
		uint8_t act_x_src :1;
		uint8_t zero7     :1;
	}bits;
}tru_adxl345_act_tap_status_t;

#define TRU_ADXL345_ACT_TAP_AXES_PTR(ptr) ((tru_adxl345_act_tap_status_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t rate      :4;
		uint8_t low_power :1;
		uint8_t zero5_7   :3;
	}bits;
}tru_adxl345_bw_rate_t;

#define TRU_ADXL345_BW_RATE_PTR(ptr) ((tru_adxl345_bw_rate_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t wakeup    :2;
		uint8_t sleep     :1;
		uint8_t measure   :1;
		uint8_t autosleep :1;
		uint8_t link      :1;
		uint8_t zero6_7   :2;
	}bits;
}tru_adxl345_power_ctl_t;

#define TRU_ADXL345_POWER_CTL_PTR(ptr) ((tru_adxl345_power_ctl_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t overrun    :1;
		uint8_t watermark  :1;
		uint8_t freefall   :1;
		uint8_t inactivity :1;
		uint8_t activity   :1;
		uint8_t doubletap  :1;
		uint8_t singletap  :1;
		uint8_t dataready  :1;
	}bits;
}tru_adxl345_int_enable_t;

#define TRU_ADXL345_INT_ENABLE_PTR(ptr) ((tru_adxl345_int_enable_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t overrun    :1;
		uint8_t watermark  :1;
		uint8_t freefall   :1;
		uint8_t inactivity :1;
		uint8_t activity   :1;
		uint8_t doubletap  :1;
		uint8_t singletap  :1;
		uint8_t dataready  :1;
	}bits;
}tru_adxl345_int_map_t;

#define TRU_ADXL345_INT_MAP_PTR(ptr) ((tru_adxl345_int_map_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t overrun    :1;
		uint8_t watermark  :1;
		uint8_t freefall   :1;
		uint8_t inactivity :1;
		uint8_t activity   :1;
		uint8_t doubletap  :1;
		uint8_t singletap  :1;
		uint8_t dataready  :1;
	}bits;
}tru_adxl345_int_source_t;

#define TRU_ADXL345_INT_SOURCE_PTR(ptr) ((tru_adxl345_int_source_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t range     :2;
		uint8_t justify   :1;
		uint8_t fullres   :1;
		uint8_t zero4     :1;
		uint8_t intinvert :1;
		uint8_t spi       :1;
		uint8_t selftest  :1;
	}bits;
}tru_adxl345_data_format_t;

#define TRU_ADXL345_DATA_FORMAT_PTR(ptr) ((tru_adxl345_data_format_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t samples  :5;
		uint8_t trigger  :1;
		uint8_t fifomode :2;
	}bits;
}tru_adxl345_fifo_ctl_t;

#define TRU_ADXL345_FIFO_CTL_PTR(ptr) ((tru_adxl345_fifo_ctl_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t entries  :6;
		uint8_t zero6    :1;
		uint8_t fifotrig :1;
	}bits;
}tru_adxl345_fifo_status_t;

#define TRU_ADXL345_FIFO_STATUS_PTR(ptr) ((tru_adxl345_fifo_status_t *)ptr)

void tru_adxl345_i2c_init(uint32_t l4_sp_clk_freq_hz, uint32_t i2c_dev_speed_khz, uint8_t mode10bit, uint16_t dev_addr);
void tru_adxl345_i2c_read_bm(void *buf, uint32_t len, uint32_t reg_addr_start);
void tru_adxl345_i2c_read(void *buf, uint32_t len, uint32_t reg_addr_start);
void tru_adxl345_i2c_write(void *buf, uint32_t len, uint32_t reg_addr_start);
void tru_adxl345_i2c_stop_flush_fifo(void);

#endif

#ifdef __cplusplus
}
#endif

#endif
