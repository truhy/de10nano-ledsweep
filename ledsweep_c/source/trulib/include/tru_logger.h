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

	Version: 20230619

	Provides debug logging support for bare-metal program development.
*/

#ifndef TRU_LOGGER_H
#define TRU_LOGGER_H

#include "tru_config.h"
#include <stdio.h>

// Define a macro named _NL to select the correct line endings for different modes.  NL stands for NewLine
#if defined(SEMIHOSTING)
	#define _NL "\n"
	#define DEBUG_PRINTF(str, ...) printf(str, ##__VA_ARGS__)
#elif defined(TRU_PRINTF_UART)
	#define _NL "\r\n"
	#define DEBUG_PRINTF(str, ...) printf(str, ##__VA_ARGS__)
#else
	#define _NL "\n"
	#define DEBUG_PRINTF(str, ...) (void)0
#endif

#endif
