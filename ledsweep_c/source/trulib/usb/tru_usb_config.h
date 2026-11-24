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

	USB 2.0 setup configuration descriptor support.

	The USB 2.0 spec defines a configuration descriptor, but it is really a
	header for a configuration set of other descriptors.  It is confusingly
	named configuration instead of configuration header.

	For the USB 2.0 spec we will distinguish them as:
	configuration set = configuration descriptor + other descriptors within the configuration
	configuration = configuration descriptor only

	For our application model we will refer to them as:
	configuration = configuration descriptor + other descriptors within the configuration
	configuration header = configuration descriptor only
*/

#ifndef TRU_USB_CONFIG_H
#define TRU_USB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_interface.h"
#include <stdbool.h>
#include <stdint.h>

#define TRU_USB_CONFIG_DESC_ATTRIB_RES_D7        0x80
#define TRU_USB_CONFIG_DESC_ATTRIB_SELF_POWERED  0x40
#define TRU_USB_CONFIG_DESC_ATTRIB_REMOTE_WAKEUP 0x20

#define TRU_USB_CONFIG_DESC_MAXPOWER_MAX          250
#define TRU_USB_CONFIG_DESC_MAXPOWER_SELF_POWERED 0

// USB 2.0 configuration descriptor (not a set, think of this as a header)
typedef struct __attribute__ ((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint16_t total_length;
	uint8_t num_interfaces;
	uint8_t config_num;
	uint8_t string_index;
	uint8_t attributes;
	uint8_t max_power;
}tru_usb_config_desc_t;

#define TRU_USB_CONFIG_DESC_PTR(ptr) ((tru_usb_config_desc_t *)(ptr))

// Model of configuration descriptor set
typedef struct{
	uint8_t *desc;  // Raw configuration descriptor
	uint32_t size;  // Size of the configuration descriptor set
}tru_usb_config_desc_set_t;

// Model of configuration descriptor set
typedef struct{
	uint8_t config_num;
	uint8_t *desc;                    // Pointer to configuration descriptor set item
	uint32_t size;                    // Size of the configuration descriptor set
	tru_usb_interfaces_t interfaces;  // Interfaces within the configuration
}tru_usb_config_t;

// Model of configurations for a single speed
typedef struct{
	tru_usb_config_t *items;  // Array
	uint32_t num_elements;    // Number of array elements
}tru_usb_spd_configs_t;

// Model of all configurations
typedef struct{
	// Speed dependent configuration descriptors..
	tru_usb_spd_configs_t hs_configs;  // High speed configurations (speed dependent descriptors)
	tru_usb_spd_configs_t fs_configs;  // Full speed configurations (speed dependent descriptors)
	tru_usb_spd_configs_t ls_configs;  // Low speed configurations (speed dependent descriptors)

	// Active configurations..
	tru_usb_spd_configs_t *curr_spd_configs;   // Pointer to current speed configurations
	tru_usb_spd_configs_t *other_spd_configs;  // Pointer to current other speed configurations

	// Other parameters..
	tru_usb_config_t *prev_config;
	tru_usb_config_t *curr_config;
}tru_usb_configs_t;

void tru_usb_configs_init(
	tru_usb_configs_t *configs,
	tru_usb_config_desc_set_t *hs_desc_info_items, uint32_t hs_desc_info_items_num_entries,
	tru_usb_config_desc_set_t *fs_desc_info_items, uint32_t fs_desc_info_items_num_entries,
	tru_usb_config_desc_set_t *ls_descs_info_item, uint32_t ls_desc_info_items_num_entries
);
void tru_usb_configs_deinit(tru_usb_configs_t *configs);
void tru_usb_configs_init_spd_config(tru_usb_spd_configs_t *spd_configs, tru_usb_config_desc_set_t *desc_info_items, uint32_t desc_info_items_num_entries);
void tru_usb_configs_deinit_spd_config(tru_usb_spd_configs_t *spd_configs);
void tru_usb_configs_desc_type_to_other(tru_usb_spd_configs_t *spd_configs, uint32_t desc_buf_size);
void tru_usb_configs_init_spd(tru_usb_configs_t *configs, uint8_t speed);
void tru_usb_configs_deinit_spd(tru_usb_configs_t *configs);
uint8_t tru_usb_configs_get_num(void *config_desc);
tru_usb_config_t *tru_usb_configs_find_config(tru_usb_spd_configs_t *spd_configs, uint8_t config_num);

#ifdef __cplusplus
}
#endif

#endif
