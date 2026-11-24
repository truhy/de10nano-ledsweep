/*
	MIT License

	Copyright (c) 2022 Truong Hy

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

#include "usb/synopsys/tru_dwc2.h"
#include "usb/tru_usb_ep.h"
#include "tru_iom.h"
#include "tru_cache.h"
#include "tru_logger.h"

void tru_dwc2_get_hwcfg_snpsid(void *usb_base_addr, tru_dwc2_hwcfg_t *hwcfg){
	hwcfg->snpsid = TRU_DWC2_GSNPSID_REG(usb_base_addr)->bits.gsnpsid;
}

void tru_dwc2_get_hwcfg(void *usb_base_addr, tru_dwc2_hwcfg_t *hwcfg){
	hwcfg->snpsid = TRU_DWC2_GSNPSID_REG(usb_base_addr)->bits.gsnpsid;

	hwcfg->num_dev_ep = TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.numdeveps + 1;  // Increment to include EP0
	hwcfg->tknqdepth = TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.tknqdepth;
	hwcfg->ptxqdepth = TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.ptxqdepth;
	hwcfg->nptxqdepth = TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.nptxqdepth;
	hwcfg->dma_arch = TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.dmaarch;
	hwcfg->dma_capable = (hwcfg->dma_arch == TRU_DWC2_GHWCFG2_DMA_NONE) ? false : true;

	hwcfg->fifo_usable_size_words = TRU_DWC2_GHWCFG3_REG(usb_base_addr)->bits.dfifodepth;  // Usable FIFO space, excludes EP info reserved space
	hwcfg->max_transfer_size = (1 << (TRU_DWC2_GHWCFG3_REG(usb_base_addr)->bits.xfersizewidth + 11)) - 1;
	hwcfg->max_packet_count = (1 << (TRU_DWC2_GHWCFG3_REG(usb_base_addr)->bits.pktsizewidth + 4)) - 1;

	hwcfg->num_dev_in_ep = TRU_DWC2_GHWCFG4_REG(usb_base_addr)->bits.ineps + 1;  // Increment to include EP0
	hwcfg->dma_desc_capable = TRU_DWC2_GHWCFG4_REG(usb_base_addr)->bits.dma ? true : false;
}

/*
	Clear interrupt trigger status flags. For each status bit: Read: 1 = triggered, 0 = not triggered. Write: 1 to re-arm the interrupt trigger
*/
void tru_dwc2_init_intstatus(void *usb_base_addr, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep){
	uint8_t i;
	uint32_t tmp;

	// Clear the global OTG events (device and host mode) interrupt status flags. Read: 1 = triggered, 0 = not triggered. Write: 1 to re-arm the interrupt trigger
	tmp = TRU_DWC2_GOTGINT_REG(usb_base_addr)->val;
	TRU_DWC2_GOTGINT_REG(usb_base_addr)->val = tmp;

	// Clear the device endpoint interrupt status flags. Read: 1 = triggered, 0 = not triggered. Write: 1 to re-arm the interrupt trigger
	for(i = 0; i < num_dev_in_ep; i++){
		tmp = TRU_DWC2_DIEPINT_REG(usb_base_addr, i)->val;
		TRU_DWC2_DIEPINT_REG(usb_base_addr, i)->val = tmp;
	}
	for(i = 0; i < num_dev_out_ep; i++){
		tmp = TRU_DWC2_DOEPINT_REG(usb_base_addr, i)->val;
		TRU_DWC2_DOEPINT_REG(usb_base_addr, i)->val = tmp;
	}

	// Clear the global (device and host mode) interrupt status flags. Read: 1 = triggered, 0 = not triggered. Write: 1 to re-arm the interrupt trigger
	tmp = TRU_DWC2_GINTSTS_REG(usb_base_addr)->val;
	TRU_DWC2_GINTSTS_REG(usb_base_addr)->val = tmp;
}

/*
	USB controller interrupt masks.

	Enable/disable interrupt generation (triggers) masking bits.  For each interrupt bit flag: 0 = disable, 1 = enable

	The set of interrupt status registers, e.g. GINTSTS, GOTGINT, DIEPINT, DOEPINT stores the changing events and is part of the interrupt generation.
	The set of interrupt mask registers are used for controlling which interrupt status bit will generate an actual interrupt.

	An interrupt for a particular interrupt status is generated when the interrupt status bit is set to 1 (USB controller does this) and the related
	interrupt mask bit is set to 1.  The interrupt will keep generating until the interrupt status bit is cleared by writing a 1 back into it.
	If it is not cleared, when execution exits the interrupt subroutine routine the same interrupt will immediately generate again.

	BUG: Do not set DAINTMSK register (bit 0 and bit 16) for IN EP0 and OUT EP0 to 1 when using Scatter Gather DMA mode, if set, sometimes a ZLP
	is not received for the STATUS IN stage after the DATA IN transferred the data.  A workaround is to leave these bits set at 0 because EP0 always
	generate interrupts - the controller is supposed to ignore these bits.
*/
void tru_dwc2_init_intmasks(void *usb_base_addr, bool enable_sof_trigger, tru_dwc2_dma_mode_t dma_mode){
	tru_dwc2_gintmsk_t gintmsk = { .val = TRU_DWC2_GINTMSK_REG(usb_base_addr)->val };
	tru_dwc2_doepmsk_t doepmsk = { .val = TRU_DWC2_DOEPMSK_REG(usb_base_addr)->val };
	tru_dwc2_diepmsk_t diepmsk = { .val = TRU_DWC2_DIEPMSK_REG(usb_base_addr)->val };
	tru_dwc2_diepempmsk_t diepempmsk = { .val = TRU_DWC2_DIEPEMPMSK_REG(usb_base_addr)->val };

	// Global interrupt trigger mask bits
	// Settings are different between DMA vs non-DMA mode (slave FIFO mode or slave mode)
	// Each bit decides whether the corresponding trigger generates an interrupt: 0 = disable, 1 = enable
	switch(dma_mode){
		case TRU_DWC2_DMA_MODE_SB:
		case TRU_DWC2_DMA_MODE_SG:
			// Exclude slave FIFO mode related interrupts.  DMA modes don't use those interrupts.
			gintmsk.bits.modemismsk      = 1;
			gintmsk.bits.otgintmsk       = 1;
			gintmsk.bits.sofmsk          = enable_sof_trigger;
			gintmsk.bits.rxflvlmsk       = 0;
			gintmsk.bits.nptxfem         = 0;
			gintmsk.bits.ginnakeffmsk    = 1;
			gintmsk.bits.goutnakeffmsk   = 1;
			gintmsk.bits.erlysuspmsk     = 1;
			gintmsk.bits.usbsuspmsk      = 1;
			gintmsk.bits.usbrstmsk       = 1;
			gintmsk.bits.enumdonemsk     = 1;
			gintmsk.bits.isooutdropmsk   = 1;
			gintmsk.bits.eopfmsk         = 0;
			gintmsk.bits.epmismsk        = 1;
			gintmsk.bits.iepintmsk       = 1;
			gintmsk.bits.oepintmsk       = 1;
			gintmsk.bits.incompisoinmsk  = 1;
			gintmsk.bits.incomplpmsk     = 1;
			gintmsk.bits.fetsuspmsk      = 1;
			gintmsk.bits.resetdetmsk     = 1;
			gintmsk.bits.prtintmsk       = 1;
			gintmsk.bits.hchintmsk       = 1;
			gintmsk.bits.ptxfempmsk      = 0;
			gintmsk.bits.conidstschngmsk = 0;  // Bug in some versions (e.g. 3.30a): don't enable this, else interrupt is always triggered
			gintmsk.bits.disconnintmsk   = 1;
			gintmsk.bits.sessreqintmsk   = 1;
			gintmsk.bits.wkupintmsk      = 1;
			break;
		case TRU_DWC2_DMA_MODE_FF:
		default:
			gintmsk.bits.modemismsk      = 1;
			gintmsk.bits.otgintmsk       = 1;
			gintmsk.bits.sofmsk          = enable_sof_trigger;
			gintmsk.bits.rxflvlmsk       = 1;
			gintmsk.bits.nptxfem         = 0;
			gintmsk.bits.ginnakeffmsk    = 1;
			gintmsk.bits.goutnakeffmsk   = 1;
			gintmsk.bits.erlysuspmsk     = 1;
			gintmsk.bits.usbsuspmsk      = 1;
			gintmsk.bits.usbrstmsk       = 1;
			gintmsk.bits.enumdonemsk     = 1;
			gintmsk.bits.isooutdropmsk   = 1;
			gintmsk.bits.eopfmsk         = 0;
			gintmsk.bits.epmismsk        = 1;
			gintmsk.bits.iepintmsk       = 1;
			gintmsk.bits.oepintmsk       = 1;
			gintmsk.bits.incompisoinmsk  = 1;
			gintmsk.bits.incomplpmsk     = 1;
			gintmsk.bits.fetsuspmsk      = 1;
			gintmsk.bits.resetdetmsk     = 1;
			gintmsk.bits.prtintmsk       = 1;
			gintmsk.bits.hchintmsk       = 1;
			gintmsk.bits.ptxfempmsk      = 0;
			gintmsk.bits.conidstschngmsk = 0;  // Bug in some versions (e.g. 3.30a): don't enable this, else interrupt is always triggered
			gintmsk.bits.disconnintmsk   = 1;
			gintmsk.bits.sessreqintmsk   = 1;
			gintmsk.bits.wkupintmsk      = 1;
	}
	TRU_DWC2_GINTMSK_REG(usb_base_addr)->val = gintmsk.val;

	// OUT EP interrupt trigger mask bits
	doepmsk.bits.xfercomplmsk   = 1;
	doepmsk.bits.epdisbldmsk    = 1;
	doepmsk.bits.ahberrmsk      = 1;
	doepmsk.bits.setupmsk       = 1;
	doepmsk.bits.outtknepdismsk = 1;
	doepmsk.bits.stsphsrxmsk    = 1;
	doepmsk.bits.b2bsetupmsk    = 1;
	doepmsk.bits.outpkterrmsk   = 1;
	doepmsk.bits.bnamsk         = 1;
	doepmsk.bits.bbleerrmsk     = 1;
	doepmsk.bits.nakmsk         = 1;
	doepmsk.bits.nyetmsk        = 1;
#if defined(TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED) && TRU_DWC2_DOEPINT_STPKTRX_SUPPORTED == 1
	doepmsk.bits.stpktrxmsk     = 1;
#endif
	TRU_DWC2_DOEPMSK_REG(usb_base_addr)->val = doepmsk.val;

	// IN EP interrupt trigger mask bits
	// For periodic flows types (Interrupt and Isochronous) we have devices
	// that we don't want to transmit data to the host all the time, but since
	// the flow is periodic, the host will repeatedly send out IN requests
	// (except during suspend mode), e.g. vendor HID, and so we want the USB
	// controller to keep NAKing.  We mask out and suppress the "IN NAK" and
	// "IN request when FIFO is empty" interrupts, otherwise we are overwhelmed
	// by them because there is nothing to service.  Note, the masked flags
	// are still set by the USB controller but they won't trigger an interrupt.
	// Seems there is no harm leaving them disabled for other flow types.
	diepmsk.bits.xfercomplmsk   = 1;
	diepmsk.bits.epdisbldmsk    = 1;
	diepmsk.bits.ahberrmsk      = 1;
	diepmsk.bits.timeoutmsk     = 1;
	diepmsk.bits.intkntxfempmsk = 0;  // Disable "IN request when FIFO is empty" interrupt
	diepmsk.bits.intknepmismsk  = 1;
	diepmsk.bits.inepnakeffmsk  = 1;
	diepmsk.bits.txfifoundrnmsk = 1;
	diepmsk.bits.bnamsk         = 1;
	diepmsk.bits.nakmsk         = 0;  // Disable "IN NAK" interrupt
	TRU_DWC2_DIEPMSK_REG(usb_base_addr)->val = diepmsk.val;

	// IN EP FIFO empty interrupt trigger mask bits
	// Disable IN endpoint FIFO empty status interrupt generation
	diepempmsk.bits.ineptxfempmsk0  = 0;
	diepempmsk.bits.ineptxfempmsk1  = 0;
	diepempmsk.bits.ineptxfempmsk2  = 0;
	diepempmsk.bits.ineptxfempmsk3  = 0;
	diepempmsk.bits.ineptxfempmsk4  = 0;
	diepempmsk.bits.ineptxfempmsk5  = 0;
	diepempmsk.bits.ineptxfempmsk6  = 0;
	diepempmsk.bits.ineptxfempmsk7  = 0;
	diepempmsk.bits.ineptxfempmsk8  = 0;
	diepempmsk.bits.ineptxfempmsk9  = 0;
	diepempmsk.bits.ineptxfempmsk10 = 0;
	diepempmsk.bits.ineptxfempmsk11 = 0;
	diepempmsk.bits.ineptxfempmsk12 = 0;
	diepempmsk.bits.ineptxfempmsk13 = 0;
	diepempmsk.bits.ineptxfempmsk14 = 0;
	diepempmsk.bits.ineptxfempmsk15 = 0;
	TRU_DWC2_DIEPEMPMSK_REG(usb_base_addr)->val = diepempmsk.val;

	TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val = 0x00010001;  // Enable the interrupt trigger flag for EP0 IN & OUT
}

