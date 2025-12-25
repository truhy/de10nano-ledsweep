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

	Version: 20251223

	Arm Cortex-A9 low level assembly & MPCore registers.
*/

#ifndef TRU_CORTEX_A9_H
#define TRU_CORTEX_A9_H

#include "tru_config.h"

#if(TRU_CPU_FAMILY == TRU_CPU_FAMILY_CORTEXA9)

#include <stdint.h>

#define TRU_GLOBAL_TIMER_BASE  (TRU_PERIPH_BASE + 0x200U)
#define TRU_PRIVATE_TIMER_BASE (TRU_PERIPH_BASE + 0x600U)

//===========================
// GCC inline assembly macros
//===========================

// Synchronise related
#define __wfe() __asm__ volatile("wfe":::"memory")
#define __sev() __asm__ volatile("sev")
#define __dmb() __asm__ volatile("dmb 0xF":::"memory");
#define __dsb() __asm__ volatile("dsb 0xF":::"memory");
#define __isb() __asm__ volatile("isb 0xF":::"memory");

// Cache related
#define __write_dcisw(index)  __asm__ volatile("MCR p15, 0, %0, c7, c6, 2" : : "r"(index) : "memory")
#define __write_dccsw(index)  __asm__ volatile("MCR p15, 0, %0, c7, c10, 2" : : "r"(index) : "memory")
#define __write_csselr(level) __asm__ volatile("MCR p15, 2, %0, c0, c0, 0" : : "r"(level) : "memory")
#define __write_dccmvac(va)   __asm__ volatile("MCR p15, 0, %0, c7, c10, 1" : : "r"(va) : "memory")
#define __write_dcimvac(va)   __asm__ volatile("MCR p15, 0, %0, c7, c6, 1" : : "r"(va) : "memory")
#define __write_dccimvac(va)  __asm__ volatile("MCR p15, 0, %0, c7, c14, 1" : : "r"(va) : "memory")
#define __read_sctlr(result)  __asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(result) : : "memory")
#define __read_ccsidr(result) __asm__ volatile("MRC p15, 1, %0, c0, c0, 0" : "=r"(result) : : "memory")
#define __read_clidr(result)  __asm__ volatile("MRC p15, 1, %0, c0, c0, 1" : "=r"(result) : : "memory")
#define __read_mpidr(mpidr)   __asm__ volatile("MRC p15, 0, %0, c0, c0, 5" : "=r"(mpidr) : : "memory")

// MMU related
#define __write_tlbimvaa(va)  __asm__ volatile("MRC p15, 0, %0, c8, c7, 3" : : "r"(va) : "memory")

// ============
// Global timer
// ============

// The clock source of the global and private timer is the peripheral base clock
// The peripheral base clock is 1/4 of the processor clock
// On the DE10-Nano processor, U-Boot (Quartus Prime handoff files) normally sets the clock up for 800MHz so the peripheral base clock is 800/4 = 200MHz

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t enable     :1;
		volatile uint32_t compenable :1;
		volatile uint32_t irqenable  :1;
		volatile uint32_t autoinc    :1;
		volatile uint32_t res1       :4;
		volatile uint32_t prescaler  :8;
		volatile uint32_t res2       :16;
	}bits;
}gtim_control_reg_t;

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t eventflag :1;
		volatile uint32_t res1      :31;
	}bits;
}gtim_intrstatus_reg_t;

typedef struct{
  volatile uint32_t counterl;
  volatile uint32_t counterh;
  gtim_control_reg_t control;
  gtim_intrstatus_reg_t intrstatus;
  volatile uint32_t comparel;
  volatile uint32_t compareh;
  volatile uint32_t autoinc;
}gtim_reg_t;

#define GTIM_REG ((volatile gtim_reg_t *const)TRU_GLOBAL_TIMER_BASE)

#define GTIM_CONTROL_TIMER_ENABLE_POS 0U
#define GTIM_CONTROL_TIMER_ENABLE_MSK 0x1U

#define GTIM_CONTROL_COMPARE_ENABLE_POS 1U
#define GTIM_CONTROL_COMPARE_ENABLE_MSK (0x1U << GTIM_CONTROL_COMPARE_ENABLE_POS)

#define GTIM_CONTROL_IRQ_ENABLE_POS 2U
#define GTIM_CONTROL_IRQ_ENABLE_MSK (0x1U << GTIM_CONTROL_IRQ_ENABLE_POS)

#define GTIM_CONTROL_AUTOINC_POS 3U
#define GTIM_CONTROL_AUTOINC_MSK (0x1U << GTIM_CONTROL_AUTOINC_POS)

#define GTIM_CONTROL_PRESCALER_POS 8U
#define GTIM_CONTROL_PRESCALER_MSK (0xffU << GTIM_CONTROL_PRESCALER_POS)

#define GTIM_ISR_EVENTFLAG_POS 1U
#define GTIM_ISR_EVENTFLAG_MSK 0x1U

