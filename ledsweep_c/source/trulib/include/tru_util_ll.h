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

	Version: 20240317

	Vendor specific low-level utility functions.
*/

#ifndef TRU_UTIL_LL_H
#define TRU_UTIL_LL_H

#include "tru_config.h"

#if(TRU_TARGET == TRU_C5SOC)

#include <stdint.h>

// Support macros
#define TRU_TARGET_TYPE uint32_t
#define TRU_TARGET_CAST(type, ptr) ((type)(ptr))

// Static inline function to read a memory-mapped 32-bit register
static inline uint32_t tru_iom_rd32(uint32_t *src){
	return *(volatile uint32_t *)src;
}

// Static inline function to write a memory-mapped 32-bit register
static inline void tru_iom_wr32(uint32_t *dst, uint32_t val){
	*(volatile uint32_t *)dst = val;
}

static inline uint32_t tru_u8_to_u32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3){
	return (((uint32_t)b3) << 24U) | (((uint32_t)b2) << 16U) | (((uint32_t)b1) << 8U) | b0;
}

// Static inline function to read an unaligned memory-mapped 32-bit register
static inline uint32_t tru_iom_rd32_unaligned(void *src){
	volatile uint8_t * src8 = (uint8_t *)src;
	return tru_u8_to_u32(src8[0], src8[1], src8[2], src8[3]);
}

// Static inline function to write an unaligned memory-mapped 32-bit register
static inline void tru_iom_w32_unaligned(void *dst, uint32_t val)
{
	volatile uint8_t* dst8 = (uint8_t*)dst;
	dst8[0] = (uint8_t)val;
	dst8[1] = (uint8_t)(val >> 8U);
	dst8[2] = (uint8_t)(val >> 16U);
	dst8[3] = (uint8_t)(val >> 24U);
}

static inline uint32_t *tru_align_buffer_up(void *buf, uint32_t alignment){
	if((uint32_t)buf % alignment){
		return (uint32_t *)(((uint32_t)buf & ~(alignment - 1U)) + alignment);
	}

	return buf;
}

#endif

#endif