/*
	Setup FIFO RAM for device mode.

	FIFO RAM
	========

	The Synopsys DWC2 OTG USB 2.0 controller in the Cyclone V SoC has a FIFO
	RAM of total size 32768 kbytes, but 512 is reserved for the EPINFO so the
	FIFO can be configured with:
		32768 - 512 = 32256 bytes
		32256 / 4 = 8064 words
	The FIFO RAM must be setup properly for the controller to function.

	The FIFO is used by all modes:
		- DMA single buffer (DMA SB)
		- DMA scatter gather (DMA SG)
		- non-DMA (aka slave FIFO mode or slave mode)

	Note, FIFO registers configure the RX and TX sizes and offset addresses in
	words, where 1 word = 32 bits or 4 bytes.

	Receive FIFO
	============

	A single shared RX FIFO is used for all OUT EPs.  There is no dedicated RX
	FIFO mode.

	Transmit FIFO
	=============

	Each EP can be configured to use a shared or a dedicated TX FIFO.
	Shared means that EP will use a single TX FIFO.
	Dedicated means the EP will use a separate TX FIFO.

	Non-periodic refers to Control and Bulk data flow types, and periodic refers
	to Interrupt or Isochronous data flow types.  Periodic IN EPs must use
	dedicated FIFO mode, because a dedicated FIFO supports continuous streaming
	bandwidth to be maintained. Non-periodic IN EPs may use shared or dedicated
	FIFO mode because a non-continuous bandwidth is permitted.

	The TXFNUM of each endpoint's DIEPCTL[N] register selects the mode:
		- if set to 0 then the EP uses the TX FIFO 0, which is the shared FIFO
		  when more than one EP uses the same number, else it is dedicated for
		  IN EP 0
		- if set to an individual number then the EP uses its own dedicated
		  TX FIFO

	DMA SG mode
	===========

	DMA SG mode require the TX FIFOs to be configured in dedicated FIFO mode,
	else it causes random transfer complete interrupt not to trigger for IN EPs
	1 to 15.  It is possibly a bug.

	Incorrect configuration
	=======================

	Setting the FIFO with incorrect size will cause random (or perhaps time
	related) transfer complete interrupt not to trigger.  When it does not
	trigger our application will hang, because we are waiting for that interrupt
	after a transfer is started.  This does not apply if you are using polling
	instead of interrupts.

	Dynamic FIFO RAM sizing
	=======================

	The FIFO RAM can be re-sized dynamically - which means you can change the
	RX and TX FIFO size during execution, i.e. on the fly.  This is useful for
	re-configuring as soon as a device is plugged in, and wanting to maximise
	FIFO RAM to only the EPs that are actually used by the device.

	Maximum Packet Size (MPS)
	=========================

	USB 2.0 High Speed:
	Isochronous = 1024 bytes
	Interrupt = 1024 bytes
	Bulk = 512 bytes
	Control = 64 bytes

	Acronyms
	========

	DMA SG  = DMA Scatter Gather
	DMA SB  = DMA Single Buffer
	IN EP   = IN EndPoint.  IN direction is transmission from device (HPS) to
	          host (PC)
	OUT EP  = OUT EndPoint.  OUT direction is transmission from host (PC) to
	          device (HPS)
	RX FIFO = receive FIFO
	TX FIFO = transmit FIFO
	MPS     = Maximum Packet Size
*/
void tru_dwc2_init_fifo_shared(void *usb_base_addr, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep, uint32_t fifo_size_words){
	//uint16_t rxres_wsize = 3*3+1 + 1 + 2*num_dev_out_ep;  // RXFIFO reserved size: 3*Setup+1 + 1*GlobalOutNAK + 2*NumOutEP
	//uint16_t rxres_wsize = 3*3+1 + 1 + 1*num_dev_out_ep;  // RXFIFO reserved size: 3*Setup+1 + 1*GlobalOutNAK + 1*NumOutEP (for EPDisableStatus)
	//uint16_t rxres_wsize = 3*3+1 + 1;  // Preferred: RXFIFO reserved size: 3*Setup+1 + 1*GlobalOutNAK
	//uint16_t rxfifo_wsize = 3*3+1 + 1 + 2*num_dev_out_ep + 3*(1024/4+1);
	uint16_t rxfifo_wsize = fifo_size_words / 2;
#if defined(TRU_DWC2_DEV_MIN_RXFIFO_WORDSIZE)
	if(rxfifo_wsize < TRU_DWC2_DEV_MIN_RXFIFO_WORDSIZE){
		rxfifo_wsize = TRU_DWC2_DEV_MIN_RXFIFO_WORDSIZE;
	}
#endif
	uint16_t txfifo_wsize = fifo_size_words - rxfifo_wsize;  // Assign remaining avail space
	uint16_t fifo_woffset = rxfifo_wsize;  // Internal zero based FIFO address offset (in 32-bit words)

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("RXFIFO %u\n", 4 * rxfifo_wsize);
#endif

	// ===========================================
	// Setup EP shared receive FIFO for all OUT EP
	// ===========================================

	//TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->bits.rxfdep = rxfifo_wsize;
	TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->val = (TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->val & ~TRU_DWC2_GRXFSIZ_RXFDEP_SET_MSK) | rxfifo_wsize;

	TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr)->val = (uint32_t)txfifo_wsize << TRU_DWC2_GNPTXFSIZ_NPTXFDEP_LSB | fifo_woffset;

	// Enable shared TX FIFO mode
	for(uint8_t ep_num = 1; ep_num < num_dev_in_ep; ep_num++){
		// DMA SG mode require these to be set.  To be consistent, set dedicated TX FIFO size and address offset with the same values as the shared TX FIFO
		//TRU_DWC2_DIEPTXF_REG(usb_base_addr, ep_num)->bits.inepntxfstaddr = fifo_woffset;
		//TRU_DWC2_DIEPTXF_REG(usb_base_addr, ep_num)->bits.inepntxfdep = txfifo_wsize;
		TRU_DWC2_DIEPTXF_REG(usb_base_addr, ep_num)->val = (uint32_t)txfifo_wsize << TRU_DWC2_DIEPTXFN_INEPNTXFDEP_LSB | fifo_woffset;

		// Set IN EPs to use shared TX FIFO number 0
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.txfnum = 0;
	}

	//TRU_DWC2_GDFIFOCFG_REG(usb_base_addr)->bits.epinfobaseaddr = fifo_woffset;

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("GRXFSIZ (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GRXFSIZ_REG(usb_base_addr), (unsigned int)TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->val);
	LOG("GNPTXFSIZ (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr), (unsigned int)TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr)->val);
	for(uint8_t fifo_num = 1; fifo_num < num_dev_in_ep; fifo_num++){
		LOG("DIEPTXF%.2u (0x%.8x): 0x%.8x\n", fifo_num, (uintptr_t)TRU_DWC2_DIEPTXF_REG(usb_base_addr, fifo_num), (unsigned int)TRU_DWC2_DIEPTXF_REG(usb_base_addr, fifo_num)->val);
	}
#endif

	// Flush receive FIFO and all transmit FIFOs
	tru_dwc2_flush_rxtxfifo_wait(usb_base_addr, TRU_DWC2_GRSTCTL_TXFNUM_ALL);
}

/*
	We don't have the Synopsys DWC2 USB2.0 OTG controller IP technical reference manual, but from various information it seems we need to apply certain rules.

	The receive FIFO is shared (single FIFO) for EP OUTs.
	The transmit FIFO can be shared (single FIFO) or dedicated (individual FIFOs) for EP INs.

	Dedicated transmit FIFO mode
	============================

	TX FIFO size for each EP IN needs at least max packet size, recommended 2x max packet size
    RX FIFO size for all EP OUT is shared so uses a single RX FIFO with the following requirements in words (each word is 32-bits):
    - 3*NumSetup + 1*Control  (for setup packets + control)
    - 1*GlobalOutNAK  (for global Out NAK)
    - 1*(LargestEPOutMPS/4 + 1*Status)  (for largest packet, recommended 2*(LargestEPOutMPS/4 + 1)
    - 2*NumEPOut  (for each used EP OUT)

    The general ideal formula is:
    RXFIFOSize = GRXFSIZ = 3*3+1 + 1 + 2*(LargestEPOutMPS/4 + 1) + 2*NumEPOut
    TXFIFOSize[n] = DIEPTXF[n] = 2*EPInMPS

    Where:
		NumSetup = number of setup packets, recommended 3 setup packets
		NumEPOut = number of EP OUT
		LargestEPOutMPS = Largest MPS (Maximum Packet Size) of all EP OUTs
		EPInMPS = EP IN MPS (Maximum Packet Size)

	The calculation I've come up with is:
	1. reserve rx fifo space for some of the requirements
	2. sum up IN EP MPS and add LargestEPOutMPS
	3. using available (excludes reserves) and sum (excludes LargestEPOutMPS) as an average, we allocate the space some what distributed uniformly for the TX FIFOs
	4. allocate remaining space to RX FIFO
*/
void tru_dwc2_init_fifo_dedicated(void *usb_base_addr, uint16_t epins_mps[16], uint16_t epouts_mps[16], uint32_t num_dev_in_ep, uint32_t num_dev_out_ep, uint32_t fifo_size_words){
	uint16_t fifo_woffset;
	uint16_t largest_epout_mps = 0;
	uint16_t rxres_wsize = 3*3+1 + 1 + 2*num_dev_out_ep;  // RXFIFO reserved size: 3*Setup+1 + 1*GlobalOutNAK + 2*NumOutEP (for EPTransferCompleteStatus + EPDisableStatus)
	//uint16_t rxres_wsize = 3*3+1 + 1 + 1*num_dev_out_ep;  // RXFIFO reserved size: 3*Setup+1 + 1*GlobalOutNAK + 1*NumOutEP (for EPDisableStatus)
	//uint16_t rxres_wsize = 3*3+1 + 1;  // RXFIFO reserved size: 3*Setup+1 + 1*GlobalOutNAK
	uint16_t txfifo_wsize[16] = { 2 * INT_ALIGN_UP(epins_mps[0], 4) / 4 };  // TXFIFO0 for OUT EP0 is always for setup so we reserve 2x max packet size
	uint16_t rxfifo_wsize;
	uint32_t avail_size = 4*fifo_size_words - 4*rxres_wsize - 4*txfifo_wsize[0];
	uint32_t txfifo_size;
	uint32_t sum = 0;
	uint32_t sum2 = 0;

	// ====================
	// Calculate FIFO sizes
	// ====================

	// Find largest OUT EP MPS
	for(uint8_t ep_num = 0; ep_num < num_dev_out_ep; ep_num++){
		if(epouts_mps[ep_num] > largest_epout_mps) largest_epout_mps = INT_ALIGN_UP(epouts_mps[ep_num], 4);
	}
	largest_epout_mps += 4;  // Add 4 bytes (1 word) for 1x status per packet, recall we need at least 1*(LargestEPOutMPS/4 + 1)

#if defined(TRU_DWC2_DEV_MIN_RXFIFO_WORDSIZE)
	// Reserved RX FIFO space is less than minimum?
	if((rxres_wsize + largest_epout_mps) < TRU_DWC2_DEV_MIN_RXFIFO_WORDSIZE){
		// Recalculate
		rxres_wsize = TRU_DWC2_DEV_MIN_RXFIFO_WORDSIZE - (rxres_wsize + largest_epout_mps);
		avail_size = 4*fifo_size_words - 4*rxres_wsize - 4*txfifo_wsize[0];
	}
#endif

	// Sum up IN EP MPS (calculation step 2)
	for(uint8_t ep_num = 1; ep_num < num_dev_in_ep; ep_num++){
		if(ep_num > 0) sum += INT_ALIGN_UP(epins_mps[ep_num], 4);
	}
	sum += largest_epout_mps;  // Add largest OUT EP MPS to the sum

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("TXFIFO0 %u\n", 4 * txfifo_wsize[0]);
#endif

	// Determine TX FIFO sizes (calculation step 3)
	for(uint8_t ep_num = 1; ep_num < num_dev_in_ep; ep_num++){
		if(INT_ALIGN_UP(epins_mps[ep_num], 4)){  // If MPS is non-zero
			txfifo_size = INT_ALIGN_UP(epins_mps[ep_num], 4) * avail_size / (sum * INT_ALIGN_UP(epins_mps[ep_num], 4)) * INT_ALIGN_UP(epins_mps[ep_num], 4);
		}else{
			txfifo_size = 0;
		}
		txfifo_wsize[ep_num] = txfifo_size / 4;
		sum2 += txfifo_size;

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
		LOG("TXFIFO%u %u\n", ep_num, 4 * txfifo_wsize[ep_num]);
#endif
	}

	// Determine RX FIFO size (calculation step 4)
	rxfifo_wsize = ((avail_size - sum2) - (avail_size - sum2) % largest_epout_mps) / 4 + rxres_wsize;

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("RXFIFO %u\n", 4 * rxfifo_wsize);
#endif

	// ===========================================
	// Setup EP shared receive FIFO for all OUT EP
	// ===========================================

	//TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->bits.rxfdep = rxfifo_wsize;
	TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->val = (TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->val & ~TRU_DWC2_GRXFSIZ_RXFDEP_SET_MSK) | rxfifo_wsize;
	fifo_woffset = rxfifo_wsize;  // Internal zero based FIFO address offset (in 32-bit words)

	// ==================================
	// Setup dedicated TX FIFO for IN EPs
	// ==================================

	// Setup EP0 transmit FIFO with size and FIFO address offset (EP0 in)
	// If dedicated, since EP0 is Control data flow only, the max packet size = HS Control = 64 bytes
	//TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr)->bits.nptxfstaddr = fifo_woffset;
	//TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr)->bits.nptxfdep = txfifo_wsize[0];
	TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr)->val = (uint32_t)txfifo_wsize[0] << TRU_DWC2_GNPTXFSIZ_NPTXFDEP_LSB | fifo_woffset;
	fifo_woffset += txfifo_wsize[0];

	// Enable dedicated TX FIFO mode
	for(uint8_t ep_num = 1; ep_num < num_dev_in_ep; ep_num++){
		// Set dedicated TX FIFO size and address offset
		//TRU_DWC2_DIEPTXF_REG(usb_base_addr, ep_num)->bits.inepntxfstaddr = fifo_woffset;
		//TRU_DWC2_DIEPTXF_REG(usb_base_addr, ep_num)->bits.inepntxfdep = txfifo_wsize[ep_num];
		TRU_DWC2_DIEPTXF_REG(usb_base_addr, ep_num)->val = (uint32_t)txfifo_wsize[ep_num] << TRU_DWC2_DIEPTXFN_INEPNTXFDEP_LSB | fifo_woffset;
		fifo_woffset += txfifo_wsize[ep_num];

		// Set IN EPs to use a dedicated TX FIFO number
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.txfnum = ep_num;
	}

	//TRU_DWC2_GDFIFOCFG_REG(usb_base_addr)->bits.epinfobaseaddr = fifo_woffset;

#if defined(TRU_CFG_USB_LOG_INTR) && TRU_CFG_USB_LOG_INTR == 1
	LOG("GRXFSIZ (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GRXFSIZ_REG(usb_base_addr), (unsigned int)TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->val);
	LOG("GNPTXFSIZ (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr), (unsigned int)TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr)->val);
	for(uint8_t fifo_num = 1; fifo_num < num_dev_in_ep; fifo_num++){
		LOG("DIEPTXF%.2u (0x%.8x): 0x%.8x\n", fifo_num, (uintptr_t)TRU_DWC2_DIEPTXF_REG(usb_base_addr, fifo_num), (unsigned int)TRU_DWC2_DIEPTXF_REG(usb_base_addr, fifo_num)->val);
	}
#endif

	// Flush receive FIFO and all transmit FIFOs
	tru_dwc2_flush_rxtxfifo_wait(usb_base_addr, TRU_DWC2_GRSTCTL_TXFNUM_ALL);
}

/*
	Reallocate dynamic FIFOs.

	Before reallocation we must ensure:
	- All IN and OUT endpoints are disabled, i.e. EPENA=0 bit for all endpoints is not primed for transferring
	- NAK mode is enabled in the core on all IN endpoints
	- Global OUT NAK mode is enabled in the core

	After reallocation we must ensure:
	- To flush all FIFOs (RX and TX)
	- Clear the global OUT NAK mode
*/
void tru_dwc2_realloc_fifo(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode, uint16_t epins_mps[16], uint16_t epouts_mps[16], bool use_globaloutnak, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep, uint32_t fifo_size_words){
	tru_dwc2_snak_epin_wait_all(usb_base_addr, num_dev_in_ep);
	tru_dwc2_snak_epout_wait_all(usb_base_addr, num_dev_out_ep);  // This is not in the list but it doesn't hurt to set them anyway
	if(use_globaloutnak){
		tru_dwc2_set_globaloutnak_wait(usb_base_addr, dma_mode);
		tru_dwc2_init_fifo_dedicated(usb_base_addr, epins_mps, epouts_mps, num_dev_in_ep, num_dev_out_ep, fifo_size_words);  // This also flushes FIFOs
		tru_dwc2_clr_globaloutnak_wait(usb_base_addr);
	}else{
		tru_dwc2_init_fifo_dedicated(usb_base_addr, epins_mps, epouts_mps, num_dev_in_ep, num_dev_out_ep, fifo_size_words);  // This also flushes FIFOs
	}
}

// Reset the controller
void tru_dwc2_grstctl_reset(void *usb_base_addr, uint32_t snpsid){
	if((snpsid & TRU_DWC2_GSNPSID_REV_MASK) < (TRU_DWC2_GSNPSID_CORE_REV_4_20A & TRU_DWC2_GSNPSID_REV_MASK)){
		// Old revision reset procedure

		while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.csftrst == 1);  // Just in case a 3rd party library (e.g. STM32 CubeMX/HAL) started a reset but did not wait for it to complete, let's do an initial check
		TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.csftrst = 1;          // Enable core reset.  Warning: setting this to a 1 when a reset in progress will make it sticky with 1, i.e. will never reset to a 0
		while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.csftrst == 1);  // Wait for core reset register to become 0, i.e. to complete
	}else{
		// Newer revision reset procedure
		// Ref: dwc2_core_reset() in the linux kernel file /drivers/usb/dwc2/core.c
		// e.g.: https://github.com/milkv-duo/duo-buildroot-sdk/blob/develop/linux_5.10/drivers/usb/dwc2/core.c#L423

		TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.csftrst = 1;  // Enable core reset
		while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.csftrst_done == 0);  // Wait for core reset done register to become 1, i.e. to complete

		// Note: GRSTCTL.CSFTRST is not self clearing, even though some reference manuals says that it self clears
		tru_dwc2_grstctl_t grstctl = { .val = TRU_DWC2_GRSTCTL_REG(usb_base_addr)->val };
		grstctl.bits.csftrst = 0;  // Disable core reset bit
		grstctl.bits.csftrst_done = 1;
		TRU_DWC2_GRSTCTL_REG(usb_base_addr)->val = grstctl.val;
	}
}

