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

	DMA Scatter Gather mode definitions for the Synopsys DWC2 USB 2.0 OTG
	controller.

	The DMA Scatter Gather mode uses a descriptor list, i.e. array of
	descriptors.  A descriptor item consists of two 32-bit fields:
		1. status
		2. address

	The status field contains multiple flags and transfer size. Some of the
	flags are read-only.  The OUT EP transfer size must be 32-bit aligned, the
	IN EP does not have this boundary alignment requirement.

	The address field contains the buffer address to transfer data from or to
	and must be 32-bit aligned.
 */

#ifndef TRU_DWC2_SGDMA_DEF_H
#define TRU_DWC2_SGDMA_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"
#include "tru_dwc2_def.h"
#include <stdint.h>

// Sets the maximum number of descriptors
#define TRU_DWC2_MAX_SGDMA_XFER_BUFFERS 3

// DMA scatter gather object
typedef struct{
	tru_dwc2_dev_sgdma_desc_reg_t *dma_descs;  // This is the actual SG DMA descriptor list
	uint32_t num_desc;  // Number of SG DMA descriptors (number of list items)
	uint32_t size_mask;
	uint32_t curr_index;
	uint32_t last_index;
}tru_dwc2_sgdma_t;

// A macro to cast a pointer
#define TO_TRU_DWC2_SGDMA_PTR(ptr) ((tru_dwc2_sgdma_t *)ptr)

#ifdef __cplusplus
}
#endif

#endif
