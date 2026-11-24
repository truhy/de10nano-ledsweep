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

	USB 2.0 device class HID (Human Interface Device) definitions.
*/

#ifndef TRU_USB_HID_DEF_H
#define TRU_USB_HID_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_pd_info.h"
#include "usb/synopsys/tru_dwc2_sgdma.h"
#include <stdint.h>

// Interface class code
#define TRU_USB_HID_INTERFACE_CLASS 0x03

// Interface subclass code
#define TRU_USB_HID_INTERFACE_SUBCLASS_UNDEFINED 0x00
#define TRU_USB_HID_INTERFACE_SUBCLASS_BOOT      0x01

// Interface protocol code
#define TRU_USB_HID_INTERFACE_PROTOCOL_NONE     0x00
#define TRU_USB_HID_INTERFACE_PROTOCOL_KEYBOARD 0x01
#define TRU_USB_HID_INTERFACE_PROTOCOL_MOUSE    0x02

// HID class descriptor type
#define TRU_USB_HID_DESC_TYPE_HID      0x21
#define TRU_USB_HID_DESC_TYPE_REPORT   0x22
#define TRU_USB_HID_DESC_TYPE_PHYSICAL 0x23

// HID specific setup request
#define TRU_USB_HID_REQ_GET_REPORT   0x01
#define TRU_USB_HID_REQ_GET_IDLE     0x02
#define TRU_USB_HID_REQ_GET_PROTOCOL 0x03
#define TRU_USB_HID_REQ_SET_REPORT   0x09
#define TRU_USB_HID_REQ_SET_IDLE     0x0a
#define TRU_USB_HID_REQ_SET_PROTOCOL 0x0b

// Report type for setup GET_REPORT and SET_REPORT requests
#define TRU_USB_HID_REQ_REPORT_TYPE_INPUT   0x01
#define TRU_USB_HID_REQ_REPORT_TYPE_OUTPUT  0x02
#define TRU_USB_HID_REQ_REPORT_TYPE_FEATURE 0x03

// Report descriptor Main items mask (upper 6 bits)
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_INPUT_MASK          0x80
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_OUTPUT_MASK         0x90
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_FEATURE_MASK        0xb0
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_MASK     0xa0
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_END_COLLECTION_MASK 0xc0

// Report descriptor Global items mask (upper 6 bits)
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_USAGE_PAGE_MASK    0x04
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_LOGICAL_MIN_MASK   0x14
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_LOGICAL_MAX_MASK   0x24
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_PHYSICAL_MIN_MASK  0x34
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_PHYSICAL_MAX_MASK  0x44
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_UNIT_EXPONENT_MASK 0x54
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_UNIT_MASK          0x64
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_REPORT_SIZE_MASK   0x74
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_REPORT_ID_MASK     0x84
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_REPORT_COUNT_MASK  0x94
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_PUSH_MASK          0xa4
#define TRU_USB_HID_REPORT_DESC_GLOBAL_ITEM_POP_MASK           0xb4

// Report descriptor Local items mask (upper 6 bits)
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_USAGE_MASK            0x02
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_USAGE_MIN_MASK        0x12
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_USAGE_MAX_MASK        0x22
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_DESIGNATOR_INDEX_MASK 0x32
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_DESIGNATOR_MIN_MASK   0x42
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_DESIGNATOR_MAX_MASK   0x52
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_STRING_INDEX_MASK     0x72
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_STRING_MIN_MASK       0x82
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_STRING_MAX_MASK       0x92
#define TRU_USB_HID_REPORT_DESC_LOCAL_ITEM_DELIMITER_MASK        0xa2

// Report descriptor long item code
#define TRU_USB_HID_REPORT_DESC_LONG_ITEM 0xfe

// Report descriptor Main items Collection code
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_DATA_PHYSICAL       0x00
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_DATA_APPLICATION    0x01
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_DATA_LOGICAL        0x02
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_DATA_REPORT         0x03
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_DATA_NAMED_ARRAY    0x04
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_DATA_USAGE_SWITCH   0x05
#define TRU_USB_HID_REPORT_DESC_MAIN_ITEM_COLLECTION_DATA_USAGE_MODIFIER 0x06

