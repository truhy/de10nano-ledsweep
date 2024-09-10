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

	Version: 20240316

	FPGA IRQ handler.
*/

#include "fpga_irqh.h"
#include "RTE_Components.h"   // CMSIS
#include CMSIS_device_header  // CMSIS
#include "tru_util_ll.h"
#include "tru_logger.h"

static pio_ledsw_t *fpga_pio = NULL;

// User IRQ handler for interrupt triggered from FPGA IRQ0
static void fpga_72_irqhandler(void){
	uint32_t fpga_inputs = tru_iom_rd32((TRU_TARGET_TYPE *)PIO0_DATA);  // Read FPGA input keys from memory mapped L2H bridge

	// Check which key was pressed
	switch(fpga_inputs){
		case PIO0_INPUT_F2H_KEY0:
			if(fpga_pio->delay_ms >= PIO0_ANIM_DELAY_STEP_ADJ) fpga_pio->delay_ms -= PIO0_ANIM_DELAY_STEP_ADJ;
			DEBUG_PRINTF("Key 0 pressed! Delay: %lu\n", fpga_pio->delay_ms);
			break;

		case PIO0_INPUT_F2H_KEY1:
			fpga_pio->delay_ms += PIO0_ANIM_DELAY_STEP_ADJ;
			DEBUG_PRINTF("Key 1 pressed! Delay: %lu\n", fpga_pio->delay_ms);
			break;

		case PIO0_INPUT_F2H_KEYX:
			// Toggle anim on off
			if(fpga_pio->anim_en == PIO0_OUTPUT_LED_ANIM_ON){
				fpga_pio->anim_en = PIO0_OUTPUT_LED_ANIM_OFF;
				DEBUG_PRINTF("Keys 0 & 1 pressed! Anim off\n");
			}else{
				fpga_pio->anim_en = PIO0_OUTPUT_LED_ANIM_ON;
				DEBUG_PRINTF("Keys 0 & 1 pressed! Anim on\n");
			}
			break;

		default: DEBUG_PRINTF("Unknown!\n");
	}

	tru_iom_wr32((TRU_TARGET_TYPE *)PIO0_IRQ_CLR, PIO0_INPUT_F2H_KEYX);  // Clear (re-arm) interrupt triggered flag for selected pins
}

void fpga_init(pio_ledsw_t *pio){
	fpga_pio = pio;

	// Register and enable the specified IRQ
	IRQ_SetHandler(C5SOC_F2H0_IRQn, fpga_72_irqhandler);  // Register user interrupt handler
	IRQ_SetPriority(C5SOC_F2H0_IRQn, GIC_IRQ_PRIORITY_LEVEL30_7);  // Set lowest usable priority
	IRQ_SetMode(C5SOC_F2H0_IRQn, IRQ_MODE_TYPE_IRQ | IRQ_MODE_CPU_0 | IRQ_MODE_TRIG_LEVEL | IRQ_MODE_TRIG_LEVEL_HIGH);
	//IRQ_SetMode(C5SOC_F2H0_IRQn, IRQ_MODE_TYPE_IRQ | IRQ_MODE_CPU_0 | IRQ_MODE_TRIG_EDGE | IRQ_MODE_TRIG_EDGE_RISING);  // F2H IRQ and GIC edge doesn't work on Cyclone V SoC, it behaves as trigger level
	IRQ_Enable(C5SOC_F2H0_IRQn);  // Enable the interrupt

	// Initialise memory mapped registers of Quartus Prime PIOs (Parallel Port IO IP)
	tru_iom_wr32((TRU_TARGET_TYPE *)PIO0_DIR, 0);  // Set data direction to input
	tru_iom_wr32((TRU_TARGET_TYPE *)PIO0_IRQ_MSK, PIO0_INPUT_F2H_KEYX);  // Unmask (enable triggerable) interrupt for selected pins
}

void fpga_deinit(void){
	IRQ_Disable(C5SOC_F2H0_IRQn);
	IRQ_SetHandler(C5SOC_F2H0_IRQn, NULL);
	fpga_pio = NULL;
}

// Animate LEDs
void update_pio0_led_anim(pio_ledsw_t *pio){
	if(pio->anim_en == PIO0_OUTPUT_LED_ANIM_ON){
		tru_iom_wr32((TRU_TARGET_TYPE *)PIO0_OUT_CLR, pio->leds);  // Turn off the current LED

		// Do we change flow direction?
		if(pio->leds == PIO0_OUTPUT_LED_0_ON && pio->flow == PIO0_OUTPUT_LED_FLOW_R){
			pio->flow = PIO0_OUTPUT_LED_FLOW_L;
		}else if(pio->leds == PIO0_OUTPUT_LED_7_ON && pio->flow == PIO0_OUTPUT_LED_FLOW_L){
			pio->flow = PIO0_OUTPUT_LED_FLOW_R;
		}

		pio->leds = (pio->flow == PIO0_OUTPUT_LED_FLOW_L) ? pio->leds << 1U : pio->leds >> 1U;  // Advance to the next LED

		tru_iom_wr32((TRU_TARGET_TYPE *)PIO0_OUT_SET, pio->leds);  // Turn on the next LED
	}
}
