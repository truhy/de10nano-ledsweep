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

	Version: 20250311

	GIC (PrimeCell Generic Interrupt Controller (PL390)) interrupt functions.
*/

#include "tru_irq.h"

#if defined(TRU_CMSIS) && TRU_CMSIS == 1U
#if(TRU_CPU_FAMILY == TRU_CPU_FAMILY_CORTEXA9)

// ===================
// Use CMSIS functions
// ===================

#include "irq_ctrl.h"

void tru_irq_init(void){
#if defined(TRU_STARTUP) && TRU_STARTUP == 1U && defined(ALT_INT_PROVISION_VECTOR_SUPPORT) && ALT_INT_PROVISION_VECTOR_SUPPORT == 0U
	IRQ_Initialize();  // For CMSIS
#endif
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

#elif(TRU_TARGET == TRU_TARGET_C5SOC)

// ==========================
// Use Altera HWLib functions
// ==========================

void tru_irq_init(void){
	alt_int_global_init();    // Initialise global interrupt system
	alt_int_cpu_init();       // Initialise processor interrupt system
	alt_int_cpu_enable();     // Enable processor interrupt
	alt_int_global_enable();  // Enable global interrupt
}

void tru_irq_deinit(void){
	alt_int_global_disable();
	alt_int_cpu_disable();
	alt_int_cpu_uninit();
	alt_int_global_uninit();
}

// Register and enable specified IRQ handler
void tru_irq_register(ALT_INT_INTERRUPT_t intr_id, uint32_t intr_target, uint32_t intr_priority, alt_int_callback_t handler){
	alt_int_isr_register(intr_id, handler, NULL);   // Register user interrupt handler
	alt_int_dist_target_set(intr_id, intr_target);      // Enable forwarding of the interrupt ID to the specified processor target
	alt_int_dist_priority_set(intr_id, intr_priority);  // Set priority
	alt_int_dist_enable(intr_id);                       // Enable the interrupt
}

// Unregister and disable specified IRQ handler
void tru_irq_unregister(ALT_INT_INTERRUPT_t intr_id){
	alt_int_dist_disable(intr_id);    // Disable the interrupt
	alt_int_isr_unregister(intr_id);  // Unregister user interrupt handler
}

#endif
