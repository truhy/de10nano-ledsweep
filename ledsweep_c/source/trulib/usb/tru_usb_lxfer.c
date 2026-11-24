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

#include "usb/tru_usb_lxfer.h"
#include "synopsys/tru_dwc2_sgdma.h"
#include <stdlib.h>

void tru_usb_lxfer_init(tru_usb_lxfer_t *lxfer){
	uint32_t i;

	lxfer->ep_xfer_limit = 0;

	// Initialise array fields
	for(i = 0; i < TRU_LXFER_MAX_CHUNK_BUFFERS; i++){
		lxfer->chunk.buffers[i].buf = NULL;
		lxfer->chunk.buffers[i].buf_size = 0;
		lxfer->chunk.buffers[i].xfer_size = 0;
		lxfer->chunk.buffers[i].xfer_size_aligned = 0;
		lxfer->chunk.buffers[i].xferred_size = 0;
	}
	lxfer->chunk.num_registered = 0;
	lxfer->chunk.xfer_limit = 0;
	lxfer->chunk.total_xfer_size = 0;
	lxfer->chunk.total_xferred_size = 0;

	lxfer->whole.total_xfer_size = 0;
	lxfer->whole.total_exp_size = 0;
	lxfer->whole.total_xferred_size = 0;
	lxfer->whole.offset = 0;

	tru_dwc2_sgdma_init(&lxfer->chunk.sgdma);
}

void tru_usb_lxfer_deinit(tru_usb_lxfer_t *lxfer){
	tru_dwc2_sgdma_deinit(&lxfer->chunk.sgdma);
}

uint32_t tru_usb_lxfer_get_num_registered(tru_usb_lxfer_t *lxfer){
	return lxfer->chunk.num_registered;
}

/*
	Register a buffer for data transfer.
	For internal FIFO and DMA (non-scatter gather) register only a single buffer.
	For Scatter Gather DMA you may register more than one buffer.
*/
void tru_usb_lxfer_register_buffer(tru_usb_lxfer_t *lxfer, void *buf, uint32_t size){
	if(lxfer->chunk.num_registered < TRU_LXFER_MAX_CHUNK_BUFFERS){
		lxfer->chunk.buffers[lxfer->chunk.num_registered].buf = buf;
		lxfer->chunk.buffers[lxfer->chunk.num_registered].buf_size = size;
		lxfer->chunk.num_registered++;
	}
}

void tru_usb_lxfer_unregister_buffers(tru_usb_lxfer_t *lxfer){
	lxfer->chunk.num_registered = 0;
}

tru_usb_lxfer_buf_t *tru_usb_lxfer_get_buffer(tru_usb_lxfer_t *lxfer, uint32_t index){
	return lxfer->chunk.buffers + index;
}

void tru_usb_lxfer_register_sgdma_descs(tru_usb_lxfer_t *lxfer, tru_dwc2_dev_sgdma_desc_reg_t *desc, uint32_t num_desc){
	tru_dwc2_sgdma_register_descs(&lxfer->chunk.sgdma, desc, num_desc);
}

void tru_usb_lxfer_zero_chunk_xfer_size_all(tru_usb_lxfer_t *lxfer){
	for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
		lxfer->chunk.buffers[i].xfer_size = 0;
		lxfer->chunk.buffers[i].xfer_size_aligned = 0;
		lxfer->chunk.buffers[i].xferred_size = 0;
	}
}

void tru_usb_lxfer_fill_chunk_xfer_size_tx(tru_usb_lxfer_t *lxfer, uint32_t index, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode){
	lxfer->chunk.buffers[index].xfer_size = xfer_size;
	lxfer->chunk.buffers[index].xfer_size_aligned = xfer_size;  // No alignment requirement
	lxfer->chunk.buffers[index].xferred_size = 0;
}

