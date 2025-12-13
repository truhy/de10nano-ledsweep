/*
	MIT License

	Copyright (c) 2025 Truong Hy

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

	Version: 20251206
*/

#include "tru_bsp_c5soc_custom.h"

#if(TRU_BOARD == TRU_BOARD_C5SOC_CUSTOM)

#ifdef SEMIHOSTING
	extern void initialise_monitor_handles(void);  // Reference function header from the external Semihosting library
#else
	#if (defined(TRU_PRINT_UART0) && TRU_PRINT_UART0 == 1U) || (defined(TRU_PRINT_UART1) && TRU_PRINT_UART1 == 1U)
		#include "tru_c5soc_hps_uart_ll.h"

		int __io_putchar(int ch){
			#if TRU_PRINT_UART0 == 1U
				tru_hps_uart_ll_write_char((void *)TRU_HPS_UART0_BASE, ch);  // Re-target to UART controller 0
			#elif TRU_PRINT_UART1 == 1U
				tru_hps_uart_ll_write_char((void *)TRU_HPS_UART1_BASE, ch);  // Re-target to UART controller 1
			#endif
			return ch;
		}
	#endif
#endif

void tru_bsp_init(void){
	#ifdef SEMIHOSTING
		initialise_monitor_handles();  // Initialise Semihosting
	#endif
}

#if defined(TRU_EXIT_TO_UBOOT) && TRU_EXIT_TO_UBOOT == 1U
	// ===============================================
	// Support code for Exit to U-Boot
	// Global variables for returning back into U-Boot
	// ===============================================

	// Initialised global variables for storing startup values to support U-Boot
	// Note, we want them inside the .data section so that our values set in the
	// reset handler will not be zeroed out.  Normally, they would be placed into
	// the .bss section as uninitialised variables, which would be zeroed filled.
	int uboot_argc                 __attribute__((__section__(".data")));
	char **uboot_argv              __attribute__((__section__(".data")));
	long unsigned int uboot_lr     __attribute__((__section__(".data")));
	long unsigned int uboot_cpsr   __attribute__((__section__(".data")));
	long unsigned int uboot_sys_sp __attribute__((__section__(".data")));  // This is also for the user mode, because they use the same stack pointer
	long unsigned int uboot_und_sp __attribute__((__section__(".data")));
	long unsigned int uboot_abt_sp __attribute__((__section__(".data")));
	long unsigned int uboot_svc_sp __attribute__((__section__(".data")));
	long unsigned int uboot_irq_sp __attribute__((__section__(".data")));
	long unsigned int uboot_fiq_sp __attribute__((__section__(".data")));
	long unsigned int uboot_vbar   __attribute__((__section__(".data")));

	// Exit to U-Boot
	void __attribute__((naked)) etu(int rc){
		__asm__(
			// Restore VBAR (vector base address)
			"LDR r3, =uboot_vbar                           \n"
			"MCR p15, 0, r3, c12, c0, 0                    \n"

			// Restore stack pointer for each mode
			"CPS #0x11                                     \n"
			"LDR r3, =uboot_fiq_sp                         \n"
			"LDR sp, [r3]                                  \n"
			"CPS #0x12                                     \n"
			"LDR r3, =uboot_irq_sp                         \n"
			"LDR sp, [r3]                                  \n"
			"CPS #0x13                                     \n"
			"LDR r3, =uboot_svc_sp                         \n"
			"LDR sp, [r3]                                  \n"
			"CPS #0x17                                     \n"
			"LDR r3, =uboot_abt_sp                         \n"
			"LDR sp, [r3]                                  \n"
			"CPS #0x1B                                     \n"
			"LDR r3, =uboot_und_sp                         \n"
			"LDR sp, [r3]                                  \n"
			"CPS #0x1F                                     \n"
			"LDR r3, =uboot_sys_sp                         \n"
			"LDR sp, [r3]                                  \n"

			// Restore processor mode
			// Note: U-Boot is normally in supervisor mode so alternatively we could hard-code it with CPS #0x13
			"LDR r3, =uboot_cpsr                           \n"
			"LDR r2, [r3]                                  \n"
			"BIC r2, r2, #0xffffffe0                       \n"  // Keep mode bits (first 5 bits)
			"MRS r3, cpsr                                  \n"
			"BIC r3, r3, #0x1f                             \n"  // Clear mode bits
			"ORR r3, r3, r2                                \n"
			"MSR cpsr, r3                                  \n"  // Restore processor mode

			// Restore return address
			"LDR r3, =uboot_lr                             \n"
			"LDR lr, [r3]                                  \n"

			// Return to U-Boot
			"BX lr                                         \n"
		);
	}

	// Override newlib _exit()
	void __attribute__((noreturn)) _exit(int status){
		etu(status);
		while(1);
	}
#endif

#endif
