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

	Microsoft OS (MSOS) USB 2.0 support.
*/

#ifndef TRU_USB_MSOS_H
#define TRU_USB_MSOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_msos_def.h"
#include "usb/tru_usb_pc.h"
#include <stdbool.h>
#include <stdint.h>

// MSOS class device (programming model)
typedef struct{
	tru_usb_dc_callbacks_t callbacks;  // Internal callbacks
	tru_usb_pc_t *pc;  // USB Peripheral mode Controller
	tru_usb_msos_pd_info_t *desc_info;
}tru_usb_msos_t;

// A macro to cast a pointer to a MSOS class pointer
#define TO_MSOS_PTR(ptr) ((tru_usb_msos_t *)ptr)

// For external callbacks
typedef void (*tru_usb_msos_callback_reset_t)(tru_usb_msos_t *msos);
typedef void (*tru_usb_msos_callback_enumdone_t)(tru_usb_msos_t *msos);
typedef bool (*tru_usb_msos_callback_setup_t)(tru_usb_msos_t *msos);
typedef void (*tru_usb_msos_callback_cfg_act_t)(tru_usb_msos_t *msos, tru_usb_config_t *config, bool is_activated);
typedef void (*tru_usb_msos_callback_itf_act_t)(tru_usb_msos_t *msos, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
typedef void (*tru_usb_msos_callback_epi_act_t)(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep, bool is_activated);
typedef void (*tru_usb_msos_callback_epo_act_t)(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep, bool is_activated);
typedef void (*tru_usb_msos_callback_epi_compl_t)(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep);
typedef void (*tru_usb_msos_callback_epo_compl_t)(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep);
typedef void (*tru_usb_msos_callback_sof_t)(tru_usb_msos_t *msos);

// External callbacks
typedef struct{
	tru_usb_msos_callback_reset_t cb_reset;
	tru_usb_msos_callback_enumdone_t cb_enumdone;
	tru_usb_msos_callback_setup_t cb_setup;
	tru_usb_msos_callback_cfg_act_t cb_cfg_act;
	tru_usb_msos_callback_itf_act_t cb_itf_act;
	tru_usb_msos_callback_epi_act_t cb_epi_act[16];
	tru_usb_msos_callback_epo_act_t cb_epo_act[16];
	tru_usb_msos_callback_epi_compl_t cb_epi_epcompl[16];
	tru_usb_msos_callback_epi_compl_t cb_epi_chunk[16];
	tru_usb_msos_callback_epi_compl_t cb_epi_compl[16];
	tru_usb_msos_callback_epo_compl_t cb_epo_epcompl[16];
	tru_usb_msos_callback_epo_compl_t cb_epo_chunk[16];
	tru_usb_msos_callback_epo_compl_t cb_epo_compl[16];
	tru_usb_msos_callback_sof_t cb_sof;
}tru_usb_msos_callbacks_t;

void tru_usb_msos_init(tru_usb_msos_t *msos, tru_usb_pc_t *pc, tru_usb_msos_pd_info_t *desc_info);
void tru_usb_msos_deinit(tru_usb_msos_t *msos);
void tru_usb_msos_dev_ext_init(tru_usb_msos_t *msos);
void tru_usb_msos_dev_ext_deinit(tru_usb_msos_t *msos);
void tru_usb_msos_config_ext_init(tru_usb_msos_t *msos, tru_usb_config_t *config);
void tru_usb_msos_config_ext_deinit(tru_usb_msos_t *msos, tru_usb_config_t *config);
tru_usb_msos_interface_ext_t *tru_usb_msos_find_class_itf_ext(tru_usb_msos_t *msos, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num);
void tru_usb_msos_interface_ext_init(tru_usb_msos_t *msos, tru_usb_interface_alt_setting_t *alt_setting);
void tru_usb_msos_interface_ext_deinit(tru_usb_msos_t *msos, tru_usb_interface_alt_setting_t *alt_setting);
void tru_usb_msos_register_cb_reset(tru_usb_msos_t *msos, tru_usb_msos_callback_reset_t cb_func);
void tru_usb_msos_register_cb_enumdone(tru_usb_msos_t *msos, tru_usb_msos_callback_enumdone_t cb_func);
void tru_usb_msos_register_cb_setup(tru_usb_msos_t *msos, tru_usb_msos_callback_setup_t cb_func);
void tru_usb_msos_register_cb_cfg_act(tru_usb_msos_t *msos, tru_usb_msos_callback_cfg_act_t cb_func);
void tru_usb_msos_register_cb_itf_act(tru_usb_msos_t *msos, tru_usb_msos_callback_itf_act_t cb_func);
void tru_usb_msos_register_cb_epi_act(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_act_t cb_func);
void tru_usb_msos_register_cb_epo_act(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_act_t cb_func);
void tru_usb_msos_register_cb_epi_epcompl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_compl_t cb_func);
void tru_usb_msos_register_cb_epi_chunk(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_compl_t cb_func);
void tru_usb_msos_register_cb_epi_compl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_compl_t cb_func);
void tru_usb_msos_register_cb_epo_epcompl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_compl_t cb_func);
void tru_usb_msos_register_cb_epo_chunk(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_compl_t cb_func);
void tru_usb_msos_register_cb_epo_compl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_compl_t cb_func);
void tru_usb_msos_register_cb_sof(tru_usb_msos_t *msos, tru_usb_msos_callback_sof_t cb_func);

#ifdef __cplusplus
}
#endif

#endif
