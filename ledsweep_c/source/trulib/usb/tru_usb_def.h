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

	USB 2.0 definitions.
*/

#ifndef TRU_USB_DEF_H
#define TRU_USB_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

// USB standard descriptor type
#define TRU_USB_DESC_TYPE_DEVICE             1
#define TRU_USB_DESC_TYPE_CONFIG             2
#define TRU_USB_DESC_TYPE_STRING             3
#define TRU_USB_DESC_TYPE_INTERFACE          4
#define TRU_USB_DESC_TYPE_ENDPOINT           5
#define TRU_USB_DESC_TYPE_DEVICE_QUAL        6
#define TRU_USB_DESC_TYPE_OTHER_SPEED_CONFIG 7
#define TRU_USB_DESC_TYPE_INTERFACE_POWER    8

// Device class code
#define TRU_USB_DEVICE_CLASS_NULL  0x00
#define TRU_USB_DEVICE_CLASS_AUDIO 0x01
#define TRU_USB_DEVICE_CLASS_CDC   0x02
#define TRU_USB_DEVICE_CLASS_HID   0x03
#define TRU_USB_DEVICE_CLASS_MISC  0xef

// Device subclass code
#define TRU_USB_DEVICE_SUBCLASS_NULL  0x00
#define TRU_USB_DEVICE_SUBCLASS_MULTI 0x02

// Device protocol code
#define TRU_USB_DEVICE_PROTOCOL_NULL 0x00
#define TRU_USB_DEVICE_PROTOCOL_IAD  0x01

#ifdef __cplusplus
}
#endif

#endif
