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

#include "usb/dclass/tru_usb_uac.h"
#include "tru_logger.h"

// Prototypes for callbacks
static void tru_usb_uac_callback_reset(tru_usb_uac_t *uac);
static void tru_usb_uac_callback_enumdone(tru_usb_uac_t *uac);
static bool tru_usb_uac_callback_setup(tru_usb_uac_t *uac);
static void tru_usb_uac_callback_cfg_act(tru_usb_uac_t *uac, tru_usb_config_t *config, bool is_activated);
static void tru_usb_uac_callback_itf_act(tru_usb_uac_t *uac, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
static void tru_usb_uac_callback_epi_act(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep, bool is_activated);
static void tru_usb_uac_callback_epo_act(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep, bool is_activated);
static void tru_usb_uac_callback_epi_epcompl(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep);
static void tru_usb_uac_callback_epi_chunk(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep);
static void tru_usb_uac_callback_epi_compl(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep);
static void tru_usb_uac_callback_epo_epcompl(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep);
static void tru_usb_uac_callback_epo_chunk(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep);
static void tru_usb_uac_callback_epo_compl(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep);
static void tru_usb_uac_callback_sof(tru_usb_uac_t *uac);
static void tru_usb_uac_callback_epi_set_smpfrq(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep);
static void tru_usb_uac_callback_epo_set_smpfrq(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep);

// External callbacks
static tru_usb_uac_callbacks_t user_callbacks = {
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
	.cb_epi_set_smpfrq = NULL,
	.cb_epo_set_smpfrq = NULL,
};

void tru_usb_uac_init(tru_usb_uac_t *uac, tru_usb_pc_t *pc, tru_usb_uac_pd_info_t *desc_info){
	uac->pc = pc;
	tru_usb_pc_register_dc(pc, uac, &uac->callbacks);
	uac->desc_info = desc_info;

	// Internal callbacks
	uac->callbacks.cb_reset = (tru_usb_dc_callback_reset_t)tru_usb_uac_callback_reset;
	uac->callbacks.cb_enumdone = (tru_usb_dc_callback_reset_t)tru_usb_uac_callback_enumdone;
	uac->callbacks.cb_setup = (tru_usb_dc_callback_setup_t)tru_usb_uac_callback_setup;
	uac->callbacks.cb_cfg_act = (tru_usb_dc_callback_cfg_act_t)tru_usb_uac_callback_cfg_act;
	uac->callbacks.cb_itf_act = (tru_usb_dc_callback_itf_act_t)tru_usb_uac_callback_itf_act;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epi_act[i] = (tru_usb_dc_callback_epi_act_t)tru_usb_uac_callback_epi_act;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epo_act[i] = (tru_usb_dc_callback_epo_act_t)tru_usb_uac_callback_epo_act;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epi_epcompl[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_uac_callback_epi_epcompl;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epi_chunk[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_uac_callback_epi_chunk;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epi_compl[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_uac_callback_epi_compl;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epo_epcompl[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_uac_callback_epo_epcompl;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epo_chunk[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_uac_callback_epo_chunk;
	for(uint8_t i = 0; i < 16; i++) uac->callbacks.cb_epo_compl[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_uac_callback_epo_compl;
	uac->callbacks.cb_sof = (tru_usb_dc_callback_sof_t)tru_usb_uac_callback_sof;
	uac->callbacks.cb_epi_set_smpfrq = (tru_usb_dc_callback_epi_set_smpfrq_t)tru_usb_uac_callback_epi_set_smpfrq;
	uac->callbacks.cb_epo_set_smpfrq = (tru_usb_dc_callback_epo_set_smpfrq_t)tru_usb_uac_callback_epo_set_smpfrq;
}

void tru_usb_uac_deinit(tru_usb_uac_t *uac){
}

void tru_usb_uac_config_ext_init(tru_usb_uac_t *uac, tru_usb_config_t *config){
	tru_usb_interface_t * interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_uac_interface_ext_init(uac, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

void tru_usb_uac_config_ext_deinit(tru_usb_uac_t *uac, tru_usb_config_t *config){
	tru_usb_interface_t *interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_uac_interface_ext_deinit(uac, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

tru_usb_uac_interface_ext_t *tru_usb_uac_find_class_itf_ext(tru_usb_uac_t *uac, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num){
	tru_usb_uac_interface_ext_t *itf_ext;

	switch(uac->pc->speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:
			itf_ext = uac->desc_info->hs_uac_interface_exts;
			for(uint16_t i = 0; i < uac->desc_info->hs_uac_interface_exts_num_entries; i++){
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
			itf_ext = uac->desc_info->fs_uac_interface_exts;
			for(uint16_t i = 0; i < uac->desc_info->fs_uac_interface_exts_num_entries; i++){
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
			itf_ext = uac->desc_info->ls_uac_interface_exts;
			for(uint16_t i = 0; i < uac->desc_info->ls_uac_interface_exts_num_entries; i++){
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

tru_usb_uac_ep_ext_t *tru_usb_uac_find_class_ep_ext(tru_usb_uac_t *uac, uint8_t ep_addr){
	tru_usb_uac_ep_ext_t *ep_ext;

	switch(uac->pc->speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:
			ep_ext = uac->desc_info->hs_uac_ep_exts;
			for(uint16_t i = 0; i < uac->desc_info->hs_uac_ep_exts_num_entries; i++){
				if(ep_ext->ep_addr == ep_addr) return ep_ext;
				ep_ext++;
			}
			break;
		case TRU_DWC2_DSTS_ENUMSPD_FS3060:
		case TRU_DWC2_DSTS_ENUMSPD_FS48:
			ep_ext = uac->desc_info->fs_uac_ep_exts;
			for(uint16_t i = 0; i < uac->desc_info->fs_uac_ep_exts_num_entries; i++){
				if(ep_ext->ep_addr == ep_addr) return ep_ext;
				ep_ext++;
			}
			break;
		case TRU_DWC2_DSTS_ENUMSPD_LS6:
			ep_ext = uac->desc_info->ls_uac_ep_exts;
			for(uint16_t i = 0; i < uac->desc_info->ls_uac_ep_exts_num_entries; i++){
				if(ep_ext->ep_addr == ep_addr) return ep_ext;
				ep_ext++;
			}
			break;
		default:
	}

	return NULL;
}

void tru_usb_uac_interface_ext_init(tru_usb_uac_t *uac, tru_usb_interface_alt_setting_t *alt_setting){
	alt_setting->itf_ext = tru_usb_uac_find_class_itf_ext(uac, alt_setting->config_num, alt_setting->interface_num, alt_setting->alt_setting_num);

	if(alt_setting->itf_ext == NULL) return;

	tru_usb_uac_interface_ext_t *itf_ext = alt_setting->itf_ext;  // Assign to structured type

	// Update class interface extension
	uint8_t *af_type_desc = tru_usb_uac_find_audio_format_type_desc(uac, alt_setting);
	if(af_type_desc){
		if(af_type_desc[3] == TRU_USB_UAC_AF_TYPE1 || af_type_desc[3] == TRU_USB_UAC_AF_TYPE3){
			// Update our model of class interface Audio Format
			itf_ext->af.num_channels = af_type_desc[4];
			itf_ext->af.subframe_size = af_type_desc[5];
			itf_ext->af.bit_resolution = af_type_desc[6];
		}
	}

	// Update class endpoint extension
	tru_usb_ep_t *ep = alt_setting->eps.items;
	for(uint16_t i = 0; i < alt_setting->eps.num_endpoints; i++){
		ep->ep_ext = tru_usb_uac_find_class_ep_ext(uac, ep->desc[2]);
		if(ep->ep_ext){
			if(ep->desc[2] & 0x80){
				tru_usb_pc_epi_t *pc_epi = &uac->pc->eps.epis[ep->desc[2] & 0xf];
				pc_epi->ep_ext = ep->ep_ext;
			}else{
				tru_usb_pc_epo_t *pc_epo = &uac->pc->eps.epos[ep->desc[2] & 0xf];
				pc_epo->ep_ext = ep->ep_ext;
			}
		}
	}
}

void tru_usb_uac_interface_ext_deinit(tru_usb_uac_t *uac, tru_usb_interface_alt_setting_t *alt_setting){
}

void tru_usb_uac_register_cb_reset(tru_usb_uac_t *uac, tru_usb_uac_callback_reset_t cb_func){
	user_callbacks.cb_reset = cb_func;
}

void tru_usb_uac_register_cb_enumdone(tru_usb_uac_t *uac, tru_usb_uac_callback_enumdone_t cb_func){
	user_callbacks.cb_enumdone = cb_func;
}

void tru_usb_uac_register_cb_setup(tru_usb_uac_t *uac, tru_usb_uac_callback_setup_t cb_func){
	user_callbacks.cb_setup = cb_func;
}

void tru_usb_uac_register_cb_cfg_act(tru_usb_uac_t *uac, tru_usb_uac_callback_cfg_act_t cb_func){
	user_callbacks.cb_cfg_act = cb_func;
}

void tru_usb_uac_register_cb_itf_act(tru_usb_uac_t *uac, tru_usb_uac_callback_itf_act_t cb_func){
	user_callbacks.cb_itf_act = cb_func;
}

void tru_usb_uac_register_cb_epi_act(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epi_act_t cb_func){
	user_callbacks.cb_epi_act[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epo_act(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epo_act_t cb_func){
	user_callbacks.cb_epo_act[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epi_epcompl(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_epcompl[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epi_chunk(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_chunk[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epi_compl(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_compl[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epo_epcompl(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_epcompl[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epo_chunk(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_chunk[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epo_compl(tru_usb_uac_t *uac, uint8_t ep_num, tru_usb_uac_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_compl[ep_num] = cb_func;
}

void tru_usb_uac_register_cb_epi_set_smpfrq(tru_usb_uac_t *uac, tru_usb_uac_callback_epi_set_smpfrq_t cb_func){
	user_callbacks.cb_epi_set_smpfrq = cb_func;
}

void tru_usb_uac_register_cb_epo_set_smpfrq(tru_usb_uac_t *uac, tru_usb_uac_callback_epo_set_smpfrq_t cb_func){
	user_callbacks.cb_epo_set_smpfrq = cb_func;
}

void tru_usb_uac_register_cb_sof(tru_usb_uac_t *uac, tru_usb_uac_callback_sof_t cb_func){
	user_callbacks.cb_sof = cb_func;
}

// Finds the Audio Class-Specific AC (AudioControl) Interface Descriptor Subtype belonging to the given Entity ID and interface alternate setting
uint8_t tru_usb_uac_find_entity_subtype(tru_usb_uac_t *uac, tru_usb_interface_alt_setting_t *alt_setting, uint8_t entity_id){
	uint8_t *ptr = alt_setting->desc;
	uint8_t pos = 0;

	while(pos < alt_setting->desc_remain){
		if(ptr[0] == 0) return 0;  // Invalid size field value

		if(ptr[1] == TRU_USB_UAC_DESC_TYPE_INTERFACE){
			switch(ptr[2]){
				case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_HEADER: break;  // Ignore subtype header
				default: if(ptr[3] == entity_id) return ptr[2];
			}
		}

		// Move to next descriptor
		pos += ptr[0];
		ptr += ptr[0];
	}

	return 0;
}

uint8_t *tru_usb_uac_find_audio_format_type_desc(tru_usb_uac_t *uac, tru_usb_interface_alt_setting_t *alt_setting){
	uint8_t *ptr = alt_setting->desc;
	uint8_t pos = 0;

	while(pos < alt_setting->desc_remain){
		if(ptr[0] == 0) return NULL;  // Invalid size field value

		if(ptr[1] == TRU_USB_UAC_DESC_TYPE_INTERFACE){
			switch(ptr[2]){
				case TRU_USB_UAC_AS_ITF_DESC_SUBTYPE_FORMAT_TYPE: return ptr; break;
				default:
			}
		}

		// Move to next descriptor
		pos += ptr[0];
		ptr += ptr[0];
	}

	return NULL;
}

tru_usb_uac_copyprotect_t *tru_usb_uac_find_copyprotect(tru_usb_uac_copyprotect_t *cps, uint32_t num_cps, uint8_t entity_id){
	tru_usb_uac_copyprotect_t *cp = cps;

	for(uint32_t i = 0; i < num_cps; i++){
		if(cp->entity_id == entity_id) return cp;
		cp++;
	}

	return NULL;
}

tru_usb_uac_mute_t *tru_usb_uac_find_mute(tru_usb_uac_mute_t *mutes, uint32_t num_mutes, uint8_t entity_id, uint8_t channel_num){
	tru_usb_uac_mute_t *mute = mutes;

	for(uint32_t i = 0; i < num_mutes; i++){
		if(mute->entity_id == entity_id && mute->channel_num == channel_num) return mute;
		mute++;
	}

	return NULL;
}

tru_usb_uac_volume_t *tru_usb_uac_find_volume(tru_usb_uac_volume_t *volumes, uint32_t num_volumes, uint8_t entity_id, uint8_t channel_num){
	tru_usb_uac_volume_t *volume = volumes;

	for(uint32_t i = 0; i < num_volumes; i++){
		if(volume->entity_id == entity_id && volume->channel_num == channel_num) return volume;
		volume++;
	}

	return NULL;
}

static bool tru_usb_uac_setup_set_te_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	tru_usb_uac_interface_ext_t *class_itf_ext;
	uint8_t cs;  // Control Selector
	uint32_t xfer_size;
	tru_usb_uac_copyprotect_t *copyprotect;
	void *param_block;

	if(!interface->alt_settings.curr_alt_setting->itf_ext) return false;

	cs = uac->pc->setup.request.value >> 8 & 0xff;
	switch(cs){
		case TRU_USB_UAC_CS_TE_COPY_PROTECT:
			class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			copyprotect = tru_usb_uac_find_copyprotect(
				class_itf_ext->copyprotects,
				class_itf_ext->num_copyprotects,
				entity_id
			);
			if(copyprotect){
				switch(uac->pc->setup.request.code){
					case TRU_USB_UAC_REQ_SET_CUR: param_block = copyprotect->cur; break;
					default: return false;  // Other attributes is not supported
				}

				switch(uac->pc->eps.epos[0].control_stage){
					case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							switch(uac->pc->setup.request.code){
								case TRU_USB_UAC_REQ_SET_CUR: LOG("REQ: SET CUR COPYPROTECT LEN %u\n", uac->pc->setup.request.length); break;
								default: return false;  // Other attributes is not supported
							}
						#endif

						xfer_size = 1;
						if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
						if(xfer_size > uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size;
						if(xfer_size){
							tru_usb_pc_receive(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
							return true;
						}
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

						TRU_MEMCPY(
							param_block,
							uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
							uac->pc->eps.epos[0].lxfer.chunk.total_xferred_size
						);

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("COPYPROTECT 0x%.2x\n", ((uint8_t *)param_block)[0]);
						#endif

						return true;
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
						return true;
						break;
				}
			}
			break;
	}

	return false;
}

static bool tru_usb_uac_setup_get_te_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	tru_usb_uac_interface_ext_t *class_itf_ext;
	uint32_t xfer_size;
	uint8_t cs;  // Control Selector
	tru_usb_uac_copyprotect_t *copyprotect;

	if(!interface->alt_settings.curr_alt_setting->itf_ext) return false;

	cs = uac->pc->setup.request.value >> 8 & 0xff;
	switch(cs){
		case TRU_USB_UAC_CS_TE_COPY_PROTECT:
			class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			entity_id = uac->pc->setup.request.index >> 8 & 0xff;
			copyprotect = tru_usb_uac_find_copyprotect(
				class_itf_ext->copyprotects,
				class_itf_ext->num_copyprotects,
				entity_id
			);
			if(copyprotect){
				switch(uac->pc->setup.request.code){
					case TRU_USB_UAC_REQ_GET_CUR: break;
					default: return false;  // Other attributes is not supported
				}

				switch(uac->pc->eps.epos[0].control_stage){
					case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							switch(uac->pc->setup.request.code){
								case TRU_USB_UAC_REQ_GET_CUR: LOG("REQ: GET CUR COPYPROTECT LEN %u TXD 0x%.2x\n", uac->pc->setup.request.length, copyprotect->cur[0]); break;
								default: return false;  // Other attributes is not supported
							}
						#endif

						xfer_size = 1;
						if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
						if(xfer_size > uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size;
						if(xfer_size){
							TRU_MEMCPY(uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, copyprotect->cur, xfer_size);
							tru_usb_pc_transmit(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
							return true;
						}
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
						return true;
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
						return true;
						break;
				}
			}
			break;
	}

	return false;
}

static bool tru_usb_uac_setup_set_mu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

static bool tru_usb_uac_setup_get_mu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

static bool tru_usb_uac_setup_set_su_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

static bool tru_usb_uac_setup_get_su_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

static bool tru_usb_uac_setup_set_fu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	tru_usb_uac_interface_ext_t *class_itf_ext;
	uint8_t cs;  // Control Selector
	uint8_t cn;  // Channel Number
	uint32_t xfer_size;
	void *param_block;

	if(!interface->alt_settings.curr_alt_setting->itf_ext) return false;

	cs = uac->pc->setup.request.value >> 8 & 0xff;
	switch(cs){
		case TRU_USB_UAC_CS_FU_MUTE:
			tru_usb_uac_mute_t *mute;
			class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			cn = uac->pc->setup.request.value & 0xff;
			mute = tru_usb_uac_find_mute(
				class_itf_ext->mutes,
				class_itf_ext->num_mutes,
				entity_id,
				cn
			);
			if(mute){
				switch(uac->pc->setup.request.code){
					case TRU_USB_UAC_REQ_SET_CUR: break;
					default: return false;  // Other attributes is not supported
				}

				switch(uac->pc->eps.epos[0].control_stage){
					case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							switch(uac->pc->setup.request.code){
								case TRU_USB_UAC_REQ_SET_CUR: LOG("REQ: SET CUR MUTE LEN %u\n", uac->pc->setup.request.length); break;
								default: return false;  // Other attributes is not supported
							}
						#endif

						xfer_size = (cn == 0xff) ? class_itf_ext->num_mutes - 1 : 1;
						if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
						if(xfer_size > uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size;
						if(xfer_size){
							tru_usb_pc_receive(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
							return true;
						}
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

						TRU_MEMCPY(
							mute->cur,
							uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
							uac->pc->eps.epos[0].lxfer.chunk.total_xferred_size
						);

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							if(cn == 0xff){
								for(uint16_t i = 0; i < class_itf_ext->num_mutes - 1; i++){
									LOG("CH%u MUTE 0x%.2x\n", i, rd8_unaligned(mute->cur + i));
								}
								LOG("\n");
							}else{
								LOG("CH%u MUTE 0x%.2x\n", cn, mute->cur[0]);
							}
						#endif

						return true;
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
						return true;
						break;
				}
			}
			break;
		case TRU_USB_UAC_CS_FU_VOLUME:
			tru_usb_uac_volume_t *volume;
			class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			cn = uac->pc->setup.request.value & 0xff;
			volume = tru_usb_uac_find_volume(
				class_itf_ext->volumes,
				class_itf_ext->num_volumes,
				entity_id,
				cn
			);
			if(volume){
				switch(uac->pc->setup.request.code){
					case TRU_USB_UAC_REQ_SET_CUR: param_block = volume->cur; break;
					case TRU_USB_UAC_REQ_SET_MIN: param_block = volume->min; break;
					case TRU_USB_UAC_REQ_SET_MAX: param_block = volume->max; break;
					case TRU_USB_UAC_REQ_SET_RES: param_block = volume->res; break;
					default: return false;  // Other attributes is not supported
				}

				switch(uac->pc->eps.epos[0].control_stage){
					case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							switch(uac->pc->setup.request.code){
								case TRU_USB_UAC_REQ_SET_CUR: LOG("REQ: SET CUR VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								case TRU_USB_UAC_REQ_SET_MIN: LOG("REQ: SET MIN VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								case TRU_USB_UAC_REQ_SET_MAX: LOG("REQ: SET MAX VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								case TRU_USB_UAC_REQ_SET_RES: LOG("REQ: SET RES VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								default:
							}
						#endif

						xfer_size = (cn == 0xff) ? 2 * (class_itf_ext->num_volumes - 1) : 2;
						if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
						if(xfer_size > uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size;
						if(xfer_size){
							tru_usb_pc_receive(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
							return true;
						}
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

						TRU_MEMCPY(
							param_block,
							uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
							uac->pc->eps.epos[0].lxfer.chunk.total_xferred_size
						);

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							if(cn == 0xff){
								for(uint16_t i = 0; i < class_itf_ext->num_volumes - 1; i++){
									LOG("CH%u VOLUME 0x%.4x\n", i, rd8_unaligned((uint16_t *)param_block + i));
								}
								LOG("\n");
							}else{
								LOG("CH%u VOLUME 0x%.4x\n", cn, ((uint16_t *)param_block)[0]);
							}
						#endif

						return true;
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
						return true;
						break;
				}
			}
			break;
	}

	return false;
}

static bool tru_usb_uac_setup_get_fu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	tru_usb_uac_interface_ext_t *class_itf_ext;
	uint32_t xfer_size;
	uint8_t cs;  // Control Selector
	uint8_t cn;  // Channel Number
	void *param_block;

	if(!interface->alt_settings.curr_alt_setting->itf_ext) return false;

	cs = uac->pc->setup.request.value >> 8 & 0xff;
	switch(cs){
		case TRU_USB_UAC_CS_FU_MUTE:
			tru_usb_uac_mute_t *mute;
			class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			entity_id = uac->pc->setup.request.index >> 8 & 0xff;
			cn = uac->pc->setup.request.value & 0xff;
			mute = tru_usb_uac_find_mute(
				class_itf_ext->mutes,
				class_itf_ext->num_mutes,
				entity_id,
				cn
			);
			if(mute){
				switch(uac->pc->setup.request.code){
					case TRU_USB_UAC_REQ_GET_CUR: break;
					default: return false;  // Other attributes is not supported
				}

				switch(uac->pc->eps.epos[0].control_stage){
					case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: GET CUR MUTE LEN %u\n", uac->pc->setup.request.length);
							if(cn == 0xff){
								for(uint16_t i = 0; i < class_itf_ext->num_mutes - 1; i++){
									LOG("CH%u MUTE 0x%.2x\n", i, rd8_unaligned(mute->cur + i));
								}
								LOG("\n");
							}else{
								LOG("CH%u MUTE 0x%.2x\n", cn, mute->cur[0]);
							}
						#endif

						xfer_size = (cn == 0xff) ? class_itf_ext->num_mutes - 1 : 1;
						if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
						if(xfer_size > uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size;
						if(xfer_size){
							TRU_MEMCPY(uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, mute->cur, xfer_size);
							tru_usb_pc_transmit(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
							return true;
						}
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
						return true;
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
						return true;
						break;
				}
			}
			break;
		case TRU_USB_UAC_CS_FU_VOLUME:
			tru_usb_uac_volume_t *volume;
			class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			entity_id = uac->pc->setup.request.index >> 8 & 0xff;
			cn = uac->pc->setup.request.value & 0xff;
			volume = tru_usb_uac_find_volume(
				class_itf_ext->volumes,
				class_itf_ext->num_volumes,
				entity_id,
				cn
			);
			if(volume){
				switch(uac->pc->setup.request.code){
					case TRU_USB_UAC_REQ_GET_CUR: param_block = volume->cur; break;
					case TRU_USB_UAC_REQ_GET_MIN: param_block = volume->min; break;
					case TRU_USB_UAC_REQ_GET_MAX: param_block = volume->max; break;
					case TRU_USB_UAC_REQ_GET_RES: param_block = volume->res; break;
					default: return false;  // Other attributes is not supported
				}

				switch(uac->pc->eps.epos[0].control_stage){
					case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

						#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							switch(uac->pc->setup.request.code){
								case TRU_USB_UAC_REQ_GET_CUR: LOG("REQ: GET CUR VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								case TRU_USB_UAC_REQ_GET_MIN: LOG("REQ: GET MIN VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								case TRU_USB_UAC_REQ_GET_MAX: LOG("REQ: GET MAX VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								case TRU_USB_UAC_REQ_GET_RES: LOG("REQ: GET RES VOLUME LEN %u\n", uac->pc->setup.request.length); break;
								default:
							}
							if(cn == 0xff){
								for(uint16_t i = 0; i < class_itf_ext->num_mutes - 1; i++){
									LOG("CH%u VOLUME 0x%.2x\n", i, rd8_unaligned((uint16_t *)param_block + i));
								}
								LOG("\n");
							}else{
								LOG("CH%u VOLUME 0x%.2x\n", cn, ((uint16_t *)param_block)[0]);
							}
						#endif

						xfer_size = (cn == 0xff) ? 2 * (class_itf_ext->num_volumes - 1) : 2;
						if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
						if(xfer_size > uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size;
						if(xfer_size){
							TRU_MEMCPY(uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, param_block, xfer_size);
							tru_usb_pc_transmit(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
							return true;
						}
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
						return true;
						break;
					case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
						uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
						return true;
						break;
				}
			}
			break;
	}

	return false;
}

static bool tru_usb_uac_setup_get_pu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

static bool tru_usb_uac_setup_set_pu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

static bool tru_usb_uac_setup_get_eu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

static bool tru_usb_uac_setup_set_eu_req(tru_usb_uac_t *uac, uint8_t entity_id, tru_usb_interface_t *interface){
	// Not implemented
	return false;
}

/*
	UAC class specific setup request handler for request type:
		direction: host-to-device
		type     : class
		recipient: interface

	Handles anything specific for UAC class.
*/
static bool tru_usb_uac_setup_hd_cls_intf(tru_usb_uac_t *uac){
	bool handled = false;
	uint8_t interface_num;
	tru_usb_interface_t *interface;
	uint8_t entity_id;
	uint8_t entity_subtype;

	if(uac->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
		interface_num = uac->pc->setup.request.index & 0xff;
		interface = tru_usb_interfaces_find_interface(&uac->pc->configs.curr_config->interfaces, interface_num);
		if(interface){
			tru_usb_uac_interface_ext_t *class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			// Must be Audio Control interface
			if(class_itf_ext->is_audio_control){
				entity_id = uac->pc->setup.request.index >> 8 & 0xff;
				entity_subtype = tru_usb_uac_find_entity_subtype(uac, interface->alt_settings.curr_alt_setting, entity_id);
				switch(entity_subtype){
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_INPUT_TERMINAL:
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_OUTPUT_TERMINAL:
						handled = tru_usb_uac_setup_set_te_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_MIXER_UNIT:
						handled = tru_usb_uac_setup_set_mu_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_SELECTOR_UNIT:
						handled = tru_usb_uac_setup_set_su_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_FEATURE_UNIT:
						handled = tru_usb_uac_setup_set_fu_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_PROCESSING_UNIT:
						handled = tru_usb_uac_setup_set_pu_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_EXTENSION_UNIT:
						handled = tru_usb_uac_setup_set_eu_req(uac, entity_id, interface);
						break;
					default:
				}
			}
		}
	}

	return handled;
}

/*
	UAC class specific setup request handler for request type:
		direction: device-to-host
		type     : class
		recipient: interface

	Handles anything specific for UAC class.
*/
static bool tru_usb_uac_setup_dh_cls_intf(tru_usb_uac_t *uac){
	bool handled = false;
	uint8_t interface_num;
	tru_usb_interface_t *interface;
	uint8_t entity_id;
	uint8_t entity_subtype;

	if(uac->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
		interface_num = uac->pc->setup.request.index & 0xff;
		interface = tru_usb_interfaces_find_interface(&uac->pc->configs.curr_config->interfaces, interface_num);
		if(interface){
			tru_usb_uac_interface_ext_t *class_itf_ext = interface->alt_settings.curr_alt_setting->itf_ext;
			// Must be Audio Control interface
			if(class_itf_ext->is_audio_control){
				entity_id = uac->pc->setup.request.index >> 8 & 0xff;
				entity_subtype = tru_usb_uac_find_entity_subtype(uac, interface->alt_settings.curr_alt_setting, entity_id);
				switch(entity_subtype){
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_INPUT_TERMINAL:
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_OUTPUT_TERMINAL:
						handled = tru_usb_uac_setup_get_te_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_MIXER_UNIT:
						handled = tru_usb_uac_setup_get_mu_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_SELECTOR_UNIT:
						handled = tru_usb_uac_setup_get_su_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_FEATURE_UNIT:
						handled = tru_usb_uac_setup_get_fu_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_PROCESSING_UNIT:
						handled = tru_usb_uac_setup_get_pu_req(uac, entity_id, interface);
						break;
					case TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_EXTENSION_UNIT:
						handled = tru_usb_uac_setup_get_eu_req(uac, entity_id, interface);
						break;
					default:
				}
			}
		}
	}

	return handled;
}

static bool tru_usb_uac_setup_hd_cls_ep(tru_usb_uac_t *uac){
	bool handled = false;
	uint8_t ep;  // EndPoint address
	uint8_t cs;  // Control Selector
	tru_usb_uac_ep_ext_t *ep_ext;
	uint32_t xfer_size;

	if(uac->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
		ep = uac->pc->setup.request.index & 0xff;
		cs = uac->pc->setup.request.value >> 8 & 0xff;

		switch(cs){
			case TRU_USB_UAC_CS_EP_SAMPLING_FREQ:
				ep_ext = tru_usb_uac_find_class_ep_ext(uac, ep);
				if(ep_ext && ep_ext->samp_freq){
					uint8_t *samp_freq_attrib;

					switch(uac->pc->setup.request.code){
						case TRU_USB_UAC_REQ_SET_CUR: samp_freq_attrib = ep_ext->samp_freq->cur; break;
						case TRU_USB_UAC_REQ_SET_MIN: samp_freq_attrib = ep_ext->samp_freq->min; break;
						case TRU_USB_UAC_REQ_SET_MAX: samp_freq_attrib = ep_ext->samp_freq->max; break;
						case TRU_USB_UAC_REQ_SET_RES: samp_freq_attrib = ep_ext->samp_freq->res; break;
						default: return false;
					}

					switch(uac->pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

							#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
								switch(uac->pc->setup.request.code){
									case TRU_USB_UAC_REQ_SET_CUR: LOG("REQ: SET CUR SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
									case TRU_USB_UAC_REQ_SET_MIN: LOG("REQ: SET MIN SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
									case TRU_USB_UAC_REQ_SET_MAX: LOG("REQ: SET MAX SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
									case TRU_USB_UAC_REQ_SET_RES: LOG("REQ: SET RES SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
									default: return false;
								}
							#endif

							xfer_size = 3;
							if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
							if(xfer_size > uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf_size;
							if(xfer_size){
								tru_usb_pc_receive(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
								return true;
							}
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
							uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

							TRU_MEMCPY(
								samp_freq_attrib,
								uac->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
								uac->pc->eps.epos[0].lxfer.chunk.total_xferred_size
							);

							// EP is IN direction?
							if(ep & 0x80){
								tru_usb_pc_epi_t *epi = &uac->pc->eps.epis[ep & 0xf];
								tru_usb_uac_callback_epi_set_smpfrq(uac, epi);
							}else{
								tru_usb_pc_epo_t *epo = &uac->pc->eps.epos[ep & 0xf];
								tru_usb_uac_callback_epo_set_smpfrq(uac, epo);
							}

							#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
								LOG("SAMPFREQ %lu\n", rd32_unaligned(samp_freq_attrib));
							#endif

							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
				break;
		}
	}

	return handled;
}

static bool tru_usb_uac_setup_dh_cls_ep(tru_usb_uac_t *uac){
	bool handled = false;
	uint8_t ep;  // EndPoint address
	uint8_t cs;  // Control Selector
	tru_usb_uac_ep_ext_t *class_ep_ext;
	uint32_t xfer_size;

	if(uac->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
			ep = uac->pc->setup.request.index & 0xff;
			cs = uac->pc->setup.request.value >> 8 & 0xff;

			switch(cs){
				case TRU_USB_UAC_CS_EP_SAMPLING_FREQ:
					class_ep_ext = tru_usb_uac_find_class_ep_ext(uac, ep);
					if(class_ep_ext && class_ep_ext->samp_freq){
						uint8_t *samp_freq_attrib;

						switch(uac->pc->setup.request.code){
							case TRU_USB_UAC_REQ_SET_CUR: samp_freq_attrib = class_ep_ext->samp_freq->cur; break;
							case TRU_USB_UAC_REQ_SET_MIN: samp_freq_attrib = class_ep_ext->samp_freq->min; break;
							case TRU_USB_UAC_REQ_SET_MAX: samp_freq_attrib = class_ep_ext->samp_freq->max; break;
							case TRU_USB_UAC_REQ_SET_RES: samp_freq_attrib = class_ep_ext->samp_freq->res; break;
							default: return false;
						}

						switch(uac->pc->eps.epos[0].control_stage){
							case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
								uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

								#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
									switch(uac->pc->setup.request.code){
										case TRU_USB_UAC_REQ_SET_CUR: LOG("REQ: GET CUR SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
										case TRU_USB_UAC_REQ_SET_MIN: LOG("REQ: GET MIN SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
										case TRU_USB_UAC_REQ_SET_MAX: LOG("REQ: GET MAX SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
										case TRU_USB_UAC_REQ_SET_RES: LOG("REQ: GET RES SAMPFREQ LEN %u\n", uac->pc->setup.request.length); break;
										default: return false;
									}
								#endif

								xfer_size = 3;
								if(xfer_size > uac->pc->setup.request.length) xfer_size = uac->pc->setup.request.length;
								if(xfer_size > uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size) xfer_size = uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf_size;
								if(xfer_size){
									TRU_MEMCPY(uac->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, samp_freq_attrib, xfer_size);
									tru_usb_pc_transmit(uac->pc, 0, xfer_size, uac->pc->setup.request.length);
									return true;
								}
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
								uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
								uac->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
								return true;
								break;
						}
					}
					break;
			}
	}

	return handled;
}

/*
	Common UAC class setup request handler.
	This extends the standard setup requests and also adds new setup requests to support UAC devices.
*/
static bool tru_usb_uac_callback_setup(tru_usb_uac_t *uac){
	bool handled = false;

	// ========================================================================================================================
	// Process handled or unhandled setup request.  Implements UAC class extension requests, i.e. extend the standard requests.
	// ========================================================================================================================

	// What request direction?
	switch(uac->pc->setup.request.type.bits.dir){
		case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
			// What request type?
			switch(uac->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(uac->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(uac->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(uac->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
			// What request type?
			switch(uac->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(uac->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(uac->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(uac->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;
	}

	// =========================================================================
	// Process unhandled setup request.  Implements UAC class specific requests.
	// =========================================================================

	if(!uac->pc->setup.is_handled){
		// What request direction?
		switch(uac->pc->setup.request.type.bits.dir){
			case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
				// What request type?
				switch(uac->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(uac->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(uac->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: handled = handled | tru_usb_uac_setup_hd_cls_intf(uac); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: handled = handled | tru_usb_uac_setup_hd_cls_ep(uac); break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(uac->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;

			case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
				// What request type?
				switch(uac->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(uac->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(uac->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: handled = handled | tru_usb_uac_setup_dh_cls_intf(uac); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: handled = handled | tru_usb_uac_setup_dh_cls_ep(uac); break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(uac->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;
		}
	}

	if(user_callbacks.cb_setup) user_callbacks.cb_setup(uac);

	return handled;
}

static void tru_usb_uac_callback_reset(tru_usb_uac_t *uac){
	if(user_callbacks.cb_reset) user_callbacks.cb_reset(uac);
}

static void tru_usb_uac_callback_enumdone(tru_usb_uac_t *uac){
	if(user_callbacks.cb_enumdone) user_callbacks.cb_enumdone(uac);
}

void tru_usb_uac_callback_cfg_act(tru_usb_uac_t *uac, tru_usb_config_t *config, bool is_activated){
	if(is_activated){
		tru_usb_uac_config_ext_init(uac, config);
	}else{
		tru_usb_uac_config_ext_deinit(uac, config);
	}
	if(user_callbacks.cb_cfg_act) user_callbacks.cb_cfg_act(uac, config, is_activated);
}

void tru_usb_uac_callback_itf_act(tru_usb_uac_t *uac, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated){
	if(is_activated){
		tru_usb_uac_interface_ext_init(uac, alt_setting);
	}else{
		tru_usb_uac_interface_ext_deinit(uac, alt_setting);
	}
	if(user_callbacks.cb_itf_act) user_callbacks.cb_itf_act(uac, alt_setting, is_activated);
}

void tru_usb_uac_callback_epi_act(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep, bool is_activated){
	if(user_callbacks.cb_epi_act[ep->num]) user_callbacks.cb_epi_act[ep->num](uac, ep, is_activated);
}

void tru_usb_uac_callback_epo_act(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep, bool is_activated){
	if(user_callbacks.cb_epo_act[ep->num]) user_callbacks.cb_epo_act[ep->num](uac, ep, is_activated);
}

void tru_usb_uac_callback_epi_epcompl(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_epcompl[ep->num]) user_callbacks.cb_epi_epcompl[ep->num](uac, ep);
}

void tru_usb_uac_callback_epi_chunk(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_chunk[ep->num]) user_callbacks.cb_epi_chunk[ep->num](uac, ep);
}

void tru_usb_uac_callback_epi_compl(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_compl[ep->num]) user_callbacks.cb_epi_compl[ep->num](uac, ep);
}

void tru_usb_uac_callback_epo_epcompl(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_epcompl[ep->num]) user_callbacks.cb_epo_epcompl[ep->num](uac, ep);
}

void tru_usb_uac_callback_epo_chunk(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_chunk[ep->num]) user_callbacks.cb_epo_chunk[ep->num](uac, ep);
}

void tru_usb_uac_callback_epo_compl(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_compl[ep->num]) user_callbacks.cb_epo_compl[ep->num](uac, ep);
}

void tru_usb_uac_callback_sof(tru_usb_uac_t *uac){
	if(user_callbacks.cb_sof) user_callbacks.cb_sof(uac);
}

void tru_usb_uac_callback_epi_set_smpfrq(tru_usb_uac_t *uac, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_set_smpfrq) user_callbacks.cb_epi_set_smpfrq(uac, ep);
}

void tru_usb_uac_callback_epo_set_smpfrq(tru_usb_uac_t *uac, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_set_smpfrq) user_callbacks.cb_epo_set_smpfrq(uac, ep);
}
