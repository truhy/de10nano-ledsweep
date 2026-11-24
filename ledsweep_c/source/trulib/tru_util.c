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

	Version: 20260607
*/

#include "tru_util.h"

void unaligned_memcpy(void *dst, const void *src, size_t size){
	if(((uintptr_t)dst & 0x3) || ((uintptr_t)src & 0x3) || (size & 0x3)){  // The source or destination address is not 32 bits aligned?
		if(dst != src){  // Is not copying to itself?
			unsigned char *dst8 = dst;
			const unsigned char *src8 = src;

			// Do byte copy
			for(size_t i = 0; i < size; i++){
				*dst8 = *src8;
				dst8++;
				src8++;
			}
		}
	}else{
		memcpy(dst, src, size);  // Memory pointers are aligned so we can use standard memcpy()
	}
}