/*
	Setup global USB AHB configuration settings.

	Notes:

	Bug on GAHBCFG register with these settings:
		From my own tests, when using DMA mode with hbstlen set to 0 (1 word or single) and the TX FIFO is set to two times the max transfer packet size,
		sometimes the controller fail to trigger the IN EP transfer complete interrupt, it seems to happen randomly

	Interrupt enable:
		The GLBLINTRMSK register bit is the main USB controller interrupt generation enable bit, write 1 to enable, interrupts will immediately generate
*/
void tru_dwc2_init_gahbcfg(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode){
	tru_dwc2_gahbcfg_t gahbcfg = { .val = TRU_DWC2_GAHBCFG_REG(usb_base_addr)->val };

	gahbcfg.bits.glblintrmsk = 0;
	//gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD1ORSINGLE;
	//gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD4ORINCR;
	//gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD8;
	//gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD16ORINCR4;
	//gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD32;
	//gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD64ORINCR8;
	//gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD128;
	gahbcfg.bits.hbstlen = TRU_DWC2_GAHBCFG_HBSTLEN_WORD256ORINCR16;
	gahbcfg.bits.dmaen = ((dma_mode == TRU_DWC2_DMA_MODE_SB || dma_mode == TRU_DWC2_DMA_MODE_SG) ? 1 : 0);
	//gahbcfg.bits.nptxfemplvl = TRU_DWC2_GAHBCFG_NPTXFEMPLVL_EMPTY;
	//gahbcfg.bits.ptxfemplvl = TRU_DWC2_GAHBCFG_PTXFEMPLVL_EMPTY;
	gahbcfg.bits.nptxfemplvl = TRU_DWC2_GAHBCFG_NPTXFEMPLVL_HALFEMPTY;
	gahbcfg.bits.ptxfemplvl = TRU_DWC2_GAHBCFG_PTXFEMPLVL_HALFEMPTY;
	gahbcfg.bits.remmemsupp = 0;
	gahbcfg.bits.notialldmawrit = 0;

	TRU_DWC2_GAHBCFG_REG(usb_base_addr)->val = gahbcfg.val;
}

// Setup USB-PHY related configuration settings
void tru_dwc2_init_gusbcfg_phy(void *usb_base_addr, tru_dwc2_phy_t *phy, bool forcedev){
	tru_dwc2_gusbcfg_t gusbcfg = { .val = TRU_DWC2_GUSBCFG_REG(usb_base_addr)->val };

	gusbcfg.bits.forcehstmode = TRU_DWC2_GUSBCFG_FORCEHSTMODE_DIS;
	gusbcfg.bits.forcedevmode = (forcedev) ? TRU_DWC2_GUSBCFG_FORCEDEVMODE_ENA : TRU_DWC2_GUSBCFG_FORCEDEVMODE_DIS;

	if(phy->speed == TRU_DWC2_PHYSPEED_FS){
		gusbcfg.bits.physel = TRU_DWC2_GUSBCFG_PHYSEL_USB11;
		gusbcfg.bits.fsintf = TRU_DWC2_GUSBCFG_FSINTF_FS6PIN;

		switch(phy->iftype){
			case TRU_DWC2_PHYIFTYPE_INTERNAL:
				gusbcfg.bits.ulpifsl = 0;
				gusbcfg.bits.ulpiclksusm = 0;
				gusbcfg.bits.ulpi = TRU_DWC2_GUSBCFG_ULPI_DIS;
				break;
			case TRU_DWC2_PHYIFTYPE_ULPI:
				gusbcfg.bits.ulpifsl = 1;
				gusbcfg.bits.ulpiclksusm = 1;
				gusbcfg.bits.ulpi = TRU_DWC2_GUSBCFG_ULPI_ENA;
				break;
			case TRU_DWC2_PHYIFTYPE_UTMI:
				gusbcfg.bits.ulpifsl = 0;
				gusbcfg.bits.ulpiclksusm = 0;
				gusbcfg.bits.ulpi = TRU_DWC2_GUSBCFG_ULPI_DIS;

				if(phy->width == TRU_DWC2_PHYWIDTH_8BIT){
					gusbcfg.bits.phyif = TRU_DWC2_GUSBCFG_PHYIF_8BIT;
					if(tru_dwc2_is_device_mode(usb_base_addr)){
						gusbcfg.bits.usbtrdtim = 5;
					}
				}else{
					gusbcfg.bits.phyif = TRU_DWC2_GUSBCFG_PHYIF_16BIT;
					if(tru_dwc2_is_device_mode(usb_base_addr)){
						gusbcfg.bits.usbtrdtim = 9;
					}
				}
				break;
			default:
		}
	}else{
		gusbcfg.bits.physel = TRU_DWC2_GUSBCFG_PHYSEL_USB20;

		switch(phy->iftype){
			case TRU_DWC2_PHYIFTYPE_INTERNAL:
				gusbcfg.bits.ulpifsl = 0;
				gusbcfg.bits.ulpiclksusm = 0;
				gusbcfg.bits.ulpi = TRU_DWC2_GUSBCFG_ULPI_DIS;
				break;
			case TRU_DWC2_PHYIFTYPE_ULPI:
				gusbcfg.bits.ulpi_utmi_sel = TRU_DWC2_GUSBCFG_ULPI_UTMI_SEL_ULPI;
				gusbcfg.bits.ulpifsl = 0;
				gusbcfg.bits.ulpiclksusm = 0;
				gusbcfg.bits.ulpi = TRU_DWC2_GUSBCFG_ULPI_ENA;

				if(phy->rate == TRU_DWC2_PHYRATE_SDR){
					gusbcfg.bits.ddrsel = TRU_DWC2_GUSBCFG_DDRSEL_SDR;
				}else{
					gusbcfg.bits.ddrsel = TRU_DWC2_GUSBCFG_DDRSEL_DDR;
				}
				break;
			case TRU_DWC2_PHYIFTYPE_UTMI:
				gusbcfg.bits.ulpi_utmi_sel = TRU_DWC2_GUSBCFG_ULPI_UTMI_SEL_UTMI;
				gusbcfg.bits.ulpifsl = 0;
				gusbcfg.bits.ulpiclksusm = 0;
				gusbcfg.bits.ulpi = TRU_DWC2_GUSBCFG_ULPI_DIS;

				if(phy->width == TRU_DWC2_PHYWIDTH_8BIT){
					gusbcfg.bits.phyif = TRU_DWC2_GUSBCFG_PHYIF_8BIT;
					if(tru_dwc2_is_device_mode(usb_base_addr)){
						gusbcfg.bits.usbtrdtim = 5;
					}
				}else{
					gusbcfg.bits.phyif = TRU_DWC2_GUSBCFG_PHYIF_16BIT;
					if(tru_dwc2_is_device_mode(usb_base_addr)){
						gusbcfg.bits.usbtrdtim = 9;
					}
				}
				break;
			default:
		}
	}
	gusbcfg.bits.ulpiautores = TRU_DWC2_GUSBCFG_ULPIAUTORES_DIS;
	gusbcfg.bits.ulpiextvbusdrv = TRU_DWC2_GUSBCFG_ULPIEXTVBUSDRV_INT;
	gusbcfg.bits.ulpiextvbusindicator = TRU_DWC2_GUSBCFG_ULPIEXTVBUSINDICATOR_INT;
	gusbcfg.bits.termseldlpulse = TRU_DWC2_GUSBCFG_TERMSELDLPULSE_TXVALID;
	gusbcfg.bits.complement = TRU_DWC2_GUSBCFG_COMPLEMENT_NONINVERT;
	gusbcfg.bits.indicator = TRU_DWC2_GUSBCFG_INDICATOR_QUALIFIED;
	gusbcfg.bits.txenddelay = TRU_DWC2_GUSBCFG_TXENDDELAY_NORMAL;
	gusbcfg.bits.corrupttxpkt = TRU_DWC2_GUSBCFG_CORRUPTTXPKT_NORMAL;

	gusbcfg.bits.toutcal = 0x7;
	gusbcfg.bits.srpcap = TRU_DWC2_GUSBCFG_SRPCAP_DIS;
	gusbcfg.bits.hnpcap = TRU_DWC2_GUSBCFG_HNPCAP_DIS;

	TRU_DWC2_GUSBCFG_REG(usb_base_addr)->val = gusbcfg.val;
}

void tru_dwc2_force_devmode(void *usb_base_addr){
	TRU_DWC2_GUSBCFG_REG(usb_base_addr)->bits.forcedevmode = TRU_DWC2_GUSBCFG_FORCEDEVMODE_ENA;
}

void tru_dwc2_force_hostmode(void *usb_base_addr){
	TRU_DWC2_GUSBCFG_REG(usb_base_addr)->bits.forcehstmode = TRU_DWC2_GUSBCFG_FORCEHSTMODE_ENA;
}

void tru_dwc2_init_gccfg(void *usb_base_addr, tru_dwc2_phy_t *phy){
	tru_dwc2_gccfg_t gccfg = { .val = TRU_DWC2_GCCFG_REG(usb_base_addr)->val };

	gccfg.bits.vbden = 1;
	gccfg.bits.sden = 0;
	gccfg.bits.pden = 0;
	gccfg.bits.dcden = 0;
	gccfg.bits.bcden = 0;
	if(phy->speed == TRU_DWC2_PHYSPEED_FS){
		gccfg.bits.pwrdwn = 1;
	}else{
		gccfg.bits.pwrdwn = 0;
	}

	TRU_DWC2_GCCFG_REG(usb_base_addr)->val = gccfg.val;
}

// Setup device mode configuration settings
void tru_dwc2_init_dcfg(void *usb_base_addr, tru_dwc2_phy_t *phy, tru_dwc2_dma_mode_t dma_mode, uint8_t devspd){
	tru_dwc2_dcfg_t dcfg = { .val = TRU_DWC2_DCFG_REG(usb_base_addr)->val };

	dcfg.bits.devspd = devspd;  // Enumerated speed limit
	dcfg.bits.nzstsouthshk = 0;
	dcfg.bits.ena32khzsusp = 0;
	dcfg.bits.devaddr = 0;
	dcfg.bits.perfrint = 0;
	dcfg.bits.endevoutnak = 0;
	if(phy->iftype == TRU_DWC2_PHYIFTYPE_ULPI || phy->iftype == TRU_DWC2_PHYIFTYPE_UTMI){
		// XCVRDLY: Transceiver delay.  1 = Enable delay to default timing, 0 = Disable delay (use default timing)
		// This necessary for some ULPI PHYs e.g. Microchip USB334x (USB3341, USB3343, USB3346, USB3347), otherwise it enumerates only to full-speed
		dcfg.bits.xcvrdly = 1;
	}
	dcfg.bits.descdma = (dma_mode == TRU_DWC2_DMA_MODE_SG) ? 1 : 0;  // Select DMA mode, 0 = normal vs 1 = scatter gather
	dcfg.bits.perschintvl = 0;
	dcfg.bits.resvalid = 2;

	TRU_DWC2_DCFG_REG(usb_base_addr)->val = dcfg.val;
}

// Setup device mode control settings
void tru_dwc2_init_dctl(void *usb_base_addr){
	tru_dwc2_dctl_t dctl = { .val = TRU_DWC2_DCTL_REG(usb_base_addr)->val };

	//dctl.bits.cgnpinnak = 1;
	//dctl.bits.cgoutnak = 1;
	dctl.bits.gmc = 1;
	//dctl.bits.gmc = 3;
	dctl.bits.ignrfrmnum = 0;
	//dctl.bits.ignrfrmnum = 1;
	dctl.bits.sftdiscon = 0;

	TRU_DWC2_DCTL_REG(usb_base_addr)->val = dctl.val;
}

void tru_dwc2_init_dthrctl(void *usb_base_addr){
	tru_dwc2_dthrctl_t dthrctl = { .val = TRU_DWC2_DTHRCTL_REG(usb_base_addr)->val };

	dthrctl.bits.nonisothren = 0;
	dthrctl.bits.isothren = 0;
	dthrctl.bits.txthrlen = 8;
	dthrctl.bits.ahbthrratio = 0;
	dthrctl.bits.rxthren = 0;
	dthrctl.bits.rxthrlen = 8;
	dthrctl.bits.arbprken = 1;

	TRU_DWC2_DTHRCTL_REG(usb_base_addr)->val = dthrctl.val;
}

void tru_dwc2_init_pcgcctl(void *usb_base_addr){
	tru_dwc2_pcgcctl_t pcgcctl = { .val = TRU_DWC2_PCGCCTL_REG(usb_base_addr)->val };

	pcgcctl.bits.stppclk = 0;
	pcgcctl.bits.gatehclk = 0;
	pcgcctl.bits.physusp = 0;
	pcgcctl.bits.enl1gtg = 0;
	pcgcctl.bits.physleep = 0;
	pcgcctl.bits.susp = 0;

	TRU_DWC2_PCGCCTL_REG(usb_base_addr)->val = pcgcctl.val;
}

