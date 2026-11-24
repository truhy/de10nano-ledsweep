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

	Version: 20260208

	USB 2.0 device class CDC-ACM support.
*/

#ifndef TRU_USB_CDC_H
#define TRU_USB_CDC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_cdc_def.h"
#include "usb/tru_usb_pc.h"
#include <stdint.h>

// USB CDC class device (programming model)
typedef struct{
	tru_usb_dc_callbacks_t callbacks;  // Internal callbacks
	tru_usb_pc_t *pc;  // USB Peripheral mode Controller
	tru_usb_cdc_pd_info_t *desc_info;
}tru_usb_cdc_t;

// A macro to cast a pointer to a CDC class pointer
#define TO_CDC_PTR(ptr) ((tru_usb_cdc_t *)ptr)

// For external callbacks
typedef void (*tru_usb_cdc_callback_reset_t)(tru_usb_cdc_t *cdc);
typedef void (*tru_usb_cdc_callback_enumdone_t)(tru_usb_cdc_t *cdc);
typedef bool (*tru_usb_cdc_callback_setup_t)(tru_usb_cdc_t *cdc);
typedef void (*tru_usb_cdc_callback_cfg_act_t)(tru_usb_cdc_t *cdc, tru_usb_config_t *config, bool is_activated);
typedef void (*tru_usb_cdc_callback_itf_act_t)(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
typedef void (*tru_usb_cdc_callback_epi_act_t)(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep, bool is_activated);
typedef void (*tru_usb_cdc_callback_epo_act_t)(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep, bool is_activated);
typedef void (*tru_usb_cdc_callback_epi_compl_t)(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep);
typedef void (*tru_usb_cdc_callback_epo_compl_t)(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep);
typedef void (*tru_usb_cdc_callback_sof_t)(tru_usb_cdc_t *cdc);
typedef void (*tru_usb_cdc_callback_setlinecontrol_t)(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, tru_usb_cdc_linecontrol_t *linecontrol);
typedef void (*tru_usb_cdc_callback_sendbreak_t)(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, uint16_t dur_ms);

// External callbacks
typedef struct{
	tru_usb_cdc_callback_reset_t cb_reset;
	tru_usb_cdc_callback_enumdone_t cb_enumdone;
	tru_usb_cdc_callback_setup_t cb_setup;
	tru_usb_cdc_callback_cfg_act_t cb_cfg_act;
	tru_usb_cdc_callback_itf_act_t cb_itf_act;
	tru_usb_cdc_callback_epi_act_t cb_epi_act[16];
	tru_usb_cdc_callback_epo_act_t cb_epo_act[16];
	tru_usb_cdc_callback_epi_compl_t cb_epi_epcompl[16];
	tru_usb_cdc_callback_epi_compl_t cb_epi_chunk[16];
	tru_usb_cdc_callback_epi_compl_t cb_epi_compl[16];
	tru_usb_cdc_callback_epo_compl_t cb_epo_epcompl[16];
	tru_usb_cdc_callback_epo_compl_t cb_epo_chunk[16];
	tru_usb_cdc_callback_epo_compl_t cb_epo_compl[16];
	tru_usb_cdc_callback_sof_t cb_sof;
	tru_usb_cdc_callback_setlinecontrol_t cb_setlinecontrol;
	tru_usb_cdc_callback_sendbreak_t cb_sendbreak;
}tru_usb_cdc_callbacks_t;

void tru_usb_cdc_init(tru_usb_cdc_t *cdc, tru_usb_pc_t *pc, tru_usb_cdc_pd_info_t *desc_info);
void tru_usb_cdc_deinit(tru_usb_cdc_t *cdc);
void tru_usb_cdc_config_ext_init(tru_usb_cdc_t *cdc, tru_usb_config_t *config);
void tru_usb_cdc_config_ext_deinit(tru_usb_cdc_t *cdc, tru_usb_config_t *config);
tru_usb_cdc_interface_ext_t *tru_usb_cdc_find_class_itf_ext(tru_usb_cdc_t *cdc, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num);
void tru_usb_cdc_interface_ext_init(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting);
void tru_usb_cdc_interface_ext_deinit(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting);
void tru_usb_cdc_register_cb_reset(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_reset_t cb_func);
void tru_usb_cdc_register_cb_enumdone(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_enumdone_t cb_func);
void tru_usb_cdc_register_cb_setup(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_setup_t cb_func);
void tru_usb_cdc_register_cb_cfg_act(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_cfg_act_t cb_func);
void tru_usb_cdc_register_cb_itf_act(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_itf_act_t cb_func);
void tru_usb_cdc_register_cb_epi_act(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_act_t cb_func);
void tru_usb_cdc_register_cb_epo_act(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_act_t cb_func);
void tru_usb_cdc_register_cb_epi_epcompl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_compl_t cb_func);
void tru_usb_cdc_register_cb_epi_chunk(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_compl_t cb_func);
void tru_usb_cdc_register_cb_epi_compl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_compl_t cb_func);
void tru_usb_cdc_register_cb_epo_epcompl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_compl_t cb_func);
void tru_usb_cdc_register_cb_epo_chunk(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_compl_t cb_func);
void tru_usb_cdc_register_cb_epo_compl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_compl_t cb_func);
void tru_usb_cdc_register_cb_sof(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_sof_t cb_func);
void tru_usb_cdc_register_cb_setlinecontrol(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_setlinecontrol_t cb_func);
void tru_usb_cdc_register_cb_sendbreak(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_sendbreak_t cb_func);
void tru_usb_cdc_fill_notif_network_connection(void *buf, uint16_t value, uint16_t interface_num);
void tru_usb_cdc_fill_notif_response_avail(void *buf, uint16_t interface_num);
void tru_usb_cdc_fill_notif_serial_state(void *buf, uint16_t interface_num, tru_usb_cdc_uartstate_t *uartstate);
void tru_usb_cdc_tx_notif_network_connection(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t value, uint16_t interface_num);
void tru_usb_cdc_tx_notif_response_avail(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t interface_num);
void tru_usb_cdc_tx_notif_serial_state(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t interface_num, bool onchangeonly);
tru_usb_cdc_uartstate_t tru_usb_cdc_get_serial_state(tru_usb_cdc_t *cdc, uint16_t interface_num);
void tru_usb_cdc_set_serial_state(tru_usb_cdc_t *cdc, uint16_t interface_num, tru_usb_cdc_uartstate_t *uartstate);
bool tru_usb_cdc_notif_serial_state_is_changed(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t interface_num);

#ifdef __cplusplus
}
#endif

#endif
