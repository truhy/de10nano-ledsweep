/*
	MIT License

	Copyright (c) 2023 Truong Hy

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

	Version: 20230319

	Utility functions.
*/

#ifndef TRU_UTIL_H
#define TRU_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef TRU_MEMCPY
	#define TRU_MEMCPY memcpy
#endif
#ifndef TRU_UNALIGNED_MEMCPY
	#define TRU_UNALIGNED_MEMCPY unaligned_memcpy
#endif

void unaligned_memcpy(void *dst, const void *src, uint32_t size);
uint16_t le_buf_to_uint16(void *buf);
uint16_t be_buf_to_uint16(void *buf);
uint16_t swap_uint16(uint16_t val);
int16_t swap_int16(int16_t val);
uint32_t swap_uint32(uint32_t val);
int32_t swap_int32(int32_t val);

#endif
