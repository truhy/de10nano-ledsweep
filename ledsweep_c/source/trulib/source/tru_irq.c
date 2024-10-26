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

	GIC (PrimeCell Generic Interrupt Controller (PL390)) interrupt functions.
*/

#include "tru_irq.h"

#if(TRU_CMSIS == 1U)

// ===================
// Use CMSIS functions
// ===================

#include "irq_ctrl.h"

void tru_irq_init(void){
#if(TRU_STARTUP == 1U && ALT_INT_PROVISION_VECTOR_SUPPORT == 0U)
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

#else

// ==========================
// Use Altera HWLib functions
// ==========================

ALT_STATUS_CODE tru_irq_init(void){
	ALT_STATUS_CODE status;

	// Initialise global interrupt system
	status = alt_int_global_init();
	if(status != ALT_E_SUCCESS){
		//printf("Error: alt_int_global_init() failed, status: %li\n", status);
		return status;
	}

	// Initialise processor interrupt system
	status = alt_int_cpu_init();
	if(status != ALT_E_SUCCESS){
		//printf("Error: alt_int_cpu_init() failed, status: %li\n", status);
		return status;
	}

	// Enable processor interrupt
	status = alt_int_cpu_enable();
	if (status != ALT_E_SUCCESS){
		//printf("ERROR: alt_int_cpu_enable() failed, status: %li\n", status);
		return status;
	}

	// Enable global interrupt
	status = alt_int_global_enable();
	if (status != ALT_E_SUCCESS){
		//printf("ERROR: alt_int_global_enable() failed, status: %li\n", status);
		return status;
	}

	return ALT_E_SUCCESS;
}

ALT_STATUS_CODE tru_irq_deinit(void){
	ALT_STATUS_CODE status_outcome = ALT_E_SUCCESS;
	ALT_STATUS_CODE status;

	status = alt_int_global_disable();
	if(status != ALT_E_SUCCESS){
		//printf("ERROR: alt_int_global_disable() failed, status: %li\n", status);
		if(status_outcome == ALT_E_SUCCESS) status_outcome = status;
	}

	status = alt_int_cpu_disable();
	if(status != ALT_E_SUCCESS){
		//printf("ERROR: alt_int_cpu_disable() failed, status: %li\n", status);
		if(status_outcome == ALT_E_SUCCESS) status_outcome = status;
	}

	status = alt_int_cpu_uninit();
	if(status != ALT_E_SUCCESS){
		//printf("ERROR: alt_int_cpu_uninit() failed, status: %li\n", status);
		if(status_outcome == ALT_E_SUCCESS) status_outcome = status;
	}

	status = alt_int_global_uninit();
	if(status != ALT_E_SUCCESS){
		//printf("ERROR: alt_int_global_uninit() failed, status: %li\n", status);
		if(status_outcome == ALT_E_SUCCESS) status_outcome = status;
	}

	return status_outcome;
}

// Register and enable specified IRQ handler
void tru_irq_register(ALT_INT_INTERRUPT_t intr_id, uint32_t intr_target, uint32_t intr_priority, alt_int_callback_t callback, void *context){
	alt_int_isr_register(intr_id, callback, context);   // Register user interrupt handler
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
