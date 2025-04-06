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

	Version: 20250318

	Vendor specific low-level utility functions.
*/

#ifndef TRU_UTIL_LL_H
#define TRU_UTIL_LL_H

#include "tru_config.h"
#include <stdint.h>

// Divide and align up
#define DIV_CEIL(_n, _d) (((_n) + (_d) - 1) / (_d))

// Integer alignment
#define TRU_INT_ALIGN_DN(_n, _align) ((_n) & ~((_align) - 1))
#define TRU_INT_ALIGN_UP(_n, _align) (((_n) + (_align) - 1) & ~((_align) - 1))

#define TO_U32PTR(_ptr) ((uint32_t *)_ptr)

#define U32_B0(_n) ((uint8_t) (( (uint32_t)_n)        & 0x000000ff))
#define U32_B1(_n) ((uint8_t) ((((uint32_t)_n) >>  8) & 0x000000ff))
#define U32_B2(_n) ((uint8_t) ((((uint32_t)_n) >> 16) & 0x000000ff))
#define U32_B3(_n) ((uint8_t) ((((uint32_t)_n) >> 24) & 0x000000ff))

static inline uint32_t u32_align_up(uint32_t n, uint32_t a){
	//return DIV_CEIL(n, a) * a;
	return TRU_INT_ALIGN_UP(n, a);
}

// Convert little-endian buffer to u16
static inline uint16_t buf_le_to_u16(void *src){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	return (((uint8_t *)src)[1] << 8) | ((uint8_t *)src)[0];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uint32_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return U32_B2(src32[0]) << 8 | U32_B1(src32[0]);
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return U32_B3(src32[0]) << 8 | U32_B2(src32[0]);
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return U32_B0(src32[1]) << 8 | U32_B3(src32[0]);
			break;
		default:
			return U32_B1(TO_U32PTR(src)[0]) << 8 | U32_B0(TO_U32PTR(src)[0]);
	}
#endif
}

// Convert little-endian buffer to u32
static inline uint32_t buf_le_to_u32(void *src){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	return (((unsigned char *)src)[3] << 24) | (((unsigned char *)src)[2] << 16) | (((unsigned char *)src)[1] << 8) | ((unsigned char *)src)[0];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uint32_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return U32_B0(src32[1]) << 24 | U32_B3(src32[0]) << 16 | U32_B2(src32[0]) << 8 | U32_B1(src32[0]);
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return U32_B1(src32[1]) << 24 | U32_B0(src32[1]) << 16 | U32_B3(src32[0]) << 8 | U32_B2(src32[0]);
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return U32_B2(src32[1]) << 24 | U32_B1(src32[1]) << 16 | U32_B0(src32[1]) << 8 | U32_B3(src32[0]);
			break;
		default:
			return U32_B3(TO_U32PTR(src)[0]) << 24 | U32_B2(TO_U32PTR(src)[0]) << 16 | U32_B1(TO_U32PTR(src)[0]) << 8 | U32_B0(TO_U32PTR(src)[0]);
	}
#endif
}

// Convert big-endian buffer to u16
static inline uint16_t buf_be_to_u16(void *src){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	return (((uint8_t *)src)[0] << 8) | ((uint8_t *)src)[1];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uint32_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return U32_B1(src32[0]) << 8 | U32_B2(src32[0]);
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return U32_B2(src32[0]) << 8 | U32_B3(src32[0]);
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return U32_B3(src32[0]) << 8 | U32_B0(src32[1]);
			break;
		default:
			return U32_B0(TO_U32PTR(src)[0]) << 8 | U32_B1(TO_U32PTR(src)[0]);
	}
#endif
}

// Convert big-endian buffer to u32
static inline uint32_t buf_be_to_u32(void *src){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	return (((unsigned char *)src)[0] << 24) | (((unsigned char *)src)[1] << 16) | (((unsigned char *)src)[2] << 8) | ((unsigned char *)src)[3];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uint32_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return U32_B1(src32[0]) << 24 | U32_B2(src32[0]) << 16 | U32_B3(src32[0]) << 8 | U32_B0(src32[1]);
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return U32_B2(src32[0]) << 24 | U32_B3(src32[0]) << 16 | U32_B0(src32[1]) << 8 | U32_B1(src32[1]);
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return U32_B3(src32[0]) << 24 | U32_B0(src32[1]) << 16 | U32_B1(src32[1]) << 8 | U32_B2(src32[1]);
			break;
		default:
			return U32_B0(TO_U32PTR(src)[0]) << 24 | U32_B1(TO_U32PTR(src)[0]) << 16 | U32_B2(TO_U32PTR(src)[0]) << 8 | U32_B3(TO_U32PTR(src)[0]);
	}
