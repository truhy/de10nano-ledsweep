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

	Version: 20240127

	Arm Cortex-A9 low level assembly codes.
*/

#ifndef TRU_CORTEX_A9_H
#define TRU_CORTEX_A9_H

// GCC inline assembly macros
#define __wfe() __asm__ volatile("wfe":::"memory")
#define __sev() __asm__ volatile("sev")
#define __dmb() __asm__ volatile("dmb 0xF":::"memory");
#define __write_dcisw(index)  __asm__ volatile("MCR p15, 0, %0, c7, c6, 2" : : "r" (index) : "memory")
#define __write_dccsw(index)  __asm__ volatile("MCR p15, 0, %0, c7, c10, 2" : : "r" (index) : "memory")
#define __write_csselr(level) __asm__ volatile("MCR p15, 2, %0, c0, c0, 0" : : "r" (level) : "memory")
#define __read_sctlr(result)  __asm__ volatile("MRC p15, 0, %0, c1, c0, 0" : "=r" (result) : : "memory")
#define __read_ccsidr(result) __asm__ volatile("MRC p15, 1, %0, c0, c0, 0" : "=r" (result) : : "memory")
#define __read_clidr(result)  __asm__ volatile("MRC p15, 1, %0, c0, c0, 1" : "=r" (result) : : "memory")
#define __read_mpidr(mpidr)   __asm__ volatile("MRC p15, 0, %0, c0, c0, 5" : "=r" (mpidr) : : "memory")

#endif