#define TRU_USB_HID_REPORT_DESC_PARSER_STACK_ITEM_SIZE 8
#define TRU_USB_HID_REPORT_DESC_PARSER_STACK_MAX_COUNT 256

// HID descriptor - in my opinion, this is a header (first 6 bytes)
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint16_t release_num;
	uint8_t country_code;
	uint8_t num_descriptors;
	uint8_t report_desc_type;  // This is always HID report descriptor type
	uint16_t report_desc_len;  // This is always HID report descriptor length
}tru_usb_hid_desc_t;

// =============================
// Programming model definitions
// =============================

// ============================================
// Programming model HID descriptor definitions
// ============================================

// HID descriptor info (programming model)
typedef struct{
	uint8_t *desc;  // Raw HID descriptor
	uint32_t size;  // Size of descriptor
}tru_usb_hid_info_t;

// HID descriptor info set (programming model)
typedef struct{
	tru_usb_hid_info_t *items; // Array
	uint32_t num_elements;     // Number of array elements
}tru_usb_hid_info_set_t;

// ========================================
// Programming model HID report definitions
// ========================================

typedef struct{
	uint8_t item[TRU_USB_HID_REPORT_DESC_PARSER_STACK_ITEM_SIZE];
}tru_usb_hid_report_desc_parser_stack_item_t;

// HID report info (programming model)
typedef struct{
	uint8_t id;
	uint8_t *report;
	uint32_t size_bits;
	uint32_t size;
	uint16_t idle_rate;
}tru_usb_hid_report_info_t;

// HID reports info set (programming model)
typedef struct{
	tru_usb_hid_report_info_t *inputs;
	tru_usb_hid_report_info_t *outputs;
	tru_usb_hid_report_info_t *features;
	uint8_t num_input_reports;
	uint8_t num_output_reports;
	uint8_t num_feature_reports;
	tru_dwc2_dev_sgdma_desc_reg_t *epi_dma_descs;  // DMA Scatter Gather descriptor list for report IN end-point
	tru_dwc2_dev_sgdma_desc_reg_t *epo_dma_descs;  // DMA Scatter Gather descriptor list for report OUT end-point
	uint32_t epi_num_descs;  // DMA Scatter Gather number of descriptor list items for report IN end-point
	uint32_t epo_num_descs;  // DMA Scatter Gather number of descriptor list items for report OUT end-point

	uint8_t curr_input_index;
	uint8_t curr_output_index;
	uint8_t epin_num;
	uint8_t epout_num;
}tru_usb_hid_reports_info_set_t;

// =======================================================
// Programming model class interface extension definitions
// =======================================================

// HID class interface extension (programming model)
typedef struct{
	tru_usb_interface_ext_hdr_t ext_hdr;
	tru_usb_hid_desc_t *hid_desc;
	tru_usb_hid_info_t report_desc_info;
	tru_usb_hid_info_set_t physical_desc_info_set;
	tru_usb_hid_reports_info_set_t *reports_info_set;
}tru_usb_hid_interface_ext_t;

// A macro to cast a pointer to a HID class interface extension pointer
#define TO_HID_INTERFACE_EXT_PTR(ptr) ((tru_usb_hid_interface_ext_t *)ptr)

// HID  peripheral device descriptor set
typedef struct{
	// ====================================
	// Common peripheral device descriptors
	// ====================================
	tru_usb_pd_info_t *pd;

	// =========================================
	// HID class interface extension descriptors
	// =========================================

	tru_usb_hid_interface_ext_t *hs_hid_interface_exts;
	uint16_t hs_hid_interface_exts_num_entries;

	tru_usb_hid_interface_ext_t *fs_hid_interface_exts;
	uint16_t fs_hid_interface_exts_num_entries;

	tru_usb_hid_interface_ext_t *ls_hid_interface_exts;
	uint16_t ls_hid_interface_exts_num_entries;
}tru_usb_hid_pd_info_t;

#ifdef __cplusplus
}
#endif

#endif
