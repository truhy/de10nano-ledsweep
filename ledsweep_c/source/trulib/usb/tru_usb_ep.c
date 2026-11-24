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

#include "usb/tru_usb_ep.h"
#include <stdlib.h>

void tru_usb_eps_init(tru_usb_eps_t *eps, uint8_t num_eps){
	if(num_eps){
		eps->items = malloc(num_eps * sizeof(tru_usb_ep_t));
		if(eps->items){
			eps->num_endpoints = num_eps;
		}else{
			eps->num_endpoints = 0;
		}
	}else{
		eps->items = NULL;
		eps->num_endpoints = 0;
	}

	tru_usb_ep_t *ep = eps->items;
	for(uint16_t i = 0; i < num_eps; i++){
		ep->desc = NULL;
		ep->ep_ext = NULL;
		ep++;
	}
}

void tru_usb_eps_deinit(tru_usb_eps_t *eps){
	free(eps->items);
	eps->items = NULL;
}