void tru_dwc2_log_details(void *usb_base_addr, uint32_t num_dev_in_ep){
	uint32_t sid = TRU_DWC2_GSNPSID_REG(usb_base_addr)->val;
	LOG("GSNPSID (0x%.8x): 0x%.8x (%.1x.%.3x)\n", (uintptr_t)TRU_DWC2_GSNPSID_REG(usb_base_addr), (unsigned int)sid, (unsigned int)sid >> 12 & 0xf, (unsigned int)sid & 0xfff);
	uint32_t cid = TRU_DWC2_CID_REG(usb_base_addr)->val;
	LOG("CID (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_CID_REG(usb_base_addr), (unsigned int)cid);
	LOG("GUSBCFG (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GUSBCFG_REG(usb_base_addr), (unsigned int)TRU_DWC2_GUSBCFG_REG(usb_base_addr)->val);
	LOG("GAHBCFG (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GAHBCFG_REG(usb_base_addr), (unsigned int)TRU_DWC2_GAHBCFG_REG(usb_base_addr)->val);
	LOG("GOTGCTL (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GOTGCTL_REG(usb_base_addr), (unsigned int)TRU_DWC2_GOTGCTL_REG(usb_base_addr)->val);
	LOG("GRSTCTL (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GRSTCTL_REG(usb_base_addr), (unsigned int)TRU_DWC2_GRSTCTL_REG(usb_base_addr)->val);
	LOG("GRXSTSR (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GRXSTSR_REG(usb_base_addr), (unsigned int)TRU_DWC2_GRXSTSR_REG(usb_base_addr)->val);
	LOG("GNPTXSTS (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GNPTXSTS_REG(usb_base_addr), (unsigned int)TRU_DWC2_GNPTXSTS_REG(usb_base_addr)->val);
	LOG("GPVNDCTL (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GPVNDCTL_REG(usb_base_addr), (unsigned int)TRU_DWC2_GPVNDCTL_REG(usb_base_addr)->val);
	LOG("GDFIFOCFG (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GDFIFOCFG_REG(usb_base_addr), (unsigned int)TRU_DWC2_GDFIFOCFG_REG(usb_base_addr)->val);
	LOG("GCCFG (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GCCFG_REG(usb_base_addr), (unsigned int)TRU_DWC2_GCCFG_REG(usb_base_addr)->val);
	LOG("PCGCCTL (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_PCGCCTL_REG(usb_base_addr), (unsigned int)TRU_DWC2_PCGCCTL_REG(usb_base_addr)->val);
	LOG("GINTMSK (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GINTMSK_REG(usb_base_addr), (unsigned int)TRU_DWC2_GINTMSK_REG(usb_base_addr)->val);
	LOG("GINTSTS (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GINTSTS_REG(usb_base_addr), (unsigned int)TRU_DWC2_GINTSTS_REG(usb_base_addr)->val);
	//LOG("HPTXFSIZ (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_HPTXFSIZ_REG(usb_base_addr), (unsigned int)TRU_DWC2_HPTXFSIZ_REG(usb_base_addr)->val);
	LOG("GRXFSIZ (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GRXFSIZ_REG(usb_base_addr), (unsigned int)TRU_DWC2_GRXFSIZ_REG(usb_base_addr)->val);
	LOG("GNPTXFSIZ (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr), (unsigned int)TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr)->val);
	for(uint8_t fifo_num = 1; fifo_num < num_dev_in_ep; fifo_num++){
		LOG("DIEPTXF%.2u (0x%.8x): 0x%.8x\n", fifo_num, (uintptr_t)TRU_DWC2_DIEPTXF_REG(usb_base_addr, fifo_num), (unsigned int)TRU_DWC2_DIEPTXF_REG(usb_base_addr, fifo_num)->val);
	}
	LOG("DCFG (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_DCFG_REG(usb_base_addr), (unsigned int)TRU_DWC2_DCFG_REG(usb_base_addr)->val);
	LOG("DCTL (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_DCTL_REG(usb_base_addr), (unsigned int)TRU_DWC2_DCTL_REG(usb_base_addr)->val);
	LOG("DAINTMSK (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_DAINTMSK_REG(usb_base_addr), (unsigned int)TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val);
	LOG("DIEPMSK (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_DIEPMSK_REG(usb_base_addr), (unsigned int)TRU_DWC2_DIEPMSK_REG(usb_base_addr)->val);
	LOG("DOEPMSK (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_DOEPMSK_REG(usb_base_addr), (unsigned int)TRU_DWC2_DOEPMSK_REG(usb_base_addr)->val);
	LOG("DIEPEMPMSK (0x%.8x): 0x%.8x\n", (uintptr_t)TRU_DWC2_DIEPEMPMSK_REG(usb_base_addr), (unsigned int)TRU_DWC2_DIEPEMPMSK_REG(usb_base_addr)->val);
	/*
	for(uint8_t ep_num = 0; ep_num < TRU_DWC2_MAX_PAIRS_EP; ep_num++){
		LOG("DIEPINT%u (0x%.8x): 0x%.8x\n", ep_num, (uintptr_t)TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num), (unsigned int)TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num)->val);
		LOG("DOEPINT%u (0x%.8x): 0x%.8x\n", ep_num, (uintptr_t)TRU_DWC2_DOEPINT_REG(usb_base_addr, ep_num), (unsigned int)TRU_DWC2_DOEPINT_REG(usb_base_addr, ep_num)->val);
		LOG("DIEPCTL%u (0x%.8x): 0x%.8x DIEPTSIZ%u (0x%.8x): 0x%.8x\n", ep_num, (uintptr_t)TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num), (unsigned int)TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val, ep_num, (uintptr_t)TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num), (unsigned int)TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->val);
		LOG("DOEPCTL%u (0x%.8x): 0x%.8x DOEPTSIZ%u (0x%.8x): 0x%.8x\n", ep_num, (uintptr_t)TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num), (unsigned int)TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val, ep_num, (uintptr_t)TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num), (unsigned int)TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->val);
	}
	*/

	for(uint8_t ep_num = 0; ep_num < num_dev_in_ep; ep_num++){

	}

	LOG("Version: %.1x.%.3x\n", (unsigned int)sid >> 12 & 0xf, (unsigned int)sid & 0xfff);
	LOG("Num of device mode endpoints: %u\n", TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.numdeveps + 1);  // Increment to include EP0
	LOG("Num of IN endpoints: %u\n", TRU_DWC2_GHWCFG4_REG(usb_base_addr)->bits.ineps + 1);
	LOG("Token queue depth: %u\n", TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.tknqdepth);
	LOG("Periodic TX FIFO queue depth: %u\n", TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.ptxqdepth);
	LOG("Non-periodic TX FIFO queue depth: %u\n", TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.nptxqdepth);
	LOG("Dynamic FIFO sizing capable: %s\n", TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.dynfifosizing ? "Yes" : "No");
	LOG("Transfer size width (bits): %u\n", TRU_DWC2_GHWCFG3_REG(usb_base_addr)->bits.xfersizewidth + 11);
	LOG("Packet size counter width (bits): %u\n", TRU_DWC2_GHWCFG3_REG(usb_base_addr)->bits.pktsizewidth + 4);
	LOG("FIFO total size: %u words (%u bytes)\n", TRU_DWC2_GDFIFOCFG_REG(usb_base_addr)->bits.gdfifocfg, TRU_DWC2_GDFIFOCFG_REG(usb_base_addr)->bits.gdfifocfg * 4);
	LOG("FIFO usable size: %u words (%u bytes)\n", TRU_DWC2_GHWCFG3_REG(usb_base_addr)->bits.dfifodepth, TRU_DWC2_GHWCFG3_REG(usb_base_addr)->bits.dfifodepth * 4);
	LOG("Dedicated FIFO in device mode: %s\n", TRU_DWC2_GHWCFG4_REG(usb_base_addr)->bits.dedfifomode ? "Yes" : "No");

	LOG("DMA architecture: ");
	switch(TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.dmaarch){
		case TRU_DWC2_GHWCFG2_DMA_NONE: LOG("DMA not supported, slave FIFO only\n"); break;
		case TRU_DWC2_GHWCFG2_DMA_EXTERNAL: LOG("External DMA\n"); break;
		case TRU_DWC2_GHWCFG2_DMA_INTERNAL: LOG("Internal DMA\n"); break;
		default: LOG("Unknown\n");
	}
	LOG("DMA scatter gather capable: %s\n", TRU_DWC2_GHWCFG4_REG(usb_base_addr)->bits.dma ? "Yes" : "No");

	LOG("OTG mode: ");
	switch(TRU_DWC2_GHWCFG2_REG(usb_base_addr)->bits.otgmode){
		case TRU_DWC2_GHWCFG2_HNP_SRP_CAPABLE_OTG_HOST_DEV: LOG("HNP and SRP capable OTG host & device\n"); break;
		case TRU_DWC2_GHWCFG2_SRP_HNP_SRP_CAPABLE_OTG_HOST_DEV: LOG("SRP capable OTG host & device\n"); break;
		case TRU_DWC2_GHWCFG2_NON_HNP_NON_SRP_CAPABLE_OTG_HOST_DEV: LOG("Non-HNP and Non-SRP capable OTG host & device\n"); break;
		case TRU_DWC2_GHWCFG2_SRP_CAPABLE_DEV: LOG("SRP capable device\n"); break;
		case TRU_DWC2_GHWCFG2_NON_OTG_DEV: LOG("Non-OTG device\n"); break;
		case TRU_DWC2_GHWCFG2_SRP_CAPABLE_HOST: LOG("SRP capable host\n"); break;
		case TRU_DWC2_GHWCFG2_NON_OTG_HOST: LOG("Non-OTG host\n"); break;
		default: LOG("Unknown\n");
	}
}

// Enable interrupt generation for TX FIFO (IN endpoint) empty
void tru_dwc2_set_intmask_txfifo_empty(void *usb_base_addr, uint32_t txfnum){
	TRU_DWC2_DIEPINT_REG(usb_base_addr, txfnum)->bits.txfemp = 1;  // Clear the interrupt trigger
	TRU_DWC2_DIEPEMPMSK_REG(usb_base_addr)->val |= 1 << txfnum;  // Enable trigger to generate interrupts
}

// Disable interrupt generation for TX FIFO (IN endpoint) empty
void tru_dwc2_clr_intmask_txfifo_empty(void *usb_base_addr, uint32_t txfnum){
	TRU_DWC2_DIEPEMPMSK_REG(usb_base_addr)->val &= ~(1 << txfnum);
}

void tru_dwc2_set_dev_test_mode(void *usb_base_addr, uint8_t test_mode){
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.tstctl = test_mode;
}

void tru_dwc2_clr_dev_test_mode(void *usb_base_addr){
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.tstctl = TRU_DWC2_DCTL_TSTCTL_DIS;
}

void tru_dwc2_ahb_master_busy_wait(void *usb_base_addr){
	while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.ahbidle == 0);  // 0 = not idle, 1 = idle
}

bool tru_dwc2_is_ahb_master_idle(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.ahbidle ? true : false;
}

bool tru_dwc2_is_ahb_master_busy(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.ahbidle ? false : true;
}

bool tru_dwc2_is_dma_req_idle(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.dmareq ? false : true;
}

bool tru_dwc2_is_dma_req_busy(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.dmareq ? true : false;
}

/*
	Check whether last request for receive FIFO flush is completed.
*/
bool tru_dwc2_is_flush_rxfifo_idle(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh ? false : true;
}

/*
	Check whether last request for receive FIFO flush is still pending.
*/
bool tru_dwc2_is_flush_rxfifo_busy(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh ? true : false;
}

/*
	Check whether last request for transmit FIFO flush is completed.
*/
bool tru_dwc2_is_flush_txfifo_idle(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh ? false : true;
}

/*
	Check whether last request for transmit FIFO flush is still pending.
*/
bool tru_dwc2_is_flush_txfifo_busy(void *usb_base_addr){
	return TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh ? true : false;
}

/*
	Check whether last request for receive and transmit FIFO flush is completed.
*/
bool tru_dwc2_is_flush_rxtxfifo_idle(void *usb_base_addr){
	return (TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh || TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh) ? false : true;
}

/*
	Check whether last request for receive and transmit FIFO flush is still pending.
*/
bool tru_dwc2_is_flush_rxtxfifo_busy(void *usb_base_addr){
	return (TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh || TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh) ? true : false;
}

/*
	Flush the receive FIFO.
*/
void tru_dwc2_flush_rxfifo_wait(void *usb_base_addr){
	while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.ahbidle == 0);  // Wait for AHB master idle state: 0 = not idle, 1 = idle
	TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh = 1;          // Flush the receive FIFO
	while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh);        // Wait for flush to complete: 0 = done, 1 = flush not done
}

/*
	Flush the specified transmit FIFO(s).
*/
void tru_dwc2_flush_txfifo_wait(void *usb_base_addr, uint32_t txfnum){
	while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.ahbidle == 0);  // Wait for AHB master idle state: 0 = not idle, 1 = idle
	TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfnum = txfnum;
	TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh = 1;          // Flush the transmit FIFO
	while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh);        // Wait for flush to complete: 0 = done, 1 = flush not done
}

void tru_dwc2_flush_txfifo_all_wait(void *usb_base_addr){
	tru_dwc2_flush_txfifo_wait(usb_base_addr, TRU_DWC2_GRSTCTL_TXFNUM_ALL);
}

/*
	Flush the specified receive FIFO and the transmit FIFO(s) at the same time.
*/
void tru_dwc2_flush_rxtxfifo_wait(void *usb_base_addr, uint32_t txfnum){
	while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.ahbidle == 0);  // Wait for AHB master idle state: 0 = not idle, 1 = idle
	TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfnum = txfnum;
	TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh = 1;          // Flush the transmit FIFO
	TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh = 1;          // Flush the receive FIFO
	while(TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.rxfflsh || TRU_DWC2_GRSTCTL_REG(usb_base_addr)->bits.txfflsh);       // Wait for flush to complete: 0 = done, 1 = flush not done
}

/*
	Get USB address.
*/
uint8_t tru_dwc2_get_address(void *usb_base_addr){
	return TRU_DWC2_DCFG_REG(usb_base_addr)->bits.devaddr;
}

/*
	Set USB address.
*/
void tru_dwc2_set_address(void *usb_base_addr, uint8_t address){
	TRU_DWC2_DCFG_REG(usb_base_addr)->bits.devaddr = address;
}

/*
	Enable interrupt trigger and activate an in endpoint.
*/
void tru_dwc2_activate_epin(void *usb_base_addr, uint8_t ep_num, uint8_t flow_type, uint16_t mps, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid){
	tru_dwc2_diepctl_t diepctl = { .val = TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val };

	TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val |= 0x00000001 << ep_num;  // Enable the interrupt trigger flag for the endpoint

	diepctl.bits.mps = (ep_num == 0) ? tru_dwc2_ep0_mps_to_regcode(mps) : mps;
	diepctl.bits.eptype = flow_type;
	diepctl.bits.usbactep = 1;

	switch(stall){
		case TRU_DWC2_STALL_CLEAR: diepctl.bits.stall = 0; break;
		case TRU_DWC2_STALL_SET: diepctl.bits.stall = 1; break;
		default:
	}

	switch(nak){
		case TRU_DWC2_NAK_CLEAR: diepctl.bits.cnak = 1; break;
		case TRU_DWC2_NAK_SET: diepctl.bits.snak = 1; break;
		default:
	}

	if(ep_num){
		switch(dpid){
			case TRU_DWC2_DPID_EO_D0_OR_EVEN: diepctl.bits.d0pid_even = 1; break;
			case TRU_DWC2_DPID_EO_D1_OR_ODD: diepctl.bits.d1pid_odd = 1; break;
			default:
		}
	}

	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val = diepctl.val;
}

/*
	Enable interrupt trigger and activate an out endpoint.
*/
void tru_dwc2_activate_epout(void *usb_base_addr, uint8_t ep_num, uint8_t flow_type, uint16_t mps, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid){
	tru_dwc2_doepctl_t doepctl = { .val = TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val };

	TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val |= 0x00010000 << ep_num;  // Enable the interrupt trigger flag for the endpoint

	doepctl.bits.mps = (ep_num == 0) ? tru_dwc2_ep0_mps_to_regcode(mps) : mps;
	doepctl.bits.eptype = flow_type;
	doepctl.bits.usbactep = 1;

	switch(stall){
		case TRU_DWC2_STALL_CLEAR: doepctl.bits.stall = 0; break;
		case TRU_DWC2_STALL_SET: doepctl.bits.stall = 1; break;
		default:
	}

	switch(nak){
		case TRU_DWC2_NAK_CLEAR: doepctl.bits.cnak = 1; break;
		case TRU_DWC2_NAK_SET: doepctl.bits.snak = 1; break;
		default:
	}

	if(ep_num){
		switch(dpid){
			case TRU_DWC2_DPID_EO_D0_OR_EVEN: doepctl.bits.d0pid_even = 1; break;
			case TRU_DWC2_DPID_EO_D1_OR_ODD: doepctl.bits.d1pid_odd = 1; break;
			default:
		}
	}

	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val = doepctl.val;
}

/*
	Stop data transfer and deactivate an IN endpoint.
	Note: Endpoint 0 cannot be deactivated.
*/
void tru_dwc2_deactivate_epin(void *usb_base_addr, uint8_t ep_num){
	tru_dwc2_disable_epin_wait(usb_base_addr, ep_num, true);

	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep = 0;  // Deactivate the endpoint
	TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val &= ~(0x00000001 << ep_num);  // Disable the interrupt trigger flag for the endpoint

	// DIEPTSIZ[N] register - clear the endpoint transfer size and packet count
	if(ep_num == 0){
		TRU_DWC2_DIEPTSIZ0_REG(usb_base_addr)->bits.xfersize = 0;
		TRU_DWC2_DIEPTSIZ0_REG(usb_base_addr)->bits.pktcnt = 0;
	}else{
		TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->bits.xfersize = 0;
		TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->bits.pktcnt = 0;
	}
}

/*
	Stop data transfer and deactivate for all IN endpoints except for endpoint 0.
	Note: Endpoint 0 cannot be deactivated.
*/
void tru_dwc2_deactivate_epin_all(void *usb_base_addr, uint32_t num_dev_in_ep){
	for(uint8_t ep_num = 1; ep_num < num_dev_in_ep; ep_num++){
		tru_dwc2_disable_epin_wait(usb_base_addr, ep_num, true);

		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep = 0;  // Deactivate the endpoint
		TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val &= ~(0x00000001 << ep_num);  // Disable the interrupt trigger flag for the endpoint

		// DIEPTSIZ[N] register - clear the endpoint transfer size and packet count
		if(ep_num == 0){
			TRU_DWC2_DIEPTSIZ0_REG(usb_base_addr)->bits.xfersize = 0;
			TRU_DWC2_DIEPTSIZ0_REG(usb_base_addr)->bits.pktcnt = 0;
		}else{
			TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->bits.xfersize = 0;
			TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->bits.pktcnt = 0;
		}
	}
}

/*
	Stop data transfer and deactivate an OUT endpoint.
	Note: Endpoint 0 cannot be deactivated.
*/
void tru_dwc2_deactivate_epout(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dma_mode_t dma_mode){
	tru_dwc2_disable_epout_wait(usb_base_addr, ep_num, dma_mode, true);

	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep = 0;  // Deactivate the endpoint
	TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val &= ~(0x00010000 << ep_num);  // Disable the interrupt trigger flag for the endpoint

	// DOEPTSIZ[N] register - clear the endpoint transfer size and packet count
	if(ep_num == 0){
		TRU_DWC2_DOEPTSIZ0_REG(usb_base_addr)->bits.xfersize = 0;
		TRU_DWC2_DOEPTSIZ0_REG(usb_base_addr)->bits.pktcnt = 0;
	}else{
		TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->bits.xfersize = 0;
		TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->bits.pktcnt = 0;
	}
}

/*
	Stop data transfer and deactivate for all OUT endpoints except for endpoint 0.
	Note: Endpoint 0 cannot be deactivated.
*/
void tru_dwc2_deactivate_epout_all(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode, uint32_t num_dev_out_ep){
	tru_dwc2_disable_epout_wait_all(usb_base_addr, dma_mode, true, num_dev_out_ep);

	for(uint8_t ep_num = 1; ep_num < num_dev_out_ep; ep_num++){
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep = 0;  // Deactivate the endpoint
		TRU_DWC2_DAINTMSK_REG(usb_base_addr)->val &= ~(0x00010000 << ep_num);  // Disable the interrupt trigger flag for the endpoint

		// DOEPTSIZ[N] register - clear the endpoint transfer size and packet count
		if(ep_num == 0){
			TRU_DWC2_DOEPTSIZ0_REG(usb_base_addr)->bits.xfersize = 0;
			TRU_DWC2_DOEPTSIZ0_REG(usb_base_addr)->bits.pktcnt = 0;
		}else{
			TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->bits.xfersize = 0;
			TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->bits.pktcnt = 0;
		}
	}
}

/*
	Set stall on the endpoint.
*/
void tru_dwc2_stall_epin(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.stall = 1;
}

/*
	Clear stall on the endpoint.
*/
void tru_dwc2_stall_clr_epin(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dpid_eo_t dpid){
	tru_dwc2_diepctl_t diepctl;

	if(ep_num){
		switch(dpid){
			case TRU_DWC2_DPID_EO_D0_OR_EVEN:
				diepctl.val = TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val;
				diepctl.bits.stall = 0;
				diepctl.bits.d0pid_even = 1;
				TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val = diepctl.val;
				break;
			case TRU_DWC2_DPID_EO_D1_OR_ODD:
				diepctl.val = TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val;
				diepctl.bits.stall = 0;
				diepctl.bits.d1pid_odd = 1;
				TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val = diepctl.val;
				break;
			default:
				TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.stall = 0;
		}
	}else{
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.stall = 0;
	}
}

/*
	Set stall on the endpoint.
*/
void tru_dwc2_stall_epout(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.stall = 1;
}

/*
	Clear stall on the endpoint.
*/
void tru_dwc2_stall_clr_epout(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dpid_eo_t dpid){
	tru_dwc2_doepctl_t doepctl;

	if(ep_num){
		switch(dpid){
			case TRU_DWC2_DPID_EO_D0_OR_EVEN:
				doepctl.val = TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val;
				doepctl.bits.stall = 0;
				doepctl.bits.d0pid_even = 1;
				TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val = doepctl.val;
				break;
			case TRU_DWC2_DPID_EO_D1_OR_ODD:
				doepctl.val = TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val;
				doepctl.bits.stall = 0;
				doepctl.bits.d1pid_odd = 1;
				TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val = doepctl.val;
				break;
				break;
			default: TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.stall = 0;
		}
	}else{
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.stall = 0;
	}
}

/*
	Returns endpoint 0 MPS register code for the given max packet size.
*/
uint8_t tru_dwc2_ep0_mps_to_regcode(uint8_t mps){
	// Convert max packet size to USB controller register code
	switch(mps){
		case 8: return 3; break;
		case 16: return 2; break;
		case 32: return 1; break;
		default: return 0;
	}
}

uint8_t tru_dwc2_ep0_regcode_to_mps(uint8_t code){
	// Convert USB controller register code to max packet size
	switch(code){
		case 1: return 32; break;
		case 2: return 16; break;
		case 3: return 8; break;
		default: return 64;
	}
}

/*
	Calculates packet count of the transfer size (rounding up).
*/
uint16_t tru_dwc2_ep_mps_xfer_to_pkt_cnt(uint16_t mps, uint32_t xfer_size){
	// Calculate packet count (rounding up)
	// No error checking please ensure request length does not give more than max packet count = 1
	if(xfer_size == 0){
		return 1;
	}else if(xfer_size % mps){
		return xfer_size / mps + 1;
	}else{
		return xfer_size / mps;
	}
}

uint16_t tru_dwc2_get_txfifo_freespace_words(void *usb_base_addr, tru_dwc2_txfifo_mode_t txfifo_mode, uint8_t ep_num){
	if(txfifo_mode == TRU_DWC2_TXFIFO_SHARED){
		return TRU_DWC2_GNPTXSTS_REG(usb_base_addr)->bits.nptxfspcavail;
	}else{
		return TRU_DWC2_DTXFSTS_REG(usb_base_addr, ep_num)->bits.ineptxfspcavail;
	}
}

/*
	Returns the max transfer size supported by the IN EP size register for a specified endpoint and transfer size.

	When a transfer request is too big we must split it and transfer in smaller sizes.  The controller imposes
	constraints on the transfer length:
	- register size field limit
	- register packet field count limit

	Constraints that apply to FIFO slave mode and particular versions of the DWC2 OTG USB IP:
	- ensure the TX FIFO free space is always zero or a multiple of max packet size
*/
uint32_t tru_dwc2_get_max_xfer_epin(void *usb_base_addr, uint8_t ep_num, uint32_t xfer_size, uint32_t ep_xfer_limit, uint16_t mps, tru_dwc2_dma_mode_t dma_mode, tru_dwc2_txfifo_mode_t txfifo_mode, tru_dwc2_hwcfg_t *hwcfg){
	if(dma_mode == TRU_DWC2_DMA_MODE_SG) return xfer_size;  // This is handled by the SGDMA descriptor function instead

	uint32_t max_xfer_size = (ep_xfer_limit == 0) ? xfer_size : (xfer_size > ep_xfer_limit) ? ep_xfer_limit : xfer_size;
	uint32_t max_xfer_limit_by_reg_size = (ep_num == 0) ? TRU_DWC2_DIEPTSIZ0_XFERSIZE_SET_MSK / mps * mps : hwcfg->max_transfer_size / mps * mps;  // Multiple of max packet size
	uint32_t max_xfer_limit_by_reg_mps = (ep_num == 0) ? TRU_DWC2_DEV_EP_EP0_MAX_PACKET_COUNT * mps : hwcfg->max_packet_count * mps;  // Multiple of max packet size
	uint32_t max_xfer_limit = (max_xfer_limit_by_reg_size < max_xfer_limit_by_reg_mps) ? max_xfer_limit_by_reg_size : max_xfer_limit_by_reg_mps;  // Select the smaller size

	switch(dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			// Limit to register size and packet count
			if(max_xfer_size > max_xfer_limit) max_xfer_size = max_xfer_limit;

			// Limit the write size to the TXFIFO free space
			uint32_t txfifo_freespace = 4 * tru_dwc2_get_txfifo_freespace_words(usb_base_addr, txfifo_mode, ep_num);
			if(txfifo_freespace == 0) return 0;
			if(max_xfer_size > txfifo_freespace) max_xfer_size = txfifo_freespace;

			// In slave mode (manual FIFO transfer mode) there is a hardware limitation that appears to be related to in internal
			// fifo queue counter. Only a maximum of n packets can be pushed into the TXFIFO at once (i.e. together with DIEPTSIZx).
#ifdef TRU_DWC2_DEV_MAX_TXFIFO_PACKETS
			if(max_xfer_size > TRU_DWC2_DEV_MAX_TXFIFO_PACKETS * mps) max_xfer_size = TRU_DWC2_DEV_MAX_TXFIFO_PACKETS * mps;
#endif

			// Perhaps this applies to all Cortex-A9?  I have only confirmed it on the Altera Cyclone V SoC
#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC
#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
			// In slave FIFO mode and when L2 cache is enabled we cannot write to the TXFIFO with data larger than 4K-1, else
			// the interrupts "TX FIFO empty" and "transfer complete" will not trigger!
			// A work around is to limit the TXFIFO write size so that it is less than 4096
			if(tru_l2_is_enabled()){
				if(max_xfer_size >= TRU_DWC2_DEV_DFIFO_L2_WRITE_BOUNDARY){
					max_xfer_size = (TRU_DWC2_DEV_DFIFO_L2_WRITE_BOUNDARY - 1);
				}
			}
#endif
#endif

			break;
		case TRU_DWC2_DMA_MODE_SB:
			// Limit to register size and packet count
			if(max_xfer_size > max_xfer_limit) max_xfer_size = max_xfer_limit;

			break;
		default:
	}

	return max_xfer_size;
}

/*
	Returns the max transfer size that is supported by the OUT EP size register for a specified endpoint and transfer size.

	When a transfer request is too big we must split it and transfer in smaller sizes.  The controller imposes
	constraints on the transfer length:
	- register size field limit
	- register packet field count limit
*/
uint32_t tru_dwc2_get_max_xfer_epout(void *usb_base_addr, uint8_t ep_num, uint32_t xfer_size, uint32_t ep_xfer_limit, uint16_t mps, tru_dwc2_dma_mode_t dma_mode, tru_dwc2_hwcfg_t *hwcfg){
	if(dma_mode == TRU_DWC2_DMA_MODE_SG) return xfer_size;  // This is handled by the SGDMA descriptor function instead

	uint32_t max_xfer_size = (ep_xfer_limit == 0) ? xfer_size : (xfer_size > ep_xfer_limit) ? ep_xfer_limit : xfer_size;
	uint32_t max_xfer_limit_by_reg_size = (ep_num == 0) ? TRU_DWC2_DOEPTSIZ0_XFERSIZE_SET_MSK / mps * mps : hwcfg->max_transfer_size / mps * mps;  // Multiple of max packet size
	uint32_t max_xfer_limit_by_reg_mps = (ep_num == 0) ? TRU_DWC2_DEV_EP_EP0_MAX_PACKET_COUNT * mps : hwcfg->max_packet_count * mps;  // Multiple of max packet size
	uint32_t max_xfer_limit = (max_xfer_limit_by_reg_size < max_xfer_limit_by_reg_mps) ? max_xfer_limit_by_reg_size : max_xfer_limit_by_reg_mps;  // Select the smaller size

	switch(dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			// Limit to register size and packet count
			if(max_xfer_size > max_xfer_limit) max_xfer_size = max_xfer_limit;

			break;
		case TRU_DWC2_DMA_MODE_SB:
			// Limit to register size and packet count
			if(max_xfer_size > max_xfer_limit) max_xfer_size = max_xfer_limit;

			break;
		default:
	}

	return max_xfer_size;
}

void tru_dwc2_update_oddeven_epin(void *usb_base_addr, uint8_t ep_num, uint8_t interval, uint32_t *frame_target){
	 tru_dwc2_update_frame_target(usb_base_addr, interval, TRU_DWC2_DSTS_SOFFN_LIMIT, frame_target);

	// Flip the odd/even flag to attempt a transfer on the next frame
	// Note: the EP remain enabled so no need to re-enable
	// EONUM: 0 = Even, 1 = Odd
	// DWC2 controller looks at LSB of the upcoming next frame number to decide if it should accept or reject a receive transfer
	// For Isochronous we need to set the even or odd number that corresponds to the upcoming frame number LSB that we want to transfer data on
	// Is target frame odd?
#if defined(TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN) && TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN == 1
	if(*frame_target & 1){
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.d0pid_even = 1;  // Set transfer on the upcoming even frame number
	}else{
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.d1pid_odd = 1;  // Set transfer on the upcoming odd frame number
	}
#else
	if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.dpid_eonum){
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.d0pid_even = 1;  // Set transfer on the upcoming even frame number
	}else{
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.d1pid_odd = 1;  // Set transfer on the upcoming odd frame number
	}
#endif
}

void tru_dwc2_update_oddeven_epout(void *usb_base_addr, uint8_t ep_num, uint8_t interval, uint32_t *frame_target){
	tru_dwc2_update_frame_target(usb_base_addr, interval, TRU_DWC2_DSTS_SOFFN_LIMIT, frame_target);

	// Flip the odd/even flag to attempt a transfer on the next frame
	// Note: the EP remain enabled so no need to re-enable
	// EONUM: 0 = Even, 1 = Odd
	// DWC2 controller looks at LSB of the upcoming next frame number to decide if it should accept or reject a receive transfer
	// For Isochronous we need to set the even or odd number that corresponds to the upcoming frame number LSB that we want to transfer data on
	// Is target frame odd?
#if defined(TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN) && TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN == 1
	if(*frame_target & 1){
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.d0pid_even = 1;  // Set transfer on the upcoming even frame number
	}else{
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.d1pid_odd = 1;  // Set transfer on the upcoming odd frame number
	}
#else
	if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.dpid_eonum){
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.d0pid_even = 1;  // Set transfer on the upcoming even frame number
	}else{
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.d1pid_odd = 1;  // Set transfer on the upcoming odd frame number
	}
#endif
}

void tru_usb_dwc2_update_frame_target_sgdma(tru_usb_lxfer_t *lxfer, void *usb_base_addr, uint32_t interval, uint32_t *frame_target){
	tru_dwc2_update_frame_target(usb_base_addr, interval, TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_LIMIT, frame_target);

	bool is_last;
	for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
		if(!tru_dwc2_sgdma_desc_is_dma_done(lxfer->chunk.sgdma.dma_descs + i)){
			tru_dwc2_sgdma_setup_desc_frame_target_epout(&lxfer->chunk.sgdma, i, *frame_target);
			//*frame_target += interval;
		}
		is_last = (i + 1 == lxfer->chunk.num_registered) ? true : ((lxfer->chunk.buffers[i + 1].xfer_size == 0) ? true : false);
		if(is_last) break;
	}
}

/*
	Enable a data transfer on the IN endpoint (transmit endpoint).

	Notes
	=====

	EP0 registers are slightly different from EP1 to EP15 registers.
	EP1 to EP15 registers are all the same.
	The mps and eptype fields are assumed to be set already so the existing
	values will be used.
*/
void tru_dwc2_enable_epin(
	void *usb_base_addr,
	uint8_t ep_num,
	uint16_t mps,
	uint8_t xfer_type,
	void *xfer_buf,
	uint32_t xfer_size,
	uint32_t interval,
	uint32_t *frame_target,
	tru_usb_lxfer_t *lxfer,
	uint8_t speed,
	tru_dwc2_dma_mode_t dma_mode,
	tru_dwc2_stall_t stall,
	tru_dwc2_nak_t nak,
	tru_dwc2_dpid_eo_t dpid
){
	tru_dwc2_diepctl_t diepctl = { .val = TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val };
	tru_dwc2_dieptsiz_t dieptsiz = { .val = TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->val };

	diepctl.bits.epena = 1;

	switch(nak){
		case TRU_DWC2_NAK_CLEAR: diepctl.bits.cnak = 1; break;
		case TRU_DWC2_NAK_SET: diepctl.bits.snak = 1; break;
		default:
	}

	switch(stall){
		case TRU_DWC2_STALL_CLEAR: diepctl.bits.stall = 0; break;
		case TRU_DWC2_STALL_SET: diepctl.bits.stall = 1; break;
		default:
	}

	// DMA mode
	switch(dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			if(ep_num == 0){
				tru_dwc2_dieptsiz0_t *dieptsiz0 = (tru_dwc2_dieptsiz0_t *)&dieptsiz;
				dieptsiz0->bits.xfersize = xfer_size;
				dieptsiz0->bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, dieptsiz0->bits.xfersize);  // Calculate packet count (rounding up)
			}else{
				dieptsiz.bits.xfersize = xfer_size;
				dieptsiz.bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, dieptsiz.bits.xfersize);  // Calculate packet count (rounding up)
				dieptsiz.bits.mc = 1;  // For high bandwidth periodic transfer set this to 3
			}
			TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->val = dieptsiz.val;
			break;
		case TRU_DWC2_DMA_MODE_SB:
			TRU_DWC2_DIEPDMA_REG(usb_base_addr, ep_num)->buf = (uintptr_t)xfer_buf;  // SB mode: address of DMA buffer
			if(ep_num == 0){
				tru_dwc2_dieptsiz0_t *dieptsiz0 = (tru_dwc2_dieptsiz0_t *)&dieptsiz;
				dieptsiz0->bits.xfersize = xfer_size;
				dieptsiz0->bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, dieptsiz0->bits.xfersize);  // Calculate packet count (rounding up)
			}else{
				dieptsiz.bits.xfersize = xfer_size;
				dieptsiz.bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, dieptsiz.bits.xfersize);  // Calculate packet count (rounding up)
				dieptsiz.bits.mc = 1;  // For high bandwidth periodic transfer set this to 3
			}
			TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->val = dieptsiz.val;
			break;
		case TRU_DWC2_DMA_MODE_SG:
			TRU_DWC2_DIEPDMA_REG(usb_base_addr, ep_num)->buf = (uintptr_t)xfer_buf;  // SG mode: address of DMA descriptor list
			break;
		default:
	}

	// Note, DATA0/1 pid for bulk mode is automatically toggled by the controller so no need to do anything for that transfer type.
	// For isochronous mode (except for DMA scatter gather mode) the even/odd frame must be set by software
	if(ep_num){
		switch(dpid){
			case TRU_DWC2_DPID_EO_D0_OR_EVEN: diepctl.bits.d0pid_even = 1; break;  // Set DATA0 for bulk and even frame for isochronous
			case TRU_DWC2_DPID_EO_D1_OR_ODD: diepctl.bits.d1pid_odd = 1; break;  // Set DATA1 for bulk and odd frame for isochronous
			case TRU_DWC2_DPID_EO_AUTO:
				if(xfer_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
					// Set data PID number for Isochronous data flow (not for DMA scatter gather mode)
					switch(dma_mode){
						case TRU_DWC2_DMA_MODE_FF:
						case TRU_DWC2_DMA_MODE_SB:
							tru_dwc2_update_frame_target(usb_base_addr, interval, TRU_DWC2_DSTS_SOFFN_LIMIT, frame_target);

							// DWC2 controller looks at LSB of the upcoming next frame number to decide if it should transfer
							// For Isochronous we need to set the even or odd number that corresponds to the upcoming frame number LSB that we want to transfer data on
							// Flip the odd/even flag to attempt a transfer on the next frame
							// EONUM: 0 = Even, 1 = Odd
#if defined(TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN) && TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN == 1
							if(*frame_target & 1){  // Is target frame odd?
								diepctl.bits.d1pid_odd = 1;  // Set transfer on odd frame number
							}else{
								diepctl.bits.d0pid_even = 1;  // Set transfer on even frame number
							}
#else
							if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.dpid_eonum){  // Odd?
								diepctl.bits.d0pid_even = 1;  // Set transfer on even frame number
							}else{
								diepctl.bits.d1pid_odd = 1;  // Set transfer on odd frame number
							}
#endif
							break;
						case TRU_DWC2_DMA_MODE_SG:
							tru_usb_dwc2_update_frame_target_sgdma(lxfer, usb_base_addr, interval, frame_target);
							break;
						default:
					}
				}
				break;
			default:
		}
	}

	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val = diepctl.val;
}

/*
	Enable a data transfer on the OUT endpoint (receive endpoint).

	Notes
	=====

	EP0 registers are slightly different from EP1 to EP15 registers.
	EP1 to EP15 registers are all the same.
	The mps and eptype fields are assumed to be set already so the existing
	values will be used.

	Hardware bug fix for starting a transfer on OUT EP0
	===================================================

	Note, for the SETUP stage, i.e. when we want to receive a setup, the NAK mode must be enabled.  This ensures
	that only the setup request is pushed into the RXFIFO without any data from the DATA OUT stage, else the controller's
	packet slicer will eventually enter into a bad state:
	- DOEPTSIZ0 register xfersize field underflows
	- may generate wrong interrupt for the received setup as a transfer complete trigger instead of a setup trigger
	- in slave FIFO mode it generates 2x SETUP RECEIVED status then a SETUP DONE status
	- no longer generates interrupt for the transfer complete and setup trigger
	This doesn't happen all the time but seems to be random or perhaps related to how fast the STATUS OUT data comes in.

	If we repeatedly receive (host to device) setup requests with DATA OUT e.g. CDC SET_LINE_CODING then
	it will hang after a while

	Basically, for the SETUP stage we must not clear NAK mode, and our choices are:
	- set the snak bit, or
	- leave as is, because after a complete transfer the controller automatically goes into NAK mode anyway

	A device cannot STALL or NAK the setup packet, it can only STALL or NAK the DATA OUT and STATUS OUT stage.

	Note:
	- for the STATUS OUT stage the NAK mode can be cleared or set
	- for the DATA OUT stage the NAK mode must be cleared

	Starting a transfer on EP0
	==========================

	Setup stage
	===========

	FIFO slave mode:
	A transfer starts by these fields:
	- DOEPTSIZ0 xfersize > 0
	(The DOEPCTL0 epena field is ignored)

	Standard DMA mode:
	A transfer starts by these fields:
	- DOEPCTL0 epena = 1

 	SG DMA mode:
	A transfer starts by these fields:
	- DOEPCTL0 epena = 1

	Data & status stage
	===================

	FIFO slave mode:
	A transfer starts by these fields:
	- DOEPCTL0 cnak = 1 for DATA OUT

	Standard DMA mode:
	A transfer starts by these fields:
	- DOEPCTL0 epena = 1
	- DOEPCTL0 cnak = 1 for DATA OUT

 	SG DMA mode:
	A transfer starts by these fields:
	- DOEPCTL0 epena = 1
	- DOEPCTL0 cnak = 1 for DATA OUT
*/
void tru_dwc2_enable_epout(
	void *usb_base_addr,
	uint8_t ep_num,
	uint16_t mps,
	uint8_t xfer_type,
	void *xfer_buf,
	uint32_t xfer_size,
	uint32_t interval,
	uint32_t *frame_target,
	tru_usb_lxfer_t *lxfer,
	uint8_t speed,
	tru_dwc2_dma_mode_t dma_mode,
	tru_dwc2_stall_t stall,
	tru_dwc2_nak_t nak,
	tru_dwc2_dpid_eo_t dpid
){
	tru_dwc2_doepctl_t doepctl = { .val = TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val };
	tru_dwc2_doeptsiz_t doeptsiz = { .val = TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->val };

	doepctl.bits.epena = 1;

	switch(nak){
		case TRU_DWC2_NAK_CLEAR: doepctl.bits.cnak = 1; break;
		case TRU_DWC2_NAK_SET: doepctl.bits.snak = 1; break;
		default:
	}

	switch(stall){
		case TRU_DWC2_STALL_CLEAR: doepctl.bits.stall = 0; break;
		case TRU_DWC2_STALL_SET: doepctl.bits.stall = 1; break;
		default:
	}

	// DMA mode
	switch(dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			if(ep_num == 0){
				tru_dwc2_doeptsiz0_t *doeptsiz0 = (tru_dwc2_doeptsiz0_t *)&doeptsiz;
				doeptsiz0->bits.xfersize = xfer_size;
				doeptsiz0->bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, doeptsiz0->bits.xfersize);  // Calculate packet count (rounding up)
				doeptsiz0->bits.supcnt = 1;
			}else{
				doeptsiz.bits.xfersize = xfer_size;
				doeptsiz.bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, doeptsiz.bits.xfersize);  // Calculate packet count (rounding up)
			}
			TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->val = doeptsiz.val;
			break;
		case TRU_DWC2_DMA_MODE_SB:
			TRU_DWC2_DOEPDMA_REG(usb_base_addr, ep_num)->buf = (uintptr_t)xfer_buf;  // SB mode: address of DMA buffer
			if(ep_num == 0){
				tru_dwc2_doeptsiz0_t *doeptsiz0 = (tru_dwc2_doeptsiz0_t *)&doeptsiz;
				doeptsiz0->bits.xfersize = xfer_size;
				doeptsiz0->bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, doeptsiz0->bits.xfersize);  // Calculate packet count (rounding up)
				doeptsiz0->bits.supcnt = 1;
			}else{
				doeptsiz.bits.xfersize = xfer_size;
				doeptsiz.bits.pktcnt = tru_dwc2_ep_mps_xfer_to_pkt_cnt(mps, doeptsiz.bits.xfersize);  // Calculate packet count (rounding up)
			}
			TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->val = doeptsiz.val;
			break;
		case TRU_DWC2_DMA_MODE_SG:
			TRU_DWC2_DOEPDMA_REG(usb_base_addr, ep_num)->buf = (uintptr_t)xfer_buf;  // SG mode: address of DMA descriptor list
			break;
		default:
	}

	// Note, DATA0/1 pid for bulk mode is automatically toggled by the controller so no need to do anything for that transfer type.
	// For isochronous mode (except for DMA scatter gather mode) the even/odd frame must be set by software
	if(ep_num){
		switch(dpid){
			case TRU_DWC2_DPID_EO_D0_OR_EVEN: doepctl.bits.d0pid_even = 1; break;  // Set DATA0 for bulk and even frame for isochronous
			case TRU_DWC2_DPID_EO_D1_OR_ODD: doepctl.bits.d1pid_odd = 1; break;  // Set DATA1 for bulk and odd frame for isochronous
			case TRU_DWC2_DPID_EO_AUTO:
				if(xfer_type == TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS){
					switch(dma_mode){
						case TRU_DWC2_DMA_MODE_FF:
						case TRU_DWC2_DMA_MODE_SB:
							// To receive data in the next frame for an Isochronous OUT endpoint
							// we need to enable EP after the EOPF (OTG_GINTSTS) trigger and before the SOF (OTG_GINTSTS) trigger.
							// EOPF = End of Periodic Frame transfer, SOF = Start Of Frame, which happens every 1 millisecond
							//while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.eopf == 0);  // Wait for EOPF

							tru_dwc2_update_frame_target(usb_base_addr, interval, TRU_DWC2_DSTS_SOFFN_LIMIT, frame_target);

							// DWC2 controller looks at LSB of the upcoming next frame number to decide if it should transfer
							// For Isochronous we need to set the even or odd number that corresponds to the upcoming frame number LSB that we want to transfer data on
							// Flip the odd/even flag to attempt a transfer on the next frame
							// EONUM: 0 = Even, 1 = Odd
#if defined(TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN) && TRU_DWC2_UPDATE_DXEPCTL_ODDEVEN_BY_SOFFN == 1
							if(*frame_target & 1){  // Is target frame odd?
								doepctl.bits.d0pid_even = 1;  // Set transfer on even frame number
							}else{
								doepctl.bits.d1pid_odd = 1;  // Set transfer on odd frame number
							}
#else
							if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.dpid_eonum){  // Odd?
								doepctl.bits.d0pid_even = 1;  // Set transfer on even frame number
							}else{
								doepctl.bits.d1pid_odd = 1;  // Set transfer on odd frame number
							}
#endif
							break;
						case TRU_DWC2_DMA_MODE_SG:
							tru_usb_dwc2_update_frame_target_sgdma(lxfer, usb_base_addr, interval, frame_target);
							break;
						default:
					}
				}
				break;
			default:
		}
	}

	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val = doepctl.val;
}

void tru_dwc2_snak_epin(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;
}

void tru_dwc2_snak_epin_wait(void *usb_base_addr, uint8_t ep_num){
	//TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.inepnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;  // Set NAK mode
	while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode
	// An alternative way to wait for NAK mode to become active
	//while(TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num)->bits.inepnakeff == 0);  // Wait for controller to set NAK mode
	//TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.inepnakeffmsk = 0;  // Re-enable interrupt generation on this trigger
}

void tru_dwc2_snak_epin_wait_all(void *usb_base_addr, uint32_t num_dev_in_ep){
	for(uint8_t ep_num = 0; ep_num < num_dev_in_ep; ep_num++){
		//TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.inepnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;  // Set NAK mode
		while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode
		// An alternative way to wait for NAK mode to become active
		//while(TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num)->bits.inepnakeff == 0);  // Wait for controller to set NAK mode
		//TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.inepnakeffmsk = 0;  // Re-enable interrupt generation on this trigger
	}
}

void tru_dwc2_cnak_epin(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.cnak = 1;
}

void tru_dwc2_cnak_epin_wait(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 0;  // Clear NAK mode
	while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 1);  // Wait for controller to clear NAK mode
}

void tru_dwc2_cnak_epin_wait_all(void *usb_base_addr, uint32_t num_dev_in_ep){
	for(uint8_t ep_num = 0; ep_num < num_dev_in_ep; ep_num++){
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 0;  // Clear NAK mode
		while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 1);  // Wait for controller to clear NAK mode
	}
}

