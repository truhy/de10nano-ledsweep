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

	Version: 20251207

	GIC (PrimeCell Generic Interrupt Controller (PL390)) interrupt functions.
*/

#include "tru_irq.h"

#if(TRU_CPU_FAMILY == TRU_CPU_FAMILY_CORTEXA9)

// ===================
// Use CMSIS functions
// ===================

#include "irq_ctrl.h"

void tru_irq_init(void){
	irq_mask(0);  // Enable IRQ mode interrupts for this CPU
}

void tru_irq_deinit(void){
	irq_mask(1);  // Disable IRQ mode interrupts for this CPU
}

void tru_irq_register(IRQn_ID_t intr_id, uint32_t intr_target, uint32_t intr_priority, IRQHandler_t handler){
	IRQ_SetHandler(intr_id, handler);  // Register user interrupt handler
	IRQ_SetPriority(intr_id, intr_priority);
	IRQ_SetMode(intr_id, IRQ_MODE_TYPE_IRQ | intr_target << IRQ_MODE_CPU_Pos | IRQ_MODE_TRIG_LEVEL | IRQ_MODE_TRIG_LEVEL_HIGH);
	IRQ_Enable(intr_id);  // Enable the interrupt
}

void tru_irq_unregister(IRQn_ID_t intr_id){
	IRQ_Disable(intr_id);                      // Disable user interrupt handler
	IRQ_SetHandler(intr_id, (IRQHandler_t)0);  // Unregister user interrupt handler
}

#endif
