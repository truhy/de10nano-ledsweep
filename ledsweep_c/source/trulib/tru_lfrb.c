/*
	MIT License

	Copyright (c) 2025 Truong Hy

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

	Version: 20250417
*/

#include "tru_lfrb.h"

uintptr_t tru_lfrb_remain(tru_lfrb_t *lfrb){
	uintptr_t roffset_v = lfrb->roffset_v;  // Buffer the offset because it can change during this function
	if(lfrb->woffset_v >= roffset_v){  // Read offset is not wrapped?
		return lfrb->len_v - lfrb->woffset_v + roffset_v - 1;  // There is a dummy place holder byte
	}else{
		return roffset_v - lfrb->woffset_v - 1;  // There is a dummy place holder byte
	}
}

uintptr_t tru_lfrb_used(tru_lfrb_t *lfrb){
	uintptr_t woffset_v = lfrb->woffset_v;  // Buffer the offset  because it can change during this function
	if(woffset_v >= lfrb->roffset_v){  // Write offset is not wrapped?
		return woffset_v - lfrb->roffset_v;
	}else{
		return lfrb->len_v - lfrb->roffset_v + woffset_v;
	}
}

void tru_lfrb_woffset_add(tru_lfrb_t *lfrb, uintptr_t len){
	lfrb->woffset = (lfrb->woffset + len) % lfrb->len;
	lfrb->woffset_v = (lfrb->woffset_v + len) % lfrb->len_v;
}

void tru_lfrb_roffset_add(tru_lfrb_t *lfrb, uintptr_t len){
	lfrb->roffset = (lfrb->roffset + len) % lfrb->len;
	lfrb->roffset_v = (lfrb->roffset_v + len) % lfrb->len_v;
}
