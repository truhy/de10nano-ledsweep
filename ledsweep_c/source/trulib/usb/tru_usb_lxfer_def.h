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

	Custom data transfer request definitions.

	To support DMA Scatter Gather mode the chunk buffer is an array of buffers.
	In non DMA Scatter Gather mode only the first element of the array is used,
	i.e. a single buffer is used.
*/

#ifndef TRU_USB_LXFER_DEF_H
#define TRU_USB_LXFER_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "synopsys/tru_dwc2_sgdma_def.h"
#include <stdint.h>

// Max number of chunk buffers
// You may set this to 1 when using only DMA normal mode (non scatter gather mode)
#define TRU_LXFER_MAX_CHUNK_BUFFERS 3

// Transfer buffer
typedef struct{
	uint8_t *buf;                // Data buffer
	uint32_t buf_size;           // Size of the buffer
	uint32_t xfer_size;          // Transfer request size
	uint32_t xfer_size_aligned;  // Transfer request size aligned - required for DMA mode
	uint32_t xferred_size;       // Actual transferred size
}tru_usb_lxfer_buf_t;

// Chunk transfer request and buffer model structure
typedef struct{
	tru_usb_lxfer_buf_t buffers[TRU_LXFER_MAX_CHUNK_BUFFERS];  // Array of buffers
	uint32_t num_registered;                                   // Number of active chunk buffers - this should be 1 in non DMA Scatter Gather mode
	tru_dwc2_sgdma_t sgdma;                                    // Scatter Gather DMA object containing a chained scatter list of DDMA (Descriptor DMA) descriptors, assumes one for each chunk buffer
	uint32_t total_xfer_size;                                  // Total chunk transfer request size
	uint32_t total_xfer_size_aligned;                          // Total chunk transfer request size aligned - required for DMA mode
	uint32_t total_xferred_size;                               // Total chunk actual transferred size
	uint32_t xfer_limit;                                       // Chunk transfer limit constraint.  Set to zero for no constraint
}tru_usb_lxfer_chunk_t;

// Whole transfer request model structure
typedef struct{
	uint32_t total_exp_size;      // Total whole host expected transfer size for data flow types: Control, Interrupt and Bulk.  Note, in certain cases the transfer size may not be the expected size
	uint32_t total_xfer_size;     // Total whole transfer request size
	uint32_t total_xferred_size;  // Total whole actual transferred size
	uint32_t offset;              // The offset of the current chunk transfer relative to the whole buffer (assumes linear)
}tru_usb_lxfer_whole_t;

// Large transfer model structure
typedef struct{
	uint32_t ep_xfer_limit;       // Endpoint transfer limit constraint.  Set to zero for no constraint
	tru_usb_lxfer_chunk_t chunk;
	tru_usb_lxfer_whole_t whole;
}tru_usb_lxfer_t;

#ifdef __cplusplus
}
#endif

#endif