void tru_dwc2_snak_epout(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;
}

void tru_dwc2_snak_epout_wait(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;  // Set NAK mode
	while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode
}

void tru_dwc2_snak_epout_wait_all(void *usb_base_addr, uint32_t num_dev_out_ep){
	for(uint8_t ep_num = 0; ep_num < num_dev_out_ep; ep_num++){
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;  // Set NAK mode
		while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode
	}
}

void tru_dwc2_cnak_epout(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.cnak = 1;
}

void tru_dwc2_cnak_epout_wait(void *usb_base_addr, uint8_t ep_num){
	TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 0;  // Clear NAK mode
	while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 1);  // Wait for controller to clear NAK mode
}

void tru_dwc2_cnak_epout_wait_all(void *usb_base_addr, uint32_t num_dev_out_ep){
	for(uint8_t ep_num = 0; ep_num < num_dev_out_ep; ep_num++){
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 0;  // Clear NAK mode
		while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 1);  // Wait for controller to clear NAK mode
	}
}

void tru_dwc2_set_globalinnak_np(void *usb_base_addr){
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgnpinnak = 1;  // Start global IN NAK for non-periodic endpoints
}

// Note, the ginnakeff interrupt trigger bit is automatically cleared by the controller after global IN NAK is stopped
void tru_dwc2_set_globalinnak_np_wait(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode){
	// The non-periodic global IN NAK is not active?
	//if(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.gnpinnaksts == 0){
	if(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.ginnakeff == 0){
		// Turn on non-periodic global IN NAK and wait for trigger to ensure NAKing has started
		TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.ginnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
		TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgnpinnak = 1;  // Set non-periodic global IN NAK
		//while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.gnpinnaksts == 0);  // Wait for controller to start non-periodic global IN NAK mode
		while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.ginnakeff == 0);  // Wait for controller to start non-periodic global IN NAK mode
		TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.ginnakeffmsk = 1;  // Re-enable interrupt generation on this trigger
	}
}

