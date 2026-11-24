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

	Custom data transfer request which consists of a circular buffer to
	support large transfer (whole transfer).

	When a data transfer request is too big for the USB controller endpoint
	we must transfer in many smaller chunks.  The buffer can hold a single
	chunk only and reused (overwritten) per transfer.  The library function
	automatically transfers each chunk until the large transfer is completed.
	Chunk transfer information updates the large transfer information as the
	transfer progresses.
*/

#ifndef TRU_USB_LXFER_H
#define TRU_USB_LXFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "synopsys/tru_dwc2.h"
#include "usb/tru_usb_lxfer_def.h"
#include "usb/tru_usb_ep.h"
#include <stdbool.h>
#include <stdint.h>

void tru_usb_lxfer_init(tru_usb_lxfer_t *lxfer);
void tru_usb_lxfer_deinit(tru_usb_lxfer_t *lxfer);
uint32_t tru_usb_lxfer_get_num_registered(tru_usb_lxfer_t *lxfer);
void tru_usb_lxfer_register_buffer(tru_usb_lxfer_t *lxfer, void* buf, uint32_t size);
void tru_usb_lxfer_unregister_buffers(tru_usb_lxfer_t *lxfer);
tru_usb_lxfer_buf_t *tru_usb_lxfer_get_buffer(tru_usb_lxfer_t *lxfer, uint32_t index);
void tru_usb_lxfer_register_sgdma_descs(tru_usb_lxfer_t *lxfer, tru_dwc2_dev_sgdma_desc_reg_t *desc, uint32_t num_desc);
void tru_usb_lxfer_zero_chunk_xfer_size_all(tru_usb_lxfer_t *lxfer);
void tru_usb_lxfer_fill_chunk_xfer_size_tx(tru_usb_lxfer_t *lxfer, uint32_t index, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode);
void tru_usb_lxfer_fill_chunk_xfer_size_rx(tru_usb_lxfer_t *lxfer, uint32_t index, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode);
void tru_usb_lxfer_fill_chunk_xfer_size_all_tx(tru_usb_lxfer_t *lxfer, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode);
void tru_usb_lxfer_fill_chunk_xfer_size_all_rx(tru_usb_lxfer_t *lxfer, uint32_t xfer_size, tru_dwc2_dma_mode_t dma_mode);
void tru_usb_lxfer_update_chunk_total_xfer_size(tru_usb_lxfer_t *lxfer);

#ifdef __cplusplus
}
#endif

#endif
