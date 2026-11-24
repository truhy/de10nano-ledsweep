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

	USB 2.0 Peripheral Device support.
*/

#ifndef TRU_USB_PD_INFO_H
#define TRU_USB_PD_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usb/tru_usb_dev.h"
#include "usb/tru_usb_config.h"
#include "usb/tru_usb_string.h"

// Peripheral device descriptor set
typedef struct{
	// ==================
	// Device descriptors
	// ==================

	tru_usb_dev_desc_t *hs_dev_desc;
	tru_usb_dev_desc_t *fs_dev_desc;
	tru_usb_dev_desc_t *ls_dev_desc;

	// ============================
	// Device qualifier descriptors
	// ============================

	tru_usb_dev_qual_desc_t *hs_dev_qual_desc;
	tru_usb_dev_qual_desc_t *fs_dev_qual_desc;

	// =========================
	// Configuration descriptors
	// =========================

	tru_usb_config_desc_set_t *hs_config_descs;
	uint32_t hs_config_descs_num_entries;

	tru_usb_config_desc_set_t *fs_config_descs;
	uint32_t fs_config_descs_num_entries;

	tru_usb_config_desc_set_t *ls_config_descs;
	uint32_t ls_config_descs_num_entries;

	// ==================
	// String descriptors
	// ==================

	tru_usb_string_desc_info_t *string_descs;
	uint16_t string_descs_num_entries;
}tru_usb_pd_info_t;

#ifdef __cplusplus
}
#endif

#endif
