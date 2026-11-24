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

#include "usb/tru_usb_interface.h"
#include <stdlib.h>

void tru_usb_interfaces_init(tru_usb_interfaces_t *interfaces, uint8_t num_interfaces){
	uint16_t interface_num;

	if(num_interfaces){
		interfaces->items = malloc(num_interfaces * sizeof(tru_usb_interface_t));
		if(interfaces->items){
			interfaces->num_interfaces = num_interfaces;

			for(interface_num = 0; interface_num < num_interfaces; interface_num++){
				tru_usb_interfaces_alt_init(interfaces, 1, interface_num, 0, 0);  // The altsettings is not populated yet so we set dummy values
			}
		}else{
			interfaces->num_interfaces = 0;
		}
	}else{
		interfaces->items = NULL;
		interfaces->num_interfaces = 0;
	}
}

void tru_usb_interfaces_deinit(tru_usb_interfaces_t *interfaces){
	uint16_t interface_num;

	if(interfaces){
		for(interface_num = 0; interface_num < interfaces->num_interfaces; interface_num++){
			tru_usb_interfaces_alt_deinit(interfaces->items + interface_num);
		}
		free(interfaces->items);
		interfaces->items = NULL;
		interfaces->num_interfaces = 0;
	}
}

void tru_usb_interfaces_alt_init(tru_usb_interfaces_t *interfaces, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num, uint8_t num_alt_settings){
	uint16_t i;

	if(num_alt_settings){
		interfaces->items[interface_num].alt_settings.items = malloc(num_alt_settings * sizeof(tru_usb_interface_alt_setting_t));
		if(interfaces->items[interface_num].alt_settings.items){
			interfaces->items[interface_num].alt_settings.num_alt_settings = num_alt_settings;

			for(i = 0; i < num_alt_settings; i++){
				interfaces->items[interface_num].interface_num = interface_num;
				interfaces->items[interface_num].alt_settings.items[i].config_num = config_num;
				interfaces->items[interface_num].alt_settings.items[i].interface_num = interface_num;
				interfaces->items[interface_num].alt_settings.items[i].alt_setting_num = alt_setting_num;
				interfaces->items[interface_num].alt_settings.items[i].desc = NULL;
				tru_usb_eps_init(&interfaces->items[interface_num].alt_settings.items[i].eps, 0);
				interfaces->items[interface_num].alt_settings.items[i].desc_offset = 0;
				interfaces->items[interface_num].alt_settings.items[i].desc_remain = 0;
				interfaces->items[interface_num].alt_settings.items[i].status = 0;
				interfaces->items[interface_num].alt_settings.items[i].itf_ext = NULL;
			}

			interfaces->items[interface_num].alt_settings.prev_alt_setting = interfaces->items[interface_num].alt_settings.curr_alt_setting;
			interfaces->items[interface_num].alt_settings.curr_alt_setting = interfaces->items[interface_num].alt_settings.items;  // Default to first alternate setting
		}
	}else{
		interfaces->items[interface_num].alt_settings.num_alt_settings = 0;
		interfaces->items[interface_num].alt_settings.items = NULL;
		interfaces->items[interface_num].alt_settings.prev_alt_setting = NULL;
		interfaces->items[interface_num].alt_settings.curr_alt_setting = NULL;
	}
}

void tru_usb_interfaces_alt_deinit(tru_usb_interface_t *interface){
	uint16_t i;

	for(i = 0; i < interface->alt_settings.num_alt_settings; i++){
		tru_usb_eps_deinit(&interface->alt_settings.items[i].eps);
	}
	free(interface->alt_settings.items);
	interface->alt_settings.items = NULL;
	interface->alt_settings.num_alt_settings = 0;
	interface->alt_settings.prev_alt_setting = NULL;
	interface->alt_settings.curr_alt_setting = NULL;
}

uint8_t tru_usb_interfaces_get_num(void *interface_desc){
	return TRU_USB_INTERFACE_DESC_PTR(interface_desc)->interface_num;
}

uint8_t tru_usb_interfaces_get_altnum(void *interface_desc){
	return TRU_USB_INTERFACE_DESC_PTR(interface_desc)->alt_setting_num;
}

tru_usb_interface_t *tru_usb_interfaces_find_interface(tru_usb_interfaces_t *interfaces, uint8_t interface_num){
	uint16_t i;

	for(i = 0; i < interfaces->num_interfaces; i++){
		if(interfaces->items[i].alt_settings.num_alt_settings){
			if(interfaces->items[i].alt_settings.items[0].desc[2] == interface_num){
				return interfaces->items + i;
			}
		}
	}

	return NULL;
}

tru_usb_interface_alt_setting_t *tru_usb_interfaces_find_alt_setting(tru_usb_interface_t *interface, uint8_t alt_setting_num){
	uint16_t i;

	for(i = 0; i < interface->alt_settings.num_alt_settings; i++){
		if(interface->alt_settings.items[i].desc[3] == alt_setting_num){
			return interface->alt_settings.items + i;
		}
	}

	return NULL;
}

tru_usb_interface_alt_setting_t *tru_usb_interfaces_find_interface_alt_setting(tru_usb_interfaces_t *interfaces, uint8_t interface_num, uint8_t alt_setting_num){
	uint16_t i;
	uint16_t j;

	for(i = 0; i < interfaces->num_interfaces; i++){
		for(j = 0; j < interfaces->items[i].alt_settings.num_alt_settings; j++){
			if(interfaces->items[i].alt_settings.items[j].desc[2] == interface_num && interfaces->items[i].alt_settings.items[j].desc[3] == alt_setting_num){
				return interfaces->items[i].alt_settings.items + j;
			}
		}
	}

	return NULL;
}

void *tru_usb_interfaces_find_desc_type(void *interface_desc, uint32_t interface_desc_size, uint8_t desc_type){
	uint32_t pos = 0;
	uint8_t *buf = interface_desc;

	while(pos < interface_desc_size){
		if(buf[0] == 0) return NULL;  // Invalid size field value

		// Is found?
		if(buf[1] == desc_type) return buf;

		// Move to next descriptor
		pos += buf[0];
		buf += buf[0];
	}

	return NULL;
}
