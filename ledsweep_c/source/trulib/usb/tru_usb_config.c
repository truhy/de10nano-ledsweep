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

#include "usb/tru_usb_config.h"
#include "usb/tru_usb_def.h"
#include "synopsys/tru_dwc2.h"
#include <stdlib.h>

void tru_usb_configs_init(
	tru_usb_configs_t *configs,
	tru_usb_config_desc_set_t *hs_desc_info_items, uint32_t hs_desc_info_items_num_entries,
	tru_usb_config_desc_set_t *fs_desc_info_items, uint32_t fs_desc_info_items_num_entries,
	tru_usb_config_desc_set_t *ls_desc_info_items, uint32_t ls_desc_info_items_num_entries
){
	tru_usb_configs_init_spd_config(&configs->hs_configs, hs_desc_info_items, hs_desc_info_items_num_entries);
	tru_usb_configs_init_spd_config(&configs->fs_configs, fs_desc_info_items, fs_desc_info_items_num_entries);
	tru_usb_configs_init_spd_config(&configs->ls_configs, ls_desc_info_items, ls_desc_info_items_num_entries);
	configs->curr_spd_configs = NULL;
	configs->other_spd_configs = NULL;
	configs->curr_config = NULL;
}

void tru_usb_configs_deinit(tru_usb_configs_t *configs){
	tru_usb_configs_deinit_spd(configs);
	if(configs->hs_configs.num_elements){
		tru_usb_configs_deinit_spd_config(&configs->hs_configs);
	}
	if(configs->fs_configs.num_elements){
		tru_usb_configs_deinit_spd_config(&configs->fs_configs);
	}
	if(configs->ls_configs.num_elements){
		tru_usb_configs_deinit_spd_config(&configs->ls_configs);
	}
	configs->prev_config = NULL;
	configs->curr_config = NULL;
	configs->curr_spd_configs = NULL;
	configs->other_spd_configs = NULL;
}

/*
	Parses the specified configuration group of descriptors and initialise configuration, interfaces and endpoints.
*/
void tru_usb_configs_init_spd_config(tru_usb_spd_configs_t *spd_configs, tru_usb_config_desc_set_t *desc_info_items, uint32_t desc_info_items_num_entries){
	uint16_t config_index;
	uint8_t *buf;
	uint32_t offset;
	uint8_t interface_num;
	uint8_t prev_interface_num;
	uint8_t alt_setting_num;
	uint8_t endpoint_index;
	uint8_t *first_interface_buf;
	uint32_t first_interface_offset;
	uint16_t alt_setting_count;

	if(desc_info_items_num_entries){
		spd_configs->items = malloc(desc_info_items_num_entries * sizeof(tru_usb_config_t));
		for(config_index = 0; config_index < desc_info_items_num_entries; config_index++){
			spd_configs->items[config_index].config_num = TRU_USB_CONFIG_DESC_PTR(desc_info_items[config_index].desc)->config_num;
			spd_configs->items[config_index].desc = desc_info_items[config_index].desc;
			spd_configs->items[config_index].size = desc_info_items[config_index].size;
		}
		spd_configs->num_elements = desc_info_items_num_entries;

		// Iterate configurations
		for(config_index = 0; config_index < spd_configs->num_elements; config_index++){
			buf = spd_configs->items[config_index].desc;
			offset = 0;

			// Are there any interface descriptors (buf[4] == number of interfaces)?
			if(buf[4] > 0){
				// Allocate and initialise interfaces
				tru_usb_interfaces_init(&spd_configs->items[config_index].interfaces, buf[4]);
				if(spd_configs->items[config_index].interfaces.num_interfaces == buf[4]){
					// Move to the first interface descriptor
					offset += buf[0];
					buf += buf[0];

					/*
						Since there is not a field for the number of alternate settings we have to iterate through all
						descriptors within the configuration to find them
					*/

					first_interface_offset = offset;
					first_interface_buf = buf;
					interface_num = 0;
					prev_interface_num = 0;
					alt_setting_count = 0;
					// Iterate descriptors
					while(offset < spd_configs->items[config_index].size){
						switch(buf[1]){
							case TRU_USB_DESC_TYPE_INTERFACE:
								// Detect alternate interface by checking whether the interface number is the same as previously found
								if(prev_interface_num == buf[2]){
									alt_setting_count++;
								}else{
									// Initialise alternate settings (including alternate setting 0)
									tru_usb_interfaces_alt_init(&spd_configs->items[config_index].interfaces, spd_configs->items[config_index].config_num, prev_interface_num, 0, alt_setting_count);

									prev_interface_num = buf[2];
									alt_setting_count = 1;
								}
								break;
						}

						// Move to the next descriptor
						offset += buf[0];
						buf += buf[0];
					}
					// Process last set of alternate settings
					if(alt_setting_count){
						// Initialise alternate settings (including alternate setting 0)
						tru_usb_interfaces_alt_init(&spd_configs->items[config_index].interfaces, spd_configs->items[config_index].config_num, prev_interface_num, 0, alt_setting_count);
					}else{
						// Error, there are no interface descriptors!
					}

					/*
						Iterate descriptors in the configuration again to setup interfaces and endpoints
					*/

					offset = first_interface_offset;
					buf = first_interface_buf;
					interface_num = 0;
					alt_setting_num = 0;
					endpoint_index = 0;
					// Iterate descriptors
					while(offset < spd_configs->items[config_index].size){
						switch(buf[1]){
							case TRU_USB_DESC_TYPE_INTERFACE:
								interface_num = buf[2];  // Get the interface number from current configuration
								alt_setting_num = buf[3];  // Get interface alternate setting number from current configuration
								spd_configs->items[config_index].interfaces.items[interface_num].interface_num = interface_num;
								spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].config_num = spd_configs->items[config_index].config_num;
								spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].interface_num = interface_num;
								spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].alt_setting_num = alt_setting_num;
								spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].desc = buf;
								spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].desc_offset = offset;
								spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].desc_remain = spd_configs->items[config_index].size - offset;
								tru_usb_eps_init(&spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].eps, buf[4]);
								endpoint_index = 0;
								break;
							case TRU_USB_DESC_TYPE_ENDPOINT:
								spd_configs->items[config_index].interfaces.items[interface_num].alt_settings.items[alt_setting_num].eps.items[endpoint_index].desc = buf;
								endpoint_index++;
								break;
						}

						// Move to the next descriptor
						offset += buf[0];
						buf += buf[0];
					}
				}
			}
		}
	}else{
		spd_configs->items = NULL;
		spd_configs->num_elements = 0;
	}
}

