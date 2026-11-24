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
*/

#include "usb/dclass/tru_usb_cdc.h"
#include "tru_logger.h"

// Prototypes for callbacks
static bool tru_usb_cdc_callback_setup(tru_usb_cdc_t *cdc);
static void tru_usb_cdc_callback_reset(tru_usb_cdc_t *cdc);
static void tru_usb_cdc_callback_enumdone(tru_usb_cdc_t *cdc);
static void tru_usb_cdc_callback_cfg_act(tru_usb_cdc_t *cdc, tru_usb_config_t *config, bool is_activated);
static void tru_usb_cdc_callback_itf_act(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
static void tru_usb_cdc_callback_epi_act(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep, bool is_activated);
static void tru_usb_cdc_callback_epo_act(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep, bool is_activated);
static void tru_usb_cdc_callback_epi_epcompl(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep);
static void tru_usb_cdc_callback_epi_chunk(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep);
static void tru_usb_cdc_callback_epi_compl(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep);
static void tru_usb_cdc_callback_epo_epcompl(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep);
static void tru_usb_cdc_callback_epo_chunk(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep);
static void tru_usb_cdc_callback_epo_compl(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep);
static void tru_usb_cdc_callback_sof(tru_usb_cdc_t *cdc);
static void tru_usb_cdc_callback_setlinecontrol(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, tru_usb_cdc_linecontrol_t *linecontrol);
static void tru_usb_cdc_callback_sendbreak(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, uint16_t dur_ms);

// External callbacks
static tru_usb_cdc_callbacks_t user_callbacks = {
	.cb_reset          = NULL,
	.cb_enumdone       = NULL,
	.cb_setup          = NULL,
	.cb_cfg_act        = NULL,
	.cb_itf_act        = NULL,
	.cb_epi_act        = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_act        = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_epcompl    = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_chunk      = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_compl      = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_epcompl    = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_chunk      = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_compl      = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_sof            = NULL,
	.cb_setlinecontrol = NULL,
	.cb_sendbreak = NULL,
};

void tru_usb_cdc_init(tru_usb_cdc_t *cdc, tru_usb_pc_t *pc, tru_usb_cdc_pd_info_t *desc_info){
	cdc->pc = pc;
	tru_usb_pc_register_dc(pc, cdc, &cdc->callbacks);
	cdc->desc_info = desc_info;

	// Internal callbacks
	cdc->callbacks.cb_reset = (tru_usb_dc_callback_reset_t)tru_usb_cdc_callback_reset;
	cdc->callbacks.cb_enumdone = (tru_usb_dc_callback_reset_t)tru_usb_cdc_callback_enumdone;
	cdc->callbacks.cb_setup = (tru_usb_dc_callback_setup_t)tru_usb_cdc_callback_setup;
	cdc->callbacks.cb_cfg_act = (tru_usb_dc_callback_cfg_act_t)tru_usb_cdc_callback_cfg_act;
	cdc->callbacks.cb_itf_act = (tru_usb_dc_callback_itf_act_t)tru_usb_cdc_callback_itf_act;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epi_act[i] = (tru_usb_dc_callback_epi_act_t)tru_usb_cdc_callback_epi_act;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epo_act[i] = (tru_usb_dc_callback_epo_act_t)tru_usb_cdc_callback_epo_act;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epi_epcompl[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_cdc_callback_epi_epcompl;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epi_chunk[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_cdc_callback_epi_chunk;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epi_compl[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_cdc_callback_epi_compl;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epo_epcompl[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_cdc_callback_epo_epcompl;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epo_chunk[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_cdc_callback_epo_chunk;
	for(uint8_t i = 0; i < 16; i++) cdc->callbacks.cb_epo_compl[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_cdc_callback_epo_compl;
	cdc->callbacks.cb_sof = (tru_usb_dc_callback_sof_t)tru_usb_cdc_callback_sof;
}

void tru_usb_cdc_deinit(tru_usb_cdc_t *cdc){
}

void tru_usb_cdc_config_ext_init(tru_usb_cdc_t *cdc, tru_usb_config_t *config){
	tru_usb_interface_t * interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_cdc_interface_ext_init(cdc, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

void tru_usb_cdc_config_ext_deinit(tru_usb_cdc_t *cdc, tru_usb_config_t *config){
	tru_usb_interface_t *interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_cdc_interface_ext_deinit(cdc, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

tru_usb_cdc_interface_ext_t *tru_usb_cdc_find_class_itf_ext(tru_usb_cdc_t *cdc, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num){
	tru_usb_cdc_interface_ext_t *itf_ext;
	uint16_t num_entries = 0;

	switch(cdc->pc->speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:
			if(cdc->desc_info->hs_cdc_interface_exts){
				itf_ext = cdc->desc_info->hs_cdc_interface_exts;
				num_entries = cdc->desc_info->hs_cdc_interface_exts_num_entries;
			}else if(cdc->desc_info->fs_cdc_interface_exts){
				itf_ext = cdc->desc_info->fs_cdc_interface_exts;
				num_entries = cdc->desc_info->fs_cdc_interface_exts_num_entries;
			}else if(cdc->desc_info->ls_cdc_interface_exts){
				itf_ext = cdc->desc_info->ls_cdc_interface_exts;
				num_entries = cdc->desc_info->ls_cdc_interface_exts_num_entries;
			}
			for(uint16_t i = 0; i < num_entries; i++){
				if(
					itf_ext->ext_hdr.config_num == config_num &&
					itf_ext->ext_hdr.interface_num == interface_num &&
					itf_ext->ext_hdr.alt_setting_num == alt_setting_num
				){
					return itf_ext;
				}
				itf_ext++;
			}
			break;
		case TRU_DWC2_DSTS_ENUMSPD_FS3060:
		case TRU_DWC2_DSTS_ENUMSPD_FS48:
			if(cdc->desc_info->fs_cdc_interface_exts){
				itf_ext = cdc->desc_info->fs_cdc_interface_exts;
				num_entries = cdc->desc_info->fs_cdc_interface_exts_num_entries;
			}else if(cdc->desc_info->ls_cdc_interface_exts){
				itf_ext = cdc->desc_info->ls_cdc_interface_exts;
				num_entries = cdc->desc_info->ls_cdc_interface_exts_num_entries;
			}
			for(uint16_t i = 0; i < num_entries; i++){
				if(
					itf_ext->ext_hdr.config_num == config_num &&
					itf_ext->ext_hdr.interface_num == interface_num &&
					itf_ext->ext_hdr.alt_setting_num == alt_setting_num
				){
					return itf_ext;
				}
				itf_ext++;
			}
			break;
		case TRU_DWC2_DSTS_ENUMSPD_LS6:
			if(cdc->desc_info->ls_cdc_interface_exts){
				itf_ext = cdc->desc_info->ls_cdc_interface_exts;
				num_entries = cdc->desc_info->ls_cdc_interface_exts_num_entries;
			}
			for(uint16_t i = 0; i < num_entries; i++){
				if(
					itf_ext->ext_hdr.config_num == config_num &&
					itf_ext->ext_hdr.interface_num == interface_num &&
					itf_ext->ext_hdr.alt_setting_num == alt_setting_num
				){
					return itf_ext;
				}
				itf_ext++;
			}
			break;
		default:
	}

	return NULL;
}

void tru_usb_cdc_interface_ext_init(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting){
	alt_setting->itf_ext = tru_usb_cdc_find_class_itf_ext(cdc, alt_setting->config_num, alt_setting->interface_num, alt_setting->alt_setting_num);
}

void tru_usb_cdc_interface_ext_deinit(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting){
}

void tru_usb_cdc_register_cb_reset(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_reset_t cb_func){
	user_callbacks.cb_reset = cb_func;
}

void tru_usb_cdc_register_cb_enumdone(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_enumdone_t cb_func){
	user_callbacks.cb_enumdone = cb_func;
}

void tru_usb_cdc_register_cb_setup(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_setup_t cb_func){
	user_callbacks.cb_setup = cb_func;
}

void tru_usb_cdc_register_cb_cfg_act(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_cfg_act_t cb_func){
	user_callbacks.cb_cfg_act = cb_func;
}

void tru_usb_cdc_register_cb_itf_act(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_itf_act_t cb_func){
	user_callbacks.cb_itf_act = cb_func;
}

void tru_usb_cdc_register_cb_epi_act(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_act_t cb_func){
	user_callbacks.cb_epi_act[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_epo_act(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_act_t cb_func){
	user_callbacks.cb_epo_act[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_epi_epcompl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_epcompl[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_epi_chunk(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_chunk[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_epi_compl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_compl[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_epo_epcompl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_epcompl[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_epo_chunk(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_chunk[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_epo_compl(tru_usb_cdc_t *cdc, uint8_t ep_num, tru_usb_cdc_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_compl[ep_num] = cb_func;
}

void tru_usb_cdc_register_cb_sof(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_sof_t cb_func){
	user_callbacks.cb_sof = cb_func;
}

void tru_usb_cdc_register_cb_setlinecontrol(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_setlinecontrol_t cb_func){
	user_callbacks.cb_setlinecontrol = cb_func;
}

void tru_usb_cdc_register_cb_sendbreak(tru_usb_cdc_t *cdc, tru_usb_cdc_callback_sendbreak_t cb_func){
	user_callbacks.cb_sendbreak = cb_func;
}

void tru_usb_cdc_fill_notif_network_connection(void *buf, uint16_t value, uint16_t interface_num){
	TRU_USB_CDC_NTF_PTR(buf)->header.type.val = 0xa1;
	TRU_USB_CDC_NTF_PTR(buf)->header.code = TRU_USB_CDC_NTF_NETWORK_CONNECTION;
	TRU_USB_CDC_NTF_PTR(buf)->header.value = value;
	TRU_USB_CDC_NTF_PTR(buf)->header.index = interface_num;
	TRU_USB_CDC_NTF_PTR(buf)->header.length = 0;
}

void tru_usb_cdc_fill_notif_response_avail(void *buf, uint16_t interface_num){
	TRU_USB_CDC_NTF_PTR(buf)->header.type.val = 0xa1;
	TRU_USB_CDC_NTF_PTR(buf)->header.code = TRU_USB_CDC_NTF_RESPONSE_AVAILABLE;
	TRU_USB_CDC_NTF_PTR(buf)->header.value = 0;
	TRU_USB_CDC_NTF_PTR(buf)->header.index = interface_num;
	TRU_USB_CDC_NTF_PTR(buf)->header.length = 0;
}

void tru_usb_cdc_fill_notif_serial_state(void *buf, uint16_t interface_num, tru_usb_cdc_uartstate_t *uartstate){
	TRU_USB_CDC_NTF_PTR(buf)->header.type.val = 0xa1;
	TRU_USB_CDC_NTF_PTR(buf)->header.code = TRU_USB_CDC_NTF_SERIAL_STATE;
	TRU_USB_CDC_NTF_PTR(buf)->header.value = 0;
	TRU_USB_CDC_NTF_PTR(buf)->header.index = interface_num;
	TRU_USB_CDC_NTF_PTR(buf)->header.length = 2;
	*(tru_usb_cdc_uartstate_t *)TRU_USB_CDC_NTF_PTR(buf)->data = *uartstate;
}

// Transmit network connection notification
void tru_usb_cdc_tx_notif_network_connection(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t value, uint16_t interface_num){
	// The endpoint is not busy?
	if(!tru_dwc2_is_epin_enabled(cdc->pc->base_addr, notif_ep_num)){
		tru_usb_cdc_fill_notif_network_connection(cdc->pc->eps.epis[notif_ep_num].lxfer.chunk.buffers[0].buf, value, interface_num);
		tru_usb_pc_transmit(cdc->pc, notif_ep_num, sizeof(tru_usb_request_t), sizeof(tru_usb_request_t));
	}
}

// Transmit response available notification
void tru_usb_cdc_tx_notif_response_avail(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t interface_num){
	// The endpoint is not busy?
	if(!tru_dwc2_is_epin_enabled(cdc->pc->base_addr, notif_ep_num)){
		tru_usb_cdc_fill_notif_response_avail(cdc->pc->eps.epis[notif_ep_num].lxfer.chunk.buffers[0].buf, interface_num);
		tru_usb_pc_transmit(cdc->pc, notif_ep_num, sizeof(tru_usb_request_t), sizeof(tru_usb_request_t));
	}
}

// Transmit serial state notification
void tru_usb_cdc_tx_notif_serial_state(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t interface_num, bool onchangeonly){
	// The endpoint is not busy?
	if(!tru_dwc2_is_epin_enabled(cdc->pc->base_addr, notif_ep_num)){
		tru_usb_interface_t *interface;
		tru_usb_cdc_interface_ext_t *class_interface_ext;

		interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, interface_num);
		if(interface){
			class_interface_ext = interface->alt_settings.curr_alt_setting->itf_ext;

			if(onchangeonly){
				tru_usb_cdc_ntf_t *buf_notif = (tru_usb_cdc_ntf_t *)cdc->pc->eps.epis[notif_ep_num].lxfer.chunk.buffers[0].buf;  // Serial state stored in EP buffer

				// Serial state vs buffer is same, i.e. no change?
				if(memcmp(class_interface_ext->uartstate, buf_notif->data, sizeof(tru_usb_cdc_uartstate_t)) == 0){
					return;
				}
			}

			tru_usb_cdc_fill_notif_serial_state(cdc->pc->eps.epis[notif_ep_num].lxfer.chunk.buffers[0].buf, interface_num, class_interface_ext->uartstate);
			tru_usb_pc_transmit(cdc->pc, notif_ep_num, sizeof(tru_usb_request_t) + sizeof(tru_usb_cdc_uartstate_t), sizeof(tru_usb_request_t) + sizeof(tru_usb_cdc_uartstate_t));
			//tru_usb_pc_transmit(cdc->pc, notif_ep_num, 10, 10);
		}
	}
}

tru_usb_cdc_uartstate_t tru_usb_cdc_get_serial_state(tru_usb_cdc_t *cdc, uint16_t interface_num){
	tru_usb_interface_t *interface;
	tru_usb_cdc_interface_ext_t *class_interface_ext;
	tru_usb_cdc_uartstate_t uartstate;

	interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, interface_num);
	if(interface){
		class_interface_ext = interface->alt_settings.curr_alt_setting->itf_ext;
		uartstate.val = class_interface_ext->uartstate->val;
	}else{
		uartstate.val = 0;
	}

	return uartstate;
}

void tru_usb_cdc_set_serial_state(tru_usb_cdc_t *cdc, uint16_t interface_num, tru_usb_cdc_uartstate_t *uartstate){
	tru_usb_interface_t *interface;
	tru_usb_cdc_interface_ext_t *class_interface_ext;

	interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, interface_num);
	if(interface){
		class_interface_ext = interface->alt_settings.curr_alt_setting->itf_ext;
		*class_interface_ext->uartstate = *uartstate;
	}
}

bool tru_usb_cdc_notif_serial_state_is_changed(tru_usb_cdc_t *cdc, uint8_t notif_ep_num, uint16_t interface_num){
	tru_usb_interface_t *interface;
	tru_usb_cdc_interface_ext_t *class_interface_ext;

	interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, interface_num);
	if(interface){
		class_interface_ext = interface->alt_settings.curr_alt_setting->itf_ext;  // Serial state stored in class extension
		tru_usb_cdc_ntf_t *buf_notif = (tru_usb_cdc_ntf_t *)cdc->pc->eps.epis[notif_ep_num].lxfer.chunk.buffers[0].buf + sizeof(tru_usb_request_t);  // Serial state stored in EP buffer

		// Serial state vs buffer is different?
		if(memcmp(class_interface_ext->uartstate, buf_notif->data, sizeof(tru_usb_cdc_uartstate_t))){
			return true;
		}
	}

	return false;
}

/*
	CDC class specific setup request handler for request type:
		direction: host-to-device
		type     : class
		recipient: interface

	Handles anything specific for CDC class.
*/
static bool tru_usb_cdc_setup_hd_cls_intf(tru_usb_cdc_t *cdc){
	uint8_t req_interface_num;
	tru_usb_interface_t *req_interface;
	tru_usb_cdc_interface_ext_t *class_interface_ext;
	uint32_t xfer_size;

	// Process the setup request
	switch(cdc->pc->setup.request.code){
		case TRU_USB_CDC_REQ_SEND_ENCAPSULATED_COMMAND:
			if(cdc->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_interface_num = cdc->pc->setup.request.index & 0xff;
				req_interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, req_interface_num);

				// Is the requested interface valid?
				if(req_interface){
					class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;

					switch(cdc->pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: SEND_ENCAP_CMD LEN %u\n", cdc->pc->setup.request.length);
#endif

							xfer_size = (cdc->pc->setup.request.length > class_interface_ext->encap->encap_cmd_buf_size) ? class_interface_ext->encap->encap_cmd_buf_size : cdc->pc->setup.request.length;
							if(xfer_size > cdc->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size) xfer_size = cdc->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size;
							if(xfer_size){
								tru_usb_pc_receive(cdc->pc, 0, xfer_size, cdc->pc->setup.request.length);
								return true;
							}
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

							TRU_MEMCPY(
								class_interface_ext->encap->encap_cmd,
								cdc->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
								cdc->pc->eps.epos[0].lxfer.chunk.total_xferred_size
							);
							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;
		case TRU_USB_CDC_REQ_SET_LINE_CODING:
			if(cdc->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_interface_num = cdc->pc->setup.request.index & 0xff;
				req_interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, req_interface_num);

				// Is the requested interface valid?
				if(req_interface){
					class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;

					switch(cdc->pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: SET_LINE_CODING LEN %u\n", cdc->pc->setup.request.length);
#endif

							xfer_size = (cdc->pc->setup.request.length > sizeof(tru_usb_cdc_linecoding_t)) ? sizeof(tru_usb_cdc_linecoding_t) : cdc->pc->setup.request.length;
							tru_usb_pc_receive(cdc->pc, 0, xfer_size, cdc->pc->setup.request.length);
							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							if(class_interface_ext->linecoding){
								LOG("OLD LINE_CODING: ");
								for(uint32_t i = 0; i < sizeof(tru_usb_cdc_linecoding_t); i++){
									LOG("%.2x", rd8_unaligned((uint8_t *)class_interface_ext->linecoding + i));
								}
								LOG("\n");
							}
#endif

							if(class_interface_ext->linecoding){
								// Update line coding
								//*class_interface_ext->line_coding = *TRU_USB_CDC_LINECODING_PTR(cdc->pc->eps.epos[0].lxfer.chunk.buffers[0].buf);
								TRU_MEMCPY(class_interface_ext->linecoding, cdc->pc->eps.epos[0].lxfer.chunk.buffers[0].buf, cdc->pc->eps.epos[0].lxfer.chunk.total_xferred_size);
							}

							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;
		case TRU_USB_CDC_REQ_SET_CONTROL_LINE_STATE:
			if(cdc->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_interface_num = cdc->pc->setup.request.index & 0xff;
				req_interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, req_interface_num);

				// Is the requested interface valid?
				if(req_interface){
					class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;

					switch(cdc->pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: SET_CONTROL_LINE_STATE ITF=%u ALT=%u DTR=%u RTS=%u\n", req_interface->alt_settings.curr_alt_setting->interface_num, req_interface->alt_settings.curr_alt_setting->alt_setting_num, TRU_USB_CDC_LINECONTROL_PTR(&cdc->pc->setup.request.value)->bits.dtr, TRU_USB_CDC_LINECONTROL_PTR(&cdc->pc->setup.request.value)->bits.rts);
#endif

							if(class_interface_ext->linecontrol){
								// Update line control
								*class_interface_ext->linecontrol = *TRU_USB_CDC_LINECONTROL_PTR(&cdc->pc->setup.request.value);
							}

							tru_usb_cdc_callback_setlinecontrol(cdc, req_interface->alt_settings.curr_alt_setting, class_interface_ext->linecontrol);

							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;
		case TRU_USB_CDC_REQ_SEND_BREAK:
			if(cdc->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_interface_num = cdc->pc->setup.request.index & 0xff;
				req_interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, req_interface_num);

				// Is the requested interface valid?
				if(req_interface){
					class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;

					switch(cdc->pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: SEND_BREAK ITF=%u ALT=%u DUR=%u ms\n", req_interface->alt_settings.curr_alt_setting->interface_num, req_interface->alt_settings.curr_alt_setting->alt_setting_num, cdc->pc->setup.request.value);
#endif

							tru_usb_cdc_callback_sendbreak(cdc, req_interface->alt_settings.curr_alt_setting, cdc->pc->setup.request.value);

							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;
	}

	return false;
}

/*
	CDC class specific setup request handler for request type:
		direction: device-to-host
		type     : class
		recipient: interface

	Handles anything specific for CDC class.
*/
static bool tru_usb_cdc_setup_dh_cls_intf(tru_usb_cdc_t *cdc){
	uint8_t req_interface_num;
	tru_usb_interface_t *req_interface;
	tru_usb_cdc_interface_ext_t *class_interface_ext;
	uint32_t xfer_size;

	// Process the setup request
	switch(cdc->pc->setup.request.code){
		case TRU_USB_CDC_REQ_GET_ENCAPSULATED_RESPONSE:
			if(cdc->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_interface_num = (uint8_t)(cdc->pc->setup.request.index & 0xff);
				req_interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, req_interface_num);

				// Is the requested interface valid?
				if(req_interface){
					class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;

					switch(cdc->pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: GET_ENCAP_RESP LEN %u\n", cdc->pc->setup.request.length);
#endif

							xfer_size = (cdc->pc->setup.request.length > class_interface_ext->encap->encap_resp_buf_size) ? class_interface_ext->encap->encap_resp_buf_size : cdc->pc->setup.request.length;
							if(xfer_size > cdc->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size) xfer_size = cdc->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size;
							if(xfer_size){
								TRU_MEMCPY(cdc->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, class_interface_ext->encap->encap_resp, xfer_size);
								tru_usb_pc_transmit(cdc->pc, 0, xfer_size, cdc->pc->setup.request.length);
								return true;
							}
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;
		case TRU_USB_CDC_REQ_GET_LINE_CODING:
			if(cdc->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_interface_num = cdc->pc->setup.request.index & 0xff;
				req_interface = tru_usb_interfaces_find_interface(&cdc->pc->configs.curr_config->interfaces, req_interface_num);

				// Is the requested interface valid?
				if(req_interface){
					class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
					
					switch(cdc->pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: GET_LINE_CODING LEN %u\n", cdc->pc->setup.request.length);
#endif

							if(class_interface_ext->linecoding){
								xfer_size = (cdc->pc->setup.request.length > sizeof(tru_usb_cdc_linecoding_t)) ? sizeof(tru_usb_cdc_linecoding_t) : cdc->pc->setup.request.length;
								TRU_MEMCPY(cdc->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, class_interface_ext->linecoding, xfer_size);
								tru_usb_pc_transmit(cdc->pc, 0, xfer_size, cdc->pc->setup.request.length);
							}else{
								tru_usb_pc_transmit_zlp_sr(cdc->pc, 0, true);
							}

							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
							cdc->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;
	}

	return false;
}

/*
	Common CDC class setup request handler.
	This extends the standard setup requests and also adds new setup requests to support CDC devices.
*/
static bool tru_usb_cdc_callback_setup(tru_usb_cdc_t *cdc){
	bool success = false;

	// ========================================================================================================================
	// Process handled or unhandled setup request.  Implements CDC class extension requests, i.e. extend the standard requests.
	// ========================================================================================================================

	// What request direction?
	switch(cdc->pc->setup.request.type.bits.dir){
		case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
			// What request type?
			switch(cdc->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(cdc->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(cdc->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(cdc->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
			// What request type?
			switch(cdc->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(cdc->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(cdc->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(cdc->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;
	}

	// =========================================================================
	// Process unhandled setup request.  Implements CDC class specific requests.
	// =========================================================================

	if(!cdc->pc->setup.is_handled){
		// What request direction?
		switch(cdc->pc->setup.request.type.bits.dir){
			case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
				// What request type?
				switch(cdc->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(cdc->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(cdc->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: success = success | tru_usb_cdc_setup_hd_cls_intf(cdc); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(cdc->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;

			case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
				// What request type?
				switch(cdc->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(cdc->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(cdc->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: success = success | tru_usb_cdc_setup_dh_cls_intf(cdc); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(cdc->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;
		}
	}

	if(user_callbacks.cb_setup) user_callbacks.cb_setup(cdc);

	return success;
}

static void tru_usb_cdc_callback_reset(tru_usb_cdc_t *cdc){
	if(user_callbacks.cb_reset) user_callbacks.cb_reset(cdc);
}

static void tru_usb_cdc_callback_enumdone(tru_usb_cdc_t *cdc){
	if(user_callbacks.cb_enumdone) user_callbacks.cb_enumdone(cdc);
}

void tru_usb_cdc_callback_cfg_act(tru_usb_cdc_t *cdc, tru_usb_config_t *config, bool is_activated){
	if(is_activated){
		tru_usb_cdc_config_ext_init(cdc, config);
	}else{
		tru_usb_cdc_config_ext_deinit(cdc, config);
	}
	if(user_callbacks.cb_cfg_act) user_callbacks.cb_cfg_act(cdc, config, is_activated);
}

void tru_usb_cdc_callback_itf_act(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated){
	if(is_activated){
		tru_usb_cdc_interface_ext_init(cdc, alt_setting);
	}else{
		tru_usb_cdc_interface_ext_deinit(cdc, alt_setting);
	}
	if(user_callbacks.cb_itf_act) user_callbacks.cb_itf_act(cdc, alt_setting, is_activated);
}

void tru_usb_cdc_callback_epi_act(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep, bool is_activated){
	if(ep->num != 0){
		tru_usb_interface_alt_setting_t *alt_setting = ep->curr_alt_setting;

		// Is endpoint a notification, i.e. interface is ACM subclass?
		if(alt_setting->desc[6] == TRU_USB_CDC_INTERFACE_SUBCLASS_ACM){
			tru_usb_cdc_uartstate_t uartstate = { .val = 0 };  // Set initial state
			tru_usb_cdc_set_serial_state(cdc, alt_setting->alt_setting_num, &uartstate);
			tru_usb_cdc_tx_notif_serial_state(cdc, ep->num, alt_setting->alt_setting_num, false);  // Enable endpoint so that it transmits the first notification
		}
	}

	if(user_callbacks.cb_epi_act[ep->num]){
		user_callbacks.cb_epi_act[ep->num](cdc, ep, is_activated);
	}
}

void tru_usb_cdc_callback_epo_act(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep, bool is_activated){
	if(user_callbacks.cb_epo_act[ep->num]){
		user_callbacks.cb_epo_act[ep->num](cdc, ep, is_activated);
	}
}

void tru_usb_cdc_callback_epi_epcompl(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_epcompl[ep->num]) user_callbacks.cb_epi_epcompl[ep->num](cdc, ep);
}

void tru_usb_cdc_callback_epi_chunk(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_chunk[ep->num]) user_callbacks.cb_epi_chunk[ep->num](cdc, ep);
}

void tru_usb_cdc_callback_epi_compl(tru_usb_cdc_t *cdc, tru_usb_pc_epi_t *ep){
	if(ep->num != 0){
		tru_usb_interface_alt_setting_t *alt_setting = ep->curr_alt_setting;

		// Is endpoint a notification, i.e. interface is ACM subclass?
		if(alt_setting->desc[6] == TRU_USB_CDC_INTERFACE_SUBCLASS_ACM){
			tru_usb_cdc_tx_notif_serial_state(cdc, ep->num, alt_setting->alt_setting_num, true);  // Transmit serial state notification if it has changed
		}
	}

	if(user_callbacks.cb_epi_compl[ep->num]) user_callbacks.cb_epi_compl[ep->num](cdc, ep);
}

void tru_usb_cdc_callback_epo_epcompl(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_epcompl[ep->num]) user_callbacks.cb_epo_epcompl[ep->num](cdc, ep);
}

void tru_usb_cdc_callback_epo_chunk(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_chunk[ep->num]) user_callbacks.cb_epo_chunk[ep->num](cdc, ep);
}

void tru_usb_cdc_callback_epo_compl(tru_usb_cdc_t *cdc, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_compl[ep->num]) user_callbacks.cb_epo_compl[ep->num](cdc, ep);
}

void tru_usb_cdc_callback_sof(tru_usb_cdc_t *cdc){
	if(user_callbacks.cb_sof) user_callbacks.cb_sof(cdc);
}

void tru_usb_cdc_callback_setlinecontrol(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, tru_usb_cdc_linecontrol_t *linecontrol){
	if(user_callbacks.cb_setlinecontrol) user_callbacks.cb_setlinecontrol(cdc, alt_setting, linecontrol);
}

void tru_usb_cdc_callback_sendbreak(tru_usb_cdc_t *cdc, tru_usb_interface_alt_setting_t *alt_setting, uint16_t dur_ms){
	if(user_callbacks.cb_sendbreak) user_callbacks.cb_sendbreak(cdc, alt_setting, dur_ms);
}
