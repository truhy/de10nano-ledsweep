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

	USB 2.0 setup interface descriptor support.
*/

#ifndef TRU_USB_INTERFACE_H
#define TRU_USB_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_ep.h"
#include <stdint.h>

#define TRU_USB_VENDOR_INTERFACE_CLASS 0xff

// USB 2.0 interface descriptor
typedef struct __attribute__ ((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t interface_num;
	uint8_t alt_setting_num;
	uint8_t num_endpoints;
	uint8_t interface_class;
	uint8_t interface_subclass;
	uint8_t interface_protocol;
	uint8_t string_index;
}tru_usb_interface_desc_t;

#define TRU_USB_INTERFACE_DESC_PTR(ptr) ((tru_usb_interface_desc_t *)(ptr))

// Model of interface alternate setting
typedef struct{
	uint8_t config_num;
	uint8_t interface_num;
	uint8_t alt_setting_num;
	uint8_t *desc;         // Pointer to descriptor of this interface alternate setting
	uint32_t desc_offset;  // Offset of this descriptor from the start of the configuration
	uint32_t desc_remain;  // Remaining size from this descriptor to end of the configuration
	uint16_t status;       // Status used by GET_STATUS, CLEAR_FEATURE, SET_FEATURE
	tru_usb_eps_t eps;     // Endpoints
	void *itf_ext;         // USB class specific interface & alternate setting extension
}tru_usb_interface_alt_setting_t;

#define TO_USB_ALTSETTING_PTR(ptr) ((tru_usb_interface_alt_setting_t *)(ptr))

// Model of interface alternate settings.
// Note, since USB 2.0 alternate settings starts numbering at 1, in my model I
// represent the first interface as alternate setting 0.  This makes coding
// easier with only a single structure (an array) that contains the interface
// with its alternate settings.
typedef struct{
	tru_usb_interface_alt_setting_t *items;  // Interface alternate settings items (array)
	uint16_t num_alt_settings;               // Total interface alternate settings including alternate setting 0
	tru_usb_interface_alt_setting_t *prev_alt_setting;
	tru_usb_interface_alt_setting_t *curr_alt_setting;
}tru_usb_interface_alt_settings_t;

// Model of interface
typedef struct{
	uint8_t interface_num;
	tru_usb_interface_alt_settings_t alt_settings;
}tru_usb_interface_t;

// Model of interfaces
typedef struct{
	tru_usb_interface_t *items;  // Interface items (array)
	uint16_t num_interfaces;     // Total interface items
}tru_usb_interfaces_t;

// My own class interface extension header (programming model)
typedef struct{
	uint8_t config_num;
	uint8_t interface_num;
	uint8_t alt_setting_num;
}tru_usb_interface_ext_hdr_t;

// A macro to cast a pointer to a USB class interface extension header pointer
#define TO_USB_INTERFACE_EXT_HDR_PTR(ptr) ((tru_usb_interface_ext_hdr_t *)ptr)

void tru_usb_interfaces_init(tru_usb_interfaces_t *interfaces, uint8_t num_interfaces);
void tru_usb_interfaces_deinit(tru_usb_interfaces_t *interfaces);
void tru_usb_interfaces_alt_init(tru_usb_interfaces_t *interfaces, uint8_t config_num, uint8_t interface_num, uint8_t alt_setting_num, uint8_t num_alt_settings);
void tru_usb_interfaces_alt_deinit(tru_usb_interface_t *interface);
uint8_t tru_usb_interfaces_get_num(void *interface_desc);
uint8_t tru_usb_interfaces_get_altnum(void *interface_desc);
tru_usb_interface_t *tru_usb_interfaces_find_interface(tru_usb_interfaces_t *interfaces, uint8_t interface_num);
tru_usb_interface_alt_setting_t *tru_usb_interfaces_find_alt_setting(tru_usb_interface_t *interface, uint8_t alt_setting_num);
tru_usb_interface_alt_setting_t *tru_usb_interfaces_find_interface_alt_setting(tru_usb_interfaces_t *interfaces, uint8_t interface_num, uint8_t alt_setting_num);
void *tru_usb_interfaces_find_desc_type(void *interface_desc, uint32_t interface_desc_size, uint8_t desc_type);

#ifdef __cplusplus
}
#endif

#endif
