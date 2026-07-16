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

	Version: 20260707
*/

#include "fpga_led_pio.h"

// Trulib includes
#include "tru_iom.h"

// Animate LEDs
void update_pio0_led_anim(pio_ledsw_t *pio){
	if(pio->anim_en == PIO0_OUTPUT_LED_ANIM_ON){
		iom_wr32(PIO0_OUT_CLR, pio->leds);  // Turn off the current LED

		// Do we change flow direction?
		if(pio->leds == PIO0_OUTPUT_LED_0_ON && pio->flow == PIO0_OUTPUT_LED_FLOW_R){
			pio->flow = PIO0_OUTPUT_LED_FLOW_L;
		}else if(pio->leds == PIO0_OUTPUT_LED_7_ON && pio->flow == PIO0_OUTPUT_LED_FLOW_L){
			pio->flow = PIO0_OUTPUT_LED_FLOW_R;
		}

		pio->leds = (pio->flow == PIO0_OUTPUT_LED_FLOW_L) ? pio->leds << 1U : pio->leds >> 1U;  // Advance to the next LED

		iom_wr32(PIO0_OUT_SET, pio->leds);  // Turn on the next LED
	}
}
