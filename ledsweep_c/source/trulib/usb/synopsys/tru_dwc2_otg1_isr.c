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

#include "usb/synopsys/tru_dwc2_otg1_isr.h"
#include "tru_logger.h"

// ISR USB context pointer
tru_usb_pc_t *tru_dwc2_pc1_isr_context;

/*
	Note: IN = host point of view, i.e. this device transmits data into the host

	Continue or end a transfer on endpoint 0.
	Note, the interrupt status bit must be cleared first before starting another transmission or reception.
*/
static void tru_dwc2_pc1_isr_diepint0_continue_xfer(tru_usb_pc_t *pc){
	tru_usb_pc_epi_t *epin = &pc->eps.epis[0];

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epi_epcompl[0]){
			pc->dc_info[i].callbacks->cb_epi_epcompl[0](pc->dc_info[i].devclass, epin);
		}
	}

	if(
		epin->lxfer.chunk.total_xferred_size == epin->lxfer.chunk.total_xfer_size &&
		epin->lxfer.chunk.total_xferred_size != 0
	){
		// Call back device class functions
		for(uint32_t i = 0; i < pc->num_dc_info; i++){
			if(pc->dc_info[i].callbacks->cb_epi_chunk[0]){
				pc->dc_info[i].callbacks->cb_epi_chunk[0](pc->dc_info[i].devclass, epin);
			}
		}
	}

	// ===================================
	// Check whether transfer is completed
	// ===================================

	// Register level (aka low level hardware): is there more to transfer?
	if(epin->lxfer.chunk.total_xferred_size < epin->lxfer.chunk.total_xfer_size){
		// Start another endpoint transfer to continue the short transfer
		tru_usb_pc_transmit_resume_ep(pc, 0);
		return;  // Exit
	}

	// Library level (aka upper level software): is there more to transfer and the previous transfer was not a ZLP?
	if(
		epin->lxfer.whole.total_xferred_size < epin->lxfer.whole.total_xfer_size &&
		epin->lxfer.chunk.total_xferred_size != 0
	){
		epin->lxfer.whole.offset = epin->lxfer.whole.total_xferred_size;

		// Start another short transfer to continue the long transfer
		//tru_usb_lxfer_zero_chunk_xfer_size_all(&epin->lxfer);
		tru_usb_lxfer_fill_chunk_xfer_size_all_tx(&epin->lxfer, epin->lxfer.whole.total_xfer_size - epin->lxfer.whole.total_xferred_size, pc->dma_mode);
		tru_usb_pc_transmit_sr(pc, 0, false);
		return;  // Exit
	}

	switch(pc->dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
		case TRU_DWC2_DMA_MODE_SB:
			// ==============================================================================
			// Check whether we need to send a ZLP to complete the transfer
			// Note: This does not apply for DMA SG, use the SHORT flag to send a ZLP instead
			// ==============================================================================

			// Note EP0 IN supports only Control data flow so a condition to check data flow type here is unneeded here
			//
			// The USB 2.0 specification rule for data flow packet size constraint:
			// The Data stage of a control transfer from an endpoint to the host is complete when the endpoint does one of the following:
			//   1. Has transferred exactly the amount of data specified during the Setup stage (expected amount)
			//   2. Transfers a packet with a payload size less than wMaxPacketSize or transfers a zero-length packet

			// We send a ZLP when all of these conditions are met:
			//   1. The total transferred size was not the specified amount AND
			//   2. the last packet sent equals the maximum packet size AND
			//   3. the last packet sent is not a ZLP
			if(
				(epin->lxfer.whole.total_xferred_size != epin->lxfer.whole.total_exp_size) &&
				(epin->xfer_size % epin->mps == 0) &&
				(epin->lxfer.chunk.total_xfer_size != 0)
			){
				tru_usb_pc_transmit_zlp_sr(pc, 0, false);  // Schedule a ZLP transmission to complete the transfer
				return;  // Exit
			}

			break;
		case TRU_DWC2_DMA_MODE_SG:
			break;
	}

	// ==================
	// Transfer completed
	// ==================

	// Note: EP0 can only be Control flow so no need to support other data flows

	tru_usb_pc_setup_handler(pc);  // Call handler to complete the setup request

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epi_compl[0]){
			pc->dc_info[i].callbacks->cb_epi_compl[0](pc->dc_info[i].devclass, epin);
		}
	}
}

/*
	Note: IN = host point of view, so device transmits data into host

	USB IN endpoint 0 device mode interrupt request handler.
	Note, the interrupt status bit should be cleared first before enabling another transmission or reception.
*/
static void tru_dwc2_pc1_isr_diepint0(tru_usb_pc_t *pc, tru_dwc2_diepmsk_t *diepmsk){
	tru_dwc2_diepint_t diepint = { .val = TRU_DWC2_DIEPINT_REG(pc->base_addr, 0)->val };

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("INTR: DIEPINT0 (0x%.8lx): 0x%.8lx\n", (uint32_t)TRU_DWC2_DIEPINT_REG(pc->base_addr, 0), diepint.val);
#endif

	// No interrupt flags set?
	//if((diepint.val & diepmsk->val == 0 && (diepint.bits.txfemp == 0 && diepempmsk == 0)) return;

	// Endpoint transfer completed interrupt triggered?
	if(diepint.bits.xfercompl & diepmsk->bits.xfercomplmsk){
		TRU_DWC2_DIEPINT_REG(pc->base_addr, 0)->bits.xfercompl = 1;  // Clear this interrupt flag early
		diepint.bits.xfercompl = 0;  // Exclude this from our copy because we already cleared it

#if defined(TRU_CFG_USB_LOG_DIEPTSIZ) && TRU_CFG_USB_LOG_DIEPTSIZ == 1
		LOG("DI %i %i %lu %i\n",
			TRU_DWC2_DIEPTSIZ0_REG(pc->base_addr)->bits.pktcnt,
			TRU_DWC2_DIEPTSIZ0_REG(pc->base_addr)->bits.xfersize,
			pc->eps.epis[0].xfer_size,
			pc->eps.epos[0].control_stage
		);
#endif

		// For FIFO slave mode the transfer update is handled by the FIFO write process
		if(pc->dma_mode != TRU_DWC2_DMA_MODE_FF){
			tru_usb_pc_update_lxfer_details_epin(pc, 0);
		}
		tru_usb_pc_end_lxfer_details_epin(pc, 0);

#if defined(TRU_CFG_USB_LOG_XPROGRESS) && TRU_CFG_USB_LOG_XPROGRESS == 1
		//tru_dwc2_pc1_print_sg_status(pc, 0);
		if(pc->eps.epis[0].lxfer.chunk.total_xfer_size == 0){
			LOG("INTR: XFERCOMPL EI0: %lu (ZLP) total: %lu/%lu (%lu) bytes\n", pc->eps.epis[0].xferred_size, pc->eps.epis[0].lxfer.chunk.total_xferred_size, pc->eps.epis[0].lxfer.chunk.total_xfer_size, pc->eps.epis[0].lxfer.chunk.total_xfer_size_aligned);
		}else{
			LOG("INTR: XFERCOMPL EI0: %lu total: %lu/%lu (%lu) bytes\n", pc->eps.epis[0].xferred_size, pc->eps.epis[0].lxfer.chunk.total_xferred_size, pc->eps.epis[0].lxfer.chunk.total_xfer_size, pc->eps.epis[0].lxfer.chunk.total_xfer_size_aligned);
		}
#endif

		tru_dwc2_pc1_isr_diepint0_continue_xfer(pc);
	}

#if TRU_USB_PC_TXFIFOE_IRQ_FILL_FIFO == 1
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
		// Note: the TXFEMP trigger bit is read-only and does not require manual clearing, the USB controller will automatically clear this bit
		// We read live registers instead of a copy because they may have changed after the above transfer complete interrupt handling
		if(TRU_DWC2_DIEPINT_REG(pc->base_addr, 0)->bits.txfemp){
			if(TRU_DWC2_DIEPEMPMSK_REG(pc->base_addr)->val & (1 << 0)){
				tru_usb_pc_process_txfifo_write(pc, 0);
			}
		}
	}
#endif

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
#if defined(TRU_DWC2_DIEPINT_BBLEERR_SUPPORTED) && TRU_DWC2_DIEPINT_BBLEERR_SUPPORTED == 1
	if(diepint.bits.bbleerr){
		LOG("INTR: BBLEERR EI%u:\n", 0);
	}
#endif
	if(diepint.bits.ahberr & diepmsk->bits.ahberrmsk){
		LOG("INTR: AHBERR EI%u:\n", 0);
	}
#endif

	TRU_DWC2_DIEPINT_REG(pc->base_addr, 0)->val = diepint.val;  // Clear remaining interrupt flags
}

