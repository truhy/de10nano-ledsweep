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
*/

#include "usb/synopsys/tru_dwc2_sgdma.h"
#include "usb/synopsys/tru_dwc2.h"
#include "usb/tru_usb_ep.h"
#include "tru_iom.h"
#include "tru_logger.h"
#include <string.h>
#include <stdlib.h>

void tru_dwc2_sgdma_init(tru_dwc2_sgdma_t *sgdma){
	sgdma->num_desc = 0;
	sgdma->curr_index = 0;
	sgdma->last_index = 0;
}

void tru_dwc2_sgdma_deinit(tru_dwc2_sgdma_t *sgdma){
}

void tru_dwc2_sgdma_register_descs(tru_dwc2_sgdma_t *sgdma, tru_dwc2_dev_sgdma_desc_reg_t *descs, uint32_t num_desc){
	if(num_desc < TRU_DWC2_MAX_SGDMA_XFER_BUFFERS){
		sgdma->dma_descs = descs;
		sgdma->num_desc = num_desc;
		memset(sgdma->dma_descs, 0, sgdma->num_desc * sizeof(tru_dwc2_dev_sgdma_desc_reg_t));
	}
}

/*
	Setup a single transmitting Scatter Gather DMA descriptor at the specified list index.
	Note:
	  Short packet flag doesn't seem to have any effect or is not used in the Cyclone V Synopsys USB controller version.
*/
void tru_dwc2_sgdma_setup_desc_epin(tru_dwc2_sgdma_t *sgdma, uint32_t index, void *buf, uint32_t len, bool is_short, bool is_last, bool is_ioc, uint8_t ep_flow_type, uint16_t ep_mps, uint16_t ep_frame_target, uint8_t pid){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + index;

	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_BUSY;  // Set the DMA descriptor to busy (not ready)
	desc->status.bits.xfersts = TRU_DWC2_DEV_SGDMA_STS_XFER_SUCCESS;
	desc->buf = (uintptr_t)buf;

	if(index == 0){
		sgdma->curr_index = index;
	}

	if(ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
		if(is_last){
			desc->status.bits_iso_in.last = 1;  // This is the last descriptor
			sgdma->last_index = index;
		}else{
			desc->status.bits_iso_in.last = 0;
		}
		desc->status.bits_iso_in.ioc = is_ioc ? 1 : 0;  // Generate an interrupt when this descriptor's transfer has completed?
		//desc->status.bits_iso_in.zlp = is_short ? 1 : 0;  // Send a ZLP after the transfer?
		desc->status.bits_iso_in.zlp = 0;

		sgdma->size_mask = TRU_DWC2_DEV_SGDMA_ISOC_TX_NBYTES_MASK;
		desc->status.bits_iso_in.txsize = len;
		desc->status.bits_iso_in.framenum = ep_frame_target;
		desc->status.bits_iso_in.pid = pid;
	}else{
		if(is_last){
			desc->status.bits.last = 1;  // This is the last descriptor
			sgdma->last_index = index;
		}else{
			desc->status.bits.last = 0;
		}
		desc->status.bits.ioc = is_ioc ? 1 : 0;  // Generate an interrupt when this descriptor's transfer has completed?
		desc->status.bits.zlp = is_short ? 1 : 0;  // Is this a short packet?

		sgdma->size_mask = TRU_DWC2_DEV_SGDMA_NBYTES_MASK;
		desc->status.bits.xfersize = len;
	}

	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_READY;  // Set the DMA descriptor to ready
}

