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

	Version: 20241021

	FPGA PIO LEDs.
*/

#ifndef PIO_LED_H
#define PIO_LED_H

#include "fpga_pio_intel.h"
#include "tru_c5soc_hps_ll.h"
#include <stdint.h>

// PIO0 must match with Platform Designer in the hardware design
// PIO0 registers from PIO instance base (Parallel I/O IP) to L2F bridge (light-weight HPS to FPGA bridge)
// Note: The L2F is an AXI bridge between HPS as master to FPGA as slave, which is memory mapped to a block of memory addresses
#define PIO0_BASE     0x00000000UL
#define PIO0_L2F_BASE (TRU_HPS_L2F_BASE + PIO0_BASE)
#define PIO0_DATA     (PIO0_L2F_BASE + PIO_DATA_OFFSET)
#define PIO0_DIR      (PIO0_L2F_BASE + PIO_DIR_OFFSET)
#define PIO0_IRQ_MSK  (PIO0_L2F_BASE + PIO_IRQ_MSK_OFFSET)
#define PIO0_IRQ_CLR  (PIO0_L2F_BASE + PIO_IRQ_CLR_OFFSET)
#define PIO0_OUT_SET  (PIO0_L2F_BASE + PIO_OUT_SET_OFFSET)
#define PIO0_OUT_CLR  (PIO0_L2F_BASE + PIO_OUT_CLR_OFFSET)

// PIO0 input from FPGA bit mapping for IRQ trigger
#define PIO0_INPUT_F2H_KEY0 (1UL << 0UL)
#define PIO0_INPUT_F2H_KEY1 (1UL << 1UL)
#define PIO0_INPUT_F2H_KEYX (PIO0_INPUT_F2H_KEY0 | PIO0_INPUT_F2H_KEY1)

// PIO0 output from HPS bit mapping for LEDs
#define PIO0_OUTPUT_LED_0_ON     (1UL << 0UL)
#define PIO0_OUTPUT_LED_1_ON     (1UL << 1UL)
#define PIO0_OUTPUT_LED_2_ON     (1UL << 2UL)
#define PIO0_OUTPUT_LED_3_ON     (1UL << 3UL)
#define PIO0_OUTPUT_LED_4_ON     (1UL << 4UL)
#define PIO0_OUTPUT_LED_5_ON     (1UL << 5UL)
#define PIO0_OUTPUT_LED_6_ON     (1UL << 6UL)
#define PIO0_OUTPUT_LED_7_ON     (1UL << 7UL)
#define PIO0_OUTPUT_LED_FLOW_L   0U
#define PIO0_OUTPUT_LED_FLOW_R   1U
#define PIO0_OUTPUT_LED_ANIM_ON  0U
#define PIO0_OUTPUT_LED_ANIM_OFF 1U

#define PIO0_ANIM_DELAY_STEP_ADJ 10U

// Represents FPGA PIO LEDs
typedef struct{
	uint32_t leds;
	uint8_t flow;
	volatile uint8_t anim_en;
	uint32_t delay_ms;
}pio_ledsw_t;

void update_pio0_led_anim(pio_ledsw_t *pio);

#endif