// Basic plain running timer mode: timer stopped, no compare, no interrupt, no auto-reset counter, no prescaler
static inline void gtim_setup_basic_mode(void){
	GTIM_REG->control.val &= ~(GTIM_CONTROL_TIMER_ENABLE_MSK | GTIM_CONTROL_COMPARE_ENABLE_MSK | GTIM_CONTROL_IRQ_ENABLE_MSK | GTIM_CONTROL_AUTOINC_MSK | GTIM_CONTROL_PRESCALER_MSK);
}

static inline void gtim_start(void){
	GTIM_REG->control.val |= (GTIM_CONTROL_TIMER_ENABLE_MSK | GTIM_CONTROL_COMPARE_ENABLE_MSK);
}

static inline void gtim_zero_counter(void){
	GTIM_REG->counterl = 0U;
	GTIM_REG->counterh = 0U;
}

// Start the timer (counting starts)
static inline void gtim_enable(void){
	GTIM_REG->control.bits.enable = 1;
}

// Stop the timer (counting stopped)
static inline void gtim_disable(void){
	GTIM_REG->control.bits.enable = 0;
}

static inline uint64_t gtim_get_counter(void){
	volatile uint32_t upper = GTIM_REG->counterh;
	volatile uint32_t lower = GTIM_REG->counterl;

	while(GTIM_REG->counterh != upper){
		lower = GTIM_REG->counterl;
		upper = GTIM_REG->counterh;
	}

	return (uint64_t)upper << 32U | lower;
}

static inline void gtim_set_counter(uint64_t counter){
	GTIM_REG->counterl = (uint32_t)counter;
	GTIM_REG->counterh = (uint32_t)(counter >> 32U);
}

// Clear interrupt event
static inline void gtim_clear_event(void){
	GTIM_REG->intrstatus.bits.eventflag = 1;
}

// ========================
// Private timer & watchdog
// ========================

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t enable     :1;
		volatile uint32_t autoreload :1;
		volatile uint32_t irqenable  :1;
		volatile uint32_t res1       :5;
		volatile uint32_t prescaler  :8;
		volatile uint32_t res2       :16;
	}bits;
}ptim_control_reg_t;

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t eventflag :1;
		volatile uint32_t res1      :31;
	}bits;
}ptim_intrstatus_reg_t;

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t enable     :1;
		volatile uint32_t autoreload :1;
		volatile uint32_t irqenable  :1;
		volatile uint32_t mode       :1;
		volatile uint32_t res1       :4;
		volatile uint32_t prescaler  :8;
		volatile uint32_t res2       :16;
	}bits;
}ptim_wdcontrol_reg_t;

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t eventflag :1;
		volatile uint32_t res1      :31;
	}bits;
}ptim_wdintrstatus_reg_t;

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t resetflag :1;
		volatile uint32_t res1      :31;
	}bits;
}ptim_wresetstatus_reg_t;

typedef struct{
	 volatile uint32_t load;
	 volatile uint32_t counter;
	 ptim_control_reg_t control;
	 ptim_intrstatus_reg_t intrstatus;
	 volatile uint32_t wdload;
	 volatile uint32_t wdcounter;
	 ptim_wdcontrol_reg_t wdcontrol;
	 ptim_wdintrstatus_reg_t wdintrstatus;
	 ptim_wresetstatus_reg_t wdresetstatus;
	 volatile uint32_t wddisable;
}ptim_reg_t;

#define PTIM_REG ((volatile ptim_reg_t *const)TRU_PRIVATE_TIMER_BASE)

#define PTIM_CONTROL_TIMER_ENABLE_POS 0U
#define PTIM_CONTROL_TIMER_ENABLE_MSK 0x1U

#define PTIM_CONTROL_AUTORELOAD_POS 1U
#define PTIM_CONTROL_AUTORELOAD_MSK (0x1U << PTIM_CONTROL_AUTORELOAD_POS)

#define PTIM_CONTROL_IRQ_ENABLE_POS 2U
#define PTIM_CONTROL_IRQ_ENABLE_MSK (0x1U << PTIM_CONTROL_IRQ_ENABLE_POS)

#define PTIM_CONTROL_PRESCALER_POS 8U
#define PTIM_CONTROL_PRESCALER_MSK (0xffU << PTIM_CONTROL_PRESCALER_POS)

#define PTIM_ISR_EVENTFLAG_POS 1U
#define PTIM_ISR_EVENTFLAG_MSK 0x1U

// Basic plain running timer mode: timer stopped, no autoreload, no interrupt, no prescaler
static inline void ptim_setup_basic_mode(void){
	PTIM_REG->control.val &= ~(PTIM_CONTROL_TIMER_ENABLE_MSK | PTIM_CONTROL_AUTORELOAD_MSK | GTIM_CONTROL_IRQ_ENABLE_MSK | PTIM_CONTROL_PRESCALER_MSK);
}

// Start the timer (counting starts)
static inline void ptim_enable(void){
	PTIM_REG->control.bits.enable = 1;
}

// Stop the timer (counting stopped)
static inline void ptim_disable(void){
	PTIM_REG->control.bits.enable = 0;
}

// Clear interrupt event
static inline void ptim_clear_event(void){
	PTIM_REG->intrstatus.bits.eventflag = 1;
}

#endif

#endif
