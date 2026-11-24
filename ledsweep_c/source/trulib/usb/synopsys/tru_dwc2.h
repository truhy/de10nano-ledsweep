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

	Synopsys DWC2 USB 2.0 OTG controller low-level support.

	Not much information is available on it, only brief details are found within the Cyclone V Technical Reference Manual 2020.07.24.

	I've found some details by looking at these open source files:
		Altera linux-socfpga
		  - drivers/usb/dwc2/hw.h
		  - drivers/usb/dwc2/gadget.c

		android-wear-6.0.1 for BCM (Broadcom)
		  - drivers/usb/dwc_otg/dwc_otg_regs.h

	I made numerous trial and error tests in order to find how many of the registers actually work.

	Synopsys jargon
	---------------

	Slave mode = FIFO mode
	Descriptor DMA mode = Scatter Gather DMA mode
	Internal DMA mode = Single buffer DMA mode
	Non-Scatter Gather DMA mode = Single buffer DMA mode

	DIEPCTLx, DOEPCTLx, DIEPTSIZx and DOEPTSIZx registers
	------------------------------------------------------

	Bits:
		ACT = 1 = activate = endpoint on, controller does some house keeping,
		          such as STALL, NAK, NYET, etc
		ACT = 0 = deactivate = endpoint off, completeley disabled, no house
		          keeping etc
		EPENA = 1 = enable = enable a data transfer, once completed, this is
		            turned off automatically (EPENA = 0)
		EPENA = 0 = disable = no data transfer

	When EPENA is set the USB controller automatically decrements the
	transfer size as it progresses, when the transfer is completed the EPENA
	bit is automatically set back to zero disabling further data transfers.

	The difference between end-point activate and disable:
	activate = endpoint on, responds with NAK
	deactivate = endpoint off, i.e. no transfers not even NAK,
				 ignores other bit values in the register, e.g. transfer
				 size, SNAK, etc
	enable = starts a data transfer as soon as IN or OUT token is received
	disable = no data transfer
 */

#ifndef TRU_DWC2_H
#define TRU_DWC2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"

#include "tru_dwc2_def.h"
#include "port/tru_dwc2_port.h"
#include "usb/tru_usb_lxfer_def.h"
#include "tru_dwc2_sgdma.h"

#include <stdbool.h>
#include <stdint.h>

__attribute__((always_inline)) static inline
bool tru_dwc2_is_device_mode(void *usb_base_addr){
	return (TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.curmod == TRU_DWC2_GINTSTS_CURMOD_DEVICE) ? true : false;
}

__attribute__((always_inline)) static inline
bool tru_dwc2_is_host_mode(void *usb_base_addr){
	return (TRU_DWC2_GINTSTS_REG(usb_base_addr)->bits.curmod == TRU_DWC2_GINTSTS_CURMOD_HOST) ? true : false;
}

// Determine next frame target
__attribute__((always_inline)) static inline void tru_dwc2_update_frame_target(void *usb_base_addr, uint8_t interval, uint32_t limit, uint32_t *frame_target){
	*frame_target = TRU_DWC2_DSTS_REG(usb_base_addr)->bits.soffn;
	*frame_target = (*frame_target + interval) & limit;  // Jump to next target frame by an increment of endpoint interval
	//*frame_target = (*frame_target - *frame_target % interval + interval) & limit;  // Jump to next target frame by an increment of endpoint interval
}

__attribute__((always_inline)) static inline
// GDFIFOCFG[31:16] field is the FIFO total size when this register is at the default value, also this is set back to default when the controller is reset
uint32_t tru_dwc2_get_gdfifocfg(void *usb_base_addr, tru_dwc2_hwcfg_t *hwcfg){
	return TRU_DWC2_GDFIFOCFG_REG(usb_base_addr)->bits.gdfifocfg;
}