/*
	Note: IN = host point of view, this device transmits data into the host

	Continue or end a transfer on specified endpoint 1 to 15.
	Note, the interrupt status bit should be cleared first before enabling another transmission or reception.
*/
static void tru_dwc2_pc1_isr_diepint1_15_continue_xfer(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epi_t *epin = &pc->eps.epis[ep_num];

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epi_epcompl[ep_num]){
			pc->dc_info[i].callbacks->cb_epi_epcompl[ep_num](pc->dc_info[i].devclass, epin);
		}
	}

	if(
		epin->lxfer.chunk.total_xferred_size == epin->lxfer.chunk.total_xfer_size &&
		epin->lxfer.chunk.total_xferred_size != 0
	){
		// Call back device class functions
		for(uint32_t i = 0; i < pc->num_dc_info; i++){
			if(pc->dc_info[i].callbacks->cb_epi_chunk[ep_num]){
				pc->dc_info[i].callbacks->cb_epi_chunk[ep_num](pc->dc_info[i].devclass, epin);
			}
		}
	}

	// ===================================
	// Check whether transfer is completed
	// ===================================

	// Register level (aka low level hardware): is there more to transfer?
	if(epin->lxfer.chunk.total_xferred_size < epin->lxfer.chunk.total_xfer_size){
		// Start another endpoint transfer to continue the short transfer
		tru_usb_pc_transmit_resume_ep(pc, ep_num);
		return;  // Exit
	}

	// Library level (aka upper level software): is there more to transfer and the previous transfer was not a ZLP?
	if(
		epin->lxfer.whole.total_xferred_size < epin->lxfer.whole.total_xfer_size &&
		epin->lxfer.chunk.total_xferred_size != 0
	){
		epin->lxfer.whole.offset = epin->lxfer.whole.total_xferred_size;

		// Start another short transfer to continue the long transfer
		//tru_usb_lxfer_zero_chunk_xfer_size_all(&epin->lxfer);
		tru_usb_lxfer_fill_chunk_xfer_size_all_tx(&epin->lxfer, epin->lxfer.whole.total_xfer_size - epin->lxfer.whole.total_xferred_size, pc->dma_mode);
		tru_usb_pc_transmit_sr(pc, ep_num, false);
		return;  // Exit
	}

	switch(pc->dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
		case TRU_DWC2_DMA_MODE_SB:
			// ==============================================================================
			// Check whether we need to send a ZLP to complete the transfer
			// Note: This does not apply for DMA SG, use the SHORT flag to send a ZLP instead
			// ==============================================================================

			switch(epin->flow_type){
				case TRU_USB_EP_TRANSFER_TYPE_CONTROL:
				case TRU_USB_EP_TRANSFER_TYPE_INTERRUPT:
				case TRU_USB_EP_TRANSFER_TYPE_BULK:
					// The USB 2.0 specification rule for data flow packet size constraint:
					// For the data flow transfer types control, interrupt and bulk, the transfer from an endpoint to the host is
					// complete when the endpoint does one of the following:
					//   1. Has transferred exactly the amount of data specified during the Setup stage (expected amount)
					//   2. Transfers a packet with a payload size less than wMaxPacketSize or transfers a zero-length packet

					// We send a ZLP when all of these conditions are met:
					//   1. The total transferred size was not the specified amount AND
					//   2. the last packet sent equals the maximum packet size AND
					//   3. the last packet sent is not a ZLP
					if(
						epin->lxfer.whole.total_xferred_size != epin->lxfer.whole.total_exp_size &&
						epin->xfer_size % epin->mps == 0 &&
						epin->lxfer.chunk.total_xfer_size != 0
					){
						tru_usb_pc_transmit_zlp_sr(pc, ep_num, false);  // Schedule a ZLP transmission to complete the transfer
						return;  // Exit
					}
					break;
			}

			break;
		case TRU_DWC2_DMA_MODE_SG:
			break;
	}

	// ==================
	// Transfer completed
	// ==================

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epi_compl[ep_num]){
			pc->dc_info[i].callbacks->cb_epi_compl[ep_num](pc->dc_info[i].devclass, epin);
		}
	}
}

/*
	Note: IN = host point of view, so device transmits data into host

	USB IN endpoints 1 to 15 device mode interrupt request handler.
	Note, the interrupt status bit should be cleared first before enabling another transmission or reception.
*/
static void tru_dwc2_pc1_isr_diepint1_15(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_diepmsk_t *diepmsk){
	tru_dwc2_diepint_t diepint = { .val = TRU_DWC2_DIEPINT_REG(pc->base_addr, ep_num)->val };

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("INTR: DIEPINT%u (0x%.8lx): 0x%.8lx\n", ep_num, (uint32_t)TRU_DWC2_DIEPINT_REG(pc->base_addr, ep_num), diepint.val);
#endif

	// No interrupt flags set?
	//if((diepint.val & diepmsk->val == 0 && (diepint.bits.txfemp == 0 && diepempmsk == 0)) return;

	// Endpoint transfer completed interrupt triggered?
	if(diepint.bits.xfercompl & diepmsk->bits.xfercomplmsk){
		TRU_DWC2_DIEPINT_REG(pc->base_addr, ep_num)->bits.xfercompl = 1;  // Clear this interrupt flag early
		diepint.bits.xfercompl = 0;  // Exclude this from our copy because we already cleared it

		// For FIFO slave mode the transfer update is handled by the FIFO write process
		if(pc->dma_mode != TRU_DWC2_DMA_MODE_FF){
			tru_usb_pc_update_lxfer_details_epin(pc, ep_num);
		}
		tru_usb_pc_end_lxfer_details_epin(pc, ep_num);

#if defined(TRU_CFG_USB_LOG_XPROGRESS) && TRU_CFG_USB_LOG_XPROGRESS == 1
		tru_usb_pc_epi_t *epin = &pc->eps.epis[ep_num];
		//tru_dwc2_pc1_print_sg_status(pc, ep_num);
		if(epin->lxfer.chunk.total_xfer_size == 0){
			LOG("INTR: XFERCOMPL EI%u: %lu (ZLP) total: %lu/%lu (%lu) bytes\n", ep_num, epin->xferred_size, epin->lxfer.chunk.total_xferred_size, epin->lxfer.chunk.total_xfer_size, epin->lxfer.chunk.total_xfer_size_aligned);
		}else{
			LOG("INTR: XFERCOMPL EI%u: %lu total: %lu/%lu (%lu) bytes\n", ep_num, epin->xferred_size, epin->lxfer.chunk.total_xferred_size, epin->lxfer.chunk.total_xfer_size, epin->lxfer.chunk.total_xfer_size_aligned);
		}
#endif

		tru_dwc2_pc1_isr_diepint1_15_continue_xfer(pc, ep_num);
	}

#if TRU_USB_PC_TXFIFOE_IRQ_FILL_FIFO == 1
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
		// Note: the TXFEMP trigger bit is read-only and does not require manual clearing, the USB controller will automatically clear this bit
		// We read live registers instead of a copy because they may have changed after the above transfer complete interrupt handling
		if(TRU_DWC2_DIEPINT_REG(pc->base_addr, ep_num)->bits.txfemp){
			if(TRU_DWC2_DIEPEMPMSK_REG(pc->base_addr)->val & (1 << ep_num)){
				tru_usb_pc_process_txfifo_write(pc, ep_num);
			}
		}
	}
#endif

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
#if defined(TRU_DWC2_DIEPINT_BBLEERR_SUPPORTED) && TRU_DWC2_DIEPINT_BBLEERR_SUPPORTED == 1
	if(diepint.bits.bbleerr){
		LOG("INTR: BBLEERR EI%u:\n", ep_num);
	}
#endif
	if(diepint.bits.ahberr & diepmsk->bits.ahberrmsk){
		LOG("INTR: AHBERR EI%u:\n", ep_num);
	}
#endif

	TRU_DWC2_DIEPINT_REG(pc->base_addr, ep_num)->val = diepint.val;  // Clear remaining interrupt flags
}

/*
	Note: OUT = host point of view, i.e. this device receives data out from the host

	Continue or end a transfer on endpoint 0.
	Note, the interrupt status bit should be cleared first before enabling another transmission or reception.
*/
static void tru_dwc2_pc1_isr_doepint0_continue_xfer(tru_usb_pc_t *pc){
	tru_usb_pc_epo_t *epout = &pc->eps.epos[0];

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epo_epcompl[0]){
			pc->dc_info[i].callbacks->cb_epo_epcompl[0](pc->dc_info[i].devclass, epout);
		}
	}

	if(
		epout->lxfer.chunk.total_xferred_size != 0 &&
		(epout->lxfer.chunk.total_xferred_size == epout->lxfer.chunk.total_xfer_size ||
		epout->xferred_size_prezlp != epout->xfer_size)
	){
		// Call back device class functions
		for(uint32_t i = 0; i < pc->num_dc_info; i++){
			if(pc->dc_info[i].callbacks->cb_epo_chunk[0]){
				pc->dc_info[i].callbacks->cb_epo_chunk[0](pc->dc_info[i].devclass, epout);
			}
		}
	}

	// ===================================
	// Check whether transfer is completed
	// ===================================

	if(epout->control_stage != TRU_USB_PC_EP_CONTROL_STAGE_SETUP){
		// Register level (aka low level hardware): host transferred exactly as requested?  else, the host wants to end the transfer early, i.e. sent less than endpoint requested length
		if(epout->xferred_size_prezlp == epout->xfer_size){
			// Register level (aka low level hardware): is there more to transfer and not a ZLP?
			if(
				(epout->lxfer.chunk.total_xferred_size < epout->lxfer.chunk.total_xfer_size &&
				epout->xferred_size != 0)
			){
				// Start another endpoint transfer to continue the short transfer
				tru_usb_pc_receive_resume_ep(pc, 0);
				return;  //Exit
			}

			// Library level (aka upper level software): is there more to transfer and the previous chunk was not a ZLP?
			if(
				epout->lxfer.whole.total_xferred_size < epout->lxfer.whole.total_xfer_size &&
				epout->xferred_size != 0
			){
				epout->lxfer.whole.offset = epout->lxfer.whole.total_xferred_size;

				// Start another short transfer to continue the long transfer
				//tru_usb_lxfer_zero_chunk_xfer_size_all(&epout->lxfer);
				tru_usb_lxfer_fill_chunk_xfer_size_all_rx(&epout->lxfer, epout->lxfer.whole.total_xfer_size - epout->lxfer.whole.total_xferred_size, pc->dma_mode);
				tru_usb_pc_receive_sr(pc, 0, TRU_DWC2_STALL_CLEAR, false);
				return;  // Exit
			}
		}
	}

	// ===========================================================
	// We leave the ZLP handling with the setup handler to process
	// ===========================================================

	// ==================
	// Transfer completed
	// ==================

	// Note: EP0 can only be Control flow so no need to support other data flows

	tru_usb_pc_setup_handler(pc);  // Call handler to complete the setup request

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epo_compl[0]){
			pc->dc_info[i].callbacks->cb_epo_compl[0](pc->dc_info[i].devclass, epout);
		}
	}
}

