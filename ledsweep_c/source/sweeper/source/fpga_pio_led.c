#include "fpga_pio_led.h"

// Trulib includes
#include "tru_util_ll.h"

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
