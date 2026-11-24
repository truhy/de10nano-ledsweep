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

	USb 2.0 end-point support.
*/

#ifndef TRU_USB_EP_H
#define TRU_USB_EP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum{
	TRU_USB_EP_DIR_OUT = 0,
	TRU_USB_EP_DIR_IN  = 1
}tru_usb_ep_dir_t;

// Transfer types
#define TRU_USB_EP_TRANSFER_TYPE_CONTROL     0
#define TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS 1
#define TRU_USB_EP_TRANSFER_TYPE_BULK        2
#define TRU_USB_EP_TRANSFER_TYPE_INTERRUPT   3

// Isochronous synchronization type
#define TRU_USB_EP_ISO_SYNC_TYPE_NONE  0
#define TRU_USB_EP_ISO_SYNC_TYPE_ASYNC 1
#define TRU_USB_EP_ISO_SYNC_TYPE_ADAPT 2
#define TRU_USB_EP_ISO_SYNC_TYPE_SYNC  3

// Usage type
#define TRU_USB_EP_USAGE_TYPE_DATA             0
#define TRU_USB_EP_USAGE_TYPE_FEEDBACK         1
#define TRU_USB_EP_USAGE_TYPE_IMPLICIT_FB_DATA 2
#define TRU_USB_EP_USAGE_TYPE_RES              3

// Isochronous and Interrupt (periodic) additional transaction per microframe
#define TRU_USB_EP_PERIODIC_ADD_TRANS_NONE 0
#define TRU_USB_EP_PERIODIC_ADD_TRANS_ONE  1
#define TRU_USB_EP_PERIODIC_ADD_TRANS_TWO  2
#define TRU_USB_EP_PERIODIC_ADD_TRANS_RES  3

// Low speed MPS
#define TRU_USB_EP_LS_CONTROL_MAX_MPS     8
#define TRU_USB_EP_LS_BULK_MAX_MPS        0
#define TRU_USB_EP_LS_ISOCHRONOUS_MAX_MPS 0
#define TRU_USB_EP_LS_INTERRUPT_MAX_MPS   8

// Full speed MPS
#define TRU_USB_EP_FS_CONTROL_MAX_MPS     64
#define TRU_USB_EP_FS_BULK_MAX_MPS        64
#define TRU_USB_EP_FS_ISOCHRONOUS_MAX_MPS 1023
#define TRU_USB_EP_FS_INTERRUPT_MAX_MPS   64

// High speed MPS
#define TRU_USB_EP_HS_CONTROL_MAX_MPS     64
#define TRU_USB_EP_HS_BULK_MAX_MPS        512
#define TRU_USB_EP_HS_ISOCHRONOUS_MAX_MPS 1024
#define TRU_USB_EP_HS_INTERRUPT_MAX_MPS   1024

typedef union{
	uint8_t val;
	struct{
		uint8_t ep_num :4;
		uint8_t res4_6 :3;
		uint8_t dir    :1;
	}bits;
}tru_usb_ep_desc_address_t;

#define TRU_USB_EP_DESC_ADDRESS_PTR(ptr) ((tru_usb_ep_desc_address_t *)ptr)

typedef union{
	uint8_t val;
	struct{
		uint8_t transfer_type :2;
		uint8_t sync_type     :2;
		uint8_t usage_type    :2;
		uint8_t res6_7        :2;
	}bits;
}tru_usb_ep_desc_attributes_t;

#define TRU_USB_EP_DESC_ATTRIBUTES_PTR(ptr) ((tru_usb_ep_desc_attributes_t *)ptr)

typedef union{
	uint16_t val;
	struct{
		uint16_t mps       :11;
		uint16_t add_trans :2;
		uint16_t res13_15  :3;
	}bits;
}tru_usb_ep_desc_mps_t;

#define TRU_USB_EP_DESC_MPS_PTR(ptr) ((tru_usb_ep_desc_mps_t *)ptr)

// USB device endpoint descriptor
typedef struct __attribute__ ((packed)){
	uint8_t length;
	uint8_t desc_type;
	tru_usb_ep_desc_address_t address;
	tru_usb_ep_desc_attributes_t attributes;
	tru_usb_ep_desc_mps_t mps;
	uint8_t interval;
}tru_usb_ep_desc_t;

#define TRU_USB_EP_DESC_PTR(ptr) ((tru_usb_ep_desc_t *)ptr)

// USB endpoint
typedef struct{
	uint8_t *desc;		// Pointer to USB configuration descriptor
	void *ep_ext;
}tru_usb_ep_t;

// USB endpoints
typedef struct{
	tru_usb_ep_t *items;
	uint8_t num_endpoints;
}tru_usb_eps_t;

void tru_usb_eps_init(tru_usb_eps_t *eps, uint8_t num_eps);
void tru_usb_eps_deinit(tru_usb_eps_t *eps);

#ifdef __cplusplus
}
#endif

#endif
