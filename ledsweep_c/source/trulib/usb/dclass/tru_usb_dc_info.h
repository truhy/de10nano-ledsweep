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

	USB device class (DC) and other info.
*/

#ifndef TRU_USB_DC_INFO_H
#define TRU_USB_DC_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usb/tru_usb_config.h"
#include "usb/tru_usb_interface.h"
#include "usb/tru_usb_pc_ep.h"

typedef void (*tru_usb_dc_callback_reset_t)(void *dc);
typedef void (*tru_usb_dc_callback_enumdone_t)(void *dc);
typedef bool (*tru_usb_dc_callback_setup_t)(void *dc);
typedef void (*tru_usb_dc_callback_cfg_act_t)(void *dc, tru_usb_config_t *config, bool is_activated);
typedef void (*tru_usb_dc_callback_itf_act_t)(void *dc, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
typedef void (*tru_usb_dc_callback_epi_act_t)(void *dc, tru_usb_pc_epi_t *ep, bool is_activated);
typedef void (*tru_usb_dc_callback_epo_act_t)(void *dc, tru_usb_pc_epo_t *ep, bool is_activated);
typedef void (*tru_usb_dc_callback_epi_compl_t)(void *dc, tru_usb_pc_epi_t *ep);
typedef void (*tru_usb_dc_callback_epo_compl_t)(void *dc, tru_usb_pc_epo_t *ep);
typedef void (*tru_usb_dc_callback_sof_t)(void *dc);
typedef void (*tru_usb_dc_callback_epi_set_smpfrq_t)(void *dc, tru_usb_pc_epi_t *ep);
typedef void (*tru_usb_dc_callback_epo_set_smpfrq_t)(void *dc, tru_usb_pc_epo_t *ep);
typedef void (*tru_usb_dc_callback_itf_set_smpfrq_t)(void *dc, tru_usb_interface_alt_setting_t *alt_setting);

typedef struct{
	tru_usb_dc_callback_reset_t cb_reset;
	tru_usb_dc_callback_enumdone_t cb_enumdone;
	tru_usb_dc_callback_setup_t cb_setup;
	tru_usb_dc_callback_cfg_act_t cb_cfg_act;
	tru_usb_dc_callback_itf_act_t cb_itf_act;
	tru_usb_dc_callback_epi_act_t cb_epi_act[16];
	tru_usb_dc_callback_epo_act_t cb_epo_act[16];
	tru_usb_dc_callback_epi_compl_t cb_epi_epcompl[16];
	tru_usb_dc_callback_epi_compl_t cb_epi_chunk[16];
	tru_usb_dc_callback_epi_compl_t cb_epi_compl[16];
	tru_usb_dc_callback_epo_compl_t cb_epo_epcompl[16];
	tru_usb_dc_callback_epo_compl_t cb_epo_chunk[16];
	tru_usb_dc_callback_epo_compl_t cb_epo_compl[16];
	tru_usb_dc_callback_sof_t cb_sof;
	tru_usb_dc_callback_epi_set_smpfrq_t cb_epi_set_smpfrq;
	tru_usb_dc_callback_epo_set_smpfrq_t cb_epo_set_smpfrq;
	tru_usb_dc_callback_itf_set_smpfrq_t cb_itf_set_smpfrq;
}tru_usb_dc_callbacks_t;

typedef struct{
	void *devclass;  // Device class object
	tru_usb_dc_callbacks_t *callbacks;
}tru_usb_dc_info_t;

#ifdef __cplusplus
}
#endif

#endif