/*
	Note: OUT = host point of view, so device receives data out of host

	USB out endpoint 0 device mode interrupt request handler.
	Note, the interrupt status bit should be cleared first before enabling another transmission or reception.
*/
static void tru_dwc2_pc1_isr_doepint0(tru_usb_pc_t *pc, tru_dwc2_doepmsk_t *doepmsk){
	tru_dwc2_doepint_t doepint = { .val = TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->val };
	bool handle_setup_intr;
	bool handle_xcomp_intr;
	tru_usb_pc_epo_t *epout = &pc->eps.epos[0];

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("INTR: DOEPINT0 (0x%.8lx): 0x%.8lx\n", (uint32_t)TRU_DWC2_DOEPINT_REG(pc->base_addr, 0), doepint.val);
#endif

	// No interrupt flags set?
	//if((doepint.val & doepmsk->val) == 0) return;

	// Check whether we need to handle a setup interrupt
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
		#if TRU_DWC2_PC_HANDLE_DONE_IN_RXLVL == 1
				handle_setup_intr = false;
		#else
				// In FIFO slave mode the stpktrx is not triggered (unused) so we don't need to handle it
				handle_setup_intr = (doepint.bits.setup & doepmsk->bits.setupmsk) ? true : false;
		#endif
	}else{
		#if defined(TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED) && TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED == 1
			// If the stpktrx is triggered then setup should also be triggered,
			// otherwise we postpone handling, i.e. wait for the setup interrupt.
			// I think these interrupt flags are actually equivalent to the slave RXFIFO statuses:
			//   - stpktrx = SETUPRX
			//   - setup = SETUPDONE
			//   - stsphsrx = OUTRX
			// So the reason we need to wait is because the transfer is not actually completed yet,
			// completion is when we get either:
			//   - setup without stpktrx
			//   - stpktrx with setup
			// For non DMA scatter gather mode, setup data out and status out stage, completion is when we get:
			//   - xfercompl without stpktrx and setup
			// For DMA scatter gather mode and setup data out stage, completion is when we get:
			//   - stsphsrx + xfercompl
			if(doepint.bits.setup & doepmsk->bits.setupmsk){
				handle_setup_intr = true;
			}else if(doepint.bits.stpktrx & doepmsk->bits.stpktrxmsk){
				// I believe this is a hardware bug:
				//   Clearing the stpktrx flag also automatically clears the setup flag!
				//   The problem occurs when we process the stpktrx and clear it, but if the USB controller just happens to trigger (set)
				//   the setup flag before we do that, this will automatically clear the setup flag, causing our code unable to handle
				//   the setup interrupt and we end up with a hang!
				doepint.bits.stpktrx = 0;  // Exclude this flag from being cleared, we need to wait for the setup flag
				doepint.bits.xfercompl = 0;  // Exclude this flag from being cleared, we need to wait for the setup flag
				handle_setup_intr = false;
			}else{
				handle_setup_intr = false;
			}
		#else
			handle_setup_intr = (doepint.bits.setup & doepmsk->bits.setupmsk) ? true : false;  // On older controller versions we get a setup flag without an xfercompl flag
		#endif
	}

	if(handle_setup_intr){
		if(doepint.bits.xfercompl){
			TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->bits.xfercompl = 1;  // Clear this interrupt flag early
			doepint.bits.xfercompl = 0;  // Exclude this from our copy because we already cleared it
		}

		#if defined(TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED) && TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED == 1
			if(doepint.bits.stpktrx){
				TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->bits.stpktrx = 1;  // Clear this interrupt flag early
				doepint.bits.stpktrx = 0;  // Exclude this from our copy because we already cleared it
			}
		#endif

		if(doepint.bits.stsphsrx){
			TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->bits.stsphsrx = 1;  // Clear this interrupt flag early
			doepint.bits.stsphsrx = 0;  // Exclude this from our copy because we already cleared it
		}

		TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->bits.setup = 1;  // Clear this interrupt flag early
		doepint.bits.setup = 0;  // Exclude this from our copy because we already cleared it

		#if defined(TRU_CFG_USB_LOG_DOEPTSIZ) && TRU_CFG_USB_LOG_DOEPTSIZ == 1
			LOG("SR %i %i %i %lu %i\n",
				TRU_DWC2_DOEPTSIZ0_REG(pc->base_addr)->bits.supcnt,
				TRU_DWC2_DOEPTSIZ0_REG(pc->base_addr)->bits.pktcnt,
				TRU_DWC2_DOEPTSIZ0_REG(pc->base_addr)->bits.xfersize,
				epout->xfer_size,
				epout->control_stage
			);
		#endif

		if(epout->control_stage == TRU_USB_PC_EP_CONTROL_STAGE_SETUP){
			if(pc->dma_mode != TRU_DWC2_DMA_MODE_FF){
				tru_usb_pc_update_lxfer_details_epout(pc, 0);
				tru_usb_pc_end_lxfer_details_epout(pc, 0);
			}

			#if defined(TRU_CFG_USB_LOG_XPROGRESS) && TRU_CFG_USB_LOG_XPROGRESS == 1
				//tru_dwc2_pc1_print_sg_status(pc, 0);
				LOG("INTR: SETUP EO0: %lu total: %lu/%lu (%lu) bytes\n", epout->xferred_size, epout->lxfer.chunk.total_xferred_size, epout->lxfer.chunk.total_xfer_size, epout->lxfer.chunk.total_xfer_size_aligned);
			#endif

			tru_dwc2_pc1_isr_doepint0_continue_xfer(pc);
		}else{
			#if defined(TRU_CFG_USB_LOG_DOEPTSIZ) && TRU_CFG_USB_LOG_DOEPTSIZ == 1
				LOG("Invalid setup intr\n");
			#endif
		}
	}

	// Check whether we need to handle a transfer complete interrupt
	switch(pc->dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			#if TRU_DWC2_PC0_HANDLE_DONE_IN_RXLVL == 1
				handle_xcomp_intr = false;
			#else
				handle_xcomp_intr = (doepint.bits.xfercompl & doepmsk->bits.xfercomplmsk) ? true : false;
			#endif
			break;
		case TRU_DWC2_DMA_MODE_SG:
			if(doepint.bits.xfercompl & doepmsk->bits.xfercomplmsk){
				if(!(doepint.bits.stsphsrx & doepmsk->bits.stsphsrxmsk) && epout->control_stage == TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT){
						doepint.bits.xfercompl = 0;  // Exclude this flag from being cleared, we need to wait for the stsphsrx flag
						handle_xcomp_intr = false;
				}else{
					handle_xcomp_intr = true;
				}
			}else{
				handle_xcomp_intr = false;
			}
			break;
		default:
			handle_xcomp_intr = (doepint.bits.xfercompl & doepmsk->bits.xfercomplmsk) ? true : false;
	}

	if(handle_xcomp_intr){
		#if defined(TRU_CFG_USB_LOG_DOEPTSIZ) && TRU_CFG_USB_LOG_DOEPTSIZ == 1
			LOG("DO %i %i %i %lu %i\n",
				TRU_DWC2_DOEPTSIZ0_REG(pc->base_addr)->bits.supcnt,
				TRU_DWC2_DOEPTSIZ0_REG(pc->base_addr)->bits.pktcnt,
				TRU_DWC2_DOEPTSIZ0_REG(pc->base_addr)->bits.xfersize,
				epout->xfer_size,
				epout->control_stage
			);
		#endif

		// If we get a xfercompl without the stpktrx + setup or stsphsrx interrupts then it should be status out or data out stage
		if(epout->control_stage == TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT || epout->control_stage == TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT){
			TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->bits.xfercompl = 1;  // Clear this interrupt flag early
			doepint.bits.xfercompl = 0;  // Exclude this from our copy because we already cleared it

			#if defined(TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED) && TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED == 1
				if(doepint.bits.stpktrx){
					TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->bits.stpktrx = 1;  // Clear this interrupt flag early
					doepint.bits.stpktrx = 0;  // Exclude this from our copy because we already cleared it
				}
			#endif

			if(doepint.bits.stsphsrx){
				TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->bits.stsphsrx = 1;  // Clear this interrupt flag early
				doepint.bits.stsphsrx = 0;  // Exclude this from our copy because we already cleared it
			}

			if(pc->dma_mode != TRU_DWC2_DMA_MODE_FF){
				tru_usb_pc_update_lxfer_details_epout(pc, 0);
				tru_usb_pc_end_lxfer_details_epout(pc, 0);
			}

			#if defined(TRU_CFG_USB_LOG_XPROGRESS) && TRU_CFG_USB_LOG_XPROGRESS == 1
				if(epout->lxfer.chunk.total_xfer_size == 0){
					LOG("INTR: XFERCOMPL EO0: %lu (ZLP) total: %lu/%lu (%lu) bytes\n", epout->xferred_size, epout->lxfer.chunk.total_xferred_size, epout->lxfer.chunk.total_xfer_size, epout->lxfer.chunk.total_xfer_size_aligned);
				}else{
					LOG("INTR: XFERCOMPL EO0: %lu total: %lu/%lu (%lu) bytes\n", epout->xferred_size, epout->lxfer.chunk.total_xferred_size, epout->lxfer.chunk.total_xfer_size, epout->lxfer.chunk.total_xfer_size_aligned);
				}
			#endif

			tru_dwc2_pc1_isr_doepint0_continue_xfer(pc);
		}else{
			// Ouch, we are in setup stage!  Let's wait for the stpktrx + setup or stsphsrx interrupts

			#if defined(TRU_CFG_USB_LOG_DOEPTSIZ) && TRU_CFG_USB_LOG_DOEPTSIZ == 1
				LOG("Invalid xcompl intr\n");
			#endif

			doepint.bits.xfercompl = 0;  // Exclude this flag from being cleared, we need to wait for the stpktrx + setup or stsphsrx trigger
		}
	}

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	if(doepint.bits.stsphsrx & doepmsk->bits.stsphsrxmsk){
		LOG("INTR: STSPHSRX EO%u\n", 0);
	}

	if(doepint.bits.b2bsetup & doepmsk->bits.b2bsetupmsk){
		LOG("INTR: B2B SETUP EO%u\n", 0);
	}

	if(doepint.bits.outtknepdis & doepmsk->bits.outtknepdismsk){
		LOG("INTR: OUTTKNEPDIS EO%u\n", 0);
	}

	if(doepint.bits.bna & doepmsk->bits.bnamsk){
		LOG("INTR: BNA EO%u\n", 0);
	}

	if(doepint.bits.outpkterr & doepmsk->bits.outpkterrmsk){
		LOG("INTR: OUTPKTERR EO%u\n", 0);
	}

	if(doepint.bits.bbleerr & doepmsk->bits.bbleerrmsk){
		LOG("INTR: BBLEERR EO%u\n", 0);
	}

	if(doepint.bits.ahberr & doepmsk->bits.ahberrmsk){
		LOG("INTR: AHBERR EO%u\n", 0);
	}

	if(doepint.bits.nak & doepmsk->bits.nakmsk){
		LOG("INTR: NAK EO%u\n", 0);
	}

