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

	Version: 20240211

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
void tru_uart_ll_wait_empty(uint32_t uart_base_addr){
	while((tru_mem_rd_word(uart_base_addr + TRU_UART_LSR_OFFSET) & 0x00000040) == 0);  // Flush UART and wait
}

void tru_uart_ll_write_str(uint32_t uart_base_addr, const char *str, uint32_t len){
	// FIFO & threshold mode enabled?
	char fifo_th_en = (tru_mem_rd_word(uart_base_addr + TRU_UART_SFE_OFFSET) && tru_mem_rd_word(uart_base_addr + TRU_UART_STET_OFFSET)) ? 1 : 0;

	// Write input bytes to UART controller, one at a time
	for(uint32_t i = 0; i < len; i++){
		// Wait until the UART controller is ready to accept a byte in its transmit buffer, i.e. there is free space?
		// They are masochists - using the same bit but with the opposite logic depending on the mode set!
		if(fifo_th_en){
			while(tru_mem_rd_word(uart_base_addr + TRU_UART_LSR_OFFSET) & 0x00000020);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 1 = not empty, 0 = empty
		}else{
			while((tru_mem_rd_word(uart_base_addr + TRU_UART_LSR_OFFSET) & 0x00000020) == 0);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 0 = not empty, 1 = empty
		}

		// Write a single character to UART controller transmit holding register
		tru_mem_wr_word(uart_base_addr + TRU_UART_RBR_THR_DLL_OFFSET, str[i]);
	}
}

void tru_uart_ll_write_char(uint32_t uart_base_addr, const char c){
	// FIFO & threshold mode enabled?
	char fifo_th_en = (tru_mem_rd_word(uart_base_addr + TRU_UART_SFE_OFFSET) && tru_mem_rd_word(uart_base_addr + TRU_UART_STET_OFFSET)) ? 1 : 0;

	// Wait until the UART controller is ready to accept a byte in its transmit buffer, i.e. there is free space?
	// They are masochists - using the same bit but with the opposite logic depending on the mode set!
	if(fifo_th_en){
		while(tru_mem_rd_word(uart_base_addr + TRU_UART_LSR_OFFSET) & 0x00000020);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 1 = not empty, 0 = empty
	}else{
		while((tru_mem_rd_word(uart_base_addr + TRU_UART_LSR_OFFSET) & 0x00000020) == 0);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 0 = not empty, 1 = empty
	}

	// Write a single character to UART controller transmit holding register
	tru_mem_wr_word(uart_base_addr + TRU_UART_RBR_THR_DLL_OFFSET, c);
}

void tru_uart_ll_write_hex_nibble(uint32_t uart_base_addr, unsigned char nibble){
	if(nibble > 9){
		tru_uart_ll_write_char(uart_base_addr, (char)(nibble + 87U));  // Convert to ASCII character
	}else{
		tru_uart_ll_write_char(uart_base_addr, (char)(nibble + 48U));  // Convert to ASCII character
	}
}

void tru_uart_ll_write_inthex(uint32_t uart_base_addr, int num, unsigned int bits){
	for(unsigned int i = bits; i; i -= 4){
		tru_uart_ll_write_hex_nibble(uart_base_addr, (unsigned char)(num >> (i - 4) & 0xf));
	}
}

#endif
