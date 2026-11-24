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
*/

#include "usb/dclass/tru_usb_hid.h"
#include "tru_logger.h"
#include <stdlib.h>

// Prototypes for callbacks
static void tru_usb_hid_callback_reset(tru_usb_hid_t *hid);
static void tru_usb_hid_callback_enumdone(tru_usb_hid_t *hid);
static bool tru_usb_hid_callback_setup(tru_usb_hid_t *hid);
static void tru_usb_hid_callback_cfg_act(tru_usb_hid_t *hid, tru_usb_config_t *config, bool is_activated);
static void tru_usb_hid_callback_itf_act(tru_usb_hid_t *hid, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated);
static void tru_usb_hid_callback_epi_act(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep, bool is_activated);
static void tru_usb_hid_callback_epo_act(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep, bool is_activated);
static void tru_usb_hid_callback_epi_chunk(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep);
static void tru_usb_hid_callback_epi_compl(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep);
static void tru_usb_hid_callback_epo_chunk(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep);
static void tru_usb_hid_callback_epo_compl(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep);
static void tru_usb_hid_callback_sof(tru_usb_hid_t *hid);

// External callbacks
static tru_usb_hid_callbacks_t user_callbacks = {
	.cb_reset       = NULL,
	.cb_enumdone    = NULL,
	.cb_setup       = NULL,
	.cb_cfg_act     = NULL,
	.cb_itf_act     = NULL,
	.cb_epi_act     = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_act     = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_chunk   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epi_compl   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_chunk   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_epo_compl   = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	.cb_sof         = NULL,
	.cb_set_report  = NULL,
};