#if defined(TRU_DWC2_DOEPINT_NYET_SUPPORTED) && TRU_DWC2_DOEPINT_NYET_SUPPORTED == 1
	if(doepint.bits.nyet & doepmsk->bits.nyetmsk){
		LOG("INTR: NYET EO%u\n", 0);
	}
#endif
#endif

	TRU_DWC2_DOEPINT_REG(pc->base_addr, 0)->val = doepint.val;  // Clear remaining interrupt flags
}

/*
	Note: OUT = host point of view, i.e. this device receives data out from the host

	Continue or end a transfer on the specified endpoint 1 to 15.
	Note, the interrupt status bit should be cleared first before enabling another transmission or reception.
*/
static void tru_dwc2_pc1_isr_doepint1_15_continue_xfer(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epo_t *epout = &pc->eps.epos[ep_num];

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epo_epcompl[ep_num]){
			pc->dc_info[i].callbacks->cb_epo_epcompl[ep_num](pc->dc_info[i].devclass, &pc->eps.epos[ep_num]);
		}
	}

	if(
		epout->lxfer.chunk.total_xferred_size != 0 &&
		(epout->lxfer.chunk.total_xferred_size == epout->lxfer.chunk.total_xfer_size ||
		epout->xferred_size_prezlp != epout->xfer_size)
	){
		// Call back device class functions
		for(uint32_t i = 0; i < pc->num_dc_info; i++){
			if(pc->dc_info[i].callbacks->cb_epo_chunk[ep_num]){
				pc->dc_info[i].callbacks->cb_epo_chunk[ep_num](pc->dc_info[i].devclass, &pc->eps.epos[ep_num]);
			}
		}
	}

	// ===================================
	// Check whether transfer is completed
	// ===================================

	// Register level (aka low level hardware): host transferred exactly as requested?  else, the host wants to end the transfer early, i.e. sent less than endpoint requested length
	if(epout->xferred_size_prezlp == epout->xfer_size){
		// Register level (aka low level hardware) continue to transfer condition: is there more to transfer and not a ZLP?
		if(
			(epout->lxfer.chunk.total_xferred_size < epout->lxfer.chunk.total_xfer_size &&
			epout->xferred_size != 0)
		){
			// Start another endpoint transfer to continue the short transfer
			tru_usb_pc_receive_resume_ep(pc, ep_num);
			return;  // Exit
		}

		// Library level (aka upper level software): is there more to transfer and the previous chunk was not a ZLP?
		if(
			epout->lxfer.whole.total_xferred_size < epout->lxfer.whole.total_xfer_size &&
			epout->xferred_size != 0
		){
			epout->lxfer.whole.offset = epout->lxfer.whole.total_xferred_size;

			// Start another short transfer to continue the long transfer
			//tru_usb_lxfer_zero_chunk_xfer_size_all(&epout->lxfer);
			tru_usb_lxfer_fill_chunk_xfer_size_all_rx(&epout->lxfer, epout->lxfer.whole.total_xfer_size - epout->lxfer.whole.total_xferred_size, pc->dma_mode);
			tru_usb_pc_receive_sr(pc, ep_num, TRU_DWC2_STALL_CLEAR, false);
			return;  // Exit
		}
	}

	// ========================================================================
	// We leave the ZLP handling with the caller (callback function) to process
	// ========================================================================

	// ==================
	// Transfer completed
	// ==================

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_epo_compl[ep_num]){
			pc->dc_info[i].callbacks->cb_epo_compl[ep_num](pc->dc_info[i].devclass, &pc->eps.epos[ep_num]);
		}
	}
}

/*
	Note: OUT = host point of view, so device receives data out of host

	USB out endpoints 1 to 15 device mode interrupt request handler.
	Note, the interrupt status bit must be cleared first before starting another transmission or reception.
*/
static void tru_dwc2_pc1_isr_doepint1_15(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_doepmsk_t *doepmsk){
	tru_dwc2_doepint_t doepint = { .val = TRU_DWC2_DOEPINT_REG(pc->base_addr, ep_num)->val };
	bool handle_xcomp_intr;

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("INTR: DOEPINT%u (0x%.8lx): 0x%.8lx\n", ep_num, (uint32_t)TRU_DWC2_DOEPINT_REG(pc->base_addr, ep_num), doepint.val);
#endif

	// No interrupt flags set?
	//if((doepint.val & doepmsk->val) == 0) return;

	// Check whether we need to handle transfer complete interrupt
#if TRU_DWC2_PC_HANDLE_DONE_IN_RXLVL == 1
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
		handle_xcomp_intr = false;
	}else{
		handle_xcomp_intr = doepint.bits.xfercompl & doepmsk->bits.xfercomplmsk;
	}
#else
	handle_xcomp_intr = doepint.bits.xfercompl & doepmsk->bits.xfercomplmsk;