void tru_usb_lxfer_fill_chunk_xfer_size_rx(tru_usb_lxfer_t *lxfer, uint32_t index, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode){
	lxfer->chunk.buffers[index].xfer_size = xfer_size;

	// In DMA scatter gather (DMA SG) mode and OUT endpoint (data from host), the request length must be a multiple of 32 bits (4 bytes),
	// so for unaligned length we must round up for it to work.  The xcompl (transfer complete) interrupt will trigger on the correct length.
	// If you don't round up the xcompl (transfer complete) interrupt will not trigger on an unaligned length.
	if(dma_mode == TRU_DWC2_DMA_MODE_SG){
		if(xfer_size % TRU_DWC2_SGDMA_ALIGNMENT_BYTES){
			lxfer->chunk.buffers[index].xfer_size_aligned = xfer_size + (TRU_DWC2_SGDMA_ALIGNMENT_BYTES - xfer_size % TRU_DWC2_SGDMA_ALIGNMENT_BYTES);  // Align up
		}else{
			lxfer->chunk.buffers[index].xfer_size_aligned = xfer_size;  // Already aligned
		}
	}else{
		lxfer->chunk.buffers[index].xfer_size_aligned = xfer_size;  // No alignment requirement
	}

	lxfer->chunk.buffers[index].xferred_size = 0;
}

void tru_usb_lxfer_fill_chunk_xfer_size_all_tx(tru_usb_lxfer_t *lxfer, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode){
	uint32_t remain_size = xfer_size;

	if(lxfer->chunk.xfer_limit && remain_size > lxfer->chunk.xfer_limit){
		remain_size = lxfer->chunk.xfer_limit;
	}

	for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
		if(remain_size){
			lxfer->chunk.buffers[i].xfer_size = (remain_size > lxfer->chunk.buffers[i].buf_size) ? lxfer->chunk.buffers[i].buf_size : remain_size;
			lxfer->chunk.buffers[i].xfer_size_aligned = lxfer->chunk.buffers[i].xfer_size;  // For IN EP DMA SG does not need 4-byte alignment
			lxfer->chunk.buffers[i].xferred_size = 0;

			remain_size -= lxfer->chunk.buffers[i].xfer_size;
		}else{
			lxfer->chunk.buffers[i].xfer_size = 0;
			lxfer->chunk.buffers[i].xfer_size_aligned = 0;
			lxfer->chunk.buffers[i].xferred_size = 0;
		}
	}
}

void tru_usb_lxfer_fill_chunk_xfer_size_all_rx(tru_usb_lxfer_t *lxfer, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode){
	uint32_t remain_size = xfer_size;

	if(lxfer->chunk.xfer_limit && remain_size > lxfer->chunk.xfer_limit){
		remain_size = lxfer->chunk.xfer_limit;
	}

	for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
		if(remain_size){
			lxfer->chunk.buffers[i].xfer_size = (remain_size > lxfer->chunk.buffers[i].buf_size) ? lxfer->chunk.buffers[i].buf_size : remain_size;

			// In DMA scatter gather (DMA SG) mode and OUT endpoint (data from host), the request length must be a multiple of 32 bits (4 bytes),
			// so for unaligned length we must round up for it to work.  The xcompl (transfer complete) interrupt will trigger on the correct length.
			// If you don't round up the xcompl (transfer complete) interrupt will not trigger on an unaligned length.
			if(dma_mode == TRU_DWC2_DMA_MODE_SG){
				uint32_t r = lxfer->chunk.buffers[i].xfer_size % TRU_DWC2_SGDMA_ALIGNMENT_BYTES;
				if(r){
					lxfer->chunk.buffers[i].xfer_size_aligned = lxfer->chunk.buffers[i].xfer_size + TRU_DWC2_SGDMA_ALIGNMENT_BYTES - r;  // Align up
				}else{
					lxfer->chunk.buffers[i].xfer_size_aligned = lxfer->chunk.buffers[i].xfer_size;  // Already aligned
				}
			}else{
				lxfer->chunk.buffers[i].xfer_size_aligned = lxfer->chunk.buffers[i].xfer_size;  // No alignment requirement
			}

			lxfer->chunk.buffers[i].xferred_size = 0;

			remain_size -= lxfer->chunk.buffers[i].xfer_size;
		}else{
			lxfer->chunk.buffers[i].xfer_size = 0;
			lxfer->chunk.buffers[i].xfer_size_aligned = 0;
			lxfer->chunk.buffers[i].xferred_size = 0;
		}
	}
}

void tru_usb_lxfer_update_chunk_total_xfer_size(tru_usb_lxfer_t *lxfer){
	lxfer->chunk.total_xfer_size = 0;
	lxfer->chunk.total_xfer_size_aligned = 0;
	for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
		lxfer->chunk.total_xfer_size += lxfer->chunk.buffers[i].xfer_size;
		lxfer->chunk.total_xfer_size_aligned += lxfer->chunk.buffers[i].xfer_size_aligned;
	}
}