/*
	Setup a single receiving Scatter Gather DMA descriptor at the specified list index.
*/
void tru_dwc2_sgdma_setup_desc_epout(tru_dwc2_sgdma_t *sgdma, uint32_t index, void *buf, uint32_t len, bool is_last, bool is_ioc, uint8_t ep_flow_type, uint16_t ep_frame_target){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + index;

	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_BUSY;  // Set the DMA descriptor to busy (not ready)
	desc->status.bits.xfersts = TRU_DWC2_DEV_SGDMA_STS_XFER_SUCCESS;
	desc->buf = (uintptr_t)buf;

	if(index == 0){
		sgdma->curr_index = index;
	}

	if(ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
		if(is_last){
			desc->status.bits_iso_out.last = 1;  // This is the last descriptor
			sgdma->last_index = index;
		}else{
			desc->status.bits_iso_out.last = 0;
		}
		desc->status.bits_iso_out.ioc = is_ioc ? 1 : 0;  // Generate an interrupt when this descriptor's's transfer has completed?
		desc->status.bits_iso_out.zlp = 0;

		sgdma->size_mask = TRU_DWC2_DEV_SGDMA_ISOC_RX_NBYTES_MASK;
		desc->status.bits_iso_out.rxsize = len;
		//dma_desc->status.bits_iso_out.reserved11 = 0;
		desc->status.bits_iso_out.framenum = ep_frame_target;
		desc->status.bits_iso_out.pid = 0;
	}else{
		if(is_last){
			desc->status.bits.last = 1;  // This is the last descriptor
			sgdma->last_index = index;
		}else{
			desc->status.bits.last = 0;
		}
		desc->status.bits.ioc = is_ioc ? 1 : 0;  // Generate an interrupt when this descriptor's's transfer has completed?
		desc->status.bits.zlp = 0;

		sgdma->size_mask = TRU_DWC2_DEV_SGDMA_NBYTES_MASK;
		desc->status.bits.xfersize = len;
		//dma_desc->status.bits.reserved16_22 = 0;
		desc->status.bits.mtrf = 0;
		desc->status.bits.rxsetup = 0;
	}

	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_READY;  // Set the DMA descriptor to ready
}

void tru_dwc2_sgdma_setup_desc_frame_target_epin(tru_dwc2_sgdma_t *sgdma, uint32_t index, uint16_t ep_frame_target){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + index;

	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_BUSY;  // Set the DMA descriptor to busy (not ready)
	desc->status.bits_iso_in.framenum = ep_frame_target;
	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_READY;  // Set the DMA descriptor to ready
}

void tru_dwc2_sgdma_setup_desc_frame_target_epout(tru_dwc2_sgdma_t *sgdma, uint32_t index, uint16_t ep_frame_target){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + index;

	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_BUSY;  // Set the DMA descriptor to busy (not ready)
	desc->status.bits_iso_out.framenum = ep_frame_target;
	desc->status.bits.bufsts = TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_READY;  // Set the DMA descriptor to ready
}

/*
	Note: In DMA Scatter Gather mode:
		The transmit data buffer max size field has these limitations:
		1. max size field is limited to 12 bits (isochronous) and 16 bits (non-isochronous)
		2. max size field must be 32-bit aligned down
*/
uint32_t tru_dwc2_sgdma_get_desc_size_limit_epin(uint8_t ep_flow_type){
	return (ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS) ? INT_ALIGN_DN(TRU_DWC2_DEV_SGDMA_ISOC_TX_NBYTES_LIMIT, TRU_DWC2_SGDMA_ALIGNMENT_BYTES) : INT_ALIGN_DN(TRU_DWC2_DEV_SGDMA_NBYTES_LIMIT, TRU_DWC2_SGDMA_ALIGNMENT_BYTES);
}

/*
	Note: In DMA Scatter Gather mode:
		the receive data buffer max size field has these limitations:
		1. max size field is limited to 11 bits (isochronous) and 16 bits bits (non-isochronous)
		2. max size field must be 32-bit aligned down
*/
uint32_t tru_dwc2_sgdma_get_desc_size_limit_epout(uint8_t ep_flow_type, uint32_t ep_mps){
	return (ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS) ? INT_ALIGN_DN(TRU_DWC2_DEV_SGDMA_ISOC_RX_NBYTES_LIMIT, TRU_DWC2_SGDMA_ALIGNMENT_BYTES) : INT_ALIGN_DN(TRU_DWC2_DEV_SGDMA_NBYTES_LIMIT, TRU_DWC2_SGDMA_ALIGNMENT_BYTES);
}