#endif

	// Endpoint transfer completed interrupt triggered?
	if(handle_xcomp_intr){
		TRU_DWC2_DOEPINT_REG(pc->base_addr, ep_num)->bits.xfercompl = 1;  // Clear this interrupt flag early
		doepint.bits.xfercompl = 0;  // Exclude this from our copy because we already cleared it

		if(pc->dma_mode != TRU_DWC2_DMA_MODE_FF){
			tru_usb_pc_update_lxfer_details_epout(pc, ep_num);
			tru_usb_pc_end_lxfer_details_epout(pc, ep_num);
		}

#if defined(TRU_CFG_USB_LOG_XPROGRESS) && TRU_CFG_USB_LOG_XPROGRESS == 1
		tru_usb_pc_epo_t *epout = &pc->eps.epos[ep_num];
		if(epout->lxfer.chunk.total_xfer_size == 0){
			LOG("INTR: XFERCOMPL EO%u:  %lu (ZLP) total %lu/%lu (%lu) bytes\n", ep_num, epout->xferred_size, epout->lxfer.chunk.total_xferred_size, epout->lxfer.chunk.total_xfer_size, epout->lxfer.chunk.total_xfer_size_aligned);
		}else{
			LOG("INTR: XFERCOMPL EO%u: %lu total: %lu/%lu (%lu) bytes\n", ep_num, epout->xferred_size, epout->lxfer.chunk.total_xferred_size, epout->lxfer.chunk.total_xfer_size, epout->lxfer.chunk.total_xfer_size_aligned);
		}
#endif

		tru_dwc2_pc1_isr_doepint1_15_continue_xfer(pc, ep_num);
	}

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	if(doepint.bits.stsphsrx & doepmsk->bits.stsphsrxmsk){
		LOG("INTR: STSPHSRX EO%u\n", ep_num);
	}

	if(doepint.bits.b2bsetup & doepmsk->bits.b2bsetupmsk){
		LOG("INTR: B2B SETUP EO%u\n", ep_num);
	}

	if(doepint.bits.outtknepdis & doepmsk->bits.outtknepdismsk){
		LOG("INTR: OUTTKNEPDIS EO%u\n", ep_num);
	}

	if(doepint.bits.bna & doepmsk->bits.bnamsk){
		LOG("INTR: BNA EO%u\n", ep_num);
	}

	if(doepint.bits.outpkterr & doepmsk->bits.outpkterrmsk){
		LOG("INTR: OUTPKTERR EO%u\n", ep_num);
	}

	if(doepint.bits.bbleerr & doepmsk->bits.bbleerrmsk){
		LOG("INTR: BBLEERR EO%u\n", ep_num);
	}

	if(doepint.bits.ahberr & doepmsk->bits.ahberrmsk){
		LOG("INTR: AHBERR EO%u\n", ep_num);
	}

	if(doepint.bits.nak & doepmsk->bits.nakmsk){
		LOG("INTR: NAK EO%u\n", ep_num);
	}

#if defined(TRU_DWC2_DOEPINT_NYET_SUPPORTED) && TRU_DWC2_DOEPINT_NYET_SUPPORTED == 1
	if(doepint.bits.nyet & doepmsk->bits.nyetmsk){
		LOG("INTR: NYET EO%u\n", ep_num);
	}
#endif

	if(doepint.bits.ahberr & doepmsk->bits.ahberrmsk){
		LOG("INTR: AHBERR EO%u\n", ep_num);
	}

#if defined(TRU_DWC2_DOEPINT_PKTDRPSTS_SUPPORTED) && TRU_DWC2_DOEPINT_PKTDRPSTS_SUPPORTED == 1
	if(doepint.bits.pktdrpsts){
		LOG("INTR: PKTDRPSTS EO%u\n", ep_num);
	}
#endif
#endif

	TRU_DWC2_DOEPINT_REG(pc->base_addr, ep_num)->val = doepint.val;  // Clear remaining interrupt flags
}

static void tru_dwc2_pc1_isr_daint_epin(tru_usb_pc_t *pc){
	tru_dwc2_daint_t daint = { .val = TRU_DWC2_DAINT_REG(pc->base_addr)->val & TRU_DWC2_DAINTMSK_REG(pc->base_addr)->val & 0x0000ffff };
	tru_dwc2_diepmsk_t diepmsk = { .val = TRU_DWC2_DIEPMSK_REG(pc->base_addr)->val };
	uint32_t daint_shift = daint.val;
	uint8_t ep_num = 0;

	// No interrupt flags set?
	if(daint_shift == 0) return;

	// Iterate in endpoint interrupt bit flags
	while(daint_shift){
		// In endpoint ep_num interrupt triggered?
		if(daint_shift & 1){
			if(ep_num == 0){
				tru_dwc2_pc1_isr_diepint0(pc, &diepmsk);
			}else{
				tru_dwc2_pc1_isr_diepint1_15(pc, ep_num, &diepmsk);
			}
		}

		// Move to next EP bit
		daint_shift >>= 1;
		ep_num++;
	}
}

/*
	USB all out endpoints device mode interrupt request handler.
*/
static void tru_dwc2_pc1_isr_daint_epout(tru_usb_pc_t *pc){
	tru_dwc2_daint_t daint = { .val = ((TRU_DWC2_DAINT_REG(pc->base_addr)->val & TRU_DWC2_DAINTMSK_REG(pc->base_addr)->val) >> 16) & 0x0000ffff };
	tru_dwc2_doepmsk_t doepmsk = { .val = TRU_DWC2_DOEPMSK_REG(pc->base_addr)->val };
	uint32_t daint_shift = daint.val;
	uint8_t ep_num = 0;

	// No interrupt flags set?
	if(daint_shift == 0) return;

	// Iterate out endpoint interrupt bit flags
	while(daint_shift){
		// Out endpoint ep_num interrupt triggered?
		if(daint_shift & 1){
			if(ep_num == 0){
				tru_dwc2_pc1_isr_doepint0(pc, &doepmsk);
			}else{
				tru_dwc2_pc1_isr_doepint1_15(pc, ep_num, &doepmsk);
			}
		}

		// Move to next EP bit mask
		daint_shift >>= 1;
		ep_num++;
	}
}

static void tru_dwc2_pc1_isr_stop_xfers(tru_usb_pc_t *pc){
	// Disable all OUT endpoints
	// When the device is unexpectedly unplugged with primed transfers, sometimes the controller is in a bad state
	// and the global OUT NAK will not work, then waiting on it will hang this application because it will never
	// turn on.  Since the device was already unplugged we can forcibly disable the endpoints
	tru_dwc2_disable_epout_forced_all(pc->base_addr, false, pc->hwcfg.num_dev_ep);
	//tru_dwc2_disable_epout_wait_all(pc->base_addr, pc->dma_mode, false, pc->hwcfg.num_dev_ep);
	for(uint8_t ep_num = 0; ep_num < pc->hwcfg.num_dev_ep; ep_num++){
		tru_usb_pc_end_lxfer_details_epout(pc, ep_num);
	}

	// Disable all IN endpoints
	for(uint8_t ep_num = 0; ep_num < pc->hwcfg.num_dev_in_ep; ep_num++){
		tru_dwc2_disable_epin_wait(pc->base_addr, ep_num, false);
		tru_usb_pc_end_lxfer_details_epin(pc, ep_num);
	}

	// We need to de-configure active USB configuration
	if(pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
		tru_usb_pc_setup_set_config_deact_ep(pc);  // Deactivate and disable endpoints (excludes IN & OUT EP0) within the active configuration and also flushes all FIFOs
		pc->state = TRU_USB_PC_DEV_STATE_DEFAULT;
	}

	/*
	// Disable OUT EP0.  Note, after a few seconds the controller will automatically enable EP0
	tru_dwc2_disable_epout_forced(pc->base_addr, 0, false);
	tru_usb_pc_end_lxfer_details_epout(pc, 0);

	// Disable IN EP0.  Note, after a few seconds the controller will automatically enable EP0
	tru_dwc2_disable_epin_wait(pc->base_addr, 0, false);
	tru_usb_pc_end_lxfer_details_epin(pc, 0);

	// We need to de-configure active USB configuration
	if(pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
		tru_usb_pc_setup_set_config_deact_ep(pc);  // Deactivate and disable endpoints (excludes IN & OUT EP0) within the active configuration and also flushes all FIFOs
		pc->state = TRU_USB_PC_DEV_STATE_DEFAULT;
	}
	 */
	// Flush (empty) all RX & TX FIFOs
	//tru_dwc2_flush_rxtxfifo_wait(pc->base_addr, TRU_DWC2_GRSTCTL_TXFNUM_ALL);  // Flush all FIFOs (receive and transmit)

#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
	tru_usb_pc_eps_clear_deferred(&pc->eps, pc->hwcfg.num_dev_in_ep, pc->hwcfg.num_dev_ep);
#endif

	// Clear global NAKs
	tru_dwc2_clr_globalinnak_np(pc->base_addr);
	tru_dwc2_clr_globaloutnak(pc->base_addr);
}

