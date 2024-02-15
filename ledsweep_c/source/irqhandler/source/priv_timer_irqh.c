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

    Version: 20230701

    Private timer IRQ handler.
*/

#include "priv_timer_irqh.h"
#include "RTE_Components.h"   // CMSIS
#include CMSIS_device_header  // CMSIS
#include "alt_clock_manager.h"

static const uint32_t TICK_RATE_HZ = 1000U;
static volatile uint32_t sys_ticks;

static void systick_irqhandler(void){
	sys_ticks++;
	__SEV();  // Signal event
}

void priv_timer_init(void){
	alt_freq_t periph_freq;

	// Install and enable specified IRQ..
	IRQ_SetHandler(SecurePhyTimer_IRQn, systick_irqhandler);  // Register user interrupt handler
	IRQ_SetPriority(SecurePhyTimer_IRQn, GIC_IRQ_PRIORITY_LEVEL30_7);  // Set lowest usable priority
	IRQ_SetMode(SecurePhyTimer_IRQn, IRQ_MODE_TYPE_IRQ | IRQ_MODE_CPU_0 | IRQ_MODE_TRIG_LEVEL | IRQ_MODE_TRIG_LEVEL_HIGH);
	IRQ_Enable(SecurePhyTimer_IRQn);  // Enable the interrupt

	// Note: The clock source of the private timer is set to the peripheral base clock
	alt_clk_freq_get(ALT_CLK_MPU_PERIPH, &periph_freq);  // Get peripheral base clock.

	// Setup private timer for the specified tick rate
	PTIM_SetLoadValue((periph_freq / TICK_RATE_HZ) - 1U);
	PTIM_SetControl(PTIM_GetControl() | 7U);
}

void priv_timer_deinit(void){
	IRQ_Disable(SecurePhyTimer_IRQn);
	IRQ_SetHandler(SecurePhyTimer_IRQn, NULL);
}

void priv_timer_delay_ms(uint32_t ms){
  uint32_t target_ticks = sys_ticks + ms;

  while (sys_ticks < target_ticks){
    __WFE();  // Power-down until next event
  }
}
