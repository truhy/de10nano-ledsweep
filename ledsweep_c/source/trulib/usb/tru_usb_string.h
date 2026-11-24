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

	USB 2.0 setup string descriptor support.
*/

#ifndef TRU_USB_STRING_H
#define TRU_USB_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// String descriptor information
typedef struct{
	uint8_t index;
	uint16_t lang_id;
	void *desc;		// Pointer to string descriptor
}tru_usb_string_desc_info_t;

// Array of string descriptor pointers
typedef struct{
	tru_usb_string_desc_info_t *items;	// Array
	uint16_t num_elements;				// Number of array elements
}tru_usb_strings_t;

void tru_usb_strings_init(tru_usb_strings_t *strings, void *string_entries, uint8_t num_entries);
void tru_usb_strings_deinit(tru_usb_strings_t *strings);
void *tru_usb_strings_find_desc(tru_usb_strings_t *strings, uint8_t index, uint16_t lang_id);

#ifdef __cplusplus
}
#endif

#endif