/*
	USB reset interrupt request handler.
*/
static void tru_dwc2_pc1_isr_usbreset(tru_usb_pc_t *pc){
	TRU_DWC2_GRSTCTL_REG(pc->base_addr)->bits.psrst = 1;  // This is a workaround for some versions: pulling out the USB cable while during a transfer may cause the USBRESET interrupt to never trigger, this restores to a proper state

	tru_dwc2_pc1_isr_stop_xfers(pc);

	// Flush all transmit FIFOs
	tru_dwc2_flush_txfifo_wait(pc->base_addr, TRU_DWC2_GRSTCTL_TXFNUM_ALL);

	tru_usb_lxfer_unregister_buffers(&pc->eps.epis[0].lxfer);
	tru_usb_lxfer_unregister_buffers(&pc->eps.epos[0].lxfer);
	tru_usb_lxfer_register_buffer(&pc->eps.epis[0].lxfer, pc->setup.epin0_buf, pc->setup.epin0_bufsize);
	tru_usb_lxfer_register_buffer(&pc->eps.epos[0].lxfer, pc->setup.epout0_buf, pc->setup.epout0_bufsize);

	//tru_dwc2_clr_dev_test_mode(pc->base_addr);  // Disable test mode

	// Clear global NAKs
	//tru_dwc2_clr_globalinnak_np(pc->base_addr);
	//tru_dwc2_clr_globaloutnak(pc->base_addr);

	// Set USB device address to 0 (default address)
	tru_dwc2_set_address(pc->base_addr, 0);
	pc->state = TRU_USB_PC_DEV_STATE_ADDRESS;

	tru_usb_dev_spd_deinit(&pc->dev);  // De-initialise device & qualifier for previous speed
	tru_usb_configs_deinit_spd(&pc->configs);  // De-initialise configurations for previous speed
	pc->speed = 0xff;  // Set no speed

	/*
	// Set EP0 MPS code in the controller register
	TRU_DWC2_DIEPCTL0_REG(pc->base_addr)->bits.mps = tru_dwc2_ep0_mps_to_regcode(TRU_USB_PC_EP_DEFAULT_EP0_MPS);
	TRU_DWC2_DOEPCTL0_REG(pc->base_addr)->bits.mps = tru_dwc2_ep0_mps_to_regcode(TRU_USB_PC_EP_DEFAULT_EP0_MPS);

	pc->eps.epos[0].control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
	pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;

	tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);

	// Clear global NAKs
	tru_dwc2_clr_globalinnak_np(pc->base_addr);
	tru_dwc2_clr_globaloutnak(pc->base_addr);
	*/
}

/*
	USB enumeration completed interrupt request handler.
	Represents the final stage of the USB reset.
	The speed is detected in this stage.
*/
static void tru_dwc2_pc1_isr_enumdone(tru_usb_pc_t *pc){
	/*
	uint16_t epins_mps[16] = {
		pc->eps.epis[0].mps,
		pc->eps.epis[1].mps,
		pc->eps.epis[2].mps,
		pc->eps.epis[3].mps,
		pc->eps.epis[4].mps,
		pc->eps.epis[5].mps,
		pc->eps.epis[6].mps,
		pc->eps.epis[7].mps,
		pc->eps.epis[8].mps,
		pc->eps.epis[9].mps,
		pc->eps.epis[10].mps,
		pc->eps.epis[11].mps,
		pc->eps.epis[12].mps,
		pc->eps.epis[13].mps,
		pc->eps.epis[14].mps,
		pc->eps.epis[15].mps
	};
	uint16_t epouts_mps[16] = {
		pc->eps.epos[0].mps,
		pc->eps.epos[1].mps,
		pc->eps.epos[2].mps,
		pc->eps.epos[3].mps,
		pc->eps.epos[4].mps,
		pc->eps.epos[5].mps,
		pc->eps.epos[6].mps,
		pc->eps.epos[7].mps,
		pc->eps.epos[8].mps,
		pc->eps.epos[9].mps,
		pc->eps.epos[10].mps,
		pc->eps.epos[11].mps,
		pc->eps.epos[12].mps,
		pc->eps.epos[13].mps,
		pc->eps.epos[14].mps,
		pc->eps.epos[15].mps,
	};
	*/

	// We got an USB enumdone without a USB reset?
	//if(tru_dwc2_get_address(pc) != 0){
	//	tru_dwc2_pc1_isr_usbreset(pc);
	//}

	pc->speed = TRU_DWC2_DSTS_REG(pc->base_addr)->bits.enumspd;  // Save the detected speed

	// Init device and configurations
	tru_usb_dev_spd_init(&pc->dev, pc->speed);  // Initialise device & qualifier for current speed
	tru_usb_configs_init_spd(&pc->configs, pc->speed);  // Initialise configurations for current speed

	// Determine the usable EP0 Maximum Packet Size (MPS)
	pc->eps.epis[0].mps_limit = pc->dev.curr_spd_dev_desc->mps;
	pc->eps.epos[0].mps_limit = pc->dev.curr_spd_dev_desc->mps;
	pc->eps.epis[0].mps = tru_dwc2_determine_mps(pc->speed, pc->eps.epis[0].flow_type, pc->eps.epis[0].mps_limit);
	pc->eps.epos[0].mps = tru_dwc2_determine_mps(pc->speed, pc->eps.epos[0].flow_type, pc->eps.epos[0].mps_limit);

	// Set EP0 MPS code in the controller register
	TRU_DWC2_DIEPCTL0_REG(pc->base_addr)->bits.mps = tru_dwc2_ep0_mps_to_regcode(pc->eps.epis[0].mps);
	TRU_DWC2_DOEPCTL0_REG(pc->base_addr)->bits.mps = tru_dwc2_ep0_mps_to_regcode(pc->eps.epos[0].mps);

	// Reallocate dynamic FIFOs
	/*
	epins_mps[0] = pc->eps.epis[0].mps;
	epouts_mps[0] = pc->eps.epos[0].mps;
	tru_dwc2_realloc_fifo(pc->base_addr, pc->dma_mode, epins_mps, epouts_mps, false, pc->hwcfg.num_dev_in_ep, pc->hwcfg.num_dev_ep, pc->hwcfg.fifo_usable_size_words);
	*/

	pc->eps.epos[0].control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
	pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
	tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
	pc->state = TRU_USB_PC_DEV_STATE_DEFAULT;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
	LOG("INIT: EP0 MPS: IN = %u OUT = %u\n", pc->eps.epis[0].mps, pc->eps.epos[0].mps);
#endif

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
	switch(pc->speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:
			pc->sof_per_sec = 8000;
			pc->sof_per_ms = 8;
			LOG("INIT: Speed = HS\n");
			break;
		case TRU_DWC2_DSTS_ENUMSPD_FS3060:
		case TRU_DWC2_DSTS_ENUMSPD_FS48:
			pc->sof_per_sec = 1000;
			pc->sof_per_ms = 1;
			LOG("INIT: Speed = FS\n");
			break;
		case TRU_DWC2_DSTS_ENUMSPD_LS6:
			pc->sof_per_sec = 1000;
			pc->sof_per_ms = 1;
			LOG("INIT: Speed = LS\n");
			break;
		default:
			pc->sof_per_sec = 1000;
			pc->sof_per_ms = 1;
			LOG("INIT: Speed = %u (invalid)\n", pc->speed);
	}
#endif

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_enumdone){
			pc->dc_info[i].callbacks->cb_enumdone(pc->dc_info[i].devclass);
		}
	}
}

static void tru_dwc2_pc1_isr_otgevt(tru_usb_pc_t *pc){
	tru_dwc2_gotgint_t gotgint = { .val = TRU_DWC2_GOTGINT_REG(pc->base_addr)->val };

	// Bvalid de-asserted?  (Represents the final stage of a disconnection)
	if(gotgint.bits.sesenddet){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: Bvalid de-asserted\n");
#endif
		//tru_dwc2_pc1_isr_stop_xfers(pc);
	}

	// Session request status change?
	if(gotgint.bits.sesreqsucstschng){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		if(TRU_DWC2_GOTGCTL_REG(pc->base_addr)->bits.sesreqscs){
			LOG("INTR: Session request success\n");
		}else{
			LOG("INTR: Session request failure\n");
		}
#endif
	}

	// Host negotiation request status change?
	if(gotgint.bits.hstnegsucstschng){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		if(TRU_DWC2_GOTGCTL_REG(pc->base_addr)->bits.hstnegscs){
			LOG("INTR: Host negotiation request success\n");
		}else{
			LOG("INTR: Host negotiation request failure\n");
		}
#endif
	}

	// Host negotiation detectecd?
	if(gotgint.bits.hstnegdet){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: Host negotiation detected\n");
#endif
	}

	// A-device waiting for B-device to connect timed-out?
	if(gotgint.bits.adevtoutchg){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: A-device timed-out\n");
#endif
	}

	// Debounce completed?
	if(gotgint.bits.dbncedone){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: Debounce completed\n");
#endif
	}

	TRU_DWC2_GOTGINT_REG(pc->base_addr)->val = gotgint.val;  // Clear all flagged interrupt status bits
}

/*
	Handle SOF interrupt.
	In full-speed mode this interrupt is triggered every USB frame, which has a rate of 1ms.
	In high-speed mode this interrupt is triggered every USB microframe, which has a rate of 125us (8x of USB frame).
*/
static void tru_dwc2_pc1_isr_sof(tru_usb_pc_t *pc){
	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_sof){
			pc->dc_info[i].callbacks->cb_sof(pc->dc_info[i].devclass);
		}
	}
}

