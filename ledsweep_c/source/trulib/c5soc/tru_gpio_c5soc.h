/*
	MIT License

	Copyright (c) 2024 Truong Hy

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

	Version: 20260208

	Low-level code for Cyclone V SoC HPS GPIO controller module.

	GPIO pins will belong to one of three GIPOx register groups.
	To determine which group to use see this table:

	----------------------------------------
	HPS GPIO pin number | Use register group
	----------------------------------------
	0 to 28             | GPIO0
	29 to 57            | GPIO1
	58 to 66            | GPIO2
*/

#ifndef TRU_GPIO_C5SOC_H
#define TRU_GPIO_C5SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include "c5soc/tru_rstmgr_c5soc.h"
#include <stdint.h>

// ==================
// Hardware registers
// ==================

// Hardware HPS GPIO module registers
#define TRU_HPS_GPIO0_BASE 0xff708000
#define TRU_HPS_GPIO1_BASE 0xff709000
#define TRU_HPS_GPIO2_BASE 0xff70a000

// First pin number of each GPIO group
#define TRU_HPS_GPIO0_FIRST_PINNUM 0
#define TRU_HPS_GPIO1_FIRST_PINNUM 29
#define TRU_HPS_GPIO2_FIRST_PINNUM 58

// For converting pin number to pin number register bit position shift
#define TRU_HPS_GPIO0_PINUM_TO_BITPOS(pinnum) (pinnum - TRU_HPS_GPIO0_FIRST_PINNUM)
#define TRU_HPS_GPIO1_PINUM_TO_BITPOS(pinnum) (pinnum - TRU_HPS_GPIO1_FIRST_PINNUM)
#define TRU_HPS_GPIO2_PINUM_TO_BITPOS(pinnum) (pinnum - TRU_HPS_GPIO2_FIRST_PINNUM)

typedef enum tru_hps_gpio_pinstate_e{
	TRU_HPS_GPIO_PIN_LOW,
	TRU_HPS_GPIO_PIN_HIGH
}tru_hps_gpio_pinstate_t;

typedef struct{
	volatile uint32_t port_wr;
	volatile uint32_t dir;
	volatile uint32_t reserved[10];
	volatile uint32_t inten;
	volatile uint32_t intmask;
	volatile uint32_t inttype_level;
	volatile uint32_t intpolarity;
	volatile uint32_t intstatus;
	volatile uint32_t intstatus_raw;
	volatile uint32_t debounce;
	volatile uint32_t intclear;
	volatile uint32_t port_rd;
	volatile uint32_t reserved2[3];
	volatile uint32_t ls_sync;
	volatile uint32_t id;
	volatile uint32_t reserved3;
	volatile uint32_t id_ver;
	volatile uint32_t config_reg2;
	volatile uint32_t config_reg1;
}tru_hps_gpio_reg_t;

// GPIO registers as type representation
#define TRU_HPS_GPIO0_REG ((volatile tru_hps_gpio_reg_t *const)TRU_HPS_GPIO0_BASE)
#define TRU_HPS_GPIO1_REG ((volatile tru_hps_gpio_reg_t *const)TRU_HPS_GPIO1_BASE)
#define TRU_HPS_GPIO2_REG ((volatile tru_hps_gpio_reg_t *const)TRU_HPS_GPIO2_BASE)
#define TRU_HPS_GPIO_REG(base_addr) ((volatile tru_hps_gpio_reg_t *const)base_addr)

// ==================
// HPS GPIO functions
// ==================

static inline void tru_hps_gpio_reset_release(void *gpio_base){
	// Release GPIO0 module from reset state, i.e. enable it (1 = hold in reset, 0 = release)
	switch((uint32_t)gpio_base){
		case TRU_HPS_GPIO0_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.gpio0 = 0; break;
		case TRU_HPS_GPIO1_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.gpio1 = 0; break;
		case TRU_HPS_GPIO2_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.gpio2 = 0; break;
		default:
	}
}

static inline void tru_hps_gpio_reset_hold(void *gpio_base){
	// Hold GPIO0 module in reset state, i.e. enable it (1 = hold in reset, 0 = release)
	switch((uint32_t)gpio_base){
		case TRU_HPS_GPIO0_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.gpio0 = 1; break;
		case TRU_HPS_GPIO1_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.gpio1 = 1; break;
		case TRU_HPS_GPIO2_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.gpio2 = 1; break;
		default:
	}
}

static inline void tru_hps_gpio_set_pin_output(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->dir |= 1 << pinnum_bitpos;
}

static inline void tru_hps_gpio_set_pin_input(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->dir &= ~(1 << pinnum_bitpos);
}

static inline void tru_hps_gpio_set_pin_debounce(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->debounce |= 1 << pinnum_bitpos;
}

static inline void tru_hps_gpio_set_pin_nodebounce(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->debounce &= ~(1 << pinnum_bitpos);
}

static inline void tru_hps_gpio_set_pin_high(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->port_wr |= 1 << pinnum_bitpos;
}

static inline void tru_hps_gpio_set_pin_low(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->port_wr &= ~(1 << pinnum_bitpos);
}

static inline void tru_hps_gpio_set_pin_state(void *gpio_base, uint32_t pinnum_bitpos, tru_hps_gpio_pinstate_t state){
	if(state == TRU_HPS_GPIO_PIN_LOW){
		TRU_HPS_GPIO_REG(gpio_base)->port_wr &= ~(1 << pinnum_bitpos);
	}else{
		TRU_HPS_GPIO_REG(gpio_base)->port_wr |= 1 << pinnum_bitpos;
	}
}

static inline void tru_hps_gpio_toggle_pin(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->port_wr ^= 1 << pinnum_bitpos;
}

static inline uint32_t tru_hps_gpio_get_pin(void *gpio_base, uint32_t pinnum_bitpos){
	return TRU_HPS_GPIO_REG(gpio_base)->port_rd & (1 << pinnum_bitpos);
}

static inline tru_hps_gpio_pinstate_t tru_hps_gpio_get_pin_state(void *gpio_base, uint32_t pinnum_bitpos){
	return (TRU_HPS_GPIO_REG(gpio_base)->port_rd & (1 << pinnum_bitpos)) ? TRU_HPS_GPIO_PIN_HIGH : TRU_HPS_GPIO_PIN_LOW;
}

static inline void tru_hps_gpio_int_enable(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->inten |= 1 << pinnum_bitpos;
}

static inline void tru_hps_gpio_int_disable(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->inten &= ~(1 << pinnum_bitpos);
}

static inline void tru_hps_gpio_edge_level(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->inttype_level |= 1 << pinnum_bitpos;  // Select edge sensitive interrupt (1 = edge, 0 = level)
}

static inline void tru_hps_gpio_level(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->inttype_level &= ~(1 << pinnum_bitpos);  // Select level sensitive interrupt (1 = edge, 0 = level)
}

static inline uint32_t tru_hps_gpio_get_pol(void *gpio_base, uint32_t pinnum_bitpos){
	return TRU_HPS_GPIO_REG(gpio_base)->intpolarity & (1 << pinnum_bitpos);
}

static inline void tru_hps_gpio_toggle_pol(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->intpolarity ^= 1 << pinnum_bitpos;
}

static inline void tru_hps_gpio_clear_int(void *gpio_base, uint32_t pinnum_bitpos){
	TRU_HPS_GPIO_REG(gpio_base)->intclear |= 1 << pinnum_bitpos;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
