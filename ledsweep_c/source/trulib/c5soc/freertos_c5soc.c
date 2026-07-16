/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
	Cyclone V SoC (Arm Cortex-A9) supporting code for the FreeRTOS function
	hooks (callbacks) and handlers.

	Some of this code is from main() of the FreeRTOS CORTEX_A9_Cyclone_V_SoC_DK
	Demo.

	21 Oct 2024 - Truong Hy:
	Added my own IRQ handlers which jumps to the FreeRTOS ones.  This is to
	support my more complete startup code, instead of Altera's HWLIB startup.
	Modified code to use CMSIS functions where possible.
*/

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#if defined(TRU_CFG_FREERTOS) && TRU_CFG_FREERTOS == 1

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Trulib includes
#include "c5soc/tru_clkmgr_c5soc.h"

// Arm CMSIS includes
#include "RTE_Components.h"   // CMSIS
#include CMSIS_device_header  // CMSIS

// Overrides the weak vector table exception handler and jump to the FreeRTOS
// FreeRTOS_SWI_Handler function found in portASM.S (Cortex-A9 port).  Note,
// their function has some input arguments, which doesn't match with this
// function prototype.  I don't have the time to look at them in the source,
// anyway to preserve any input arguments we will jump to it without changing
// anything, hence the use of naked attribute and assembly
void __attribute__ ((naked)) SVC_Handler(void){
	__asm__ volatile("LDR pc, =FreeRTOS_SWI_Handler");
}

// Overrides the weak vector table exception handler and jump to the FreeRTOS
// FreeRTOS_IRQ_Handler function found in portASM.S (Cortex-A9 port).  Note,
// their function has some input arguments, which doesn't match with this
// function prototype.  I don't have the time to look at them in the source,
// anyway to preserve any input arguments we will jump to it without changing
// anything, hence the use of naked attribute and assembly
void __attribute__ ((naked)) IRQ_Handler(void){
	__asm__ volatile("LDR pc, =FreeRTOS_IRQ_Handler");
}

void vApplicationMallocFailedHook(void){
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	for(;;);
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName){
	( void )pcTaskName;
	( void )pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for(;;);
}

void vApplicationIdleHook(void){

}

void vAssertCalled(const char *pcFile, unsigned long ulLine){
	(void)pcFile;
	(void)ulLine;
	volatile unsigned long ul = 0;

	taskENTER_CRITICAL();
	{
		/* Set ul to a non-zero value using the debugger to step out of this
		function. */
		while(ul == 0){
			portNOP();
		}
	}
	taskEXIT_CRITICAL();
}

void vApplicationTickHook(void){

}

/*
	Timer preload value formula for the PTIM_SetLoadValue() function
	================================================================

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
void vConfigureTickInterrupt(void){
	tru_hps_clk_t timerclk;
	void FreeRTOS_Tick_Handler(void);

	// Interrupts are disabled when this function is called

	// Note
	// The clock source of the private timer is set to the MPU peripheral clock
	// It is 1/4 of the processor clock.  On the DE10-Nano processor clock is normally 800MHz, in this case the MPU peripheral clock is 800/4 = 200MHz
	timerclk = get_mpu_peri_clk(TRU_HPS_INPUT_CLK_HZ);  // Get MPU peripheral clock

	// Register and enable the specified IRQ
	IRQ_SetHandler(SecurePhyTimer_IRQn, FreeRTOS_Tick_Handler);  // Register user interrupt handler
	IRQ_SetPriority(SecurePhyTimer_IRQn, GIC_IRQ_PRIORITY_LEVEL30_7);  // Set lowest usable priority
	IRQ_SetMode(SecurePhyTimer_IRQn, IRQ_MODE_TYPE_IRQ | IRQ_MODE_CPU_0 | IRQ_MODE_TRIG_LEVEL | IRQ_MODE_TRIG_LEVEL_HIGH);
	IRQ_Enable(SecurePhyTimer_IRQn);  // Enable the interrupt

	// Setup the private timer preload (see formula above) for the specified tick rate (interval frequency)
	// We use frequency instead of interval (seconds) to avoid fractions in the calculation
	PTIM_SetLoadValue(timerclk.fout / configTICK_RATE_HZ - 1U);
	PTIM_SetControl(PTIM_GetControl() | 7U);  // Start the timer with interrupt enable and auto reload (auto restarts)
}

//void vApplicationIRQHandler(uint32_t ulICCIAR){
void vApplicationFPUSafeIRQHandler(uint32_t ulICCIAR){  // If using GCC and FreeRTOS V9.0.0 or later
	__asm ("CPSIE i");  // Re-enable interrupts

	uint32_t ulInterruptID = ulICCIAR & 0x3FFUL;  // Extract ID of the triggered interrupt

	if((ulInterruptID >= 0U) && (ulInterruptID < IRQ_GIC_LINE_COUNT)){
		IRQHandler_t irq_handler = IRQ_GetHandler(ulInterruptID);
		irq_handler();  // Call the user registered IRQ handler
	}
}

// This runs just before the scheduler starts
void vApplicationDaemonTaskStartupHook(void){

}

#endif

#endif