/*
	Note:
		Because OUT EP0 is always enabled internally, the packet status SETUPRX can happen even before enabling the OUT EP0!
		This also causes the RXFLVL interrupt to happen even before enabling the OUT EP0.
*/
static void tru_dwc2_pc1_isr_rxflvl(tru_usb_pc_t *pc){
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
		//pc->grxsts.val = *TRU_DWC2_GRXSTSR_REG(pc->base_addr)->val;  // Read (no pop) the receive FIFO status
		pc->grxsts.val = TRU_DWC2_GRXSTSP_REG(pc->base_addr)->val;  // Read and pop the receive FIFO status
		uint8_t ep_num = pc->grxsts.bits.chnum;
		tru_usb_pc_epo_t *epout = pc->eps.epos + ep_num;

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: RXFLVL STATUS VAL 0x%.8lx EP %u BCNT %u STS %i\n", pc->grxsts.val, ep_num, pc->grxsts.bits.bcnt, pc->grxsts.bits.pktsts);
#endif

		// Process RX FIFO data using the packet status
		switch(pc->grxsts.bits.pktsts){
			case TRU_DWC2_GRXSTS_PKTSTS_GLOBALOUTNAK:  // Global OUT NAK
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
				LOG("RXFLVL: STS GOUTNAK\n");
#endif
				// Do nothing
				break;

			case TRU_DWC2_GRXSTS_PKTSTS_OUTRX:  // Received data packet - another GRXFLVL interrupt with PKTSTS_OUTDONE should follow for this
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
				LOG("RXFLVL: STS OUTRX\n");
#endif

				if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
					epout->rxfifo_xferred_size = pc->grxsts.bits.bcnt;
					if(pc->grxsts.bits.bcnt){
						tru_dwc2_read_fifo(pc->base_addr, epout->lxfer.chunk.buffers[0].buf + epout->lxfer.chunk.total_xferred_size, pc->grxsts.bits.bcnt);  // Read data from FIFO and put it into a buffer, reading pops it
						tru_usb_pc_update_lxfer_details_epout(pc, epout->num);
					}
				}
				break;

			case TRU_DWC2_GRXSTS_PKTSTS_OUTDONE:  // Data transfer completed
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
				LOG("RXFLVL: STS OUTDONE\n");
#endif

				if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
					tru_usb_pc_end_lxfer_details_epout(pc, epout->num);
				}

				// Handle here or let the xfer complete interrupt handle it
#if TRU_DWC2_PC_HANDLE_DONE_IN_RXLVL == 1
				if(epout->num == 0){
					tru_dwc2_pc1_isr_doepint0_continue_xfer(pc);
				}else{
					tru_dwc2_pc1_isr_doepint1_15_continue_xfer(pc, epout->num);
				}
#endif
				break;

			case TRU_DWC2_GRXSTS_PKTSTS_SETUPRX:  // Received setup packet - another GRXFLVL interrupt with PKTSTS_SETUPDONE should follow for this
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
				LOG("RXFLVL: STS SETUPRX\n");
#endif

				if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
					epout->rxfifo_xferred_size = pc->grxsts.bits.bcnt;
					if(pc->grxsts.bits.bcnt){
						epout->xferred_size_prezlp = 0;
						epout->lxfer.chunk.total_xferred_size = 0;
						epout->lxfer.whole.total_xferred_size = 0;
						tru_dwc2_read_fifo(pc->base_addr, epout->lxfer.chunk.buffers[0].buf + epout->lxfer.chunk.total_xferred_size, pc->grxsts.bits.bcnt);  // Read data from FIFO and put it into a buffer, reading pops it
						tru_usb_pc_update_lxfer_details_epout(pc, epout->num);  // Update EP size
					}
				}
				break;

			case TRU_DWC2_GRXSTS_PKTSTS_SETUPDONE:  // Setup packet transfer completed
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
				LOG("RXFLVL: STS SETUPDONE\n");
#endif

				if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
					tru_usb_pc_end_lxfer_details_epout(pc, epout->num);
				}

				// Handle here or let the setup interrupt handle it
#if TRU_DWC2_PC_HANDLE_DONE_IN_RXLVL == 1
				tru_usb_pc_setup_handler(pc);  // Call handler to process the setup request
#endif
				break;

			default:
				// ERROR - unknown packet status
				if(pc->grxsts.bits.bcnt){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
					LOG("RXFLVL: STS UNKNOWN\n");
#endif
					tru_dwc2_read_fifo_discard(pc->base_addr, pc->grxsts.bits.bcnt);  // Discard the data
				}
		}
	}
}

static void tru_dwc2_pc1_isr_innakeff(tru_usb_pc_t *pc){
	TRU_DWC2_DCTL_REG(pc->base_addr)->bits.cgnpinnak = 1;  // Clear Global IN NAK
}

static void tru_dwc2_pc1_isr_outnakeff(tru_usb_pc_t *pc){
	TRU_DWC2_DCTL_REG(pc->base_addr)->bits.cgoutnak = 1;  // Clear Global OUT NAK
}

static void tru_dwc2_pc1_isr_incomp_in(tru_usb_pc_t *pc){
	for(uint8_t ep_num = 1; ep_num < pc->hwcfg.num_dev_in_ep; ep_num++){
		if(pc->eps.epis[ep_num].flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
			tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;
			uint32_t limit = (pc->dma_mode == TRU_DWC2_DMA_MODE_SG) ? TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_LIMIT : TRU_DWC2_DSTS_SOFFN_LIMIT;
			uint32_t frame = tru_dwc2_get_framenum(pc->base_addr);
			if(tru_dwc2_is_epin_iso_incompl(pc->base_addr, pc->dma_mode, ep_num, ep->interval, frame, ep->frame_target, limit)){
				ep->iso_incomplete++;

				switch(pc->dma_mode){
					case TRU_DWC2_DMA_MODE_FF:
					case TRU_DWC2_DMA_MODE_SB:
						tru_dwc2_update_oddeven_epin(pc->base_addr, ep_num, ep->interval, &ep->frame_target);
						break;
					case TRU_DWC2_DMA_MODE_SG:
						//LOG("X %.8lx\n", ep->lxfer.chunk.sgdma.dma_descs[0].status.val);
						tru_usb_dwc2_update_frame_target_sgdma(&ep->lxfer, pc->base_addr, ep->interval, &ep->frame_target);
						break;
					default:
				}

				// Re-transmit
				//tru_usb_pc_enable_xfer_epin(pc, ep_num, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, TRU_DWC2_DPID_EO_AUTO);
			}
		}

		//if(pc->eps.epis[ep_num].flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
			//tru_dwc2_disable_epin_wait(pc->base_addr, ep_num, false);
		//}
	}
}

static void tru_dwc2_pc1_isr_incomp_out(tru_usb_pc_t *pc){
	for(uint8_t ep_num = 1; ep_num < pc->hwcfg.num_dev_ep; ep_num++){
		if(pc->eps.epos[ep_num].flow_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
			tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;
			uint32_t limit = (pc->dma_mode == TRU_DWC2_DMA_MODE_SG) ? TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_LIMIT : TRU_DWC2_DSTS_SOFFN_LIMIT;
			uint32_t frame = tru_dwc2_get_framenum(pc->base_addr);
			if(tru_dwc2_is_epout_iso_incompl(pc->base_addr, pc->dma_mode, ep_num, ep->interval, frame, ep->frame_target, limit)){
				ep->iso_incomplete++;

				switch(pc->dma_mode){
					case TRU_DWC2_DMA_MODE_FF:
					case TRU_DWC2_DMA_MODE_SB:
						tru_dwc2_update_oddeven_epout(pc->base_addr, ep_num, ep->interval, &ep->frame_target);
						break;
					case TRU_DWC2_DMA_MODE_SG:
						//LOG("IC %u, %.8lx\n", ep->num, TRU_DWC2_DOEPCTL_REG(pc->base_addr, ep->num)->val);
						tru_usb_dwc2_update_frame_target_sgdma(&ep->lxfer, pc->base_addr, ep->interval, &ep->frame_target);
						break;
					default:
				}

				// Re-receive
				//tru_usb_pc_enable_xfer_epout(pc, ep_num, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, TRU_DWC2_DPID_EO_AUTO);
			}
		}
	}
}

static void tru_dwc2_pc1_isr_wakeup(tru_usb_pc_t *pc){
	TRU_DWC2_DCTL_REG(pc->base_addr)->bits.rmtwkupsig = 0;  // Exit suspend state.  Send remote wake-up signalling to wake up the host
}

