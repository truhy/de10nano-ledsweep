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

	Version: 20240127

	Arm Cortex-A9 low level assembly codes.
*/

#ifndef TRU_CORTEX_A9_H
#define TRU_CORTEX_A9_H

#include <stdint.h>

#define TRU_PERIPH_BASE       0xfffec000UL
#define TRU_GLOBAL_TIMER_BASE (TRU_PERIPH_BASE + 0x0200U)

// GCC inline assembly macros
#define __wfe() __asm__ volatile("wfe":::"memory")
#define __sev() __asm__ volatile("sev")
#define __dmb() __asm__ volatile("dmb 0xF":::"memory");
#define __dsb() __asm__ volatile("dsb 0xF":::"memory");
#define __write_dcisw(index)  __asm__ volatile("MCR p15, 0, %0, c7, c6, 2" : : "r" (index) : "memory")
#define __write_dccsw(index)  __asm__ volatile("MCR p15, 0, %0, c7, c10, 2" : : "r" (index) : "memory")
#define __write_csselr(level) __asm__ volatile("MCR p15, 2, %0, c0, c0, 0" : : "r" (level) : "memory")
#define __write_dccmvac(va)   __asm__ volatile("MCR p15, 0, %0, c7, c10, 1" : : "r" (va) : "memory")
#define __write_dcimvac(va)   __asm__ volatile("MCR p15, 0, %0, c7, c6, 1" : : "r" (va) : "memory")
#define __write_dccimvac(va)  __asm__ volatile("MCR p15, 0, %0, c7, c14, 1" : : "r" (va) : "memory")
#define __read_sctlr(result)  __asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r" (result) : : "memory")
#define __read_ccsidr(result) __asm__ volatile("MRC p15, 1, %0, c0, c0, 0" : "=r" (result) : : "memory")
#define __read_clidr(result)  __asm__ volatile("MRC p15, 1, %0, c0, c0, 1" : "=r" (result) : : "memory")
#define __read_mpidr(mpidr)   __asm__ volatile("MRC p15, 0, %0, c0, c0, 5" : "=r" (mpidr) : : "memory")

typedef struct{
  volatile uint32_t counterl;
  volatile uint32_t counterh;
  volatile uint32_t control;
  volatile uint32_t isr;
  volatile uint32_t compl;
  volatile uint32_t comph;
  volatile uint32_t autoinc;
}global_timer_type;
#define GTIM ((global_timer_type *)TRU_GLOBAL_TIMER_BASE)

#define GTIM_CONTROL_ENABLE_POS 0U
#define GTIM_CONTROL_ENABLE_MSK 0x1U

#define GTIM_CONTROL_COMP_ENABLE_POS 1U
#define GTIM_CONTROL_COMP_ENABLE_MSK (0x1U << GTIM_CONTROL_COMP_ENABLE_POS)

#define GTIM_CONTROL_IRQ_ENABLE_POS 2U
#define GTIM_CONTROL_IRQ_ENABLE_MSK (0x1U << GTIM_CONTROL_IRQ_ENABLE_POS)

#define GTIM_CONTROL_AUTOINC_POS 3U
#define GTIM_CONTROL_AUTOINC_MSK (0x1U << GTIM_CONTROL_AUTOINC_POS)

#define GTIM_CONTROL_PRESCALER_POS 8U
#define GTIM_CONTROL_PRESCALER_MSK (0xffU << GTIM_CONTROL_PRESCALER_POS)

#define GTIM_ISR_EVENTFLAG_POS 1U
#define GTIM_ISR_EVENTFLAG_MSK 0x1U

static inline void gtim_enable(void){
	GTIM->control |= GTIM_CONTROL_ENABLE_MSK;
}

static inline void gtim_disable(void){
	GTIM->control &= ~(uint32_t)GTIM_CONTROL_ENABLE_MSK;
}

#endif
