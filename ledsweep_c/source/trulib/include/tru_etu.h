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

	Version: 20240430

	Exit to U-Boot support.
*/

#ifndef TRU_ETU_H
#define TRU_ETU_H

#if(TRU_EXIT_TO_UBOOT == 1U)

extern int uboot_argc;
extern char **uboot_argv;
extern long unsigned int uboot_lr;
extern long unsigned int uboot_cpsr;
extern long unsigned int uboot_sys_sp;  // This is also for the user mode, because they use the same stack pointer
extern long unsigned int uboot_und_sp;
extern long unsigned int uboot_abt_sp;
extern long unsigned int uboot_svc_sp;
extern long unsigned int uboot_irq_sp;
extern long unsigned int uboot_fiq_sp;
extern long unsigned int uboot_vbar;

#endif

#endif