#endif
}

// Byte swap u16
static inline uint16_t swap_u16(uint16_t num){
    return (num << 8) | (num >> 8 );
}

// Byte swap i16
static inline int16_t swap_int16(int16_t num){
    return (num << 8) | ((num >> 8) & 0x00ff);
}

// Byte swap u32
static inline uint32_t swap_u32(uint32_t num){
    num = ((num << 8) & 0xff00ff00 ) | ((num >> 8) & 0x00ff00ff );
    return (num << 16) | (num >> 16);
}

// Byte swap i32
static inline int32_t swap_i32(int32_t num){
    num = ((num << 8) & 0xff00ff00) | ((num >> 8) & 0x00ff00ff );
    return (num << 16) | ((num >> 16) & 0x0000ffff);
}

static inline uint32_t u8_to_u32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0){
	return (((uint32_t)b3) << 24) | (((uint32_t)b2) << 16) | (((uint32_t)b1) << 8) | b0;
}

// Static inline function to read an unaligned memory
static inline uint8_t tru_rd8_unaligned(void *src){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	return ((uint8_t *)src)[0];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uint32_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return U32_B1(src32[0]);
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return U32_B2(src32[0]);
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return U32_B3(src32[0]);
			break;
		default:
			return ((uint8_t *)src)[0];
	}
#endif
}

// Static inline function to write an unaligned memory
static inline void tru_w8_unaligned(void *dst, uint8_t num){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	((uint8_t *)dst)[0] = num;
#else
	// Restrict to 32-bit access
	uint32_t *dst32;
	switch((uint32_t)dst & 0x3){
		case 1:
			dst32 = (uint32_t *)((uint8_t *)dst - 1);
			dst32[0] = (dst32[0] & 0xffff00ff) | num << 8;
			break;
		case 2:
			dst32 = (uint32_t *)((uint8_t *)dst - 2);
			dst32[0] = (dst32[0] & 0xff00ffff) | num << 16;
			break;
		case 3:
			dst32 = (uint32_t *)((uint8_t *)dst - 3);
			dst32[0] = (dst32[0] & 0x00ffffff) | num << 24;
			break;
		default:
			((uint32_t *)dst)[0] = (((uint32_t *)dst)[0] & 0xffffff00) | num;
	}
#endif
}

// Static inline function to read an unaligned memory
static inline uint32_t tru_rd32_unaligned(void *src){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	return u8_to_u32(((uint8_t *)src)[3], ((uint8_t *)src)[2], ((uint8_t *)src)[1], ((uint8_t *)src)[0]);
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uint32_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return u8_to_u32(U32_B0(src32[1]), U32_B3(src32[0]), U32_B2(src32[0]), U32_B1(src32[0]));
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return u8_to_u32(U32_B1(src32[1]), U32_B0(src32[1]), U32_B3(src32[0]), U32_B2(src32[0]));
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return u8_to_u32(U32_B2(src32[1]), U32_B1(src32[1]), U32_B0(src32[1]), U32_B3(src32[0]));
			break;
		default:
			return ((uint32_t *)src)[0];
	}
#endif
}

