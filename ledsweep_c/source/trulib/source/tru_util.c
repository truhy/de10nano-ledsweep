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

#include "tru_util.h"

void unaligned_memcpy(void *dst, const void *src, uint32_t size){
	if(*((uint32_t *)dst) % 4 || *((uint32_t *)src) % 4 || size % 4){  // The source or destination address is not 32 bits aligned?
		if(dst != src){  // Is not copying to itself?
			unsigned char *dst8 = dst;
			const unsigned char *src8 = src;
			uint32_t i;

			// Do byte copy
			for(i = 0; i < size; i++){
				*dst8 = *src8;
				dst8++;
				src8++;
			}
		}
	}else{
		memcpy(dst, src, size);  // Memory pointers are aligned so we can use standard memcpy()
	}
}

// Convert little-endian buffer to uint16_t.
uint16_t le_buf_to_uint16(void *buf){
	return (((unsigned char *)buf)[1] << 8) | ((unsigned char *)buf)[0];
}

// Convert big-endian buffer to uint16_t.
uint16_t be_buf_to_uint16(void *buf){
	return (((unsigned char *)buf)[0] << 8) | ((unsigned char *)buf)[1];
}

// Byte swap unsigned short
uint16_t swap_uint16(uint16_t val){
    return (val << 8) | (val >> 8 );
}

// Byte swap short
int16_t swap_int16(int16_t val){
    return (val << 8) | ((val >> 8) & 0xFF);
}

// Byte swap unsigned int
uint32_t swap_uint32(uint32_t val){
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

// Byte swap int
int32_t swap_int32(int32_t val){
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}
