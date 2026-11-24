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

	USB 2.0 Interface Association Descriptor (IAD) definitions.

	References:
		Document: Interface Association Descriptors
		File: InterfaceAssociationDescriptor_ecn.pdf from usb_20_20240927.zip (USB 2.0 Specification 09/27/2024)
		Document: USB Interface Association Descriptor Device Class Code and Use Model Revision 1.0
		File: iadclasscode_r10.pdf
*/

#ifndef TRU_USB_IAD_DEF_H
#define TRU_USB_IAD_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define TRU_USB_IAD_DESC_TYPE 11

// Standard Interface Association Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t first_interface;
	uint8_t interface_count;
	uint8_t function_class;
	uint8_t function_subclass;
	uint8_t function_protocol;
	uint8_t function;
}tru_usb_iad_desc_t;

#ifdef __cplusplus
}
#endif

#endif
