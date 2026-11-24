/*
	MIT License

	Copyright (c) 2022 Truong Hy

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

	USB 2.0 peripheral controller (PC) mode support.

	Contains functions to manage the USB 2.0 OTG controller in peripheral mode.
	Peripheral mode is also known as device mode or gadget mode (in the Linux world).
*/

#ifndef TRU_USB_PC_H
#define TRU_USB_PC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"
#include "tru_error.h"
#include "tru_util.h"
#include "tru_iom.h"
#include "usb/tru_usb_feature_status.h"
#include "usb/tru_usb_config.h"
#include "usb/tru_usb_interface.h"
#include "usb/tru_usb_ep.h"
#include "usb/tru_usb_pc_ep.h"
#include "usb/tru_usb_string.h"
#include "usb/tru_usb_setup.h"
#include "usb/tru_usb_def.h"
#include "usb/tru_usb_dev.h"
#include "usb/tru_usb_lxfer.h"
#include "usb/tru_usb_lang_id.h"
#include "usb/dclass/tru_usb_dc_info.h"
#include "usb/dclass/tru_usb_pd_info.h"
#include "synopsys/tru_dwc2.h"
#include "synopsys/tru_dwc2_sgdma.h"
#include <stdbool.h>
#include <stdint.h>

// Configurations
#define TRU_USB_PC_SETUP_EARLY_XFER      0
#define TRU_USB_PC_SETUP_STRICT_ZLPOUT   1
//#define TRU_USB_PC_SETUP_EARLY_XFER    1
//#define TRU_USB_PC_SETUP_STRICT_ZLPOUT 0
#define TRU_USB_PC_TXFIFOE_IRQ_FILL_FIFO 1  // This is normally 1
#define TRU_USB_PC_DEFER_XFER            1  // This is normally 1

// USB device mode enumeration state statuses
typedef enum tru_usb_pc_dev_state_e{
	TRU_USB_PC_DEV_STATE_NONE,
	TRU_USB_PC_DEV_STATE_ATTACHED,
	TRU_USB_PC_DEV_STATE_POWERED,
	TRU_USB_PC_DEV_STATE_ENUM_SPEED_DONE,
	TRU_USB_PC_DEV_STATE_DEFAULT,
	TRU_USB_PC_DEV_STATE_ADDRESS,
	TRU_USB_PC_DEV_STATE_CONFIGURED,
	TRU_USB_PC_DEV_STATE_SUSPENDED,
}tru_usb_pc_dev_state_t;

// Generic function response codes
typedef enum tru_usb_pc_status_e{
	TRU_USB_PC_RC_OK,
	TRU_USB_PC_RC_EP_BUSY,
	TRU_USB_PC_RC_EP_NOTACTIVATED,
}tru_usb_pc_status_t;

// USB peripheral mode (device mode) controller structure
typedef struct{
	tru_dwc2_hwcfg_t hwcfg;
	tru_error_code_t last_err_code;
	void *base_addr;
	tru_dwc2_phy_t phy;
	bool otg_id_pin_supported;
	bool enable_sof_trigger;
	tru_dwc2_dma_mode_t dma_mode;            // DMA mode setting
	tru_dwc2_txfifo_mode_t txfifo_mode;
	tru_usb_pc_eps_t eps;                    // USB peripheral controller endpoints
	uint8_t speed_sel;                       // Device speed select
	uint8_t speed;                           // Current enumerated speed
	uint32_t sof_per_sec;                    // Number of SOF counts per second, i.e. SOF rate
	uint32_t sof_per_ms;                     // Number of SOF counts per millisecond, i.e. SOF rate
	tru_usb_setup_t setup;                   // USB setup request
	tru_usb_pc_dev_state_t state;            // USB device state
	uint16_t status;                         // USB device status used by GET_STATUS, CLEAR_FEATURE, SET_FEATURE
	tru_dwc2_grxstsp_t grxsts;               // Global receive FIFO status register value
	tru_usb_dev_t dev;                       // USB device info (device + qualifier descriptors)
	tru_usb_configs_t configs;               // USB device configurations (configuration + interface + endpoint descriptors)
	tru_usb_strings_t strings;               // USB device strings (string descriptors)
	tru_usb_dc_info_t *dc_info;              // Array of USB device class info
	uint32_t num_dc_info;                    // Number of array elements
	uint32_t dc_info_count;                  // Number of registered device class info items
}tru_usb_pc_t;

// A macro to cast a pointer
#define TO_TRU_USB_PC_PTR(ptr) ((tru_usb_pc_t *)ptr)

// External callbacks
typedef void (*tru_usb_pc_callback_t)(tru_usb_pc_t *pc);                                    // Generic callback function pointer type
typedef bool (*tru_usb_pc_callback_bool_t)(tru_usb_pc_t *pc);                               // Generic callback function pointer type that returns bool
typedef void (*tru_usb_pc_callback_ep_configured_t)(tru_usb_pc_t *pc, bool is_configured);  // Endpoint active/in-active callback function pointer type

