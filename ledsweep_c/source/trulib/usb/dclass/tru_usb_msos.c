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

#include "usb/dclass/tru_usb_msos.h"
#include "tru_logger.h"

// Prototypes for callbacks
static void tru_usb_msos_callback_reset(tru_usb_msos_t *msos);
static void tru_usb_msos_callback_enumdone(tru_usb_msos_t *msos);
static bool tru_usb_msos_callback_setup(tru_usb_msos_t *msos);
static void tru_usb_msos_callback_cfg_act(tru_usb_msos_t *msos, tru_usb_config_t *config, bool is_activated);
static void tru_usb_msos_callback_itf_act(tru_usb_msos_t *msos, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
static void tru_usb_msos_callback_epi_act(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep, bool is_activated);
static void tru_usb_msos_callback_epo_act(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep, bool is_activated);
static void tru_usb_msos_callback_epi_epcompl(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep);
static void tru_usb_msos_callback_epi_chunk(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep);
static void tru_usb_msos_callback_epi_compl(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep);
static void tru_usb_msos_callback_epo_epcompl(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep);
static void tru_usb_msos_callback_epo_chunk(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep);
static void tru_usb_msos_callback_epo_compl(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep);
static void tru_usb_msos_callback_sof(tru_usb_msos_t *msos);

// External callbacks
static tru_usb_msos_callbacks_t user_callbacks = {
	.cb_reset       = NULL,
	.cb_enumdone    = NULL,
	.cb_setup       = NULL,
	.cb_cfg_act     = NULL,
	.cb_itf_act     = NULL,
	.cb_epi_act     = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_act     = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_epcompl = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_chunk   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_compl   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_epcompl = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_chunk   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_compl   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_sof         = NULL,
};

void tru_usb_msos_init(tru_usb_msos_t *msos, tru_usb_pc_t *pc, tru_usb_msos_pd_info_t *desc_info){
	msos->pc = pc;
	tru_usb_pc_register_dc(pc, msos, &msos->callbacks);
	msos->desc_info = desc_info;

	// Internal callbacks
	msos->callbacks.cb_reset = (tru_usb_dc_callback_reset_t)tru_usb_msos_callback_reset;
	msos->callbacks.cb_enumdone = (tru_usb_dc_callback_reset_t)tru_usb_msos_callback_enumdone;
	msos->callbacks.cb_setup = (tru_usb_dc_callback_setup_t)tru_usb_msos_callback_setup;
	msos->callbacks.cb_cfg_act = (tru_usb_dc_callback_cfg_act_t)tru_usb_msos_callback_cfg_act;
	msos->callbacks.cb_itf_act = (tru_usb_dc_callback_itf_act_t)tru_usb_msos_callback_itf_act;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epi_act[i] = (tru_usb_dc_callback_epi_act_t)tru_usb_msos_callback_epi_act;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epo_act[i] = (tru_usb_dc_callback_epo_act_t)tru_usb_msos_callback_epo_act;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epi_epcompl[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_msos_callback_epi_epcompl;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epi_chunk[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_msos_callback_epi_chunk;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epi_compl[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_msos_callback_epi_compl;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epo_epcompl[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_msos_callback_epo_epcompl;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epo_chunk[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_msos_callback_epo_chunk;
	for(uint8_t i = 0; i < 16; i++) msos->callbacks.cb_epo_compl[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_msos_callback_epo_compl;
	msos->callbacks.cb_sof = (tru_usb_dc_callback_sof_t)tru_usb_msos_callback_sof;

	tru_usb_msos_dev_ext_init(msos);
}

void tru_usb_msos_deinit(tru_usb_msos_t *msos){
	tru_usb_msos_dev_ext_deinit(msos);
}

void tru_usb_msos_dev_ext_init(tru_usb_msos_t *msos){
	msos->pc->dev.hs_dev_ext = msos->desc_info->hs_msos_dev_ext;
	msos->pc->dev.fs_dev_ext = msos->desc_info->fs_msos_dev_ext;
	msos->pc->dev.ls_dev_ext = msos->desc_info->ls_msos_dev_ext;
}

void tru_usb_msos_dev_ext_deinit(tru_usb_msos_t *msos){
	msos->pc->dev.hs_dev_ext = NULL;
	msos->pc->dev.fs_dev_ext = NULL;
	msos->pc->dev.ls_dev_ext = NULL;
}

void tru_usb_msos_config_ext_init(tru_usb_msos_t *msos, tru_usb_config_t *config){
	tru_usb_interface_t * interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_msos_interface_ext_init(msos, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

void tru_usb_msos_config_ext_deinit(tru_usb_msos_t *msos, tru_usb_config_t *config){
	tru_usb_interface_t *interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_msos_interface_ext_deinit(msos, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

tru_usb_msos_interface_ext_t *tru_usb_msos_find_class_itf_ext(tru_usb_msos_t *msos, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num){
	tru_usb_msos_interface_ext_t *itf_ext;
	uint16_t num_entries = 0;

	switch(msos->pc->speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:
			if(msos->desc_info->hs_msos_interface_exts){
				itf_ext = msos->desc_info->hs_msos_interface_exts;
				num_entries = msos->desc_info->hs_msos_interface_exts_num_entries;
			}else if(msos->desc_info->fs_msos_interface_exts){
				itf_ext = msos->desc_info->fs_msos_interface_exts;
				num_entries = msos->desc_info->fs_msos_interface_exts_num_entries;
			}else if(msos->desc_info->ls_msos_interface_exts){
				itf_ext = msos->desc_info->ls_msos_interface_exts;
				num_entries = msos->desc_info->ls_msos_interface_exts_num_entries;
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
			if(msos->desc_info->fs_msos_interface_exts){
				itf_ext = msos->desc_info->fs_msos_interface_exts;
				num_entries = msos->desc_info->fs_msos_interface_exts_num_entries;
			}else if(msos->desc_info->ls_msos_interface_exts){
				itf_ext = msos->desc_info->ls_msos_interface_exts;
				num_entries = msos->desc_info->ls_msos_interface_exts_num_entries;
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
			if(msos->desc_info->ls_msos_interface_exts){
				itf_ext = msos->desc_info->ls_msos_interface_exts;
				num_entries = msos->desc_info->ls_msos_interface_exts_num_entries;
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

void tru_usb_msos_interface_ext_init(tru_usb_msos_t *msos, tru_usb_interface_alt_setting_t *alt_setting){
	alt_setting->itf_ext = tru_usb_msos_find_class_itf_ext(msos, alt_setting->config_num, alt_setting->interface_num, alt_setting->alt_setting_num);
}

void tru_usb_msos_interface_ext_deinit(tru_usb_msos_t *msos, tru_usb_interface_alt_setting_t *alt_setting){
}

void tru_usb_msos_register_cb_reset(tru_usb_msos_t *msos, tru_usb_msos_callback_reset_t cb_func){
	user_callbacks.cb_reset = cb_func;
}

void tru_usb_msos_register_cb_enumdone(tru_usb_msos_t *msos, tru_usb_msos_callback_enumdone_t cb_func){
	user_callbacks.cb_enumdone = cb_func;
}

void tru_usb_msos_register_cb_setup(tru_usb_msos_t *msos, tru_usb_msos_callback_setup_t cb_func){
	user_callbacks.cb_setup = cb_func;
}

void tru_usb_msos_register_cb_cfg_act(tru_usb_msos_t *msos, tru_usb_msos_callback_cfg_act_t cb_func){
	user_callbacks.cb_cfg_act = cb_func;
}

void tru_usb_msos_register_cb_itf_act(tru_usb_msos_t *msos, tru_usb_msos_callback_itf_act_t cb_func){
	user_callbacks.cb_itf_act = cb_func;
}

void tru_usb_msos_register_cb_epi_act(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_act_t cb_func){
	user_callbacks.cb_epi_act[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_epo_act(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_act_t cb_func){
	user_callbacks.cb_epo_act[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_epi_epcompl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_epcompl[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_epi_chunk(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_chunk[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_epi_compl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_compl[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_epo_epcompl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_epcompl[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_epo_chunk(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_chunk[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_epo_compl(tru_usb_msos_t *msos, uint8_t ep_num, tru_usb_msos_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_compl[ep_num] = cb_func;
}

void tru_usb_msos_register_cb_sof(tru_usb_msos_t *msos, tru_usb_msos_callback_sof_t cb_func){
	user_callbacks.cb_sof = cb_func;
}

/*
	MSOS vendor setup request handler for request type:
		direction: device-to-host
		type     : vendor
		recipient: device

	Handles anything specific for MSOS class.
*/
static bool tru_usb_msos_setup_dh_ven_dev(tru_usb_msos_t *msos){
	uint8_t req_page_num;
	tru_usb_msos_dev_ext_t *class_dev_ext;
	uint32_t feature_section_size;
	uint32_t feature_offset_to_copy;
	uint32_t feature_hdr_size_to_copy;
	uint32_t feature_section_size_to_copy;
	uint32_t xfer_size;

	// Process the setup request
	switch(msos->pc->setup.request.code){
		case TRU_USB_MSOS_VENDOR_CODE:
			if(msos->pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || msos->pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || msos->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				switch(msos->pc->setup.request.index){
					case TRU_USB_MSOS_ECID_FEATURE_INDEX:
						class_dev_ext = TO_MSOS_DEV_EXT_PTR(msos->pc->dev.curr_spd_dev_ext);
						if(class_dev_ext){
							switch(msos->pc->eps.epos[0].control_stage){
								case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
									msos->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1U
									LOG("REQ: MSOS_ECID_FEATURE_INDEX LEN %u\n", msos->pc->setup.request.length);
#endif

									req_page_num = msos->pc->setup.request.value & 0xff;
									feature_offset_to_copy = 65535 * req_page_num;
									feature_section_size = class_dev_ext->ecid_feature->hdr->length - sizeof(tru_usb_msos_ecid_feature_hdr_desc_t);

									xfer_size = (feature_offset_to_copy < class_dev_ext->ecid_feature->hdr->length) ? (msos->pc->setup.request.length > class_dev_ext->ecid_feature->hdr->length) ? class_dev_ext->ecid_feature->hdr->length : msos->pc->setup.request.length : 0;
									// There is something to copy?
									if(xfer_size){
										// ========================================
										// Copy descriptor bytes to transfer buffer
										// ========================================

										// Offset is within the header?
										if(feature_offset_to_copy <= sizeof(tru_usb_msos_ecid_feature_hdr_desc_t)){
											feature_hdr_size_to_copy = sizeof(tru_usb_msos_ecid_feature_hdr_desc_t) - feature_offset_to_copy;  // Calculate max number of header bytes to copy

											// Max number of header bytes is too big to copy?
											if(feature_hdr_size_to_copy > xfer_size){
												// =================
												// Copy header bytes
												// =================

												feature_hdr_size_to_copy = xfer_size;  // Re-calculate max number of header bytes to copy
												TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, class_dev_ext->ecid_feature->hdr, feature_hdr_size_to_copy);
											}else{
												// =================
												// Copy header bytes
												// =================

												TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, class_dev_ext->ecid_feature->hdr, feature_hdr_size_to_copy);

												// ==================
												// Copy section bytes
												// ==================

												feature_section_size_to_copy = xfer_size - feature_hdr_size_to_copy;  // Calculate max number of section bytes to copy

												// There is something to copy?
												if(feature_section_size_to_copy){
													// Max number of section bytes is too big to copy?
													if(feature_section_size_to_copy > feature_section_size){
														feature_section_size_to_copy = feature_section_size;  // Re-calculate max number of section bytes to copy
													}
													TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf + feature_hdr_size_to_copy, class_dev_ext->ecid_feature->sections, feature_section_size_to_copy);
												}
											}
										}else{
											// ==================
											// Copy section bytes
											// ==================

											feature_offset_to_copy -= sizeof(tru_usb_msos_ecid_feature_hdr_desc_t);  // Remove header offset to give section offset
											TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, class_dev_ext->ecid_feature->sections + feature_offset_to_copy, xfer_size);
										}
									}
									tru_usb_pc_transmit(msos->pc, 0, xfer_size, msos->pc->setup.request.length);

									return true;
									break;
								case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
									msos->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
									return true;
									break;
								case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
									msos->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
									return true;
									break;
							}
						}
						break;
				}
			}
			break;
	}

	return false;
}

/*
	MSOS vendor setup request handler for request type:
		direction: device-to-host
		type     : vendor
		recipient: interface

	Handles anything specific for MSOS class.
*/
static bool tru_usb_msos_setup_dh_ven_intf(tru_usb_msos_t *msos){
	uint8_t req_interface_num;
	uint8_t req_page_num;
	tru_usb_interface_t *req_interface;
	tru_usb_msos_interface_ext_t *class_interface_ext;
	tru_usb_msos_dev_ext_t *class_dev_ext;
	tru_usb_msos_ep_feature_t *ep_feature = NULL;
	uint32_t feature_section_size;
	uint32_t feature_offset_to_copy;
	uint32_t feature_hdr_size_to_copy;
	uint32_t feature_section_size_to_copy;
	uint32_t xfer_size;

	// Process the setup request
	switch(msos->pc->setup.request.code){
		case TRU_USB_MSOS_VENDOR_CODE:
			switch(msos->pc->setup.request.index){
				case TRU_USB_MSOS_EP_FEATURE_INDEX:
					if(msos->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
						// Get EP feature from interface level

						req_interface_num = (uint8_t)(msos->pc->setup.request.value >> 8 & 0xff);
						req_interface = tru_usb_interfaces_find_interface(&msos->pc->configs.curr_config->interfaces, req_interface_num);
						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							if(class_interface_ext){
								ep_feature = class_interface_ext->ep_feature;
							}
						}
					}else{
						// Get EP feature from device level

						class_dev_ext = TO_MSOS_DEV_EXT_PTR(msos->pc->dev.curr_spd_dev_ext);
						if(class_dev_ext){
							ep_feature = class_dev_ext->ep_feature;
						}
					}

					if(ep_feature){
						switch(msos->pc->eps.epos[0].control_stage){
							case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
								msos->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1U
								LOG("REQ: MSOS_EP_FEATURE_INDEX LEN %u\n", msos->pc->setup.request.length);
#endif

								req_page_num = msos->pc->setup.request.value & 0xff;
								feature_offset_to_copy = 65535 * req_page_num;
								feature_section_size = ep_feature->hdr->length - sizeof(tru_usb_msos_ep_feature_hdr_desc_t);

								xfer_size = (feature_offset_to_copy < ep_feature->hdr->length) ? (msos->pc->setup.request.length > ep_feature->hdr->length) ? ep_feature->hdr->length : msos->pc->setup.request.length : 0;
								// There is something to copy?
								if(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].xfer_size){
									// ========================================
									// Copy descriptor bytes to transfer buffer
									// ========================================

									// Offset is within the header?
									if(feature_offset_to_copy <= sizeof(tru_usb_msos_ep_feature_hdr_desc_t)){
										feature_hdr_size_to_copy = sizeof(tru_usb_msos_ep_feature_hdr_desc_t) - feature_offset_to_copy;  // Calculate max number of header bytes to copy

										// Max number of header bytes is too big to copy?
										if(feature_hdr_size_to_copy > xfer_size){
											// =================
											// Copy header bytes
											// =================

											feature_hdr_size_to_copy = xfer_size;  // Re-calculate max number of header bytes to copy
											TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, &ep_feature->hdr, feature_hdr_size_to_copy);
										}else{
											// =================
											// Copy header bytes
											// =================

											TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, &ep_feature->hdr, feature_hdr_size_to_copy);

											// ==================
											// Copy section bytes
											// ==================

											feature_section_size_to_copy = xfer_size - feature_hdr_size_to_copy;  // Calculate max number of section bytes to copy

											// There is something to copy?
											if(feature_section_size_to_copy){
												// Max number of section bytes is too big to copy?
												if(feature_section_size_to_copy > feature_section_size){
													feature_section_size_to_copy = feature_section_size;  // Re-calculate max number of section bytes to copy
												}
												TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf + feature_hdr_size_to_copy, ep_feature->sections, feature_section_size_to_copy);
											}
										}
									}else{
										// ==================
										// Copy section bytes
										// ==================

										feature_offset_to_copy -= sizeof(tru_usb_msos_ep_feature_hdr_desc_t);  // Remove header offset to give section offset
										TRU_MEMCPY(msos->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, ep_feature->sections + feature_offset_to_copy, msos->pc->eps.epis[0].lxfer.chunk.buffers[0].xfer_size);  // Copy to 32 bit aligned buffer
									}
								}
								tru_usb_pc_transmit(msos->pc, 0, xfer_size, msos->pc->setup.request.length);

								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
								msos->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
								msos->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
								return true;
								break;
						}
					}
					break;
			}
			break;
	}

	return false;
}

static void tru_usb_msos_callback_reset(tru_usb_msos_t *msos){
	if(user_callbacks.cb_reset) user_callbacks.cb_reset(msos);
}

static void tru_usb_msos_callback_enumdone(tru_usb_msos_t *msos){
	if(user_callbacks.cb_enumdone) user_callbacks.cb_enumdone(msos);
}

/*
	Common MSOS class setup request handler.
	This extends the standard setup requests and also adds new setup requests to support MSOS devices.
*/
static bool tru_usb_msos_callback_setup(tru_usb_msos_t *msos){
	bool success = false;

	// =========================================================================================================================
	// Process handled or unhandled setup request.  Implements MSOS class extension requests, i.e. extend the standard requests.
	// =========================================================================================================================

	// What request direction?
	switch(msos->pc->setup.request.type.bits.dir){
		case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
			// What request type?
			switch(msos->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(msos->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(msos->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(msos->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
			// What request type?
			switch(msos->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(msos->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(msos->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(msos->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;
	}

	// ==========================================================================
	// Process unhandled setup request.  Implements MSOS class specific requests.
	// ==========================================================================

	if(!msos->pc->setup.is_handled){
		// What request direction?
		switch(msos->pc->setup.request.type.bits.dir){
			case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
				// What request type?
				switch(msos->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(msos->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(msos->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(msos->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;

			case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
				// What request type?
				switch(msos->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(msos->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(msos->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(msos->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: success = success | tru_usb_msos_setup_dh_ven_dev(msos); break;
							case TRU_USB_SETUP_RCP_INTERFACE: success = success | tru_usb_msos_setup_dh_ven_intf(msos); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;
		}
	}

	if(user_callbacks.cb_setup) user_callbacks.cb_setup(msos);

	return success;
}

void tru_usb_msos_callback_cfg_act(tru_usb_msos_t *msos, tru_usb_config_t *config, bool is_activated){
	if(is_activated){
		tru_usb_msos_config_ext_init(msos, config);
	}else{
		tru_usb_msos_config_ext_deinit(msos, config);
	}
	if(user_callbacks.cb_cfg_act) user_callbacks.cb_cfg_act(msos, config, is_activated);
}

void tru_usb_msos_callback_itf_act(tru_usb_msos_t *msos, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated){
	if(is_activated){
		tru_usb_msos_interface_ext_init(msos, alt_setting);
	}else{
		tru_usb_msos_interface_ext_deinit(msos, alt_setting);
	}
	if(user_callbacks.cb_itf_act) user_callbacks.cb_itf_act(msos, alt_setting, is_activated);
}

void tru_usb_msos_callback_epi_act(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep, bool is_activated){
	if(user_callbacks.cb_epi_act[ep->num]) user_callbacks.cb_epi_act[ep->num](msos, ep, is_activated);
}

void tru_usb_msos_callback_epo_act(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep, bool is_activated){
	if(user_callbacks.cb_epo_act[ep->num]) user_callbacks.cb_epo_act[ep->num](msos, ep, is_activated);
}

void tru_usb_msos_callback_epi_epcompl(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_epcompl[ep->num]) user_callbacks.cb_epi_epcompl[ep->num](msos, ep);
}

void tru_usb_msos_callback_epi_chunk(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_chunk[ep->num]) user_callbacks.cb_epi_chunk[ep->num](msos, ep);
}

void tru_usb_msos_callback_epi_compl(tru_usb_msos_t *msos, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_compl[ep->num]) user_callbacks.cb_epi_compl[ep->num](msos, ep);
}

void tru_usb_msos_callback_epo_epcompl(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_epcompl[ep->num]) user_callbacks.cb_epo_epcompl[ep->num](msos, ep);
}

void tru_usb_msos_callback_epo_chunk(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_chunk[ep->num]) user_callbacks.cb_epo_chunk[ep->num](msos, ep);
}

void tru_usb_msos_callback_epo_compl(tru_usb_msos_t *msos, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_compl[ep->num]) user_callbacks.cb_epo_compl[ep->num](msos, ep);
}

void tru_usb_msos_callback_sof(tru_usb_msos_t *msos){
	if(user_callbacks.cb_sof) user_callbacks.cb_sof(msos);
}