// Static inline function to write an unaligned memory
static inline void tru_w32_unaligned(void *dst, uint32_t num){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	((uint8_t *)dst)[0] = (uint8_t)num;
	((uint8_t *)dst)[1] = (uint8_t)(num >> 8U);
	((uint8_t *)dst)[2] = (uint8_t)(num >> 16U);
	((uint8_t *)dst)[3] = (uint8_t)(num >> 24U);
#else
	// Restrict to 32-bit access
	uint32_t *dst32;
	switch((uint32_t)dst & 0x3){
		case 1:
			dst32 = (uint32_t *)((uint8_t *)dst - 1);
			dst32[0] = (dst32[0] & 0x000000ff) | U32_B2(num) << 24 | U32_B1(num) << 16 | U32_B0(num) << 8;
			dst32[1] = (dst32[1] & 0xffffff00) | U32_B3(num);
			break;
		case 2:
			dst32 = (uint32_t *)((uint8_t *)dst - 2);
			dst32[0] = (dst32[0] & 0x0000ffff) | U32_B1(num) << 24 | U32_B0(num) << 16;
			dst32[1] = (dst32[1] & 0xffff0000) | U32_B3(num) << 8 | U32_B2(num);
			break;
		case 3:
			dst32 = (uint32_t *)((uint8_t *)dst - 3);
			dst32[0] = (dst32[0] & 0x00ffffff) | U32_B0(num) << 24;
			dst32[1] = (dst32[1] & 0xff000000) | U32_B3(num) << 16 | U32_B2(num) << 8 | U32_B1(num);
			break;
		default:
			((uint32_t *)dst)[0] = num;
	}
#endif
}

static inline void tru_iom_rd32_discard(uint32_t *src){
	asm volatile ("" : : "r" (*src));  // Read and do nothing with it
}

// Static inline function to read a memory-mapped 32-bit register
static inline uint32_t tru_iom_rd32(uint32_t *src){
	return *(volatile uint32_t *)src;
}

// Static inline function to write a memory-mapped 32-bit register
static inline void tru_iom_wr32(uint32_t *dst, uint32_t num){
	*(volatile uint32_t *)dst = num;
}

// Static inline function to read an unaligned memory-mapped 32-bit register
static inline uint32_t tru_iom_rd32_unaligned(void *src){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	volatile uint8_t *src8 = (uint8_t *)src;
	return u8_to_u32(src8[3], src8[2], src8[1], src8[0]);
#else
	// Restrict to 32-bit access
	volatile uint32_t *src32;
	switch((uint32_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return u8_to_u32(U32_B0(src32[1]), U32_B3(src32[0]), U32_B2(src32[0]), U32_B1(src32[0]));

			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return u8_to_u32(U32_B1(src32[1]), U32_B0(src32[1]), U32_B3(src32[0]), U32_B2(src32[0]));
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return u8_to_u32(U32_B2(src32[1]), U32_B1(src32[1]), U32_B0(src32[1]), U32_B3(src32[0]));
			break;
		default:
			return *(volatile uint32_t *)src;
	}
#endif
}

// Static inline function to write an unaligned memory-mapped 32-bit register
static inline void tru_iom_w32_unaligned(void *dst, uint32_t num){
#if defined(TRU_UNALIGNED_ACCESS) && TRU_UNALIGNED_ACCESS == 1U
	volatile uint8_t *dst8 = (uint8_t *)dst;
	dst8[0] = (uint8_t)num;
	dst8[1] = (uint8_t)(num >> 8U);
	dst8[2] = (uint8_t)(num >> 16U);
	dst8[3] = (uint8_t)(num >> 24U);
#else
	// Restrict to 32-bit access
	volatile uint32_t *dst32;
	switch((uint32_t)dst & 0x3){
		case 1:
			dst32 = (uint32_t *)((uint8_t *)dst - 1);
			dst32[0] = (dst32[0] & 0x000000ff) | U32_B2(num) << 24 | U32_B1(num) << 16 | U32_B0(num) << 8;
			dst32[1] = (dst32[1] & 0xffffff00) | U32_B3(num);
			break;
		case 2:
			dst32 = (uint32_t *)((uint8_t *)dst - 2);
			dst32[0] = (dst32[0] & 0x0000ffff) | U32_B1(num) << 24 | U32_B0(num) << 16;
			dst32[1] = (dst32[1] & 0xffff0000) | U32_B3(num) << 8 | U32_B2(num);
			break;
		case 3:
			dst32 = (uint32_t *)((uint8_t *)dst - 3);
			dst32[0] = (dst32[0] & 0x00ffffff) | U32_B0(num) << 24;
			dst32[1] = (dst32[1] & 0xff000000) | U32_B3(num) << 16 | U32_B2(num) << 8 | U32_B1(num);
			break;
		default:
			*(volatile uint32_t *)dst = num;
	}
#endif
}

#endif
