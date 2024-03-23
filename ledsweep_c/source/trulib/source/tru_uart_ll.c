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

	Version: 20240316

	Vendor specific low-level UART functions.
*/

#include "tru_config.h"

#if(TRU_TARGET == TRU_C5SOC)

#include "tru_uart_ll.h"

/*
	Blocking wait on the transmit empty register to become empty.  It becomes
	empty when all pending data in the FIFO (FIFO mode) or holding register
	(non-FIFO mode) is transmitted.  This ensures all pending data has gone out.
*/
void tru_uart_ll_wait_empty(TRU_TARGET_TYPE *uart_base){
	while((tru_iom_rd32(uart_base + TRU_UART_LSR_OFFSET / 4U) & 0x00000040UL) == 0U);  // Flush UART and wait
}

void tru_uart_ll_wait_ready(TRU_TARGET_TYPE *uart_base, char fifo_th_en){
	// Wait until the UART controller is ready to accept a byte in its transmit buffer, i.e. there is free space?
	// They are masochists - using the same bit but with the opposite logic depending on the mode set!
	if(fifo_th_en){
		while(tru_iom_rd32(uart_base + TRU_UART_LSR_OFFSET / 4U) & 0x00000020UL);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 1 = not empty, 0 = empty
	}else{
		while((tru_iom_rd32(uart_base + TRU_UART_LSR_OFFSET / 4U) & 0x00000020UL) == 0U);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 0 = not empty, 1 = empty
	}
}

void tru_uart_ll_write_str(TRU_TARGET_TYPE *uart_base, const char *str, uint32_t len){
	// FIFO & threshold mode enabled?
	char fifo_th_en = (tru_iom_rd32(uart_base + TRU_UART_SFE_OFFSET / 4U) && tru_iom_rd32(uart_base + TRU_UART_STET_OFFSET / 4U)) ? 1U : 0U;

	// Write input bytes to UART controller, one at a time
	for(uint32_t i = 0U; i < len; i++){
		tru_uart_ll_wait_ready(uart_base, fifo_th_en);

		// For each '\n' character insert '\r'?
		#if defined(TRU_PRINT_UART_R_NL) && TRU_PRINT_UART_R_NL == 1U
			if(str[i] == '\n'){
				tru_iom_wr32(uart_base + TRU_UART_RBR_THR_DLL_OFFSET / 4U, '\r');
				tru_uart_ll_wait_ready(uart_base, fifo_th_en);
			}
		#endif

		tru_iom_wr32(uart_base + TRU_UART_RBR_THR_DLL_OFFSET / 4U, str[i]);  // Write a single character to UART controller transmit holding register
	}
}

void tru_uart_ll_write_char(TRU_TARGET_TYPE *uart_base, const char c){
	// FIFO & threshold mode enabled?
	char fifo_th_en = (tru_iom_rd32(uart_base + TRU_UART_SFE_OFFSET / 4U) && tru_iom_rd32(uart_base + TRU_UART_STET_OFFSET / 4U)) ? 1U : 0U;

	tru_uart_ll_wait_ready(uart_base, fifo_th_en);

	// For each '\n' character insert '\r'?
	#if defined(TRU_PRINT_UART_R_NL) && TRU_PRINT_UART_R_NL == 1U
		if(c == '\n'){
			tru_iom_wr32(uart_base + TRU_UART_RBR_THR_DLL_OFFSET / 4U, '\r');
			tru_uart_ll_wait_ready(uart_base, fifo_th_en);
		}
	#endif

	tru_iom_wr32(uart_base + TRU_UART_RBR_THR_DLL_OFFSET / 4U, c);  // Write a single character to UART controller transmit holding register
}

void tru_uart_ll_write_hex_nibble(TRU_TARGET_TYPE *uart_base, unsigned char nibble){
	if(nibble > 9){
		tru_uart_ll_write_char(uart_base, (char)(nibble + 87U));  // Convert to ASCII character
	}else{
		tru_uart_ll_write_char(uart_base, (char)(nibble + 48U));  // Convert to ASCII character
	}
}

void tru_uart_ll_write_inthex(TRU_TARGET_TYPE *uart_base, int num, unsigned int bits){
	for(unsigned int i = bits; i; i -= 4U){
		tru_uart_ll_write_hex_nibble(uart_base, (unsigned char)(num >> (i - 4U) & 0xfU));
	}
}

#endif