void tru_usb_hid_init(tru_usb_hid_t *hid, tru_usb_pc_t *pc, tru_usb_hid_pd_info_t *desc_info){
	hid->pc = pc;
	tru_usb_pc_register_dc(pc, hid, &hid->callbacks);
	hid->desc_info = desc_info;

	// Internal callbacks
	hid->callbacks.cb_reset = (tru_usb_dc_callback_reset_t)tru_usb_hid_callback_reset;
	hid->callbacks.cb_enumdone = (tru_usb_dc_callback_reset_t)tru_usb_hid_callback_enumdone;
	hid->callbacks.cb_setup = (tru_usb_dc_callback_setup_t)tru_usb_hid_callback_setup;
	hid->callbacks.cb_cfg_act =  (tru_usb_dc_callback_cfg_act_t)tru_usb_hid_callback_cfg_act;
	hid->callbacks.cb_itf_act =  (tru_usb_dc_callback_itf_act_t)tru_usb_hid_callback_itf_act;
	for(uint8_t i = 0; i < 16; i++) hid->callbacks.cb_epi_act[i] = (tru_usb_dc_callback_epi_act_t)tru_usb_hid_callback_epi_act;
	for(uint8_t i = 0; i < 16; i++) hid->callbacks.cb_epo_act[i] = (tru_usb_dc_callback_epo_act_t)tru_usb_hid_callback_epo_act;
	for(uint8_t i = 0; i < 16; i++) hid->callbacks.cb_epi_chunk[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_hid_callback_epi_chunk;
	for(uint8_t i = 0; i < 16; i++) hid->callbacks.cb_epi_compl[i] = (tru_usb_dc_callback_epi_compl_t)tru_usb_hid_callback_epi_compl;
	for(uint8_t i = 0; i < 16; i++) hid->callbacks.cb_epo_chunk[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_hid_callback_epo_chunk;
	for(uint8_t i = 0; i < 16; i++) hid->callbacks.cb_epo_compl[i] = (tru_usb_dc_callback_epo_compl_t)tru_usb_hid_callback_epo_compl;
	hid->callbacks.cb_sof =  (tru_usb_dc_callback_sof_t)tru_usb_hid_callback_sof;

	//tru_usb_hid_class_ext_init(hid);
}

void tru_usb_hid_deinit(tru_usb_hid_t *hid){
	//tru_usb_hid_class_ext_deinit(hid);
}

void tru_usb_hid_class_ext_init(tru_usb_hid_t *hid){
	uint16_t i;
	tru_usb_config_t *config;
	tru_usb_interface_alt_setting_t *alt_setting;
	tru_usb_hid_interface_ext_t *hid_interface_ext;

	// If high speed configuration exists
	if(hid->pc->configs.hs_configs.num_elements){
		hid_interface_ext = hid->desc_info->hs_hid_interface_exts;
		for(i = 0; i < hid->desc_info->hs_hid_interface_exts_num_entries; i++){
			config = tru_usb_configs_find_config(&hid->pc->configs.hs_configs, hid_interface_ext->ext_hdr.config_num);
			if(config){
				alt_setting = tru_usb_interfaces_find_interface_alt_setting(&config->interfaces, hid_interface_ext->ext_hdr.interface_num, hid_interface_ext->ext_hdr.alt_setting_num);
				if(alt_setting){
					tru_usb_hid_interface_ext_init(hid, alt_setting);
				}
			}
			hid_interface_ext++;
		}
	}

	// If full speed configuration exists
	if(hid->pc->configs.fs_configs.num_elements){
		hid_interface_ext = hid->desc_info->fs_hid_interface_exts;
		for(i = 0; i < hid->desc_info->fs_hid_interface_exts_num_entries; i++){
			config = tru_usb_configs_find_config(&hid->pc->configs.fs_configs, hid_interface_ext->ext_hdr.config_num);
			if(config){
				alt_setting = tru_usb_interfaces_find_interface_alt_setting(&config->interfaces, hid_interface_ext->ext_hdr.interface_num, hid_interface_ext->ext_hdr.alt_setting_num);
				if(alt_setting){
					tru_usb_hid_interface_ext_init(hid, alt_setting);
				}
			}
		}
	}

	// If low speed configuration exists
	if(hid->pc->configs.ls_configs.num_elements){
		hid_interface_ext = hid->desc_info->ls_hid_interface_exts;
		for(i = 0; i < hid->desc_info->ls_hid_interface_exts_num_entries; i++){
			config = tru_usb_configs_find_config(&hid->pc->configs.ls_configs, hid_interface_ext->ext_hdr.config_num);
			if(config){
				alt_setting = tru_usb_interfaces_find_interface_alt_setting(&config->interfaces, hid_interface_ext->ext_hdr.interface_num, hid_interface_ext->ext_hdr.alt_setting_num);
				if(alt_setting){
					tru_usb_hid_interface_ext_init(hid, alt_setting);
				}
			}
		}
	}
}

void tru_usb_hid_class_ext_deinit(tru_usb_hid_t *hid){
	uint16_t config_index;
	uint16_t interface_index;
	uint16_t alt_setting_index;
	tru_usb_interface_alt_setting_t *alt_setting;

	// If high speed configuration exists
	if(hid->pc->configs.hs_configs.num_elements){
		for(config_index = 0; config_index < hid->pc->configs.hs_configs.num_elements; config_index++){
			for(interface_index = 0; interface_index < hid->pc->configs.hs_configs.items[config_index].interfaces.num_interfaces; interface_index++){
				for(alt_setting_index = 0; alt_setting_index < hid->pc->configs.hs_configs.items[config_index].interfaces.items[interface_index].alt_settings.num_alt_settings; alt_setting_index++){
					alt_setting = hid->pc->configs.hs_configs.items[config_index].interfaces.items[interface_index].alt_settings.items + alt_setting_index;
					tru_usb_hid_interface_ext_deinit(hid, alt_setting);
					alt_setting->itf_ext = NULL;
				}
			}
		}
	}

	// If full speed configuration exists
	if(hid->pc->configs.fs_configs.num_elements){
		for(config_index = 0; config_index < hid->pc->configs.fs_configs.num_elements; config_index++){
			for(interface_index = 0; interface_index < hid->pc->configs.fs_configs.items[config_index].interfaces.num_interfaces; interface_index++){
				for(alt_setting_index = 0; alt_setting_index < hid->pc->configs.fs_configs.items[config_index].interfaces.items[interface_index].alt_settings.num_alt_settings; alt_setting_index++){
					alt_setting = hid->pc->configs.fs_configs.items[config_index].interfaces.items[interface_index].alt_settings.items + alt_setting_index;
					tru_usb_hid_interface_ext_deinit(hid, alt_setting);
					alt_setting->itf_ext = NULL;
				}
			}
		}
	}

	// If low speed configuration exists
	if(hid->pc->configs.ls_configs.num_elements){
		for(config_index = 0; config_index < hid->pc->configs.ls_configs.num_elements; config_index++){
			for(interface_index = 0; interface_index < hid->pc->configs.ls_configs.items[config_index].interfaces.num_interfaces; interface_index++){
				for(alt_setting_index = 0; alt_setting_index < hid->pc->configs.ls_configs.items[config_index].interfaces.items[interface_index].alt_settings.num_alt_settings; alt_setting_index++){
					alt_setting = hid->pc->configs.ls_configs.items[config_index].interfaces.items[interface_index].alt_settings.items + alt_setting_index;
					tru_usb_hid_interface_ext_deinit(hid, alt_setting);
					alt_setting->itf_ext = NULL;
				}
			}
		}
	}
}

void tru_usb_hid_config_ext_init(tru_usb_hid_t *hid, tru_usb_config_t *config){
	tru_usb_interface_t * interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_hid_interface_ext_init(hid, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

void tru_usb_hid_config_ext_deinit(tru_usb_hid_t *hid, tru_usb_config_t *config){
	tru_usb_interface_t *interface = config->interfaces.items;

	for(uint16_t i = 0; i < config->interfaces.num_interfaces; i++){
		tru_usb_hid_interface_ext_deinit(hid, interface->alt_settings.curr_alt_setting);
		interface++;
	}
}

tru_usb_hid_interface_ext_t *tru_usb_hid_find_class_itf_ext(tru_usb_hid_t *hid, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num){
	tru_usb_hid_interface_ext_t *itf_ext;
	uint16_t num_entries = 0;

	switch(hid->pc->speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:
			if(hid->desc_info->hs_hid_interface_exts){
				itf_ext = hid->desc_info->hs_hid_interface_exts;
				num_entries = hid->desc_info->hs_hid_interface_exts_num_entries;
			}else if(hid->desc_info->fs_hid_interface_exts){
				itf_ext = hid->desc_info->fs_hid_interface_exts;
				num_entries = hid->desc_info->fs_hid_interface_exts_num_entries;
			}else if(hid->desc_info->ls_hid_interface_exts){
				itf_ext = hid->desc_info->ls_hid_interface_exts;
				num_entries = hid->desc_info->ls_hid_interface_exts_num_entries;
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
			if(hid->desc_info->fs_hid_interface_exts){
				itf_ext = hid->desc_info->fs_hid_interface_exts;
				num_entries = hid->desc_info->fs_hid_interface_exts_num_entries;
			}else if(hid->desc_info->ls_hid_interface_exts){
				itf_ext = hid->desc_info->ls_hid_interface_exts;
				num_entries = hid->desc_info->ls_hid_interface_exts_num_entries;
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
			if(hid->desc_info->ls_hid_interface_exts){
				itf_ext = hid->desc_info->ls_hid_interface_exts;
				num_entries = hid->desc_info->ls_hid_interface_exts_num_entries;
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

void tru_usb_hid_interface_ext_init(tru_usb_hid_t *hid, tru_usb_interface_alt_setting_t *alt_setting){
	alt_setting->itf_ext = tru_usb_hid_find_class_itf_ext(hid, alt_setting->config_num, alt_setting->interface_num, alt_setting->alt_setting_num);

	if(alt_setting->itf_ext == NULL) return;

	tru_usb_hid_interface_ext_t *itf_ext = alt_setting->itf_ext;

	itf_ext->reports_info_set->curr_input_index = 0;
	itf_ext->reports_info_set->curr_output_index = 0;
	itf_ext->reports_info_set->epin_num = 0;
	itf_ext->reports_info_set->epout_num = 0;

	for(uint8_t i = 0; i < alt_setting->eps.num_endpoints; i++){
		// Endpoint direction: 0x80 = IN, 0x00 = OUT
		if(alt_setting->eps.items[i].desc[2] & 0x80){
			itf_ext->reports_info_set->epin_num = alt_setting->eps.items[i].desc[2] & 0x0f;
		}else{
			itf_ext->reports_info_set->epout_num = alt_setting->eps.items[i].desc[2] & 0x0f;
		}
	}

	tru_usb_hid_report_info_t *input_report_info = itf_ext->reports_info_set->inputs;
	for(uint16_t i = 0; i < itf_ext->reports_info_set->num_input_reports; i++){
		if(input_report_info->size){
			w8_unaligned(input_report_info->report, input_report_info->id);  // Preset the first byte with the report ID
			//if(epin_num) intf_ext->ep_report_info[epin_num].report_info = &report_info_set->input;
		}
		input_report_info++;
	}

	tru_usb_hid_report_info_t *output_report_info = itf_ext->reports_info_set->outputs;
	for(uint16_t i = 0; i < itf_ext->reports_info_set->num_output_reports; i++){
		if(output_report_info->size){
			w8_unaligned(output_report_info->report, output_report_info->id);  // Preset the first byte with the report ID
			//if(epout_num) intf_ext->ep_report_info[epin_num].report_info = &report_info_set->output;
		}
		output_report_info++;
	}

	tru_usb_hid_report_info_t *feature_report_info = itf_ext->reports_info_set->features;
	for(uint16_t i = 0; i < itf_ext->reports_info_set->num_feature_reports; i++){
		if(feature_report_info->size){
			w8_unaligned(feature_report_info->report, feature_report_info->id);  // Preset the first byte with the report ID
		}
		feature_report_info++;
	}
}

void tru_usb_hid_interface_ext_deinit(tru_usb_hid_t *hid, tru_usb_interface_alt_setting_t *alt_setting){
}

void tru_usb_hid_register_cb_reset(tru_usb_hid_t *hid, tru_usb_hid_callback_reset_t cb_func){
	user_callbacks.cb_reset = cb_func;
}

void tru_usb_hid_register_cb_enumdone(tru_usb_hid_t *hid, tru_usb_hid_callback_enumdone_t cb_func){
	user_callbacks.cb_enumdone = cb_func;
}

void tru_usb_hid_register_cb_setup(tru_usb_hid_t *hid, tru_usb_hid_callback_setup_t cb_func){
	user_callbacks.cb_setup = cb_func;
}

void tru_usb_hid_register_cb_cfg_act(tru_usb_hid_t *hid, tru_usb_hid_callback_cfg_act_t cb_func){
	user_callbacks.cb_cfg_act = cb_func;
}

void tru_usb_hid_register_cb_itf_act(tru_usb_hid_t *hid, tru_usb_hid_callback_itf_act_t cb_func){
	user_callbacks.cb_itf_act = cb_func;
}

void tru_usb_hid_register_cb_epi_act(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epi_act_t cb_func){
	user_callbacks.cb_epi_act[ep_num] = cb_func;
}

void tru_usb_hid_register_cb_epo_act(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epo_act_t cb_func){
	user_callbacks.cb_epo_act[ep_num] = cb_func;
}

void tru_usb_hid_register_cb_epi_chunk(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_chunk[ep_num] = cb_func;
}

void tru_usb_hid_register_cb_epi_compl(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epi_compl_t cb_func){
	user_callbacks.cb_epi_compl[ep_num] = cb_func;
}

void tru_usb_hid_register_cb_epo_chunk(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_chunk[ep_num] = cb_func;
}

void tru_usb_hid_register_cb_epo_compl(tru_usb_hid_t *hid, uint8_t ep_num, tru_usb_hid_callback_epo_compl_t cb_func){
	user_callbacks.cb_epo_compl[ep_num] = cb_func;
}

void tru_usb_hid_register_cb_sof(tru_usb_hid_t *hid, tru_usb_hid_callback_sof_t cb_func){
	user_callbacks.cb_sof = cb_func;
}

void tru_usb_hid_register_cb_set_report(tru_usb_hid_t *hid, tru_usb_hid_callback_set_report_t cb_func){
	user_callbacks.cb_set_report = cb_func;
}

/*
	Translate short item data size code to value (to the actual size).
*/
uint32_t tru_usb_hid_parse_short_item_int(uint8_t *item, uint8_t prefix_data_size){
	uint32_t int_value;

	switch(prefix_data_size){
		case 1:
			int_value = item[1];
			break;
		case 2:
			int_value = item[2];
			int_value = (int_value << 8) | item[1];
			break;
		case 4:
			int_value = item[4];
			int_value = (int_value << 8) | item[3];
			int_value = (int_value << 8) | item[2];
			int_value = (int_value << 8) | item[1];
			break;
		default:
			int_value = 0;
	}

	return int_value;
}

/*
	Process USB HID report descriptor to discover the report data sizes of each report.

	Assumes the report descriptor is valid, complete and compliant so only minimal validation, e.g. minimal buffer overflow
	checks, etc.

	The USB HID 1.11 specification describes two item types: short items and long items - but because standard HID only
	use short items, all long items are ignored.

	Notes:
		USB HID device communicates by sending/receiving reports.  This is like a database report and it's report designer files.

		A report descriptor describes the structure (design) of a report, and the report itself is the buffer containing the
		actual data such as states, settings, etc for the USB HID device.

		The report descriptor contains a table of variable length coded items.  The first section (header) of an item
		contains three fields: type, tag and size.  There is three item types: Main, Global and Local.  The Global and Local
		types have state and scope, pretty much like global and local variables, and together with push/pop items enables the
		report descriptor to be smaller, taking up less space.
*/
void tru_usb_hid_report_desc_to_report_sizes(tru_usb_hid_t *hid, tru_usb_hid_interface_ext_t *intf_ext, void *report_desc, uint32_t report_desc_size){
	tru_usb_hid_report_desc_parser_stack_item_t *item_stack;
	uint32_t item_stack_num_entries = 0;
	uint8_t report_id = 0;
	uint32_t report_size = 0;
	uint32_t report_count = 0;
	uint32_t report_size_sum;
	uint32_t pos = 0;
	uint8_t *item_pre_pop = NULL;
	uint8_t *item_prev = NULL;
	uint8_t *item = report_desc;
	uint8_t prefix_data_size;
	uint8_t long_data_size;
	tru_usb_hid_report_info_t *report_info;

	// Create report descriptor item stack (for storing short items only)
	item_stack = malloc(TRU_USB_HID_REPORT_DESC_PARSER_STACK_MAX_COUNT * sizeof(tru_usb_hid_report_desc_parser_stack_item_t));
	if(item_stack){
		// Iterate report descriptor items
		while(pos < report_desc_size){
			prefix_data_size = item[0] & 0x03;  // Bits 0 & 1 is data size code (bSize)
			if(prefix_data_size == 3) prefix_data_size = 4;  // Only code 3 is different from the size value it represents

			// Is current item a a long item?
			if(item[0] == TRU_USB_HID_REPORT_DESC_LONG_ITEM){
				// Is a long item..

				// Long item data are ignored, only the item size is processed (bDataSize)
				long_data_size = item[1];
			}else{
				// Is a short item..

				long_data_size = 0;

				// A pop item?
				switch(item[0] & 0xfc){
					case TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_POP_MASK:
						if(item_stack_num_entries > 0){
							// Pop an item..
							item_stack_num_entries--;
							item_pre_pop = item;  // Save the current item so that it can be restored later
							item = item_stack[item_stack_num_entries].item;  // Pop item as the current item
						}
						break;
					default:
				}

				// ==================================
				// Big switch to process a short item
				// ==================================
				switch(item[0] & 0xfc){
					case TRU_USB_HID_REPORT_DESC_MAIN_ITEM_INPUT_MASK:
						report_info = tru_usb_hid_find_input_report_by_id(intf_ext, report_id);
						report_size_sum = report_size * report_count;
						if(report_id != 0) report_size_sum++;  // Add an extra byte for non-zero report ID
						//report_size_sum++;  // Add an extra byte for the report ID
						report_info->size_bits += report_size_sum;
						report_info->size = report_info->size_bits / 8;
						if(report_info->size % 8) report_info->size++;  // Round up to multiple of bytes
						break;
					case TRU_USB_HID_REPORT_DESC_MAIN_ITEM_OUTPUT_MASK:
						report_info = tru_usb_hid_find_output_report_by_id(intf_ext, report_id);
						report_size_sum = report_size * report_count;
						if(report_id != 0) report_size_sum++;  // Add an extra byte for non-zero report ID
						//report_size_sum++;  // Add an extra byte for the report ID
						report_info->size_bits += report_size_sum;
						report_info->size = report_info->size_bits / 8;
						if(report_info->size % 8) report_info->size++;  // Round up to multiple of bytes
						break;
					case TRU_USB_HID_REPORT_DESC_MAIN_ITEM_FEATURE_MASK:
						report_info = tru_usb_hid_find_feature_report_by_id(intf_ext, report_id);
						report_size_sum = report_size * report_count;
						if(report_id != 0) report_size_sum++;  // Add an extra byte for non-zero report ID
						//report_size_sum++;  // Add an extra byte for the report ID
						report_info->size_bits += report_size_sum;
						report_info->size = report_info->size_bits / 8;
						if(report_info->size % 8) report_info->size++;  // Round up to multiple of bytes
						break;
					case TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_REPORT_SIZE_MASK:
						report_size = tru_usb_hid_parse_short_item_int(item, prefix_data_size);
						break;
					case TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_REPORT_ID_MASK:
						report_id = tru_usb_hid_parse_short_item_int(item, prefix_data_size);
						break;
					case TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_REPORT_COUNT_MASK:
						report_count = tru_usb_hid_parse_short_item_int(item, prefix_data_size);
						break;
					case TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_PUSH_MASK:
						if(item_stack_num_entries < TRU_USB_HID_REPORT_DESC_PARSER_STACK_MAX_COUNT){
							// We assume that push is only on short items and the previous item is never a push or a pop
							item_stack[item_stack_num_entries].item[0] = item_prev[0];
							item_stack[item_stack_num_entries].item[1] = item_prev[1];
							item_stack[item_stack_num_entries].item[2] = item_prev[2];
							item_stack[item_stack_num_entries].item[3] = item_prev[3];
							item_stack[item_stack_num_entries].item[4] = item_prev[4];
							item_stack_num_entries++;
						}
						break;
					default:
				}
			}

			// Current item is now the previous item
			item_prev = item;

			// Is the original item a pop?
			if(item_pre_pop){
				// Restore original item
				item = item_pre_pop;
				item_pre_pop = NULL;
			}

			// Move to next item
			pos += prefix_data_size + long_data_size + 1;
			item += prefix_data_size + long_data_size + 1;
		}

		// Delete the report descriptor item stack
		free(item_stack);
	}
}

void tru_usb_hid_set_idle_interface(tru_usb_hid_interface_ext_t *intf_ext, uint8_t idle_rate){
	for(uint32_t i = 0; i < intf_ext->reports_info_set->num_input_reports; i++){
		intf_ext->reports_info_set->inputs[i].idle_rate = idle_rate;
	}
}

void tru_usb_hid_set_idle_config(tru_usb_config_t *config, uint8_t idle_rate){
	uint16_t interface_index;
	uint16_t alt_setting_index;
	tru_usb_interface_alt_setting_t *alt_setting;

	// Iterate interfaces
	for(interface_index = 0; interface_index < config->interfaces.num_interfaces; interface_index++){
		// Iterate alternate interfaces
		for(alt_setting_index = 0; alt_setting_index < config->interfaces.items[interface_index].alt_settings.num_alt_settings; alt_setting_index++){
			alt_setting = config->interfaces.items[interface_index].alt_settings.items + alt_setting_index;
			if(alt_setting->itf_ext){
				tru_usb_hid_interface_ext_t *intf_ext = alt_setting->itf_ext;

				for(uint32_t i = 0; i < intf_ext->reports_info_set->num_input_reports; i++){
					intf_ext->reports_info_set->inputs[i].idle_rate = idle_rate;
				}

				for(uint32_t i = 0; i < intf_ext->reports_info_set->num_output_reports; i++){
					intf_ext->reports_info_set->outputs[i].idle_rate = idle_rate;
				}

				for(uint32_t i = 0; i < intf_ext->reports_info_set->num_feature_reports; i++){
					intf_ext->reports_info_set->features[i].idle_rate = idle_rate;
				}
			}
		}
	}
}

tru_usb_hid_report_info_t *tru_usb_hid_find_input_report_by_id(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id){
	for(uint32_t i = 0; i < intf_ext->reports_info_set->num_input_reports; i++){
		if(intf_ext->reports_info_set->inputs[i].id == report_id){
			return &intf_ext->reports_info_set->inputs[i];
		}
	}

	return NULL;
}

tru_usb_hid_report_info_t *tru_usb_hid_find_output_report_by_id(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id){
	for(uint32_t i = 0; i < intf_ext->reports_info_set->num_output_reports; i++){
		if(intf_ext->reports_info_set->outputs[i].id == report_id){
			return &intf_ext->reports_info_set->outputs[i];
		}
	}

	return NULL;
}

tru_usb_hid_report_info_t *tru_usb_hid_find_output_report_by_id_and_set_currindex(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id){
	intf_ext->reports_info_set->curr_input_index = 0;
	for(uint32_t i = 0; i < intf_ext->reports_info_set->num_output_reports; i++){
		if(intf_ext->reports_info_set->outputs[i].id == report_id){
			intf_ext->reports_info_set->curr_input_index = i;
			return &intf_ext->reports_info_set->outputs[i];
		}
	}

	return NULL;
}

tru_usb_hid_report_info_t *tru_usb_hid_find_feature_report_by_id(tru_usb_hid_interface_ext_t *intf_ext, uint8_t report_id){
	for(uint32_t i = 0; i < intf_ext->reports_info_set->num_feature_reports; i++){
		if(intf_ext->reports_info_set->features[i].id == report_id){
			return &intf_ext->reports_info_set->features[i];
		}
	}

	return NULL;
}

// Get the next input report, increment the index and set the report buffer as the endpoint transfer buffer
tru_usb_hid_report_info_t *tru_hid_get_next_input_report_info(tru_usb_pc_epi_t *ep, tru_dwc2_dma_mode_t dma_mode){
	tru_usb_interface_alt_setting_t *alt_setting = ep->curr_alt_setting;
	tru_usb_hid_interface_ext_t *intf_ext = alt_setting->itf_ext;
	tru_usb_hid_report_info_t *input_report_info = &intf_ext->reports_info_set->inputs[intf_ext->reports_info_set->curr_input_index];  // Get current report info

	// Register the current report as the transfer buffer
	tru_usb_lxfer_unregister_buffers(&ep->lxfer);
	tru_usb_lxfer_register_buffer(&ep->lxfer, input_report_info->report, input_report_info->size);
	if(dma_mode == TRU_DWC2_DMA_MODE_SG){
		tru_usb_lxfer_register_sgdma_descs(&ep->lxfer, intf_ext->reports_info_set->epi_dma_descs, intf_ext->reports_info_set->epi_num_descs);
	}

	// Increment index for next report
	intf_ext->reports_info_set->curr_input_index = (intf_ext->reports_info_set->curr_input_index + 1) % intf_ext->reports_info_set->num_input_reports;

	return input_report_info;
}

tru_usb_pc_epi_t *tru_hid_get_report_epi_from_epo(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep){
	tru_usb_interface_alt_setting_t *alt_setting = ep->curr_alt_setting;
	tru_usb_hid_interface_ext_t *intf_ext = alt_setting->itf_ext;
	tru_usb_pc_epi_t *epin = &hid->pc->eps.epis[intf_ext->reports_info_set->epin_num];

	return epin;
}

tru_usb_pc_epi_t *tru_hid_get_report_epi_from_intf_ext(tru_usb_hid_t *hid, tru_usb_hid_interface_ext_t *intf_ext){
	tru_usb_pc_epi_t *epin = &hid->pc->eps.epis[intf_ext->reports_info_set->epin_num];
	return epin;
}

// Copy endpoint transfer buffer into the output report buffer
void tru_hid_copy_output_report(tru_usb_pc_epo_t *ep, uint32_t start_offset){
	tru_usb_interface_alt_setting_t *alt_setting = ep->curr_alt_setting;
	tru_usb_hid_interface_ext_t *intf_ext = alt_setting->itf_ext;

	tru_usb_hid_report_info_t *output_report_info;
	uint32_t offset = start_offset;

	if(ep->lxfer.chunk.total_xferred_size > 0){
		if(intf_ext->reports_info_set->num_output_reports == 1){
			intf_ext->reports_info_set->curr_output_index = 0;
			output_report_info = &intf_ext->reports_info_set->inputs[0];  // Always get the first report info when there is only one report
		}else{
			uint8_t report_id = ep->lxfer.chunk.buffers[0].buf[0];  // Assume the first byte is the report ID according to the HID USB spec
			output_report_info = tru_usb_hid_find_output_report_by_id_and_set_currindex(intf_ext, report_id);  // Get the report info
		}

		// Copy received data into storage output report buffer
		if(output_report_info != NULL){
			for(uint32_t i = 0; i < ep->lxfer.chunk.num_registered; i++){
				if(offset >= output_report_info->size) break;
				memcpy(output_report_info->report + offset, ep->lxfer.chunk.buffers[i].buf, ep->lxfer.chunk.buffers[i].xferred_size);
				offset += ep->lxfer.chunk.buffers[i].xferred_size;
			}
		}
	}
}

tru_usb_hid_report_info_t *tru_hid_get_output_report_info(tru_usb_pc_epo_t *ep){
	tru_usb_interface_alt_setting_t *alt_setting = ep->curr_alt_setting;
	tru_usb_hid_interface_ext_t *intf_ext = alt_setting->itf_ext;
	tru_usb_hid_report_info_t *output_report_info = &intf_ext->reports_info_set->inputs[intf_ext->reports_info_set->curr_output_index];
	return output_report_info;
}

/*
	HID class extension setup request handler for request type:
		direction: host-to-device
		type     : standard
		recipient: device

	Extends the standard setup request handler to support HID devices.
	Handles anything additional for HID class.
*/
static bool tru_usb_hid_setup_hd_std_dev(tru_usb_hid_t *hid){
	return false;
}

/*
	HID class extension setup request handler for request type:
		direction: host-to-device
		type     : standard
		recipient: interface

	Extends the standard setup request handler to support HID devices.
	Handles anything additional for HID class.
*/
static bool tru_usb_hid_setup_hd_std_intf(tru_usb_hid_t *hid){
	return false;
}

/*
	HID class extension setup request handler for request type:
		direction: host-to-device
		type     : standard
		recipient: endpoint

	Extends the standard setup request handler to support HID devices.
	Handles anything additional for HID class.
*/
static bool tru_usb_hid_setup_hd_std_ep(tru_usb_hid_t *hid){
	return false;
}

/*
	HID class specific setup request handler for request type:
		direction: host-to-device
		type     : class
		recipient: interface

	Handles anything specific for HID class.
*/
static bool tru_usb_hid_setup_hd_cls_intf(tru_usb_hid_t *hid){
	uint8_t req_interface_num;
	uint8_t req_report_type;
	tru_usb_interface_t *req_interface;
	tru_usb_hid_interface_ext_t *class_interface_ext;
	uint8_t report_id;
	uint32_t xfer_size;
	tru_usb_hid_report_info_t *report_info;

	// Process the setup request
	switch(hid->pc->setup.request.code){
		case TRU_USB_HID_REQ_SET_IDLE:
			switch(hid->pc->eps.epos[0].control_stage){
				case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
					hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
					LOG("REQ: SET_IDLE\n");
#endif

					if(hid->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							report_id = hid->pc->setup.request.value & 0xff;

							if(report_id == 0){
								tru_usb_hid_set_idle_interface(class_interface_ext, hid->pc->setup.request.value >> 8);
							}else{
								report_info = tru_usb_hid_find_input_report_by_id(class_interface_ext, report_id);
								if(report_info){
									report_info->idle_rate = hid->pc->setup.request.value >> 8;  // Store the new HID idle value
								}
							}

							return true;
						}
					}
					break;
				case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
					hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
					return true;
					break;
			}
			break;

		case TRU_USB_HID_REQ_SET_REPORT:
			if(hid->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_report_type = (hid->pc->setup.request.value >> 8) & 0xff;

				switch(req_report_type){
					// Input report
					case TRU_USB_HID_REQ_REPORT_TYPE_INPUT:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							report_id = hid->pc->setup.request.value & 0xff;
							report_info = tru_usb_hid_find_input_report_by_id(class_interface_ext, report_id);
							if(report_info){
								xfer_size = (hid->pc->setup.request.length > report_info->size) ? report_info->size : hid->pc->setup.request.length;
								if(xfer_size){
									switch(hid->pc->eps.epos[0].control_stage){
										case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

											#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
												LOG("REQ: SET_REPORT INPUT LEN %u\n", hid->pc->setup.request.length);
											#endif

											tru_usb_pc_receive(hid->pc, 0, xfer_size, hid->pc->setup.request.length);
											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

											TRU_MEMCPY(
												report_info->report,
												hid->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
												(hid->pc->eps.epos[0].lxfer.chunk.total_xferred_size > report_info->size) ? report_info->size : hid->pc->eps.epos[0].lxfer.chunk.total_xferred_size
											);

											if(user_callbacks.cb_set_report){
												user_callbacks.cb_set_report(
													hid,
													class_interface_ext,
													report_info,
													report_id,
													req_report_type
												);
											}

											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
											return true;
											break;
									}
								}
							}
						}
						break;

					// Output report
					case TRU_USB_HID_REQ_REPORT_TYPE_OUTPUT:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							report_id = hid->pc->setup.request.value & 0xff;
							report_info = tru_usb_hid_find_output_report_by_id(class_interface_ext, report_id);
							if(report_info){
								xfer_size = (hid->pc->setup.request.length > report_info->size) ? report_info->size : hid->pc->setup.request.length;
								if(xfer_size){
									switch(hid->pc->eps.epos[0].control_stage){
										case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

											#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
												LOG("REQ: SET_REPORT OUTPUT LEN %u\n", hid->pc->setup.request.length);
											#endif

											tru_usb_pc_receive(hid->pc, 0, xfer_size, hid->pc->setup.request.length);
											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

											TRU_MEMCPY(
												report_info->report,
												hid->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
												(hid->pc->eps.epos[0].lxfer.chunk.total_xferred_size > report_info->size) ? report_info->size : hid->pc->eps.epos[0].lxfer.chunk.total_xferred_size
											);

											if(user_callbacks.cb_set_report){
												user_callbacks.cb_set_report(
													hid,
													class_interface_ext,
													report_info,
													report_id,
													req_report_type
												);
											}

											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
											return true;
											break;
									}
								}
							}
						}
						break;

					// Feature report
					case TRU_USB_HID_REQ_REPORT_TYPE_FEATURE:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							report_id = hid->pc->setup.request.value & 0xff;
							report_info = tru_usb_hid_find_feature_report_by_id(class_interface_ext, report_id);
							if(report_info){
								xfer_size = (hid->pc->setup.request.length > report_info->size) ? report_info->size : hid->pc->setup.request.length;
								if(xfer_size){
									switch(hid->pc->eps.epos[0].control_stage){
										case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT;

											#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
												LOG("REQ: SET_REPORT FEATURE LEN %u\n", hid->pc->setup.request.length);
											#endif

											tru_usb_pc_receive(hid->pc, 0, xfer_size, hid->pc->setup.request.length);
											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

											TRU_MEMCPY(
												report_info->report,
												hid->pc->eps.epos[0].lxfer.chunk.buffers[0].buf,
												(hid->pc->eps.epos[0].lxfer.chunk.total_xferred_size > report_info->size) ? report_info->size : hid->pc->eps.epos[0].lxfer.chunk.total_xferred_size
											);

											if(user_callbacks.cb_set_report){
												user_callbacks.cb_set_report(
													hid,
													class_interface_ext,
													report_info,
													report_id,
													req_report_type
												);
											}

											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
											hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
											return true;
											break;
									}
								}
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
	HID class specific setup request handler for request type:
		direction: device-to-host
		type     : standard
		recipient: interface

	Handles anything specific for HID class.
*/
static bool tru_usb_hid_setup_dh_std_intf(tru_usb_hid_t *hid){
	uint8_t req_interface_num;
	tru_usb_interface_t *req_interface;
	uint8_t req_desc_type;
	uint8_t req_physical_set_num;
	tru_usb_hid_interface_ext_t *class_interface_ext;
	tru_usb_hid_desc_t *hid_desc;
	uint32_t xfer_size;

	// Process the setup request
	switch(hid->pc->setup.request.code){
		case TRU_USB_SETUP_REQ_GET_DESCRIPTOR:
			if(hid->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_desc_type = (hid->pc->setup.request.value >> 8) & 0xff;

				switch(req_desc_type){
					case TRU_USB_HID_DESC_TYPE_HID:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							hid_desc = TO_HID_INTERFACE_EXT_PTR(req_interface->alt_settings.curr_alt_setting->itf_ext)->hid_desc;

							// HID descriptor is not NULL?
							if(hid_desc){
								switch(hid->pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
										LOG("REQ: GET_DESCRIPTOR HID LEN %u\n", hid->pc->setup.request.length);
#endif

										// Setup controller to transmit HID descriptor
										xfer_size = (hid->pc->setup.request.length > hid_desc->length) ? hid_desc->length : hid->pc->setup.request.length;
										TRU_MEMCPY(hid->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, hid_desc, xfer_size);
										tru_usb_pc_transmit(hid->pc, 0, xfer_size, hid->pc->setup.request.length);

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
							}
						}
						break;

					case TRU_USB_HID_DESC_TYPE_REPORT:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;

							// Is there a report descriptor?
							if(class_interface_ext->report_desc_info.desc){
								switch(hid->pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
										LOG("REQ: GET_DESCRIPTOR REPORT LEN %u\n", hid->pc->setup.request.length);
#endif

										// Setup controller to transmit HID report descriptor, which is always the first descriptor
										xfer_size = (hid->pc->setup.request.length > class_interface_ext->report_desc_info.size) ? class_interface_ext->report_desc_info.size : hid->pc->setup.request.length;
										TRU_MEMCPY(hid->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, class_interface_ext->report_desc_info.desc, xfer_size);
										tru_usb_pc_transmit(hid->pc, 0, xfer_size, hid->pc->setup.request.length);

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
							}
						}
						break;

					case TRU_USB_HID_DESC_TYPE_PHYSICAL:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;

							// Is there any physical descriptors?
							if(class_interface_ext->physical_desc_info_set.num_elements){
								req_physical_set_num = hid->pc->setup.request.value & 0xff;
								// According to the USB HID datasheet an invalid requested number selects the last valid number
								if(req_physical_set_num > class_interface_ext->physical_desc_info_set.num_elements){
									req_physical_set_num = class_interface_ext->physical_desc_info_set.num_elements - 1;
								}

								switch(hid->pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
										LOG("REQ: GET_DESCRIPTOR PHYSICAL LEN %u\n", hid->pc->setup.request.length);
#endif

										// Setup controller to transmit HID physical descriptor
										xfer_size = (hid->pc->setup.request.length > class_interface_ext->physical_desc_info_set.items[req_physical_set_num].size) ? class_interface_ext->physical_desc_info_set.items[req_physical_set_num].size : hid->pc->setup.request.length;
										TRU_MEMCPY(hid->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, class_interface_ext->physical_desc_info_set.items[req_physical_set_num].desc, xfer_size);
										tru_usb_pc_transmit(hid->pc, 0, xfer_size, hid->pc->setup.request.length);

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
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
	HID class specific setup request handler for request type:
		direction: device-to-host
		type     : class
		recipient: interface

	Handles anything specific for HID class.
*/
static bool tru_usb_hid_setup_dh_cls_intf(tru_usb_hid_t *hid){
	uint8_t req_interface_num;
	tru_usb_interface_t *req_interface;
	uint8_t req_report_type;
	tru_usb_hid_interface_ext_t *class_interface_ext;
	uint8_t report_id;
	tru_usb_hid_report_info_t *report_info;
	uint32_t xfer_size;

	// Process the setup request
	switch(hid->pc->setup.request.code){
		case TRU_USB_HID_REQ_GET_REPORT:
			if(hid->pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				req_report_type = (hid->pc->setup.request.value >> 8) & 0xff;

				switch(req_report_type){
					// Input report
					case TRU_USB_HID_REQ_REPORT_TYPE_INPUT:
						req_interface_num = (uint8_t)(hid->pc->setup.request.index & 0xff);
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							report_id = hid->pc->setup.request.value & 0xff;
							report_info = tru_usb_hid_find_input_report_by_id(class_interface_ext, report_id);

							switch(hid->pc->eps.epos[0].control_stage){
								case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
									hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
									LOG("REQ: GET_REPORT INPUT LEN %u\n", hid->pc->setup.request.length);
#endif

									// Setup controller to transmit HID report

									xfer_size = (hid->pc->setup.request.length > report_info->size) ? report_info->size : hid->pc->setup.request.length;
									TRU_MEMCPY(hid->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, report_info->report, xfer_size);
									tru_usb_pc_transmit(hid->pc, 0, xfer_size, hid->pc->setup.request.length);

									return true;
									break;
								case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
									hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
									return true;
									break;
								case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
									hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
									return true;
									break;
							}
						}
						break;

					// Output report
					case TRU_USB_HID_REQ_REPORT_TYPE_OUTPUT:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							report_id = hid->pc->setup.request.value & 0xff;
							report_info = tru_usb_hid_find_output_report_by_id(class_interface_ext, report_id);
							if(report_info){
								switch(hid->pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

										#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
											LOG("REQ: GET_REPORT OUTPUT LEN %u\n", hid->pc->setup.request.length);
										#endif

										// Setup controller to transmit HID report
										xfer_size = (hid->pc->setup.request.length > report_info->size) ? report_info->size : hid->pc->setup.request.length;
										TRU_MEMCPY(hid->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, report_info->report, xfer_size);
										tru_usb_pc_transmit(hid->pc, 0, xfer_size, hid->pc->setup.request.length);

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
							}
						}
						break;

					// Feature report
					case TRU_USB_HID_REQ_REPORT_TYPE_FEATURE:
						req_interface_num = hid->pc->setup.request.index & 0xff;
						req_interface = tru_usb_interfaces_find_interface(&hid->pc->configs.curr_config->interfaces, req_interface_num);

						// Is the requested interface valid?
						if(req_interface){
							report_id = hid->pc->setup.request.value && 0xff;
							class_interface_ext = req_interface->alt_settings.curr_alt_setting->itf_ext;
							report_id = hid->pc->setup.request.value & 0xff;
							report_info = tru_usb_hid_find_feature_report_by_id(class_interface_ext, report_id);
							if(report_info){
								switch(hid->pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

										#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
											LOG("REQ: GET_REPORT FEATURE LEN %u\n", hid->pc->setup.request.length);
										#endif

										// Setup controller to transmit HID report
										xfer_size = (hid->pc->setup.request.length > report_info->size) ? report_info->size : hid->pc->setup.request.length;
										TRU_MEMCPY(hid->pc->eps.epis[0].lxfer.chunk.buffers[0].buf, report_info->report, xfer_size);
										tru_usb_pc_transmit(hid->pc, 0, xfer_size, hid->pc->setup.request.length);

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										hid->pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
							}
						}
						break;
				}
			}
			break;
	}

	return false;
}

static void tru_usb_hid_callback_reset(tru_usb_hid_t *hid){
	if(user_callbacks.cb_reset) user_callbacks.cb_reset(hid);
}

static void tru_usb_hid_callback_enumdone(tru_usb_hid_t *hid){
	if(user_callbacks.cb_enumdone) user_callbacks.cb_enumdone(hid);
}

/*
	Common HID class setup request handler.
	This extends the standard setup requests and also adds new setup requests to support HID devices.
*/
static bool tru_usb_hid_callback_setup(tru_usb_hid_t *hid){
	bool success = false;

	// ========================================================================================================================
	// Process handled or unhandled setup request.  Implements HID class extension requests, i.e. extend the standard requests.
	// ========================================================================================================================

	// What request direction?
	switch(hid->pc->setup.request.type.bits.dir){
		case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
			// What request type?
			switch(hid->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(hid->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: tru_usb_hid_setup_hd_std_dev(hid); break;
						case TRU_USB_SETUP_RCP_INTERFACE: tru_usb_hid_setup_hd_std_intf(hid); break;
						case TRU_USB_SETUP_RCP_ENDPOINT: tru_usb_hid_setup_hd_std_ep(hid); break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(hid->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(hid->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
			// What request type?
			switch(hid->pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(hid->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_CLASS:
					// What recipient?
					switch(hid->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;

				case TRU_USB_SETUP_TYPE_VENDOR:
					// What recipient?
					switch(hid->pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: break;
						case TRU_USB_SETUP_RCP_INTERFACE: break;
						case TRU_USB_SETUP_RCP_ENDPOINT: break;
					}
					break;
			}
			break;
	}

	// =========================================================================
	// Process unhandled setup request.  Implements HID class specific requests.
	// =========================================================================

	if(!hid->pc->setup.is_handled){
		// What request direction?
		switch(hid->pc->setup.request.type.bits.dir){
			case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
				// What request type?
				switch(hid->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(hid->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(hid->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: success = success | tru_usb_hid_setup_hd_cls_intf(hid); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(hid->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;

			case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
				// What request type?
				switch(hid->pc->setup.request.type.bits.type){
					case TRU_USB_SETUP_TYPE_STANDARD:
						// What recipient?
						switch(hid->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: success = success | tru_usb_hid_setup_dh_std_intf(hid); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_CLASS:
						// What recipient?
						switch(hid->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: success = success | tru_usb_hid_setup_dh_cls_intf(hid); break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;

					case TRU_USB_SETUP_TYPE_VENDOR:
						// What recipient?
						switch(hid->pc->setup.request.type.bits.recipient){
							case TRU_USB_SETUP_RCP_DEVICE: break;
							case TRU_USB_SETUP_RCP_INTERFACE: break;
							case TRU_USB_SETUP_RCP_ENDPOINT: break;
						}
						break;
				}
				break;
		}
	}

	if(user_callbacks.cb_setup) user_callbacks.cb_setup(hid);

	return success;
}

void tru_usb_hid_callback_cfg_act(tru_usb_hid_t *hid, tru_usb_config_t *config, bool is_activated){
	if(is_activated){
		tru_usb_hid_config_ext_init(hid, config);
	}else{
		tru_usb_hid_config_ext_deinit(hid, config);
	}
	if(user_callbacks.cb_cfg_act) user_callbacks.cb_cfg_act(hid, config, is_activated);
}

void tru_usb_hid_callback_itf_act(tru_usb_hid_t *hid, tru_usb_interface_alt_setting_t *alt_setting, bool is_activated){
	if(is_activated){
		tru_usb_hid_interface_ext_init(hid, alt_setting);
	}else{
		tru_usb_hid_interface_ext_deinit(hid, alt_setting);
	}
	if(user_callbacks.cb_itf_act) user_callbacks.cb_itf_act(hid, alt_setting, is_activated);
}

void tru_usb_hid_callback_epi_act(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep, bool is_activated){
	if(user_callbacks.cb_epi_act[ep->num]) user_callbacks.cb_epi_act[ep->num](hid, ep, is_activated);
}

void tru_usb_hid_callback_epo_act(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep, bool is_activated){
	if(user_callbacks.cb_epo_act[ep->num]) user_callbacks.cb_epo_act[ep->num](hid, ep, is_activated);
}

void tru_usb_hid_callback_epi_chunk(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_chunk[ep->num]) user_callbacks.cb_epi_chunk[ep->num](hid, ep);
}

void tru_usb_hid_callback_epi_compl(tru_usb_hid_t *hid, tru_usb_pc_epi_t *ep){
	if(user_callbacks.cb_epi_compl[ep->num]) user_callbacks.cb_epi_compl[ep->num](hid, ep);
}

void tru_usb_hid_callback_epo_chunk(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep){
	if(ep->num != 0) tru_hid_copy_output_report(ep, ep->lxfer.whole.total_xferred_size - ep->lxfer.chunk.total_xferred_size);

	if(user_callbacks.cb_epo_chunk[ep->num]) user_callbacks.cb_epo_chunk[ep->num](hid, ep);
}

void tru_usb_hid_callback_epo_compl(tru_usb_hid_t *hid, tru_usb_pc_epo_t *ep){
	if(user_callbacks.cb_epo_compl[ep->num]) user_callbacks.cb_epo_compl[ep->num](hid, ep);
}

void tru_usb_hid_callback_sof(tru_usb_hid_t *hid){
	if(user_callbacks.cb_sof) user_callbacks.cb_sof(hid);
}
