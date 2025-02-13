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

	Private timer IRQ handler.

	Timer preload value formula
	===========================

	In the Cortex-A9 Tech Ref Manual, under Calculating timer intervals, a
	formula is provided:
		interval = ((prescaler + 1) * (preload + 1)) / peripheral_clk
		where interval (i.e. duration) is in seconds

	If we use a prescaler of 0, the formula is simplified to:
		interval = (preload + 1) / peripheral_clk

	Rearranging it for preload, gives:
		preload = peripheral_clk * interval - 1

	Interval can be written as 1/interval_freq, so the formula can be written
	as:
		preload = peripheral_clk * (1 / interval_freq) - 1
	Simplified, gives:
		preload = peripheral_clk / interval_freq - 1

	Example, if we want an interval of 1ms then we use interval = 1/1000, and
	plugging this into the formula, we get:
		preload = peripheral_clk * (1 / 1000) - 1
	Simplified, gives:
		preload = peripheral_clk / 1000 - 1
*/

#include "priv_timer_irqh.h"

// Trulib includes
#include "tru_irq.h"
#include "tru_cortex_a9.h"

// HWLIB includes
#include "alt_clock_manager.h"

// Systick interval = 1/SYSTICK_INTERVAL_FREQ
// In this case, systick interval = 1ms = 1/1000, therefore:
static const uint32_t SYSTICK_INTERVAL_FREQ = 1000U;

// Systick counter
static volatile uint32_t systicks;

// Private timer interrupt - gets triggered when timer reaches 0
static void systick_irqhandler(void){
	systicks++;
	__sev();  // Create signal event
}

void priv_timer_init(void){
	alt_freq_t periph_freq;

	// Register and enable the specified IRQ
	IRQ_SetHandler(SecurePhyTimer_IRQn, systick_irqhandler);  // Register user interrupt handler
	IRQ_SetPriority(SecurePhyTimer_IRQn, GIC_IRQ_PRIORITY_LEVEL30_7);  // Set lowest usable priority
	IRQ_SetMode(SecurePhyTimer_IRQn, IRQ_MODE_TYPE_IRQ | IRQ_MODE_CPU_0 | IRQ_MODE_TRIG_LEVEL | IRQ_MODE_TRIG_LEVEL_HIGH);
	//IRQ_SetMode(SecurePhyTimer_IRQn, IRQ_MODE_TYPE_IRQ | IRQ_MODE_CPU_0 | IRQ_MODE_TRIG_EDGE | IRQ_MODE_TRIG_EDGE_RISING);  // F2H IRQ and GIC edge doesn't work on Cyclone V SoC, it behaves as trigger level
	IRQ_Enable(SecurePhyTimer_IRQn);  // Enable the interrupt

	// Note
	// The clock source of the private timer is set to the peripheral base clock
	// It is 1/4 of the processor clock.  On the DE10-Nano processor clock is normally 800MHz, in this case the peripheral base clock is 800/4 = 200MHz
	alt_clk_freq_get(ALT_CLK_MPU_PERIPH, &periph_freq);  // Get peripheral base clock

	// Setup the private timer preload (see formula above) for the specified tick rate (interval frequency)
	// We use frequency instead of interval (seconds) to avoid fractions in the calculation
	PTIM_SetLoadValue(periph_freq / SYSTICK_INTERVAL_FREQ - 1U);
	PTIM_SetControl(PTIM_GetControl() | 7U);  // Start the timer with interrupt enable and auto reload (auto restarts)
}

void priv_timer_deinit(void){
	tru_irq_unregister(SecurePhyTimer_IRQn);
}

void priv_timer_delay_ms(uint32_t wait_ms){
	uint32_t target_ticks = systicks + wait_ms;

	while (systicks < target_ticks){
	  __wfe();  // Power-down until next event (wait for event)
	}
}