void tru_dwc2_clr_globalinnak_np(void *usb_base_addr){
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.cgnpinnak = 1;  // Stop global IN NAK for non-periodic endpoints
}

void tru_dwc2_clr_globalinnak_np_wait(void *usb_base_addr){
	// Turn off non-periodic global IN NAK and wait for trigger to ensure NAKing has stopped
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.cgnpinnak = 1;  // Stop non-periodic global IN NAK
	//while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.gnpinnaksts == 1);  // Wait for controller to stop non-periodic global IN NAK mode
	while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.ginnakeff == 1);  // Wait for controller to stop non-periodic global IN NAK mode
}

void tru_dwc2_set_globaloutnak(void *usb_base_addr){
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgoutnak = 1;  // Start global OUT NAK for all endpoints
}

// Note, the goutnakeff interrupt trigger bit is automatically cleared by the controller after global OUT NAK is stopped
void tru_dwc2_set_globaloutnak_wait(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode){
	tru_dwc2_grxstsp_t grxsts __attribute__((unused));

	// The global OUT NAK is not active?
	if(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0){
		if(dma_mode == TRU_DWC2_DMA_MODE_FF){
			// Note: In slave FIFO mode, after setting the global OUT NAK, a 32-bit RX FIFO status (global OUT NAK status) will be pushed into
			// the RX FIFO by the controller.  The global OUT NAK interrupt will only trigger after we pop out the status from the RX FIFO.

			// Turn on global OUT NAK and wait for trigger to ensure NAKing has started
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.rxflvlmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
			TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgoutnak = 1;  // Start global OUT NAK
			do{
				if(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.rxflvl == 1){
					grxsts.val = TRU_DWC2_GRXSTSP_REG(usb_base_addr)->val;  // Read and pop the receive FIFO status - we want to pop the global OUT NAK
				}
			}while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0);  // Wait for controller to start global OUT NAK mode
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.rxflvlmsk = 1;  // Re-enable interrupt generation on this trigger
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 1;  // Re-enable interrupt generation on this trigger
		}else{
			// Turn on global OUT NAK and wait for trigger to ensure NAKing has started
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
			TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgoutnak = 1;  // Start global OUT NAK
			while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0);  // Wait for controller to start global OUT NAK mode
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 1;  // Re-enable interrupt generation on this trigger
		}
	}
}

