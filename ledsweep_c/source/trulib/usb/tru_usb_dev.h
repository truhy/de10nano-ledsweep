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

	USB 2.0 setup device descriptor support.
*/

#ifndef TRU_USB_DEV_H
#define TRU_USB_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// USB device descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint16_t usb_ver;
	uint8_t dev_class;
	uint8_t dev_subclass;
	uint8_t dev_protocol;
	uint8_t mps;
	uint16_t vid;
	uint16_t pid;
	uint16_t product_ver;
	uint8_t manufacturer;
	uint8_t product;
	uint8_t serial;
	uint8_t num_configs;
}tru_usb_dev_desc_t;

// USB device qualifier descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint16_t usb_ver;
	uint8_t dev_class;
	uint8_t dev_subclass;
	uint8_t dev_protocol;
	uint8_t mps;
	uint8_t num_configs;
	uint8_t reserved;
}tru_usb_dev_qual_desc_t;

// Note:
//	The USB 2.0 spec describes a device that must use one and only one of these speed categories (driver configurations):
//		1. A low speed only device.  This supports only the single low speed.
//		2. A full speed only device.  This supports only the single full speed.
//		3. A high speed device.  This must support full speed as the minimum, meaning it must support the two speeds: high and full
//
//		This means that a device can only use one of the above during an operating state (after speed negotiation).  Note only high
//		speed device (option 3) can support "other speed" descriptors.

// USB device descriptor model
typedef struct{
	// Speed dependent descriptor buffers..
	tru_usb_dev_desc_t *hs_dev_desc;  // Pointer to high speed device descriptor
	tru_usb_dev_desc_t *fs_dev_desc;  // Pointer to full speed device descriptor
	tru_usb_dev_desc_t *ls_dev_desc;  // Pointer to low speed device descriptor
	tru_usb_dev_qual_desc_t *hs_dev_qual_desc;  // Pointer to high speed device qualifier descriptor
	tru_usb_dev_qual_desc_t *fs_dev_qual_desc;  // Pointer to full speed device qualifier descriptor
	void *hs_dev_ext;  // Pointer to high speed class specified device extension definition
	void *fs_dev_ext;  // Pointer to full speed class specified device extension definition
	void *ls_dev_ext;  // Pointer to low speed class specified device extension definition

	// Current speed descriptor buffers (will change depending on speed negotiation)..
	tru_usb_dev_desc_t *curr_spd_dev_desc;            // Pointer to current speed device descriptor (speed dependent descriptor)
	tru_usb_dev_qual_desc_t *curr_spd_dev_qual_desc;  // Pointer to current speed device qualifier descriptor (speed dependent descriptor)
	void *curr_spd_dev_ext;  // Pointer to current speed class specified device extension definition
}tru_usb_dev_t;

void tru_usb_dev_init(
	tru_usb_dev_t *dev,
	tru_usb_dev_desc_t *hs_dev_desc,
	tru_usb_dev_desc_t *fs_dev_desc,
	tru_usb_dev_desc_t *ls_dev_desc,
	tru_usb_dev_qual_desc_t *hs_dev_qual_desc,
	tru_usb_dev_qual_desc_t *fs_dev_qual_desc
);
void tru_usb_dev_deinit(tru_usb_dev_t *dev);
void tru_usb_dev_spd_init(tru_usb_dev_t *dev, uint8_t speed);
void tru_usb_dev_spd_deinit(tru_usb_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif
