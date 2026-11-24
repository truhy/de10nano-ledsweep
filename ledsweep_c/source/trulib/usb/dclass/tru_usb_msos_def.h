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

	Microsoft OS (MSOS) USB 2.0 descriptor definitions.

	Notes:
		Should contain only the fixed size definitions.
		Variable length definitions are usually specific to an instance so try
		to define those at the instance level instead.
*/

#ifndef TRU_USB_MSOS_DEF_H
#define TRU_USB_MSOS_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_pd_info.h"
#include <stdint.h>

#define TRU_USB_MSOS_DESC_TYPE 0x03
#define TRU_USB_MSOS_VENDOR_CODE 0x50

// ===============================
// Microsoft OS string definitions
// ===============================

#define TRU_USB_MSOS_STRING_DESC_INDEX 0xee

// Microsoft OS vendor string descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint16_t signature[7];
	uint8_t vendor_code;
	uint8_t padding;
}tru_usb_msos_string_desc_vendor_t;

// ==========================================================
// Microsoft OS Extended Compatibility ID feature definitions
// ==========================================================

#define TRU_USB_MSOS_ECID_FEATURE_INDEX 0x0004

// Microsoft OS Extended Compatibility ID feature descriptor header
typedef struct __attribute__((packed)){
	uint32_t length;
	uint16_t bcd_ver;
	uint16_t index;
	uint8_t count;
	uint8_t  reserved[7];
}tru_usb_msos_ecid_feature_hdr_desc_t;

// Microsoft OS Extended Compatibility ID feature descriptor function section
typedef struct __attribute__((packed)){
	uint8_t first_intf_num;
	uint8_t reserved;
	uint8_t cid[8];
	uint8_t scid[8];
	uint8_t reserved2[6];
}tru_usb_msos_ecid_feature_func_section_desc_t;

// ====================================================
// Microsoft OS Extended Properties feature definitions
// ====================================================

#define TRU_USB_MSOS_EP_FEATURE_INDEX 0x0005

// Microsoft OS Extended Properties feature descriptor header
typedef struct __attribute__((packed)){
	uint32_t length;
	uint16_t bcd_ver;
	uint16_t index;
	uint16_t count;
}tru_usb_msos_ep_feature_hdr_desc_t;

// =============================
// Programming model definitions
// =============================

// ===========================================
// Programming model MSOS Extended definitions
// ===========================================

// Microsoft OS Extended Compatibility ID feature (programming model)
typedef struct{
	tru_usb_msos_ecid_feature_hdr_desc_t *hdr;
	tru_usb_msos_ecid_feature_func_section_desc_t *sections;  // 0 or more Function sections
}tru_usb_msos_ecid_feature_t;

// Microsoft OS Extended Properties feature (programming model)
typedef struct{
	tru_usb_msos_ep_feature_hdr_desc_t *hdr;
	uint8_t *sections;  // 0 or more Property sections
}tru_usb_msos_ep_feature_t;

// =========================================================
// Programming model MSOS class device extension definitions
// =========================================================

// MSOS class device extension (programming model)
typedef struct{
	tru_usb_msos_ecid_feature_t *ecid_feature;
	tru_usb_msos_ep_feature_t *ep_feature;
}tru_usb_msos_dev_ext_t;

// A macro to cast a pointer to a MSOS class device extension pointer
#define TO_MSOS_DEV_EXT_PTR(ptr) ((tru_usb_msos_dev_ext_t *)ptr)

// =======================================================
// Programming model class interface extension definitions
// =======================================================

// MSOS class interface extension (programming model)
typedef struct{
	tru_usb_interface_ext_hdr_t ext_hdr;
	tru_usb_msos_ep_feature_t *ep_feature;
}tru_usb_msos_interface_ext_t;

// A macro to cast a pointer to a MSOS class interface extension pointer
#define TO_MSOS_INTERFACE_EXT_PTR(ptr) ((tru_usb_msos_interface_ext_t *)ptr)

// MSOS peripheral device descriptor set (programming model)
typedef struct{
	// ====================================
	// Common peripheral device descriptors
	// ====================================
	tru_usb_pd_info_t *pd;

	// =======================================
	// MSOS class device extension descriptors
	// =======================================

	tru_usb_msos_dev_ext_t *hs_msos_dev_ext;
	tru_usb_msos_dev_ext_t *fs_msos_dev_ext;
	tru_usb_msos_dev_ext_t *ls_msos_dev_ext;

	// ==========================================
	// MSOS class interface extension descriptors
	// ==========================================

	tru_usb_msos_interface_ext_t *hs_msos_interface_exts;
	uint16_t hs_msos_interface_exts_num_entries;

	tru_usb_msos_interface_ext_t *fs_msos_interface_exts;
	uint16_t fs_msos_interface_exts_num_entries;

	tru_usb_msos_interface_ext_t *ls_msos_interface_exts;
	uint16_t ls_msos_interface_exts_num_entries;
}tru_usb_msos_pd_info_t;

#ifdef __cplusplus
}
#endif

#endif
