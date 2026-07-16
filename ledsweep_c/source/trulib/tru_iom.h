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

	Version: 20260208

	I/O memory read/write support.
*/

#ifndef TRU_IOM_H
#define TRU_IOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"
#include <stdint.h>

// Divide and align up
#define DIV_CEIL(_n, _d) (((_n) + (_d) - 1) / (_d))

// Integer alignment (the input must be integer)
#define INT_ALIGN_DN(_n, _align) ((_n) & ~((_align) - 1))
#define INT_ALIGN_UP(_n, _align) (((_n) + (_align) - 1) & ~((_align) - 1))

// Round fractional to nearest and cast to int
#define ROUND_NEAREST_I32(_n) ((int32_t)(_n + ((_n < 0) ? -0.5 : 0.5)))

// Round fractional to nearest and cast to int
#define ROUND_NEAREST_U32(_n) ((uint32_t)(_n + 0.5))

#define TO_U32PTR(_p) ((uint32_t *)_p)

#define U32_B0(_n) ((uint8_t)(( (uint32_t)_n)        & 0x000000ff))
#define U32_B1(_n) ((uint8_t)((((uint32_t)_n) >>  8) & 0x000000ff))
#define U32_B2(_n) ((uint8_t)((((uint32_t)_n) >> 16) & 0x000000ff))
#define U32_B3(_n) ((uint8_t)((((uint32_t)_n) >> 24) & 0x000000ff))

#define B0(_n) ((uint8_t)(_n        & 0xff))
#define B1(_n) ((uint8_t)((_n >>  8) & 0xff))
#define B2(_n) ((uint8_t)((_n >> 16) & 0xff))
#define B3(_n) ((uint8_t)((_n >> 24) & 0xff))

static inline uint32_t u32_align_up(uint32_t n, uint32_t a){
	//return DIV_CEIL(n, a) * a;
	return INT_ALIGN_UP(n, a);
}

static inline uint32_t u8_to_u32(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0){
	return (((uint32_t)b3) << 24) | (((uint32_t)b2) << 16) | (((uint32_t)b1) << 8) | b0;
}

static inline uint8_t iom_rd8(uintptr_t addr){
	return *(volatile uint8_t*)addr;
}

static inline uint16_t iom_rd16(uintptr_t addr){
	return *(volatile uint16_t*)addr;
}

// Static inline function to read a memory-mapped 32-bit register
static inline uint32_t iom_rd32(uintptr_t src){
	return *(volatile uint32_t *)src;
}

static inline void iom_rd32_discard(uintptr_t src){
	__asm__ volatile ("" : : "r"(src));  // Read and do nothing with it
}

static inline uint64_t iom_rd64(uintptr_t addr){
	return *(volatile uint64_t*)addr;
}

static inline void iom_wr8(uintptr_t addr, uint8_t value){
	*(volatile uint8_t*)addr = value;
}

static inline void iom_wr16(uintptr_t addr, uint16_t value){
	*(volatile uint16_t*)addr = value;
}

// Static inline function to write a memory-mapped 32-bit register
static inline void iom_wr32(uintptr_t dst, uint32_t num){
	*(volatile uint32_t *)dst = num;
}

static inline void iom_wr64(uintptr_t addr, uint64_t value){
	*(volatile uint64_t*)addr = value;
}

static inline void iom_clrbits32(uintptr_t addr, uint32_t clear){
	iom_wr32(addr, iom_rd32(addr) & ~clear);
}

static inline void iom_clrbits64(uintptr_t addr, uint64_t clear){
	iom_wr64(addr, iom_rd64(addr) & ~clear);
}

static inline void iom_setbits32(uintptr_t addr, uint32_t set){
	iom_wr32(addr, iom_rd32(addr) | set);
}

static inline void iom_setbits64(uintptr_t addr, uint64_t set){
	iom_wr64(addr, iom_rd64(addr) | set);
}

static inline void iom_clrsetbits32(uintptr_t addr, uint32_t clear, uint32_t set){
	iom_wr32(addr, (iom_rd32(addr) & ~clear) | set);
}

static inline void iom_clrsetbits64(uintptr_t addr, uint64_t clear, uint64_t set){
	iom_wr64(addr, (iom_rd64(addr) & ~clear) | set);
}

