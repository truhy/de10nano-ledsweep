/*
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
*/

#include "irq_c5soc.h"
#include "c5soc.h"
#include <stddef.h>

// Define CMSIS IRQ handler table (see irq_ctrl_gic.h)
IRQHandler_t IRQTable[IRQ_GIC_LINE_COUNT] = { 0U };

// Overrride CMSIS default weak prototype (see irq_ctrl_gic.h)
int32_t IRQ_Initialize(void){
	uint32_t i;

	for (i = 0U; i < IRQ_GIC_LINE_COUNT; i++) {
		IRQTable[i] = (IRQHandler_t)NULL;
	}
	GIC_Enable();

	return (0U);
}

#if defined(TRU_CMSIS_WEAK_IRQH) && !TRU_CMSIS_WEAK_IRQH
// Disable: warning: FP registers might be clobbered despite 'interrupt' attribute: compile with '-mgeneral-regs-only' [-Wattributes]
// The warning is about some ARM CPUs, e.g. Cortex A series do not automatically save floating point registers on interrupt.
// Code is added to save and restore the VFP registers, which covers this warning so is safe to silence it.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

// Overrride CMSIS default weak prototype (see irq_ctrl_gic.h)
void __attribute__((interrupt("IRQ"))) IRQ_Handler(void){
	// Save floating point registers (VFP registers)
#if defined(TRU_NEON) && TRU_NEON == 1U && __FPU_PRESENT == 1U && __FPU_USED == 1U
	__ASM volatile(
		"SUB    sp, sp, #4      \n"  // Correct SP early by pushing a dummy, which will make it even for the r0 push below (number of pushes must be even)
		"VMRS   r0, fpscr       \n"  // Read FP status value
		"STMFD  sp!, {r0}       \n"  // Push FP status value onto the stack
		"VSTMDB sp!, {d0-d15}   \n"  // Push d0-d15 VFP registers
		"VSTMDB sp!, {d16-d31}  \n"  // Push d16-d31 VFP registers
	);
#endif

	IRQn_ID_t irq_id = IRQ_GetActiveIRQ();  // Get ID of the triggered interrupt

	if((irq_id >= 0U) && (irq_id < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)){
		IRQTable[irq_id]();  // Call the user registered IRQ handler
	}

	int32_t status = IRQ_EndOfInterrupt(irq_id);  // Set interrupt is serviced

	// Restore floating point registers (VFP registers)
#if defined(TRU_NEON) && TRU_NEON == 1U && __FPU_PRESENT == 1U && __FPU_USED == 1U
	__ASM volatile(
		"VLDMIA sp!, {d16-d31}  \n"  // Pop into d16-d31 registers
		"VLDMIA sp!, {d0-d15}   \n"  // Pop into d0-d15 registers
		"LDMFD  sp!, {r0}       \n"  // Pop into temporary r0 register
		"VMSR   fpscr, r0       \n"  // Copy r0 to fpscr register
		"ADD    sp, sp, #4      \n"  // Pop the dummy we pushed earlier
	);
#endif
}

#pragma GCC diagnostic pop
#endif

// Overrride CMSIS default weak prototype (see irq_ctrl_gic.h)
IRQHandler_t IRQ_GetHandler(IRQn_ID_t irqn) {
	IRQHandler_t h;

	// Ignore CPUID field (software generated interrupts)
	irqn &= 0x3FFU;

	if((irqn >= 0U) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT)){
		h = IRQTable[irqn];
	}else{
		h = (IRQHandler_t)0U;
	}

	return (h);
}

/*
    Sets the priority level for an IRQ.
    Arm's priority order, e.g. for binary point 2:
        0  = highest
        30 = lowest (lower priority = a higher value)
        31 = reserved.  This will not trigger, it is reserved as the default
             priority mask
*/
void irq_set_group_priority(IRQn_ID_t irqn, uint8_t grp_priority, uint8_t sub_priority){
	// Calculate and set lowest priority..
	uint32_t num_grp_bits = IRQ_GetPriorityGroupBits();  // Get the number bits assigned to group priority (controlled by the 3 bit binary point value in GICC_BPR register).  See https://developer.arm.com/documentation/ihi0048/b/Interrupt-Handling-and-Prioritization/Interrupt-prioritization/Priority-grouping?lang=en
	uint8_t num_sub_bits = 8U - num_grp_bits;
	uint8_t gp_bits;
	uint8_t sp_bits;
	uint8_t priority;

	// Invalid binary point?
	if(num_grp_bits != IRQ_PRIORITY_ERROR){
		gp_bits = ((1U << num_grp_bits) - 1U) | grp_priority;  // Calculate group priority bits offsetted to LSB bit position apply priority value
		sp_bits = ((1U << num_sub_bits) - 1U) | sub_priority;  // Calculate sub priority bits
		priority = gp_bits << num_sub_bits | sp_bits;
	}else{
		priority = 0xf7U;  // Error, assume binary point 2 and setting to lowest priority (30, 7)
	}

	// Apply
	IRQ_SetPriority(irqn, priority);
}

void irq_mask(uint8_t mask){
	// Enable IRQ enable mode
	CPSR_Type cpsr;
	cpsr.w = __get_CPSR();
	cpsr.b.I = mask;  // 1 = disable IRQ, 0 = enable IRQ
	__set_CPSR(cpsr.w);
}