void tru_dwc2_get_hwcfg_snpsid(void *usb_base_addr, tru_dwc2_hwcfg_t *hwcfg);
void tru_dwc2_get_hwcfg(void *usb_base_addr, tru_dwc2_hwcfg_t *hwcfg);
void tru_dwc2_init_intstatus(void *usb_base_addr, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep);
void tru_dwc2_init_intmasks(void *usb_base_addr, bool enable_sof_trigger, tru_dwc2_dma_mode_t dma_mode);
void tru_dwc2_init_fifo_shared(void *usb_base_addr, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep, uint32_t fifo_size_words);
void tru_dwc2_init_fifo_dedicated(void *usb_base_addr, uint16_t epins_mps[16], uint16_t epouts_mps[16], uint32_t num_dev_in_ep, uint32_t num_dev_out_ep, uint32_t fifo_size_words);
void tru_dwc2_realloc_fifo(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode, uint16_t epins_mps[16], uint16_t epouts_mps[16], bool use_globaloutnak, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep, uint32_t fifo_size_words);
void tru_dwc2_grstctl_reset(void *usb_base_addr, uint32_t snpsid);
void tru_dwc2_init_gahbcfg(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode);
void tru_dwc2_init_gusbcfg_phy(void *usb_base_addr, tru_dwc2_phy_t *phy, bool forcedev);
void tru_dwc2_force_devmode(void *usb_base_addr);
void tru_dwc2_force_hostmode(void *usb_base_addr);
void tru_dwc2_init_gccfg(void *usb_base_addr, tru_dwc2_phy_t *phy);
void tru_dwc2_init_dcfg(void *usb_base_addr, tru_dwc2_phy_t *phy, tru_dwc2_dma_mode_t dma_mode, uint8_t devspd);
void tru_dwc2_init_dctl(void *usb_base_addr);
void tru_dwc2_init_dthrctl(void *usb_base_addr);
void tru_dwc2_init_pcgcctl(void *usb_base_addr);
void tru_dwc2_log_details(void *usb_base_addr, uint32_t num_dev_in_ep);
void tru_dwc2_set_intmask_txfifo_empty(void *usb_base_addr, uint32_t txfnum);
void tru_dwc2_clr_intmask_txfifo_empty(void *usb_base_addr, uint32_t txfnum);
void tru_dwc2_set_dev_test_mode(void *usb_base_addr, uint8_t test_mode);
void tru_dwc2_set_dev_test_mode_msb(void *usb_base_addr, uint16_t test_mode_len_zero);
void tru_dwc2_clr_dev_test_mode(void *usb_base_addr);
void tru_dwc2_ahb_master_busy_wait(void *usb_base_addr);
bool tru_dwc2_is_ahb_master_idle(void *usb_base_addr);
bool tru_dwc2_is_ahb_master_busy(void *usb_base_addr);
bool tru_dwc2_is_dma_req_busy(void *usb_base_addr);
bool tru_dwc2_is_dma_req_idle(void *usb_base_addr);
bool tru_dwc2_is_flush_rxfifo_idle(void *usb_base_addr);
bool tru_dwc2_is_flush_rxfifo_busy(void *usb_base_addr);
bool tru_dwc2_is_flush_txfifo_idle(void *usb_base_addr);
bool tru_dwc2_is_flush_txfifo_busy(void *usb_base_addr);
bool tru_dwc2_is_flush_rxtxfifo_idle(void *usb_base_addr);
bool tru_dwc2_is_flush_rxtxfifo_busy(void *usb_base_addr);
void tru_dwc2_flush_rxfifo_wait(void *usb_base_addr);
void tru_dwc2_flush_txfifo_wait(void *usb_base_addr, uint32_t txfnum);
void tru_dwc2_flush_txfifo_all_wait(void *usb_base_addr);
void tru_dwc2_flush_rxtxfifo_wait(void *usb_base_addr, uint32_t txfnum);
uint8_t tru_dwc2_get_address(void *usb_base_addr);
void tru_dwc2_set_address(void *usb_base_addr, uint8_t address);
void tru_dwc2_activate_epin(void *usb_base_addr, uint8_t ep_num, uint8_t flow_type, uint16_t mps, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid);
void tru_dwc2_activate_epout(void *usb_base_addr, uint8_t ep_num, uint8_t flow_type, uint16_t mps, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid);
void tru_dwc2_deactivate_epin(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_deactivate_epin_all(void *usb_base_addr, uint32_t num_dev_in_ep);
void tru_dwc2_deactivate_epout(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dma_mode_t dma_mode);
void tru_dwc2_deactivate_epout_all(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode, uint32_t num_dev_out_ep);
void tru_dwc2_stall_epin(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_stall_clr_epin(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dpid_eo_t dpid);
void tru_dwc2_stall_epout(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_stall_clr_epout(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dpid_eo_t dpid);
uint8_t tru_dwc2_ep0_mps_to_regcode(uint8_t mps);
uint8_t tru_dwc2_ep0_regcode_to_mps(uint8_t code);
uint16_t tru_dwc2_ep_mps_xfer_to_pkt_cnt(uint16_t mps, uint32_t xfer_size);
uint16_t tru_dwc2_get_txfifo_freespace_words(void *usb_base_addr, tru_dwc2_txfifo_mode_t txfifo_mode, uint8_t ep_num);
uint32_t tru_dwc2_get_max_xfer_epin(void *usb_base_addr, uint8_t ep_num, uint32_t xfer_size, uint32_t ep_xfer_limit, uint16_t mps, tru_dwc2_dma_mode_t dma_mode, tru_dwc2_txfifo_mode_t txfifo_mode, tru_dwc2_hwcfg_t *hwcfg);
uint32_t tru_dwc2_get_max_xfer_epout(void *usb_base_addr, uint8_t ep_num, uint32_t xfer_size, uint32_t ep_xfer_limit, uint16_t mps, tru_dwc2_dma_mode_t dma_mode, tru_dwc2_hwcfg_t *hwcfg);
void tru_dwc2_update_oddeven_epin(void *usb_base_addr, uint8_t ep_num, uint8_t interval, uint32_t *frame_target);
void tru_dwc2_update_oddeven_epout(void *usb_base_addr, uint8_t ep_num, uint8_t interval, uint32_t *frame_target);
void tru_usb_dwc2_update_frame_target_sgdma(tru_usb_lxfer_t *lxfer, void *usb_base_addr, uint32_t interval, uint32_t *frame_target);
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
);
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
);
void tru_dwc2_snak_epin(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_snak_epin_wait(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_snak_epin_wait_all(void *usb_base_addr, uint32_t num_dev_in_ep);
void tru_dwc2_cnak_epin(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_cnak_epin_wait(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_cnak_epin_wait_all(void *usb_base_addr, uint32_t num_dev_in_ep);
void tru_dwc2_snak_epout(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_snak_epout_wait(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_snak_epout_wait_all(void *usb_base_addr, uint32_t num_dev_out_ep);
void tru_dwc2_cnak_epout(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_cnak_epout_wait(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_cnak_epout_wait_all(void *usb_base_addr, uint32_t num_dev_out_ep);
void tru_dwc2_set_globalinnak_np(void *usb_base_addr);
void tru_dwc2_set_globalinnak_np_wait(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode);
void tru_dwc2_clr_globalinnak_np(void *usb_base_addr);
void tru_dwc2_clr_globalinnak_np_wait(void *usb_base_addr);
void tru_dwc2_set_globaloutnak(void *usb_base_addr);
void tru_dwc2_set_globaloutnak_wait(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode);
void tru_dwc2_clr_globaloutnak(void *usb_base_addr);
void tru_dwc2_clr_globaloutnak_wait(void *usb_base_addr);
bool tru_dwc2_disable_epin_wait(void *usb_base_addr, uint8_t ep_num, bool flush_fifo);
bool tru_dwc2_disable_epin_wait_all(void *usb_base_addr, bool flush_fifo, uint32_t num_dev_in_ep);
bool tru_dwc2_disable_epin_forced(void *usb_base_addr, uint8_t ep_num, bool flush_fifo);
bool tru_dwc2_disable_epin_forced_all(void *usb_base_addr, bool flush_fifo, uint32_t num_dev_in_ep);
bool tru_dwc2_disable_epout_wait(void *usb_base_addr, uint8_t ep_num, tru_dwc2_dma_mode_t dma_mode, bool flush_fifo);
bool tru_dwc2_disable_epout_wait_all(void *usb_base_addr, tru_dwc2_dma_mode_t dma_mode, bool flush_fifo, uint32_t num_dev_out_ep);
bool tru_dwc2_disable_epout_forced(void *usb_base_addr, uint8_t ep_num, bool flush_fifo);
bool tru_dwc2_disable_epout_forced_all(void *usb_base_addr, bool flush_fifo, uint32_t num_dev_out_ep);
uint16_t tru_dwc2_determine_mps(uint8_t speed, uint8_t flow_type, uint16_t ep_mps);
bool tru_dwc2_is_epin_activated(void *usb_base_addr, uint8_t ep_num);
bool tru_dwc2_is_epin_enabled(void *usb_base_addr, uint8_t ep_num);
bool tru_dwc2_is_epin_busy(void *usb_base_addr, uint8_t ep_num);
bool tru_dwc2_is_epout_activated(void *usb_base_addr, uint8_t ep_num);
bool tru_dwc2_is_epout_enabled(void *usb_base_addr, uint8_t ep_num);
bool tru_dwc2_is_epout_busy(void *usb_base_addr, uint8_t ep_num);
void tru_dwc2_read_fifo(void *usb_base_addr, void *dst, uint16_t len);
void tru_dwc2_read_fifo_discard(void *usb_base_addr, uint16_t len);
void tru_dwc2_write_fifo(void *usb_base_addr, uint8_t ep_num, void *src, uint16_t len);
void tru_dwc2_glob_intr_enable(void *usb_base_addr);
void tru_dwc2_glob_intr_disable(void *usb_base_addr);
uint32_t tru_dwc2_dev_epin_xferred_size(void *usb_base_addr, uint8_t ep_num, uint32_t total_xfersize, uint32_t max_transfer_size_mask);
uint32_t tru_dwc2_dev_epout_xferred_size(void *usb_base_addr, uint8_t ep_num, uint32_t total_xfersize, uint32_t max_transfer_size_mask);
bool tru_dwc2_is_epin_iso_incompl(void *usb_base_addr, uint8_t dma_mode, uint8_t ep_num, uint32_t interval, uint32_t frame_num, uint32_t frame_target, uint32_t limit);
bool tru_dwc2_is_epout_iso_incompl(void *usb_base_addr, uint8_t dma_mode, uint8_t ep_num, uint32_t interval, uint32_t frame_num, uint32_t frame_target, uint32_t limit);
uint32_t tru_dwc2_get_framenum(void *usb_base_addr);

#ifdef __cplusplus
}
#endif

#endif
