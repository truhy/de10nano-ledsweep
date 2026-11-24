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

	USB 2.0 setup request support.
*/

#ifndef TRU_USB_SETUP_H
#define TRU_USB_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "synopsys/tru_dwc2_sgdma_def.h"
#include <stdbool.h>
#include <stdint.h>

// Setup request direction
#define TRU_USB_SETUP_DIR_HOST2DEV 0
#define TRU_USB_SETUP_DIR_DEV2HOST 1

// Setup request type
#define TRU_USB_SETUP_TYPE_STANDARD 0
#define TRU_USB_SETUP_TYPE_CLASS    1
#define TRU_USB_SETUP_TYPE_VENDOR   2
#define TRU_USB_SETUP_TYPE_RESERVED 3

// Setup request recipient
#define TRU_USB_SETUP_RCP_DEVICE    0
#define TRU_USB_SETUP_RCP_INTERFACE 1
#define TRU_USB_SETUP_RCP_ENDPOINT  2
#define TRU_USB_SETUP_RCP_OTHER     3

// Standard setup request codes (commands)
#define TRU_USB_SETUP_REQ_GET_STATUS        0
#define TRU_USB_SETUP_REQ_CLEAR_FEATURE     1
#define TRU_USB_SETUP_REQ_RESERVED2         2
#define TRU_USB_SETUP_REQ_SET_FEATURE       3
#define TRU_USB_SETUP_REQ_RESERVED4         4
#define TRU_USB_SETUP_REQ_SET_ADDRESS       5
#define TRU_USB_SETUP_REQ_GET_DESCRIPTOR    6
#define TRU_USB_SETUP_REQ_SET_DESCRIPTOR    7
#define TRU_USB_SETUP_REQ_GET_CONFIGURATION 8
#define TRU_USB_SETUP_REQ_SET_CONFIGURATION 9
#define TRU_USB_SETUP_REQ_GET_INTERFACE     10
#define TRU_USB_SETUP_REQ_SET_INTERFACE     11
#define TRU_USB_SETUP_REQ_SYNCH_FRAME       12

// Setup request size
#define TRU_USB_SETUP_SIZE 8

// Setup data stage transfer size limit
//#define TRU_USB_SETUP_DATA_XFER_SIZE_LIMIT 65532
#define TRU_USB_SETUP_DATA_XFER_SIZE_LIMIT 512

typedef union{
	uint8_t val;
	struct{
		uint8_t recipient :5;
		uint8_t type      :2;
		uint8_t dir       :1;
	}bits;
}tru_usb_request_type_t;

// USB setup request packet
typedef struct{
	tru_usb_request_type_t type;
	uint8_t code;
	uint16_t value;
	uint16_t index;
	uint16_t length;
}tru_usb_request_t;

#define TRU_USB_REQUEST_PTR(ptr) ((tru_usb_request_t *)(ptr))

// USB setup model
typedef struct{
	uint8_t *epin0_buf;
	uint32_t epin0_bufsize;
	uint8_t *epout0_buf;
	uint32_t epout0_bufsize;
	tru_dwc2_dev_sgdma_desc_reg_t *epin0_sgdma_descs;
	tru_dwc2_dev_sgdma_desc_reg_t *epout0_sgdma_descs;
	tru_usb_request_t request;  // Parsed setup request
	bool is_handled;            // Setup request is handled
}tru_usb_setup_t;

typedef bool (*tru_usb_setup_callback_t)(void *context);

void tru_usb_setup_init(tru_usb_setup_t *setup, tru_usb_setup_callback_t callback_setup);
void tru_usb_setup_deinit(tru_usb_setup_t *setup);
void tru_usb_setup_parse(tru_usb_setup_t *setup, void *xfer_buf);

#ifdef __cplusplus
}
#endif

#endif
