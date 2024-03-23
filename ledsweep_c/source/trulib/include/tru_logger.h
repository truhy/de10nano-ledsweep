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

	Version: 20240323

	Provides debug logging support for bare-metal program development.
*/

#ifndef TRU_LOGGER_H
#define TRU_LOGGER_H

#include "tru_config.h"
#include <stdio.h>

#if defined(TRU_DEBUG_PRINT_LEVEL) && TRU_DEBUG_PRINT_LEVEL >= 3U

	#define DEBUG_PRINTF(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)
	#define DEBUG_PRINTF_PLAIN(fmt, args...) fprintf(stderr, fmt, ##args)

#elif defined(TRU_DEBUG_PRINT_LEVEL) && TRU_DEBUG_PRINT_LEVEL >= 2U

	#define DEBUG_PRINTF(fmt, args...) fprintf(stderr, "DEBUG: %d:%s(): " fmt, __LINE__, __func__, ##args)
	#define DEBUG_PRINTF_PLAIN(fmt, args...) fprintf(stderr, fmt, ##args)

#elif defined(TRU_DEBUG_PRINT_LEVEL) && TRU_DEBUG_PRINT_LEVEL >= 1U

	#define DEBUG_PRINTF(fmt, args...) fprintf(stderr, "DEBUG: " fmt, ##args)
	#define DEBUG_PRINTF_PLAIN(fmt, args...) fprintf(stderr, fmt, ##args)

#else

	#define DEBUG_PRINTF(fmt, args...)  // Do nothing
	#define DEBUG_PRINTF_PLAIN(fmt, args...)  // Do nothing

#endif

#endif
