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

	Version: 20240514

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

// Convert little-endian buffer to u16
static inline uint16_t le_buf_to_u16(void *buf){
	return (((unsigned char *)buf)[1] << 8) | ((unsigned char *)buf)[0];
}

// Convert little-endian buffer to u32
static inline uint32_t le_buf_to_u32(void *buf){
	return (((unsigned char *)buf)[3] << 24) | (((unsigned char *)buf)[2] << 16) | (((unsigned char *)buf)[1] << 8) | ((unsigned char *)buf)[0];
}

// Convert big-endian buffer to u16
static inline uint16_t be_buf_to_u16(void *buf){
	return (((unsigned char *)buf)[0] << 8) | ((unsigned char *)buf)[1];
}

// Convert big-endian buffer to u32
static inline uint32_t be_buf_to_u32(void *buf){
	return (((unsigned char *)buf)[0] << 24) | (((unsigned char *)buf)[1] << 16) | (((unsigned char *)buf)[2] << 8) | ((unsigned char *)buf)[3];
}

// Byte swap u16
static inline uint16_t swap_u16(uint16_t val){
    return (val << 8) | (val >> 8 );
}

// Byte swap i16
static inline int16_t swap_int16(int16_t val){
    return (val << 8) | ((val >> 8) & 0xFF);
}

// Byte swap u32
static inline uint32_t swap_u32(uint32_t val){
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

// Byte swap i32
static inline int32_t swap_i32(int32_t val){
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

#endif
