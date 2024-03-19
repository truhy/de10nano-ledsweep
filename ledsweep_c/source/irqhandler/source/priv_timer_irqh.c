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

	Version: 20240319

	Private timer IRQ handler.

	Timer preload value calculation
	===============================

	In the Cortex-A9 Tech Ref Manual, under Calculating timer intervals, an
	equation is provided:
		interval = ((prescaler + 1) * (preload + 1)) / peripheral_clk
		where interval (i.e. duration) is in seconds

	If we use a prescaler of 0, the formula is simplified to:
		interval = (preload + 1) / peripheral_clk

	Rearranging it for preload, gives:
		preload = peripheral_clk * interval - 1

	Interval can be written as 1/interval_freq, so the equation can be written
	as:
		preload = peripheral_clk * (1 / interval_freq) - 1
		Simplified, gives:
		preload = peripheral_clk / interval_freq - 1

	If we want an interval of 1ms then we use interval = 1/1000, and plugging
	this into the equation, we get:
		preload = peripheral_clk * (1 / 1000) - 1
		Simplified, gives:
		preload = peripheral_clk / 1000 - 1
*/

#include "priv_timer_irqh.h"
#include "tru_irq.h"
#include "tru_cortex_a9.h"
#include "alt_clock_manager.h"

// Systick interval = 1/SYSTICK_INTERVAL_FREQ
// In this case, interval = 1/1000 = 1ms
static const uint32_t SYSTICK_INTERVAL_FREQ = 1000U;

// Systick counter
static volatile uint32_t systicks;

// Private timer interrupt - gets triggered when timer reaches 0
static void systick_irqhandler(void){
	systicks++;
	__sev();  // Signal event
}

void priv_timer_init(void){
	alt_freq_t periph_freq;

	// Register and enable the specified IRQ
	tru_irq_register(
		TRU_IRQ_PPI_TIMER_PRIVATE,   // IRQ ID
		TRU_GIC_DIST_CPU0,           // CPU0
		TRU_GIC_PRIORITY_LEVEL30_7,  // Set lowest usable priority
		systick_irqhandler           // Register user interrupt handler
	);

	// Note: The clock source of the private timer is set to the peripheral base clock
	alt_clk_freq_get(ALT_CLK_MPU_PERIPH, &periph_freq);  // Get peripheral base clock - normally 200MHz on Cortex-A9

	// Setup private timer preload for the specified tick rate (interval frequency)
	// We use frequency instead of interval (seconds) to avoid fractions in the calculation
	PTIM_SetLoadValue((periph_freq / SYSTICK_INTERVAL_FREQ) - 1U);
	PTIM_SetControl(PTIM_GetControl() | 7U);  // Start the timer with interrupt enable and auto reload (auto restarts)
}

void priv_timer_deinit(void){
	tru_irq_unregister(TRU_IRQ_PPI_TIMER_PRIVATE);
}

void priv_timer_delay_ms(uint32_t wait_ms){
  uint32_t target_ticks = systicks + wait_ms;

  while (systicks < target_ticks){
	  __wfe();  // Power-down until next event
  }
}