void tru_usb_pc_init(
	tru_usb_pc_t *pc,
	tru_usb_pd_info_t *desc_set,
	uintptr_t usb_base_addr,
	tru_dwc2_phy_t phy,
	uint8_t speed_sel,
	bool otg_id_pin_supported,
	bool enable_sof_trigger,
	tru_dwc2_dma_mode_t dma_mode,
	uint32_t num_dc_info
);
void tru_usb_pc_deinit(tru_usb_pc_t *pc);
void tru_usb_pc_register_dc(tru_usb_pc_t *pc, void *devclass, tru_usb_dc_callbacks_t *callbacks);
void tru_usb_pc_init_irq(tru_usb_pc_t *pc);
void tru_usb_pc_init_hw(tru_usb_pc_t *pc);
void tru_usb_pc_deinit_hw(tru_usb_pc_t *pc);
bool tru_usb_pc_periph_reset_held(void *usb_base_addr);
void tru_usb_pc_periph_reset_release(void *usb_base_addr);
void tru_usb_pc_start(tru_usb_pc_t *pc);
void tru_usb_pc_is_ahb_master_busy_wait(void *usb_base_addr);
void tru_usb_pc_is_flush_txrxfifo_busy_wait(void *usb_base_addr);
void tru_usb_pc_setup_sgdma_epin(tru_usb_pc_t *pc, tru_usb_pc_epi_t *epin, bool is_ioc_per_desc);
void tru_usb_pc_setup_sgdma_epout(tru_usb_pc_t *pc, tru_usb_pc_epo_t *epout, bool is_ioc_per_desc);
void tru_usb_pc_enable_xfer_epin(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid);
void tru_usb_pc_enable_xfer_epout(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid);
void tru_usb_pc_transmit_sr(tru_usb_pc_t *pc, uint8_t ep_num, bool is_first);
uint32_t tru_usb_pc_transmit(tru_usb_pc_t *pc, uint8_t ep_num, uint32_t xfer_size, uint32_t exp_size);
void tru_usb_pc_transmit_resume_ep(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_transmit_zlp_sr(tru_usb_pc_t *pc, uint8_t ep_num, bool is_first);
uint32_t tru_usb_pc_transmit_zlp(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_receive_sr(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_stall_t stall, bool is_first);
uint32_t tru_usb_pc_receive(tru_usb_pc_t *pc, uint8_t ep_num, uint32_t xfer_size, uint32_t exp_size);
void tru_usb_pc_receive_resume_ep(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_receive_zlp_sr(tru_usb_pc_t *pc, uint8_t ep_num, bool is_first);
uint32_t tru_usb_pc_receive_zlp(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_process_txfifo_write(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_process_txfifo_empty_irq(tru_usb_pc_t *pc);
void tru_usb_pc_process_deferred(tru_usb_pc_t *pc);
void tru_usb_pc_halt_epin(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_halt_epout(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_unhalt_epin(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_unhalt_epout(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_update_lxfer_details_epin(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_end_lxfer_details_epin(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_update_lxfer_details_epout(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_end_lxfer_details_epout(tru_usb_pc_t *pc, uint8_t ep_num);
void tru_usb_pc_setup_error(tru_usb_pc_t *pc);
void tru_usb_pc_setup_set_config_act_ep(tru_usb_pc_t *pc, tru_usb_config_t *req_config);
void tru_usb_pc_setup_set_config_deact_ep(tru_usb_pc_t *pc);
void tru_usb_pc_setup_set_interface_act_ep(tru_usb_pc_t *pc, tru_usb_config_t *req_config, tru_usb_interface_t *req_interface, tru_usb_interface_alt_setting_t *req_alt_setting, bool realloc_fifo);
void tru_usb_pc_setup_set_interface_deact_ep(tru_usb_pc_t *pc, tru_usb_interface_t *req_interface);
void tru_usb_pc_setup_callback_config_interfaces(tru_usb_pc_t *pc, tru_usb_config_t *config, bool is_activated);
void tru_usb_pc_setup_callback_config_ep_act(tru_usb_pc_t *pc, tru_usb_config_t *config);
void tru_usb_pc_setup_callback_config_ep_deact(tru_usb_pc_t *pc, tru_usb_config_t *config);
void tru_usb_pc_setup_callback_interface_ep_act(tru_usb_pc_t *pc, tru_usb_interface_alt_setting_t *alt_setting);
void tru_usb_pc_setup_callback_interface_ep_deact(tru_usb_pc_t *pc, tru_usb_interface_alt_setting_t *alt_setting);
bool tru_usb_pc_setup_set_config(tru_usb_pc_t *pc);
bool tru_usb_pc_setup_set_interface(tru_usb_pc_t *pc);
bool tru_usb_pc_setup_hd_std_dev(tru_usb_pc_t *pc);
bool tru_usb_pc_setup_hd_std_intf(tru_usb_pc_t *pc);
bool tru_usb_pc_setup_hd_std_ep(tru_usb_pc_t *pc);
bool tru_usb_pc_setup_dh_std_dev(tru_usb_pc_t *pc);
bool tru_usb_pc_setup_dh_std_intf(tru_usb_pc_t *pc);
bool tru_usb_pc_setup_dh_std_ep(tru_usb_pc_t *pc);
void tru_usb_pc_setup_handler(tru_usb_pc_t *pc);
void tru_usb_pc_setup_enable_transfer(tru_usb_pc_t *pc);

#ifdef __cplusplus
}
#endif

#endif
