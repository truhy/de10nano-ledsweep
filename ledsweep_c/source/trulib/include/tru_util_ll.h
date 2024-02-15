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

	Version: 20240211

	Vendor specific low-level utility functions.
*/

#ifndef TRU_UTIL_LL_H
#define TRU_UTIL_LL_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_C5SOC)

#include <stdint.h>

// Support macros
#define TRU_TARGET_REG_TYPE uint32_t
#define TRU_TARGET_CAST(type, ptr) ((type)(ptr))
#define tru_mem_rd_word(src_addr) (*TRU_TARGET_CAST(volatile TRU_TARGET_REG_TYPE *, (src_addr)))
#define tru_mem_wr_word(dst_addr, src_addr) (*TRU_TARGET_CAST(volatile TRU_TARGET_REG_TYPE *, (dst_addr)) = (src_addr))

#endif

#endif