/*
	Peripheral mode interrupt handler.
*/
static void tru_dwc2_pc1_isr(tru_usb_pc_t *pc, tru_dwc2_gintmsk_t gintmsk, tru_dwc2_gintsts_t gintsts){
	bool is_reset = false;

	// Device only: In endpoints interrupt triggered?
	// Note: IEPINT is read-only, the USB controller automatically clears this bit when the corresponding endpoint interrupt bits (DIEPINTn) is cleared
	if(gintsts.bits.iepint & gintmsk.bits.iepintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		//LOG("INTR: IEPINT\n");
#endif
		tru_dwc2_pc1_isr_daint_epin(pc);
	}

	// Device only: Out endpoints interrupt triggered?
	// Note: OEPINT is read-only, the USB controller automatically clears this bit when the corresponding endpoint interrupt bits (DOEPINTn) is cleared
	if(gintsts.bits.oepint & gintmsk.bits.oepintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		//LOG("INTR: OEPINT\n");
#endif
		tru_dwc2_pc1_isr_daint_epout(pc);
	}

	// We process the rx fifo level threshold trigger here so that it is after the out EP interrupts
	// Device & host: RxFIFO non-empty or above empty threshold level interrupt triggered?
	// Note: RXFLVL is read-only, the USB controller automatically clears this bit
	if(gintsts.bits.rxflvl & gintmsk.bits.rxflvlmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: RXFLVL (RXFIFO has data)\n");
#endif
		tru_dwc2_pc1_isr_rxflvl(pc);
	}

	// Device & host: Mode mismatch interrupt triggered?
	if(gintsts.bits.modemis & gintmsk.bits.modemismsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: MODEMIS (Mode mismatch)\n");
#endif
	}

	// Device & host: OTG event interrupt triggered?
	if(gintsts.bits.otgint & gintmsk.bits.otgintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: OTGINT (OTG event)\n");
#endif
		tru_dwc2_pc1_isr_otgevt(pc);
	}

	// Device only: Global in non-periodic NAK effective interrupt triggered?
	if(gintsts.bits.ginnakeff & gintmsk.bits.ginnakeffmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: GINNAKEFF (IN non-periodic NAK eff)\n");
#endif
		tru_dwc2_pc1_isr_innakeff(pc);
	}

	// Device only: Global out non-periodic NAK effective interrupt triggered?
	if(gintsts.bits.goutnakeff & gintmsk.bits.goutnakeffmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: GOUTNAKEFF (OUT non-periodic NAK eff)\n");
#endif
		tru_dwc2_pc1_isr_outnakeff(pc);
	}

	// Device only: Idle interrupt triggered?
	if(gintsts.bits.erlysusp & gintmsk.bits.erlysuspmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: ERLYSUSP (Idle)\n");
#endif
	}

	// Device only: USB suspend interrupt triggered?
	if(gintsts.bits.usbsusp & gintmsk.bits.usbsuspmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: USBSUSP (Suspend)\n");
#endif
	}

	// Device only: USB reset interrupt triggered?
	if(gintsts.bits.usbrst & gintmsk.bits.usbrstmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: USBRST (Reset)\n");
#endif
		is_reset = true;
		tru_dwc2_pc1_isr_usbreset(pc);
	}

	// Device only: Enumeration done interrupt triggered?
	if(gintsts.bits.enumdone & gintmsk.bits.enumdonemsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: ENUMDONE (Enum done)\n");
#endif
		tru_dwc2_pc1_isr_enumdone(pc);
	}

	// Device only: Isochronous out packet dropped interrupt triggered?
	if(gintsts.bits.isooutdrop & gintmsk.bits.isooutdropmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: ISOOUTDROP (Iso OUT dropped)\n");
#endif
	}

	// Device only: Endpoint mismatch interrupt triggered?
	if(gintsts.bits.epmis & gintmsk.bits.epmismsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: EPMIS (EP mismatch)\n");
#endif
	}

	// Device only: Incomplete Isochronous in transfer interrupt triggered?
	if(gintsts.bits.incompisoin & gintmsk.bits.incompisoinmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: INCOMPISOIN (Incomplete Iso IN)\n");
#endif
		tru_dwc2_pc1_isr_incomp_in(pc);
	}

	// Device only: Incomplete periodic out transfer interrupt triggered?
	if(gintsts.bits.incomplp & gintmsk.bits.incomplpmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: INCOMPLP (Incomplete periodic OUT)\n");
#endif
		tru_dwc2_pc1_isr_incomp_out(pc);
	}

	// Device only: Data fetch suspended interrupt triggered?
	if(gintsts.bits.fetsusp & gintmsk.bits.fetsuspmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: FETSUSP (Data fetch suspended)\n");
#endif
	}

	// Device only: Reset during partial power-down suspend state interrupt triggered?
	if(gintsts.bits.resetdet & gintmsk.bits.resetdetmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: RESETDET (Reset during partial power-down)\n");
#endif
	}

	// Device & host: Connector ID status change interrupt triggered?
	if(gintsts.bits.conidstschng & gintmsk.bits.conidstschngmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: CONIDSTSCHNG (Connection ID changed)\n");
#endif
	}

	// Device & host: Session request new session (new connection) detected interrupt triggered?
	if(gintsts.bits.sessreqint & gintmsk.bits.sessreqintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: SESSREQINT (New connection)\n");
#endif
		//tru_dwc2_pc1_isr_usbreset(pc);
	}

	// Device & host: Resume remote wake-up detected interrupt triggered?
	if(gintsts.bits.wkupint & gintmsk.bits.wkupintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: WKUPINT (Wake-up)\n");
#endif
		tru_dwc2_pc1_isr_wakeup(pc);
	}

	if(is_reset){
		// Call back device class functions
		for(uint32_t i = 0; i < pc->num_dc_info; i++){
			if(pc->dc_info[i].callbacks->cb_reset){
				pc->dc_info[i].callbacks->cb_reset(pc->dc_info[i].devclass);
			}
		}
	}

	// Device & host:
	//   Device mode: SOF (FS or HS) interrupt triggered?
	//   Host mode: SOF (FS), micro-SOF (HS), or Keep-Alive (LS) is triggered?
	//   Abbreviations: SOF = Start Of Frame, LS = Low Speed, FS = Full Speed, HS = High Speed
	if(gintsts.bits.sof & gintmsk.bits.sofmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		//LOG("INTR: SOF\n");
#endif
		tru_dwc2_pc1_isr_sof(pc);
	}

	TRU_DWC2_GINTSTS_REG(pc->base_addr)->val = gintsts.val;  // Clear interrupt status flag bits

#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
	tru_usb_pc_process_deferred(pc);
#endif
}

/*
	TODO: host mode incomplete
	Global host mode interrupt handler.
*/
/*
static void tru_dwc2_hc1_isr(tru_usb_hc_t *hc, tru_dwc2_gintmsk_t gintmsk, tru_dwc2_gintsts_t gintsts){
	// Device & host: Mode mismatch interrupt triggered?
	if(gintsts.bits.modemis & gintmsk.bits.modemismsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: MODEMIS (Mode mismatch)\n");
#endif
	}

	// Device & host: OTG event interrupt triggered?
	if(gintsts.bits.otgint & gintmsk.bits.otgintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: OTGINT (OTG event)\n");
#endif
	}

	// Device & host:
	//   Device mode: SOF (FS or HS) interrupt triggered?
	//   Host mode: SOF (FS), micro-SOF (HS), or Keep-Alive (LS) is triggered?
	//   Abbreviations: SOF = Start Of Frame, LS = Low Speed, FS = Full Speed, HS = High Speed
	if(gintsts.bits.sof & gintmsk.bits.sofmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		//LOG("INTR: SOF\n");
#endif
	}

	// Host only: Host port interrupt triggered?
	if(gintsts.bits.prtint & gintmsk.bits.prtintmsk){
	}

	// Host only: Host channel interrupt triggered?
	if(gintsts.bits.hchint & gintmsk.bits.hchintmsk){
	}

	// Host only: Periodic TxFIFO empty interrupt?
	// PTXFEMP is read-only, the USB controller automatically clears this bit
	if(gintsts.bits.ptxfemp & gintmsk.bits.ptxfempmsk){
	}

	// Device & host: Connector ID status change interrupt triggered?
	if(gintsts.bits.conidstschng & gintmsk.bits.conidstschngmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: CONIDSTSCHNG (Connection ID changed)\n");
#endif
	}

	// Host only: Disconnection detected interrupt triggered?
	if(gintsts.bits.disconnint & gintmsk.bits.disconnintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: DISCONNINT (Disconnection)\n");
#endif
	}

	// Device & host: Session request new session (new connection) detected interrupt triggered?
	if(gintsts.bits.sessreqint & gintmsk.bits.sessreqintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: SESSREQINT (New connection)\n");
#endif
	}

	// Device & host: Resume remote wake-up detected interrupt triggered?
	if(gintsts.bits.wkupint & gintmsk.bits.wkupintmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: WKUPINT (Wake-up)\n");
#endif
	}

	// Device & host: RxFIFO non-empty or above empty threshold level interrupt triggered?
	// RXFLVL is read-only, the USB controller automatically clears this bit
	if(gintsts.bits.rxflvl & gintmsk.bits.rxflvlmsk){
#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("INTR: RXFLVL (RXFIFO has data)\n");
#endif
	}

	//TRU_DWC2_GINTSTS_REG(pc->base_addr)->val = gintsts.val & gintmsk.val;  // Clear interrupt status flag bits
	TRU_DWC2_GINTSTS_REG(pc->base_addr)->val = gintsts.val;  // Clear interrupt status flag bits
}
*/

void tru_dwc2_pc1_isr_set_context(tru_usb_pc_t *pc){
	tru_dwc2_pc1_isr_context = pc;
}

tru_usb_pc_t *tru_dwc2_pc1_isr_get_context(tru_usb_pc_t *pc){
	return tru_dwc2_pc1_isr_context;
}

/*
	USB OTG interrupt handler.
*/
void tru_dwc2_otg1_isr(void){
	tru_dwc2_gintmsk_t gintmsk = { .val = TRU_DWC2_GINTMSK_REG(tru_dwc2_pc1_isr_context->base_addr)->val };
	tru_dwc2_gintsts_t gintsts = { .val = TRU_DWC2_GINTSTS_REG(tru_dwc2_pc1_isr_context->base_addr)->val };

	// Is the interrupt status flags set from device mode?
	if(gintsts.bits.curmod == TRU_DWC2_GINTSTS_CURMOD_DEVICE){
		tru_dwc2_pc1_isr(tru_dwc2_pc1_isr_context, gintmsk, gintsts);
	}else{
		// Host mode
	}
}