void tru_dwc2_clr_globaloutnak(void *usb_base_addr){
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.cgoutnak = 1;  // Stop global OUT NAK
}

void tru_dwc2_clr_globaloutnak_wait(void *usb_base_addr){
	// Turn off global OUT NAK and wait for trigger to ensure NAKing has stopped
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.cgoutnak = 1;  // Stop global OUT NAK
	while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 1);  // Wait for controller to stop global OUT NAK mode
}

/*
	Official method:

	Stop data transfer and disable the IN endpoint.
	If the endpoint is enabled (aka transfer mode, primed for data transfer) then we stop the current transfer before disabling it.

	Waiting for interrupt completion
	Since this function is used by our interrupt handler we cannot make use of interrupt generation because new interrupts will only
	happen after we have exit the interrupt handler, i.e. after this function is executed, which will be too late!
	We will have to use polling method to check the necessary interrupt triggers.

	Returns true if an endpoint was primed for a transfer.
*/
bool tru_dwc2_disable_epin_wait(void *usb_base_addr, uint8_t ep_num, bool flush_fifo){
	//if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
	if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
		// Set NAK mode first before we disable.  NAK mode will pause the transfer or transfer completes just before we set NAK, either way it will be in a safe state
		//TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.inepnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;  // Set NAK mode
		while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode
		// An alternative way to wait for NAK mode to become active
		//while(TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num)->bits.inepnakeff == 0);  // Wait for controller to set NAK mode
		//TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.inepnakeffmsk = 0;  // Re-enable interrupt generation on this trigger

		// This recheck is required! The DWC2 (USB DesignWare Controller) says that EPDIS bit must only be set if EPENA is set (i.e. primed).
		// From my experiments I've found the reason is because EPDIS is sticky, if the EPENA is not set this bit will apply on the next prime!
		// Also the EPDIS bit cannot be manually cleared, i.e. set back to 0
		// Recheck whether the endpoint is still primed?
		if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
			if(ep_num == 0){
				TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epdis = 1;  // Disable the endpoint, i.e. stop the primed transfer

				// According to the tech ref manual, EP0 doesn't support the endpoint disabled interrupt trigger
				// This is an alternative way to wait for the endpoint disabled to complete
				while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epdis == 1);  // Wait for endpoint disabled to clear
			}else{
				TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.epdisbldmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
				TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epdis = 1;  // Disable the endpoint, i.e. stop the primed transfer

				// Wait for endpoint disabled interrupt trigger
				while(TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num)->bits.epdisbld == 0);  // Wait for EP disabled interrupt to be flagged
				TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num)->bits.epdisbld = 1;  // Reset the trigger
				TRU_DWC2_DIEPMSK_REG(usb_base_addr)->bits.epdisbldmsk = 1;  // Re-enable interrupt generation on this trigger
			}

			if(flush_fifo) tru_dwc2_flush_txfifo_wait(usb_base_addr, ep_num);

			return true;
		}
	}
	return false;
}

/*
	Stop data transfer and disable all IN endpoints.
	If the endpoint is enabled (aka transfer mode, primed for data transfer) then we stop the current transfer before disabling it.

	Waiting for interrupt completion
	Since this function is used by our interrupt handler we cannot make use of interrupt generation because new interrupts will only
	happen after we have exit the interrupt handler, i.e. after this function is executed, which will be too late!
	We will have to use polling method to check the necessary interrupt triggers.

	Returns true if an endpoint was primed for a transfer.
*/
bool tru_dwc2_disable_epin_wait_all(void *usb_base_addr, bool flush_fifo, uint32_t num_dev_in_ep){
	bool ep_enabled_was_cancelled = false;

	for(uint8_t ep_num = 0; ep_num < num_dev_in_ep; ep_num++){
		ep_enabled_was_cancelled = ep_enabled_was_cancelled || tru_dwc2_disable_epin_wait(usb_base_addr, ep_num, false);
	}
	if(ep_enabled_was_cancelled && flush_fifo) tru_dwc2_flush_txfifo_wait(usb_base_addr, TRU_DWC2_GRSTCTL_TXFNUM_ALL);

	return ep_enabled_was_cancelled;
}

/*
	Non-official method:

	Instead of using the EPDIS bit we use the EPENA bit to disable the endpoint.
*/
bool tru_dwc2_disable_epin_forced(void *usb_base_addr, uint8_t ep_num, bool flush_fifo){
	bool ep_enabled_was_cancelled = false;

	//if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
	if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
		// Set NAK mode first before we disable.  NAK mode will pause the transfer or transfer completes just before we set NAK, either way it will be in a safe state
		TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;;  // Set NAK mode
		while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode

		// Recheck to see whether the endpoint is still enabled
		if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
			TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena = 0;  // Force disable the endpoint
			ep_enabled_was_cancelled = true;
		}
	}

	if(ep_enabled_was_cancelled && flush_fifo) tru_dwc2_flush_txfifo_wait(usb_base_addr, ep_num);

	return ep_enabled_was_cancelled;
}

/*
	Non-official method:

	Instead of using the EPDIS bit we use the EPENA bit to disable the endpoint.
*/
bool tru_dwc2_disable_epin_forced_all(void *usb_base_addr, bool flush_fifo, uint32_t num_dev_in_ep){
	bool ep_enabled_was_cancelled = false;

	for(uint8_t ep_num = 0; ep_num < num_dev_in_ep; ep_num++){
		//if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
		if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
			// Set NAK mode first before we disable.  NAK mode will pause the transfer or transfer completes just before we set NAK, either way it will be in a safe state
			TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;;  // Set NAK mode
			while(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode

			// Recheck to see whether the endpoint is still enabled
			if(TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
				TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena = 0;  // Force disable the endpoint
				ep_enabled_was_cancelled = true;
			}
		}
	}

	if(ep_enabled_was_cancelled && flush_fifo) tru_dwc2_flush_txfifo_all_wait(usb_base_addr);

	return ep_enabled_was_cancelled;
}

/*
	Official method:

	Stop data transfer and disable the OUT endpoint.
	If the endpoint is enabled (aka transfer mode, primed for data transfer) then we stop the current transfer before disabling it.

	Warning
	The EPDIS bit on the OUT endpoint requires the global OUT NAK to be enabled else it does not clear!

	Waiting for interrupt completion
	Since this function is used by our interrupt handler we cannot make use of interrupt generation because new interrupts will only
	happen after we have exit the interrupt handler, i.e. after this function is executed, which will be too late!
	We will have to use polling method to check the necessary interrupt triggers.

	Returns true if an endpoint was primed for a transfer.

	Note, the goutnakeff interrupt trigger bit is automatically cleared by the controller after global OUT NAK is stopped
*/
bool tru_dwc2_disable_epout_wait(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dma_mode_t dma_mode, bool flush_fifo){
	bool ep_enabled_was_cancelled = false;
	tru_dwc2_grxstsp_t grxsts __attribute__((unused));

	//if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
	if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
		// The global OUT NAK is not active?
		if(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0){
			if(dma_mode == TRU_DWC2_DMA_MODE_FF){
				// Note: In slave FIFO mode, after setting the global OUT NAK, a 32-bit RX FIFO status (global OUT NAK status) will be pushed into
				// the RX FIFO by the controller.  The global OUT NAK interrupt will only trigger after we pop out the status from the RX FIFO.

				// Turn on global OUT NAK and wait for trigger to ensure NAKing has started
				TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
				TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.rxflvlmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
				TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgoutnak = 1;  // Start global OUT NAK
				do{
					if(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.rxflvl == 1){
						grxsts.val = TRU_DWC2_GRXSTSP_REG(usb_base_addr)->val;  // Read and pop the receive FIFO status - we want to pop the global OUT NAK
					}
				//}while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.goutnaksts == 0);  // Wait for controller to start global OUT NAK mode
				}while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0);  // Wait for controller to start global OUT NAK mode
				TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.rxflvlmsk = 1;  // Re-enable interrupt generation on this trigger
			}else{
				// Turn on global OUT NAK and wait for trigger to ensure NAKing has started
				TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
				TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgoutnak = 1;  // Start global OUT NAK
				//while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.goutnaksts == 0);  // Wait for controller to start global OUT NAK mode
				while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0);  // Wait for controller to start global OUT NAK mode
			}
		}

		// Set NAK mode first before we disable.  NAK mode will pause the transfer or transfer completes just before we set NAK, either way it will be in a safe state
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;;  // Set NAK mode
		while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode

		// This recheck is required! The DWC2 (USB DesignWare Controller) says that EPDIS bit must only be set if EPENA is set (i.e. primed).
		// From my experiments I've found the reason is because EPDIS is sticky, if the EPENA is not set this bit will apply on the next prime!
		// Also the EPDIS bit cannot be manually cleared, i.e. set back to 0
		// Recheck whether the endpoint is still primed?
		if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
			if(ep_num == 0){
				TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epdis = 1;  // Disable the endpoint, i.e. stop the primed transfer

				// According to the tech ref manual, EP0 doesn't support the endpoint disabled interrupt trigger
				// This is an alternative way to wait for the endpoint disabled to complete
				while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epdis == 1);  // Wait for endpoint disabled interrupt trigger
			}else{
				TRU_DWC2_DOEPMSK_REG(usb_base_addr)->bits.epdisbldmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
				TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epdis = 1;  // Disable the endpoint, i.e. stop the primed transfer

				// Wait for endpoint disabled interrupt trigger to ensure endpoint is disabled.  Note, this trigger will only work when the global OUT NAK is active
				while(TRU_DWC2_DOEPINT_REG(usb_base_addr, ep_num)->bits.epdisbld == 0);  // Wait for EP disabled interrupt to be flagged
				TRU_DWC2_DOEPINT_REG(usb_base_addr, ep_num)->bits.epdisbld = 1;  // Reset the trigger
				TRU_DWC2_DOEPMSK_REG(usb_base_addr)->bits.epdisbldmsk = 1;  // Re-enable interrupt generation on this trigger
			}

			if(flush_fifo) tru_dwc2_flush_rxfifo_wait(usb_base_addr);

			ep_enabled_was_cancelled = true;
		}

		// Turn off global OUT NAK and wait for trigger to ensure NAKing has stopped
		TRU_DWC2_DCTL_REG(usb_base_addr)->bits.cgoutnak = 1;  // Stop global OUT NAK
		//while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.goutnaksts == 1);  // Wait for controller to stop global OUT NAK mode
		while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 1);  // Wait for controller to stop global OUT NAK mode
		TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 1;  // Re-enable interrupt generation on this trigger
	}

	return ep_enabled_was_cancelled;
}

/*
	Official method:

	Stop data transfer and disable all OUT endpoints.
	If the endpoint is enabled (aka transfer mode, primed for data transfer) then we stop the current transfer before disabling it.
	This sets only the NAK once in comparison to calling a function to set each of the endpoints.

	Warning
	The EPDIS bit on the OUT endpoint requires the global OUT NAK to be enabled else it does not clear!

	Waiting for interrupt completion
	Since this function is used by our interrupt handler we cannot make use of interrupt generation because new interrupts will only
	happen after we have exit the interrupt handler, i.e. after this function is executed, which will be too late!
	We will have to use polling method to check the necessary interrupt triggers.

	Returns true if an endpoint was primed for a transfer.

	Note, the goutnakeff interrupt trigger bit is automatically cleared by the controller after global OUT NAK is stopped
*/
bool tru_dwc2_disable_epout_wait_all(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode, bool flush_fifo, uint32_t num_dev_out_ep){
	bool ep_enabled_was_cancelled = false;
	tru_dwc2_grxstsp_t grxsts __attribute__((unused));

	// The global OUT NAK is not active?
	if(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0){
		if(dma_mode == TRU_DWC2_DMA_MODE_FF){
			// Turn on global OUT NAK and wait for trigger to ensure NAKing has started
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.rxflvlmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
			TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgoutnak = 1;  // Start global OUT NAK
			do{
				if(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.rxflvl == 1){
					grxsts.val = TRU_DWC2_GRXSTSP_REG(usb_base_addr)->val;  // Read and pop the receive FIFO status - we want to pop the Global OUT NAK
				}
			//}while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.goutnaksts == 0);  // Wait for controller to start global OUT NAK mode
			}while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0);  // Wait for controller to start global OUT NAK mode
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.rxflvlmsk = 1;  // Re-enable interrupt generation on this trigger
		}else{
			// Turn on global OUT NAK and wait for trigger to ensure NAKing has started
			TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
			TRU_DWC2_DCTL_REG(usb_base_addr)->bits.sgoutnak = 1;  // Start global OUT NAK
			//while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.goutnaksts == 0);  // Wait for controller to start global OUT NAK mode
			while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 0);  // Wait for controller to start global OUT NAK mode
		}
	}

	for(uint8_t ep_num = 0; ep_num < num_dev_out_ep; ep_num++){
		//if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
		if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
			// Set NAK mode first before we disable.  NAK mode will pause the transfer or transfer completes just before we set NAK, either way it will be in a safe state
			TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;;  // Set NAK mode
			while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode

			// This recheck is required! The DWC2 (USB DesignWare Controller) says that EPDIS bit must only be set if EPENA is set (i.e. primed).
			// From my experiments I've found the reason is because EPDIS is sticky, if the EPENA is not set this bit will apply on the next prime!
			// Also the EPDIS bit cannot be manually cleared, i.e. set back to 0
			// Recheck whether the endpoint is still primed?
			if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
				if(ep_num == 0){
					TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epdis = 1;  // Disable the endpoint, i.e. stop the primed transfer

					// According to the tech ref manual, EP0 doesn't support the endpoint disabled interrupt trigger
					// This is an alternative way to wait for the endpoint disabled to complete
					while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epdis == 1);  // Wait for endpoint disabled interrupt trigger
				}else{
					TRU_DWC2_DOEPMSK_REG(usb_base_addr)->bits.epdisbldmsk = 0;  // Disable interrupt generation on this trigger so we can poll it instead
					TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epdis = 1;  // Disable the endpoint, i.e. stop the primed transfer

					// Wait for endpoint disabled interrupt trigger to ensure endpoint is disabled.  Note, this trigger will only work when the global OUT NAK is active
					while(TRU_DWC2_DOEPINT_REG(usb_base_addr, ep_num)->bits.epdisbld == 0);  // Wait for EP disabled interrupt to be flagged
					TRU_DWC2_DOEPINT_REG(usb_base_addr, ep_num)->bits.epdisbld = 1;  // Reset the trigger
					TRU_DWC2_DOEPMSK_REG(usb_base_addr)->bits.epdisbldmsk = 1;  // Re-enable interrupt generation on this trigger
				}

				ep_enabled_was_cancelled = true;
			}
		}
	}

	if(ep_enabled_was_cancelled && flush_fifo) tru_dwc2_flush_rxfifo_wait(usb_base_addr);

	// Turn off global OUT NAK and wait for trigger to ensure NAKing has stopped
	TRU_DWC2_DCTL_REG(usb_base_addr)->bits.cgoutnak = 1;  // Stop global OUT NAK
	//while(TRU_DWC2_DCTL_REG(usb_base_addr)->bits.goutnaksts == 1);  // Wait for controller to stop global OUT NAK mode
	while(TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.goutnakeff == 1);  // Wait for controller to stop global OUT NAK mode
	TRU_DWC2_GINTMSK_REG(usb_base_addr)->bits.goutnakeffmsk = 1;  // Re-enable interrupt generation on this trigger

	return ep_enabled_was_cancelled;
}