// Static inline function to read an unaligned memory
static inline uint8_t rd8_unaligned(void *src){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return ((uint8_t *)src)[0];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
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

// Static inline function to read an unaligned memory
static inline uint32_t rd32_unaligned(void *src){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return u8_to_u32(((uint8_t *)src)[3], ((uint8_t *)src)[2], ((uint8_t *)src)[1], ((uint8_t *)src)[0]);
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
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
static inline void w8_unaligned(void *dst, uint8_t num){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	((uint8_t *)dst)[0] = num;
#else
	// Restrict to 32-bit access
	uint32_t *dst32;
	switch((uintptr_t)dst & 0x3){
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

// Static inline function to write an unaligned memory
static inline void w32_unaligned(void *dst, uint32_t num){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	((uint8_t *)dst)[0] = (uint8_t)num;
	((uint8_t *)dst)[1] = (uint8_t)(num >> 8);
	((uint8_t *)dst)[2] = (uint8_t)(num >> 16);
	((uint8_t *)dst)[3] = (uint8_t)(num >> 24);
#else
	// Restrict to 32-bit access
	uint32_t *dst32;
	switch((uintptr_t)dst & 0x3){
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

// Convert little-endian buffer to u16
static inline uint16_t buf_le_to_u16(void *src){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return (((uint8_t *)src)[1] << 8) | ((uint8_t *)src)[0];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
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

// Convert little-endian buffer to u24 (actually u32 of 24bits padded with 1 byte)
static inline uint32_t buf_le_to_u24(void *src){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return (((unsigned char *)src)[2] << 16) | (((unsigned char *)src)[1] << 8) | ((unsigned char *)src)[0];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return U32_B3(src32[0]) << 16 | U32_B2(src32[0]) << 8 | U32_B1(src32[0]);
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return U32_B0(src32[1]) << 16 | U32_B3(src32[0]) << 8 | U32_B2(src32[0]);
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return U32_B1(src32[1]) << 16 | U32_B0(src32[1]) << 8 | U32_B3(src32[0]);
			break;
		default:
			return U32_B2(TO_U32PTR(src)[0]) << 16 | U32_B1(TO_U32PTR(src)[0]) << 8 | U32_B0(TO_U32PTR(src)[0]);
	}
#endif
}

// Convert little-endian buffer to u32
static inline uint32_t buf_le_to_u32(void *src){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return (((unsigned char *)src)[3] << 24) | (((unsigned char *)src)[2] << 16) | (((unsigned char *)src)[1] << 8) | ((unsigned char *)src)[0];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
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
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return (((uint8_t *)src)[0] << 8) | ((uint8_t *)src)[1];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
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

// Convert big-endian buffer to u24 (actually u32 of 24bits padded with 1 byte)
static inline uint32_t buf_be_to_u24(void *src){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return (((unsigned char *)src)[0] << 16) | (((unsigned char *)src)[1] << 8) | (((unsigned char *)src)[2]);
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
		case 1:
			src32 = (uint32_t *)((uint8_t *)src - 1);
			return U32_B1(src32[0]) << 16 | U32_B2(src32[0]) << 8 | U32_B3(src32[0]);
			break;
		case 2:
			src32 = (uint32_t *)((uint8_t *)src - 2);
			return U32_B2(src32[0]) << 16 | U32_B3(src32[0]) << 8 | U32_B0(src32[1]);
			break;
		case 3:
			src32 = (uint32_t *)((uint8_t *)src - 3);
			return U32_B3(src32[0]) << 16 | U32_B0(src32[1]) << 8 | U32_B1(src32[1]);
			break;
		default:
			return U32_B0(TO_U32PTR(src)[0]) << 16 | U32_B1(TO_U32PTR(src)[0]) << 8 | U32_B2(TO_U32PTR(src)[0]);
	}
#endif
}

// Convert big-endian buffer to u32
static inline uint32_t buf_be_to_u32(void *src){
#if defined(TRU_CFG_UNALIGNED_ACCESS) && TRU_CFG_UNALIGNED_ACCESS == 1
	return (((unsigned char *)src)[0] << 24) | (((unsigned char *)src)[1] << 16) | (((unsigned char *)src)[2] << 8) | ((unsigned char *)src)[3];
#else
	// Restrict to 32-bit access
	uint32_t *src32;
	switch((uintptr_t)src & 0x3){
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

// Find first set (ffs), i.e. returns the lowest bit position that is set in a word
// The input word must be non-zero, there is no bit position for that so is undefined
static __attribute__((always_inline)) inline uint64_t __ffs64(uint64_t value){
	int bitpos = 0;

	if((value & 0xffffffff) == 0){ bitpos += 32; value >>= 32; }
	if((value & 0xffff) == 0) { bitpos += 16; value >>= 16; }
	if((value & 0xff) == 0) { bitpos += 8; value >>= 8; }
	if((value & 0xf) == 0) { bitpos += 4; value >>= 4; }
	if((value & 0x3) == 0) { bitpos += 2; value >>= 2; }
	if((value & 0x1) == 0) bitpos += 1;

	return bitpos;
}

#ifdef __cplusplus
}
#endif

#endif
