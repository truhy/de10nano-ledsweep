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
*/

#include "tru_uart_c5soc.h"

#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC

#include "tru_iom.h"
#include "c5soc/tru_clkmgr_c5soc.h"

void tru_hps_uart_init(uintptr_t uart_base, uint32_t baudrate){
	// Configure baud rate
	uint32_t divisor = ROUND_NEAREST_U32(get_l4_sp_clk(TRU_HPS_INPUT_CLK_HZ).fout / (16 * baudrate));
	TRU_HPS_UART_REG(uart_base)->lcr = TRU_HPS_UART_REG(uart_base)->lcr | TRU_HPS_UART_LCR_DLAB;
	TRU_HPS_UART_REG(uart_base)->ier_dlh = divisor & 0xff;
	TRU_HPS_UART_REG(uart_base)->ier_dlh = (divisor >> 8) & 0xff;
	TRU_HPS_UART_REG(uart_base)->lcr = TRU_HPS_UART_REG(uart_base)->lcr & (~TRU_HPS_UART_LCR_DLAB);

	TRU_HPS_UART_REG(uart_base)->ier_dlh = (TRU_HPS_UART_REG(uart_base)->ier_dlh) | 1;  // Configure interrupt flags
	TRU_HPS_UART_REG(uart_base)->lcr = TRU_HPS_UART_LCR_DLS_8;  // Configure 8 data bits, no parity, 1 stop bit, no break control
}

/*
	Blocking wait on the transmit empty register to become empty.  It becomes
	empty when all pending data in the FIFO (FIFO mode) or holding register
	(non-FIFO mode) is transmitted.  This ensures all pending data has gone out.
*/
void tru_hps_uart_wait_empty(uintptr_t uart_base){
	while((TRU_HPS_UART_REG(uart_base)->lsr & TRU_HPS_UART_LSR_TE_SETMSK) == 0);  // Flush UART and wait
}

/*
	Wait until the UART controller is ready to accept a byte in its transmit
	buffer, i.e. when free space becomes available.
*/
void tru_hps_uart_wait_ready(uintptr_t uart_base, char fifo_th_en){
	// Wait until the UART controller is ready to accept a byte in its transmit buffer, i.e. there is free space?
	// They are masochists - using the same bit but with the opposite logic depending on the mode set!
	if(fifo_th_en){
		while(TRU_HPS_UART_REG(uart_base)->lsr & TRU_HPS_UART_LSR_THRE_SETMSK);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 1 = not empty, 0 = empty
	}else{
		while((TRU_HPS_UART_REG(uart_base)->lsr & TRU_HPS_UART_LSR_THRE_SETMSK) == 0);  // Wait while not empty. Bit 5 of LSR reg (THRE bit), 0 = not empty, 1 = empty
	}
}

void tru_hps_uart_wait_data_avail(uintptr_t uart_base){
	while(!(TRU_HPS_UART_REG(uart_base)->lsr & TRU_HPS_UART_LSR_DR_SETMSK));
}

int tru_hps_uart_getc(uintptr_t uart_base){
	tru_hps_uart_wait_data_avail(uart_base);
	return (int)TRU_HPS_UART_REG(uart_base)->rbr_thr_dll;
}

void tru_hps_uart_putc(uintptr_t uart_base, uint8_t ch){
	int fifo_th_en = (TRU_HPS_UART_REG(uart_base)->sfe && TRU_HPS_UART_REG(uart_base)->stet) ? 1 : 0;  // FIFO & threshold mode enabled?

#if defined(TRU_CFG_LOG_RN) && TRU_CFG_LOG_RN == 1
	if(ch == '\n'){
		tru_hps_uart_wait_ready(uart_base, fifo_th_en);
		TRU_HPS_UART_REG(uart_base)->rbr_thr_dll = '\r';
	}
#endif

	tru_hps_uart_wait_ready(uart_base, fifo_th_en);
	TRU_HPS_UART_REG(uart_base)->rbr_thr_dll = ch;
}

void tru_hps_uart_puts(uintptr_t uart_base, char *str){
	if(!str) return;
	while(*str) tru_hps_uart_putc(uart_base, *str++);
}

void tru_hps_uart_putns(uintptr_t uart_base, const char *str, uint32_t len){
	for(uint32_t i = 0; i < len; i++){
		tru_hps_uart_putc(uart_base, *str++);
	}
}

// Transmit nibble as hex ASCII numeric characters
void tru_hps_uart_put_nibhex(uintptr_t uart_base, unsigned char nibble){
	if(nibble > 9){
		tru_hps_uart_putc(uart_base, (char)(nibble + 87));  // Convert to ASCII character
	}else{
		tru_hps_uart_putc(uart_base, (char)(nibble + 48));  // Convert to ASCII character
	}
}

// Transmit integer as hex ASCII numeric characters
void tru_hps_uart_put_inthex(uintptr_t uart_base, int num, unsigned int bits){
	for(unsigned int i = bits; i; i -= 4){
		tru_hps_uart_put_nibhex(uart_base, (unsigned char)(num >> (i - 4) & 0xf));
	}
}

#endif
