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

	Lock-free ring buffer (aka circular buffer).

	We can have a lock-free and wait-free circular buffer by making use of the
	atomic read and write access on 32-bit variables on the ARM v7-M
	Architecture, but we still have to follow the rule: only the reader writes
	to the read pointer, and only the writer writes to the write pointer, i.e.
	a variable must have only a single writer, but can have multiple readers.

	Note, the remaining space is subtracted by 1 to reserve an empty space which
	prevents the write offset from reaching the read offset.  This gives us the
	conditions obeying the rules above:
		1. The queue is empty when read_offset == write_offset
		2. The queue is full when write_offset+1 == read_offset
*/

#ifndef TRU_LFRB_H
#define TRU_LFRB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct{
	uintptr_t len;
	uintptr_t woffset;
	uintptr_t roffset;
	
	uintptr_t len_v;
	uintptr_t woffset_v;
	uintptr_t roffset_v;
}tru_lfrb_t;

uintptr_t tru_lfrb_remain(tru_lfrb_t *lfrb);
uintptr_t tru_lfrb_used(tru_lfrb_t *lfrb);
void tru_lfrb_woffset_add(tru_lfrb_t *lfrb, uintptr_t len);
void tru_lfrb_roffset_add(tru_lfrb_t *lfrb, uintptr_t len);

#ifdef __cplusplus
}
#endif

#endif
