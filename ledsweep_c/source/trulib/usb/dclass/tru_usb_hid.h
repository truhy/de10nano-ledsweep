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

	USB 2.0 device class HID (Human Interface Device) support.
*/

#ifndef TRU_USB_HID_H
#define TRU_USB_HID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_hid_def.h"
#include "usb/tru_usb_pc.h"
#include <stdbool.h>
#include <stdint.h>

// HID class device (programming model)
typedef struct{
	tru_usb_hid_pd_info_t *desc_set;
	tru_usb_dc_callbacks_t callbacks;  // Internal callbacks
	tru_usb_pc_t *pc;  // USB Peripheral mode Controller
	tru_usb_hid_pd_info_t *desc_info;
}tru_usb_hid_t;

// A macro to cast a pointer to a HID class pointer
#define TO_HID_PTR(ptr) ((tru_usb_hid_t *)ptr)

// For external callbacks
typedef void (*tru_usb_hid_callback_reset_t)(tru_usb_hid_t *hid);
typedef void (*tru_usb_hid_callback_enumdone_t)(tru_usb_hid_t *hid);
typedef bool (*tru_usb_hid_callback_setup_t)(tru_usb_hid_t *hid);
typedef void (*tru_usb_hid_callback_cfg_act_t)(tru_usb_hid_t *hid, tru_usb_config_t *config, bool is_activated);
typedef void (*tru_usb_hid_callback_itf_act_t)(tru_usb_hid_t *hid, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
typedef void (*tru_usb_hid_callback_epi_act_t)(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep, bool is_activated);
typedef void (*tru_usb_hid_callback_epo_act_t)(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep, bool is_activated);
typedef void (*tru_usb_hid_callback_epi_compl_t)(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep);
typedef void (*tru_usb_hid_callback_epo_compl_t)(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep);
typedef void (*tru_usb_hid_callback_sof_t)(tru_usb_hid_t *hid);
typedef void (*tru_usb_hid_callback_set_report_t)(tru_usb_hid_t *hid, tru_usb_hid_interface_ext_t *intf_ext, tru_usb_hid_report_info_t *report_info, uint8_t report_id, uint8_t report_type);  // Callback for when report data is received from a setup SET REPORT request

// External callbacks
typedef struct{
	tru_usb_hid_callback_reset_t cb_reset;
	tru_usb_hid_callback_enumdone_t cb_enumdone;
	tru_usb_hid_callback_setup_t cb_setup;
	tru_usb_hid_callback_cfg_act_t cb_cfg_act;
	tru_usb_hid_callback_itf_act_t cb_itf_act;
	tru_usb_hid_callback_epi_act_t cb_epi_act[16];
	tru_usb_hid_callback_epo_act_t cb_epo_act[16];
	tru_usb_hid_callback_epi_compl_t cb_epi_chunk[16];
	tru_usb_hid_callback_epi_compl_t cb_epi_compl[16];
	tru_usb_hid_callback_epo_compl_t cb_epo_chunk[16];
	tru_usb_hid_callback_epo_compl_t cb_epo_compl[16];
	tru_usb_hid_callback_sof_t cb_sof;
	tru_usb_hid_callback_set_report_t cb_set_report;
}tru_usb_hid_callbacks_t;

void tru_usb_hid_init(tru_usb_hid_t *hid, tru_usb_pc_t *pc, tru_usb_hid_pd_info_t *desc_info);
void tru_usb_hid_deinit(tru_usb_hid_t *hid);
void tru_usb_hid_class_ext_init(tru_usb_hid_t *hid);
void tru_usb_hid_class_ext_deinit(tru_usb_hid_t *hid);
void tru_usb_hid_config_ext_init(tru_usb_hid_t *hid, tru_usb_config_t *config);
void tru_usb_hid_config_ext_deinit(tru_usb_hid_t *hid, tru_usb_config_t *config);
tru_usb_hid_interface_ext_t *tru_usb_hid_find_class_itf_ext(tru_usb_hid_t *hid, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num);
void tru_usb_hid_interface_ext_init(tru_usb_hid_t *hid, tru_usb_interface_alt_setting_t *alt_setting);
void tru_usb_hid_interface_ext_deinit(tru_usb_hid_t *hid, tru_usb_interface_alt_setting_t *alt_setting);
void tru_usb_hid_register_cb_reset(tru_usb_hid_t *hid, tru_usb_hid_callback_reset_t cb_func);
void tru_usb_hid_register_cb_enumdone(tru_usb_hid_t *hid, tru_usb_hid_callback_enumdone_t cb_func);
void tru_usb_hid_register_cb_setup(tru_usb_hid_t *hid, tru_usb_hid_callback_setup_t cb_func);
void tru_usb_hid_register_cb_cfg_act(tru_usb_hid_t *hid, tru_usb_hid_callback_cfg_act_t cb_func);
void tru_usb_hid_register_cb_itf_act(tru_usb_hid_t *hid, tru_usb_hid_callback_itf_act_t cb_func);
void tru_usb_hid_register_cb_epi_act(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epi_act_t cb_func);
void tru_usb_hid_register_cb_epo_act(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epo_act_t cb_func);
void tru_usb_hid_register_cb_epi_chunk(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epi_compl_t cb_func);
void tru_usb_hid_register_cb_epi_compl(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epi_compl_t cb_func);
void tru_usb_hid_register_cb_epo_chunk(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epo_compl_t cb_func);
void tru_usb_hid_register_cb_epo_compl(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epo_compl_t cb_func);
void tru_usb_hid_register_cb_sof(tru_usb_hid_t *hid, tru_usb_hid_callback_sof_t cb_func);
void tru_usb_hid_register_cb_set_report(tru_usb_hid_t *hid, tru_usb_hid_callback_set_report_t cb_func);
void tru_usb_hid_create_reports_from_sizes(tru_usb_hid_t *hid, tru_usb_hid_reports_info_set_t *reports, tru_dwc2_dma_mode_t dma_mode);
uint32_t tru_usb_hid_parse_short_item_int(uint8_t *item, uint8_t prefix_data_size);
void tru_usb_hid_report_desc_to_report_sizes(tru_usb_hid_t *hid, tru_usb_hid_interface_ext_t *intf_ext, void *report_desc, uint32_t report_desc_size);
void tru_usb_hid_set_idle_interface(tru_usb_hid_interface_ext_t *intf_ext, uint8_t idle_rate);
void tru_usb_hid_set_idle_config(tru_usb_config_t *config, uint8_t idle_rate);
tru_usb_hid_report_info_t *tru_usb_hid_find_input_report_by_id(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id);
tru_usb_hid_report_info_t *tru_usb_hid_find_output_report_by_id(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id);
tru_usb_hid_report_info_t *tru_usb_hid_find_output_report_by_id_and_set_currindex(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id);
tru_usb_hid_report_info_t *tru_usb_hid_find_feature_report_by_id(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id);
tru_usb_hid_report_info_t *tru_hid_get_next_input_report_info(tru_usb_pc_epi_t *ep, tru_dwc2_dma_mode_t dma_mode);
tru_usb_pc_epi_t *tru_hid_get_report_epi_from_epo(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep);
tru_usb_pc_epi_t *tru_hid_get_report_epi_from_intf_ext(tru_usb_hid_t *hid, tru_usb_hid_interface_ext_t *intf_ext);
void tru_hid_copy_output_report(tru_usb_pc_epo_t *ep, uint32_t start_offset);
tru_usb_hid_report_info_t *tru_hid_get_output_report_info(tru_usb_pc_epo_t *ep);

#ifdef __cplusplus
}
#endif

#endif