void tru_usb_configs_deinit_spd_config(tru_usb_spd_configs_t *spd_configs){
	uint16_t config_index;

	// Iterate configurations
	for(config_index = 0; config_index < spd_configs->num_elements; config_index++){
		tru_usb_interfaces_deinit(&spd_configs->items[config_index].interfaces);
	}
	free(spd_configs->items);
	spd_configs->items = NULL;
	spd_configs->num_elements = 0;
}

/*
	Set all descriptor type field with other speed configuration descriptor type.
*/
void tru_usb_configs_set_desc_type(tru_usb_spd_configs_t *spd_configs, uint8_t desc_type){
	uint16_t config_index;

	// Iterate configurations
	for(config_index = 0; config_index < spd_configs->num_elements; config_index++){
		// Set descriptor type
		spd_configs->items[config_index].desc[1] = desc_type;  // E.g. TRU_USB_DESC_TYPE_CONFIG or TRU_USB_DESC_TYPE_OTHER_SPEED_CONFIG;
	}
}

/*
	This should be called after the USB speed is detected.
	Select the relevant configuration according to the specified speed, also if the speed is high or full speed then select other speed configuration.
*/
void tru_usb_configs_init_spd(tru_usb_configs_t *configs, uint8_t speed){
	switch(speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:  // High speed
			if(configs->hs_configs.num_elements){
				configs->curr_spd_configs = &configs->hs_configs;
				configs->other_spd_configs = (configs->fs_configs.num_elements) ? &configs->fs_configs : NULL;
			}else if(configs->fs_configs.num_elements){
				configs->curr_spd_configs = &configs->fs_configs;
				configs->other_spd_configs = NULL;
			}else if(configs->ls_configs.num_elements){
				configs->curr_spd_configs = &configs->ls_configs;
				configs->other_spd_configs = NULL;
			}else{
				configs->curr_spd_configs = NULL;
				configs->other_spd_configs = NULL;
			}
			break;
		case TRU_DWC2_DSTS_ENUMSPD_FS3060:  // Full speed
		case TRU_DWC2_DSTS_ENUMSPD_FS48:
			configs->curr_spd_configs = (configs->fs_configs.num_elements) ? &configs->fs_configs : NULL;
			configs->other_spd_configs = (configs->hs_configs.num_elements) ? &configs->hs_configs : NULL;
			break;
		case TRU_DWC2_DSTS_ENUMSPD_LS6:  // Low speed
			configs->curr_spd_configs = (configs->ls_configs.num_elements) ? &configs->ls_configs : NULL;
			configs->other_spd_configs = NULL;  // There is no other speed configuration for low speed
			break;
		default:
			configs->curr_spd_configs = NULL;
			configs->other_spd_configs = NULL;
	}
}

void tru_usb_configs_deinit_spd(tru_usb_configs_t *configs){
	configs->curr_spd_configs = NULL;
	configs->other_spd_configs = NULL;
}

// Get the configuration number from the configuration descriptor
uint8_t tru_usb_configs_get_num(void *config_desc){
	return TRU_USB_CONFIG_DESC_PTR(config_desc)->config_num;
}

tru_usb_config_t *tru_usb_configs_find_config(tru_usb_spd_configs_t *spd_configs, uint8_t config_num){
	uint16_t config_index;

	// Iterate configurations
	for(config_index = 0; config_index < spd_configs->num_elements; config_index++){
		if(TRU_USB_CONFIG_DESC_PTR(spd_configs->items[config_index].desc)->config_num == config_num){
			return spd_configs->items + config_index;
		}
	}

	return NULL;
}