/*
	Non-official method:

	The official method may hang waiting for the global OUT NAK when the USB cable is unplugged during an endpoint transfer.
	This is a workaround will only work on some versions of the controller, it fixes the hang issue of using the EPDIS bit
	and waiting for the interrupt, instead we use the EPENA bit to disable the endpoint.
*/
bool tru_dwc2_disable_epout_forced(void *usb_base_addr, uint8_t ep_num, bool flush_fifo){
	bool ep_enabled_was_cancelled = false;

	//if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
	if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
		// Set NAK mode first before we disable.  NAK mode will pause the transfer or transfer completes just before we set NAK, either way it will be in a safe state
		TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;;  // Set NAK mode
		while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode

		// Recheck whether the endpoint is still enabled
		if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
			TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena = 0;  // Force disable the endpoint
			ep_enabled_was_cancelled = true;
		}
	}

	if(ep_enabled_was_cancelled && flush_fifo) tru_dwc2_flush_rxfifo_wait(usb_base_addr);

	return ep_enabled_was_cancelled;
}

/*
	Non-official method:

	The official method may hang waiting for the global OUT NAK when the USB cable is unplugged during an endpoint transfer.
	This is a workaround will only work on some versions of the controller, it fixes the hang issue of using the EPDIS bit
	and waiting for the interrupt, instead we use the EPENA bit to disable the endpoint.
*/
bool tru_dwc2_disable_epout_forced_all(void *usb_base_addr, bool flush_fifo, uint32_t num_dev_out_ep){
	bool ep_enabled_was_cancelled = false;

	for(uint8_t ep_num = 0; ep_num < num_dev_out_ep; ep_num++){
		//if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
		if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
			// Set NAK mode first before we disable.  NAK mode will pause the transfer or transfer completes just before we set NAK, either way it will be in a safe state
			TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.snak = 1;;  // Set NAK mode
			while(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.naksts == 0);  // Wait for controller to set NAK mode

			// Recheck whether the endpoint is still enabled
			if(TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena){
				TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena = 0;  // Force disable the endpoint
				ep_enabled_was_cancelled = true;
			}
		}
	}

	if(ep_enabled_was_cancelled && flush_fifo) tru_dwc2_flush_rxfifo_wait(usb_base_addr);

	return ep_enabled_was_cancelled;
}

/*
	Returns the maximum packet size of the endpoint defined by the USB standard.
	It is determined by the connection speed, flow type & maximum packet size from the endpoint descriptor.
*/
uint16_t tru_dwc2_determine_mps(uint8_t speed, uint8_t flow_type, uint16_t ep_mps){
	switch(flow_type){

		// Control flow
		case TRU_USB_EP_TRANSFER_TYPE_CONTROL:
			switch(speed){
				case TRU_DWC2_DSTS_ENUMSPD_HS3060: return 64; break;  // Only 64 is allowed for high speed
				case TRU_DWC2_DSTS_ENUMSPD_FS3060: case TRU_DWC2_DSTS_ENUMSPD_FS48:
					switch(ep_mps){
						case 8: case 16: case 32: case 64: return ep_mps; break;  // Valid values: 8, 16, 32, 64 for full speed
						default: return 64;  // Default to 64 for invalid mps value in setup descriptor
					}
					break;
				case TRU_DWC2_DSTS_ENUMSPD_LS6: return 8; break;  // Only 8 is allowed for low speed
				default: return 0;  // Invalid!
			}
			break;

		// Isochronous flow
		case TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS:
			switch(speed){
				case TRU_DWC2_DSTS_ENUMSPD_HS3060: return (ep_mps < 1024) ? ep_mps : 1024; break;  // Range: 1 to 1024 for high speed
				case TRU_DWC2_DSTS_ENUMSPD_FS3060: case TRU_DWC2_DSTS_ENUMSPD_FS48: return (ep_mps < 1023) ? ep_mps : 1023; break;  // Range: 1 to 1023 for full speed
				case TRU_DWC2_DSTS_ENUMSPD_LS6: return 0; break;  // Low speed is not allowed!
				default: return 0;  // Invalid!
			}
			break;

		// Bulk flow
		case TRU_USB_EP_TRANSFER_TYPE_BULK:
			switch(speed){
				case TRU_DWC2_DSTS_ENUMSPD_HS3060: return 512; break;  // Only 512 is allowed for high speed
				case TRU_DWC2_DSTS_ENUMSPD_FS3060: case TRU_DWC2_DSTS_ENUMSPD_FS48:
					switch(ep_mps){
						case 8: case 16: case 32: case 64: return ep_mps; break;  // Valid values: 8, 16, 32, 64 for full speed
						default: return 64;  // Default to 64
					}
					break;
				case TRU_DWC2_DSTS_ENUMSPD_LS6: return 0; break;  // Low speed is not allowed!
				default: return 0;  // Invalid!
			}
			break;

		// Interrupt flow
		case TRU_USB_EP_TRANSFER_TYPE_INTERRUPT:
			switch(speed){
				case TRU_DWC2_DSTS_ENUMSPD_HS3060: return (ep_mps < 1024) ? ep_mps : 1024; break;  // Range: 1 to 1024 for high speed
				case TRU_DWC2_DSTS_ENUMSPD_FS3060: case TRU_DWC2_DSTS_ENUMSPD_FS48: return (ep_mps < 64) ? ep_mps : 64; break;  // Range: 1 to 64 for full speed
				case TRU_DWC2_DSTS_ENUMSPD_LS6: return (ep_mps < 8) ? ep_mps : 8; break;  // Range: 1 to 8 for low speed
				default: return 0;  // Invalid!
			}

		// Unknown
		default:
			return 0;  // Invalid!

	}
}

/*
	Tests whether the IN endpoint is activated.
*/
bool tru_dwc2_is_epin_activated(void *usb_base_addr, uint8_t ep_num){
	return (TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep) ? true : false;
}

/*
	Tests whether the IN endpoint is enabled.
	Returns true if the in endpoint is enabled, else false.
*/
bool tru_dwc2_is_epin_enabled(void *usb_base_addr, uint8_t ep_num){
	return (TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena) ? true : false;
}

bool tru_dwc2_is_epin_busy(void *usb_base_addr, uint8_t ep_num){
	return (TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.epena) ? true : false;
}

/*
	Tests whether the OUT endpoint is activated.
*/
bool tru_dwc2_is_epout_activated(void *usb_base_addr, uint8_t ep_num){
	return (TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep) ? true : false;
}

/*
	Tests whether the OUT endpoint is enabled.
	Returns true if the out endpoint is enabled, else false.
*/
bool tru_dwc2_is_epout_enabled(void *usb_base_addr, uint8_t ep_num){
	return (TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena) ? true : false;
}

bool tru_dwc2_is_epout_busy(void *usb_base_addr, uint8_t ep_num){
	return (TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.usbactep && TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->bits.epena) ? true : false;
}

// In FIFO mode a read will pop the data from the FIFO
void tru_dwc2_read_fifo(void *usb_base_addr, void *dst, uint16_t len){
	uint32_t *dst32 = dst;

	// Read 32-bit words
	uint32_t word_count = len >> 2;  // Divide by 4
	while(word_count--){
		w32_unaligned(dst32, *TRU_DWC2_DEV_DFIFO_RX_ADDR(usb_base_addr));
		dst32++;
	}

	// Read remaining bytes 1 to 3
	uint8_t rem_bytes = len & 0x03;
	if(rem_bytes){
		uint32_t word_bytes = *TRU_DWC2_DEV_DFIFO_RX_ADDR(usb_base_addr);
		((uint8_t *)dst32)[0] = (uint8_t)word_bytes;
		if(rem_bytes > 1) ((uint8_t *)dst32)[1] = (uint8_t)(word_bytes >> 8);
		if(rem_bytes > 2) ((uint8_t *)dst32)[2] = (uint8_t)(word_bytes >> 16);
	}
}

// In FIFO mode a read will pop the data from the FIFO
void tru_dwc2_read_fifo_discard(void *usb_base_addr, uint16_t len){
	// Read 32-bit words
	uint32_t word_count = len >> 2;  // Divide by 4
	while(word_count--){
		asm volatile ("" : : "r" (*TRU_DWC2_DEV_DFIFO_RX_ADDR(usb_base_addr)));  // Read 4 bytes from FIFO
	}

	// Read remaining bytes 1 to 3
	uint8_t rem_bytes = len & 0x03;
	if(rem_bytes){
		asm volatile ("" : : "r" (*TRU_DWC2_DEV_DFIFO_RX_ADDR(usb_base_addr)));  // Read 4 bytes from FIFO
	}
}

// In FIFO mode a write will push the data into the FIFO
void tru_dwc2_write_fifo(void *usb_base_addr, uint8_t ep_num, void *src, uint16_t len){
	uint32_t *src32 = src;
	uint8_t fifonum = TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->bits.txfnum;  // Get FIFO number assigned to the Endpoint

	// Write 32-bit words
	uint32_t word_count = len >> 2;  // Divide by 4
	while(word_count--){
		*TRU_DWC2_DEV_DFIFO_TX_ADDR(usb_base_addr, fifonum) = rd32_unaligned(src32);  // Write 4 bytes into FIFO
		src32++;
	}

	// Write remaining bytes 1 to 3 zero padded as a word
	uint8_t rem_bytes = len & 0x03;
	if(rem_bytes){
		uint32_t word_bytes = ((uint8_t *)src32)[0];
		if(rem_bytes > 1) word_bytes |= (((uint8_t *)src32)[1] << 8);
		if(rem_bytes > 2) word_bytes |= (((uint8_t *)src32)[2] << 16);
		*TRU_DWC2_DEV_DFIFO_TX_ADDR(usb_base_addr, fifonum) = word_bytes;  // Write 4 bytes (padded) into FIFO
	}
}

// The GLBLINTRMSK register bit is the main USB controller interrupt generation enable bit, write 1 to enable interrupts generation.  It will generate interrupts immediately
void tru_dwc2_glob_intr_enable(void *usb_base_addr){
	TRU_DWC2_GAHBCFG_REG(usb_base_addr)->bits.glblintrmsk = 1;
}

void tru_dwc2_glob_intr_disable(void *usb_base_addr){
	TRU_DWC2_GAHBCFG_REG(usb_base_addr)->bits.glblintrmsk = 0;
}

uint32_t tru_dwc2_dev_epin_xferred_size(void *usb_base_addr, uint8_t ep_num, uint32_t total_xfersize, uint32_t max_transfer_size_mask){
	if(ep_num == 0) return (total_xfersize - TRU_DWC2_DIEPTSIZ0_REG(usb_base_addr)->bits.xfersize) & TRU_DWC2_DIEPTSIZ0_XFERSIZE_SET_MSK;
	return (total_xfersize - TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num)->bits.xfersize) & max_transfer_size_mask;
}

uint32_t tru_dwc2_dev_epout_xferred_size(void *usb_base_addr, uint8_t ep_num, uint32_t total_xfersize, uint32_t max_transfer_size_mask){
	if(ep_num == 0) return (total_xfersize - TRU_DWC2_DOEPTSIZ0_REG(usb_base_addr)->bits.xfersize) & TRU_DWC2_DOEPTSIZ0_XFERSIZE_SET_MSK;
	return (total_xfersize - TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num)->bits.xfersize) & max_transfer_size_mask;
}

/*
	Check whether the IN endpoint is enabled and has an incomplete (out of sync) isochronous transfer.
	Frame count is out of sync when the LSB of the millisecond frame count is equal to the data PID.
	Returns true if incomplete, else false
*/
bool tru_dwc2_is_epin_iso_incompl(void *usb_base_addr, uint8_t dma_mode, uint8_t ep_num, uint32_t interval, uint32_t frame_num, uint32_t frame_target, uint32_t limit){
	tru_dwc2_diepctl_t diepctl = { .val = TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num)->val };

	if(diepctl.bits.epena){
		//if(dma_mode == TRU_DWC2_DMA_MODE_SG){
			if(frame_num > limit) frame_num &= limit;
			//int32_t dif = frame_target - frame_num;
			//if(dif <= 0){
				//return true;
			//}

			uint32_t dif = (frame_num - frame_target) & limit;
			if(dif >= interval){
				return true;
			}
		//}else{
			//return true;
		//}
	}

	return false;
}

/*
	Check whether the OUT endpoint is enabled and has an incomplete (out of sync) isochronous transfer.
	Frame count is out of sync when the LSB of the millisecond frame count is equal to the data PID.
	Returns true if incomplete, else false
*/
bool tru_dwc2_is_epout_iso_incompl(void *usb_base_addr, uint8_t dma_mode, uint8_t ep_num, uint32_t interval, uint32_t frame_num, uint32_t frame_target, uint32_t limit){
	tru_dwc2_doepctl_t doepctl = { .val = TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num)->val };

	if(doepctl.bits.epena){
		//if(dma_mode == TRU_DWC2_DMA_MODE_SG){
			if(frame_num > limit) frame_num &= limit;
			//int32_t dif = frame_target - frame_num;
			//if(dif <= 0){
				//return true;
			//}

			uint32_t dif = (frame_num - frame_target) & limit;
			if(dif >= interval){
				return true;
			}
		//}else{
			//return true;
		//}
	}

	return false;
}

// This is a frame counter, aka Start Of Frame (SOF).  The rate of this counter is determined by the enumerated USB speed:
// - in high-speed it is the microframe which has a rate of 125us per frame
// - in low-speed or full-speed it is the milliframe which has a rate of 1ms per frame
uint32_t tru_dwc2_get_framenum(void *usb_base_addr){
	return TRU_DWC2_DSTS_REG(usb_base_addr)->bits.soffn;
}
