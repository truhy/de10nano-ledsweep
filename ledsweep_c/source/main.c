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

		Version: 20240316
		Target : ARM Cortex-A9 on the DE10-Nano development board (Intel Cyclone V
		         SoC FPGA)
		Type   : Bare-metal C

		A bare-metal C program that demonstrates the processor system (HPS) working
		with FPGA logic.

		This is the software part of the ledsweep, programmed to work with the
		FPGA logic design.  It animates the 8x FPGA LEDs (0 to 7) by sweeping
		them from side to side.  Pressing the FPGA tactile keys (0 and 1) will
		increase and decrease the animation speed.
		
		The FPGA logic design sets up the L2F bridge so that the HPS can control the
		LEDs by using the L2F memory map registers, and also sets up the FPGA keys
		so they will trigger a GIC interrupt and is forwarded to the HPS side.

		Learning objectives:
			- To capture FPGA input pins (tactile keys) and send to FPGA-to-HPS
			  interrupt receiver
			- To control FPGA output pins (LEDs) from the processor using the
			  Light-Weight HPS to FPGA (L2F) bridge
			- To use PIO IP with the L2F bridge
			- To setup GIC interrupt handler
			- To use the Arm private timer interrupt for creating delays
			- To use edge mode interrupt capture with the PIO IP (simulates edge
			  triggered interrupt)

		This software is designed to work with the FPGA logic design (Verilog) so
		the correct run order is as follows:
			1. configure the FPGA first using the bitstream file ledsweep.sof or
			   ledsweep.rbf
			2. and then execute this program
*/

#include "RTE_Components.h"   // CMSIS
#include CMSIS_device_header  // CMSIS
#include "priv_timer_irqh.h"
#include "fpga_irqh.h"
#include "tru_logger.h"

#if(TRU_EXIT_TO_UBOOT == 1U)
	#include <stdlib.h>
#endif

#ifdef SEMIHOSTING
	extern void initialise_monitor_handles(void);  // Reference function header from the external Semihosting library
#endif

// PIO LED variable
pio_ledsw_t pio0 = {
	.leds     = PIO0_OUTPUT_LED_0_ON,
	.flow     = PIO0_OUTPUT_LED_FLOW_L,
	.anim_en  = PIO0_OUTPUT_LED_ANIM_ON,
	.delay_ms = 100
};

void init(void){
	DEBUG_PRINTF("Initialising\n");

	irq_mask(0);        // Enable IRQ mode interrupts for this CPU
	priv_timer_init();  // Initialise private timer IRQ
	fpga_init(&pio0);   // Init FPGA to HPS IRQ
}

void deint(void){
	DEBUG_PRINTF("De-initialising\n");

	fpga_deinit();
	priv_timer_deinit();
	irq_mask(1);  // Disable IRQ mode interrupts for this CPU
}

// Do LED tasks forever
void led_tasks(){
	DEBUG_PRINTF("Executing tasks\n");

	while(1){
		update_pio0_led_anim(&pio0);
		priv_timer_delay_ms(pio0.delay_ms);
	}
}

// Do LED tasks only n times
void led_tasks_n(uint32_t n){
	DEBUG_PRINTF("Executing tasks\n");

	while(n){
		update_pio0_led_anim(&pio0);
		priv_timer_delay_ms(pio0.delay_ms);
		n--;
	}
}

int main(int argc, char **argv){
	#ifdef SEMIHOSTING
		initialise_monitor_handles();  // Initialise Semihosting
	#endif

	init();

#if(TRU_EXIT_TO_UBOOT == 1U)
	uint32_t num_updates = 10 * 7;  // Default number of LED updates

	if(uboot_argc >= 2){
		num_updates = strtoul(uboot_argv[1], NULL, 0);
		if(uboot_argc >= 3) pio0.delay_ms = strtoul(uboot_argv[2], NULL, 0);
	}

	led_tasks_n(num_updates);
#else
	led_tasks();
#endif

	deint();

	DEBUG_PRINTF("Exiting main\n");
	return 0;
}