/*
	Blocking wait until DMA DONE flag is set.
*/
void tru_dwc2_sgdma_last_wait_done(tru_dwc2_sgdma_t *sgdma){
	tru_dwc2_dev_sgdma_desc_reg_t *last = sgdma->dma_descs + sgdma->last_index;
	while(last->status.bits.bufsts != TRU_DWC2_DEV_SGDMA_STS_BUF_DMA_DONE);
}

/*
	Returns whether the done status flag is set for the last enabled DMA descriptor.
	Does not error check whether the last is NULL.
*/
bool tru_dwc2_sgdma_is_last_dma_done(tru_dwc2_sgdma_t *sgdma){
	tru_dwc2_dev_sgdma_desc_reg_t *last = sgdma->dma_descs + sgdma->last_index;
	return (last->status.bits.bufsts == TRU_DWC2_DEV_SGDMA_STS_BUF_DMA_DONE) ? true : false;
}

bool tru_dwc2_sgdma_desc_is_dma_done(tru_dwc2_dev_sgdma_desc_reg_t *sgdma_desc){
	return (sgdma_desc->status.bits.bufsts == TRU_DWC2_DEV_SGDMA_STS_BUF_DMA_DONE) ? true : false;
}

bool tru_dwc2_sgdma_desc_is_last(tru_dwc2_dev_sgdma_desc_reg_t *sgdma_desc){
	return (sgdma_desc->status.bits.last == 1) ? true : false;
}

/*
	Blocking wait until DMA DONE status flag is set.
*/
void tru_dwc2_sgdma_desc_wait_done(tru_dwc2_dev_sgdma_desc_reg_t *sgdma_desc){
	while(sgdma_desc->status.bits.bufsts != TRU_DWC2_DEV_SGDMA_STS_BUF_DMA_DONE);
}

uint32_t tru_dwc2_sgdma_curr_remaining_epin(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + sgdma->curr_index;

	if(ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
		return desc->status.bits_iso_in.txsize & sgdma->size_mask;
	}else{
		return desc->status.bits.xfersize & sgdma->size_mask;
	}
}

uint32_t tru_dwc2_sgdma_curr_remaining_epout(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + sgdma->curr_index;

	if(ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
		return desc->status.bits_iso_out.rxsize & sgdma->size_mask;
	}else{
		return desc->status.bits.xfersize & sgdma->size_mask;
	}
}

/*
	Calculate the remaining transfer size.  The SG DMA controller updates the descriptor status size field to zero when it successfully transfers all of the data.
	Does not check but assumes the DMA done status flag is set in the descriptors.
*/
uint32_t tru_dwc2_sgdma_total_remaining_epin(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + sgdma->curr_index;
	uint32_t remain = 0;

	for(uint32_t i = sgdma->curr_index; i <= sgdma->last_index; i++){
		if(ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
			remain += desc->status.bits_iso_in.txsize & sgdma->size_mask;
			if(desc->status.bits_iso_in.last) break;  // Exit the loop
		}else{
			remain += desc->status.bits.xfersize & sgdma->size_mask;
			if(desc->status.bits.last) break;  // Exit the loop
		}
		desc++;
	}

	return remain;
}

/*
	Calculate the remaining transfer size.  After data is transferred the SG DMA controller decrements the size field of the descriptor status register.
*/
uint32_t tru_dwc2_sgdma_total_remaining_epout(tru_dwc2_sgdma_t *sgdma, uint8_t ep_flow_type){
	tru_dwc2_dev_sgdma_desc_reg_t *desc = sgdma->dma_descs + sgdma->curr_index;
	uint32_t remain = 0;

	for(uint32_t i = sgdma->curr_index; i <= sgdma->last_index; i++){
		if(ep_flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
			remain += desc->status.bits_iso_out.rxsize & sgdma->size_mask;
			if(desc->status.bits_iso_out.last) break;  // Exit the loop
		}else{
			remain += desc->status.bits.xfersize & sgdma->size_mask;
			if(desc->status.bits.last) break;  // Exit the loop
		}
		desc++;
	}

	return remain;
}
