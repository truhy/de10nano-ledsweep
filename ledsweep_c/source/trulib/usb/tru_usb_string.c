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
*/

#include "usb/tru_usb_string.h"
#include <stddef.h>

void tru_usb_strings_init(tru_usb_strings_t *strings, void *string_entries, uint8_t num_entries){
	strings->items = string_entries;
	strings->num_elements = num_entries;
}

void tru_usb_strings_deinit(tru_usb_strings_t *strings){

}

/*
	Find and return the pointer of the string descriptor with the specified index and language id.
*/
void *tru_usb_strings_find_desc(tru_usb_strings_t *strings, uint8_t index, uint16_t lang_id){
	uint32_t i;

	// Iterate
	for(i = 0; i < strings->num_elements; i++){
		// Is found?
		if(strings->items[i].index == index && strings->items[i].lang_id == lang_id)
			return strings->items[i].desc;
	}

	// None found
	return NULL;
}
