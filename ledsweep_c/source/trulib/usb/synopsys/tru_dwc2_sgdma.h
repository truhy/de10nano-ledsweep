/*
	MIT License

	Copyright (c) 2024 Truong Hy

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

	DMA Scatter Gather mode support for the Synopsys DWC2 USB 2.0 OTG controller.
*/

#ifndef TRU_DWC2_SGDMA_H
#define TRU_DWC2_SGDMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_dwc2_sgdma_def.h"
#include <stdbool.h>
#include <stdint.h>

void tru_dwc2_sgdma_init(tru_dwc2_sgdma_t *sgdma);
void tru_dwc2_sgdma_deinit(tru_dwc2_sgdma_t *sgdma);
void tru_dwc2_sgdma_register_descs(tru_dwc2_sgdma_t *sgdma, tru_dwc2_dev_sgdma_desc_reg_t *descs, uint32_t num_desc);
void tru_dwc2_sgdma_setup_desc_epin(tru_dwc2_sgdma_t *sgdma, uint32_t index, void *buf, uint32_t len, bool is_short, bool is_last, bool is_ioc, uint8_t ep_flow_type, uint16_t ep_mps, uint16_t ep_frame_target, uint8_t pid);
void tru_dwc2_sgdma_setup_desc_epout(tru_dwc2_sgdma_t *sgdma, uint32_t index, void *buf, uint32_t len, bool is_last, bool is_ioc, uint8_t ep_flow_type, uint16_t ep_frame_target);
uint32_t tru_dwc2_sgdma_get_desc_size_limit_epin(uint8_t ep_flow_type);
uint32_t tru_dwc2_sgdma_get_desc_size_limit_epout(uint8_t ep_flow_type, uint32_t ep_mps);
void tru_dwc2_sgdma_setup_desc_frame_target_epin(tru_dwc2_sgdma_t *sgdma, uint32_t index, uint16_t ep_frame_target);
void tru_dwc2_sgdma_setup_desc_frame_target_epout(tru_dwc2_sgdma_t *sgdma, uint32_t index, uint16_t ep_frame_target);
void tru_dwc2_sgdma_wait_done(tru_dwc2_dev_sgdma_desc_reg_t *sgdma_desc);
bool tru_dwc2_sgdma_is_last_dma_done(tru_dwc2_sgdma_t *sgdma);
bool tru_dwc2_sgdma_desc_is_dma_done(tru_dwc2_dev_sgdma_desc_reg_t *sgdma_desc);
bool tru_dwc2_sgdma_desc_is_last(tru_dwc2_dev_sgdma_desc_reg_t *sgdma_desc);
void tru_dwc2_sgdma_desc_last_wait_done(tru_dwc2_sgdma_t *sgdma);
uint32_t tru_dwc2_sgdma_curr_remaining_epin(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type);
uint32_t tru_dwc2_sgdma_curr_remaining_epout(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type);
uint32_t tru_dwc2_sgdma_total_remaining_epin(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type);
uint32_t tru_dwc2_sgdma_total_remaining_epout(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type);

#ifdef __cplusplus
}
#endif

#endif
