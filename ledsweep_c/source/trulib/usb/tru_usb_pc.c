#include "usb/tru_usb_pc.h"
#include "synopsys/tru_dwc2_otg0_isr.h"
#include "synopsys/tru_dwc2_otg1_isr.h"
#include "tru_usb_cache.h"
#include "tru_delay.h"
#include "tru_logger.h"
#include <string.h>
#include <stdlib.h>

void tru_usb_pc_init(
	tru_usb_pc_t *pc,
	tru_usb_pd_info_t *desc_set,
	uintptr_t usb_base_addr,
	tru_dwc2_phy_t phy,
	uint8_t speed_sel,
	bool otg_id_pin_supported,
	bool enable_sof_trigger,
	tru_dwc2_dma_mode_t dma_mode,
	uint32_t num_dc_info
){
	pc->last_err_code = TRU_ERROR_NONE;
	switch(usb_base_addr){
		#ifdef TRU_USB_U0_BASE
			case TRU_USB_U0_BASE:
				tru_dwc2_pc0_isr_set_context(pc);
				break;
		#endif

		#ifdef TRU_USB_U1_BASE
			case TRU_USB_U1_BASE:
				tru_dwc2_pc1_isr_set_context(pc);
				break;
		#endif

		default:
			pc->last_err_code = TRU_USB_PCD_ERROR_INVALID_USB_NUM;
			return;
	}
	pc->base_addr = (void *)usb_base_addr;
	pc->phy = phy;
	pc->otg_id_pin_supported = otg_id_pin_supported;
	pc->enable_sof_trigger = enable_sof_trigger;
	pc->dma_mode = dma_mode;
	pc->txfifo_mode = TRU_DWC2_TXFIFO_DEDICATED;
	//pc->txfifo_mode = TRU_DWC2_TXFIFO_SHARED;
	pc->speed_sel = speed_sel;
	pc->sof_per_sec = 1000;  // Default to low speed & full speed frame rate
	pc->sof_per_ms = 1;  // Default to low speed & full speed frame rate
	//pc->speed = TRU_DWC2_DEV_DSTS_ENUMSPD_LS6;  // Low speed
	pc->speed = 0xff;  // No speed
	pc->state = TRU_USB_PC_DEV_STATE_NONE;
	pc->status = 0;
	pc->num_dc_info = num_dc_info;
	pc->dc_info = malloc(num_dc_info * sizeof(tru_usb_dc_info_t));
	pc->dc_info_count = 0;

	tru_usb_pc_eps_init(&pc->eps);
	tru_usb_setup_init(&pc->setup, NULL);

	// Setup EP0 custom long transfer buffers
	tru_usb_lxfer_unregister_buffers(&pc->eps.epis[0].lxfer);
	tru_usb_lxfer_unregister_buffers(&pc->eps.epos[0].lxfer);
	tru_usb_lxfer_register_buffer(&pc->eps.epis[0].lxfer, pc->setup.epin0_buf, pc->setup.epin0_bufsize);
	tru_usb_lxfer_register_buffer(&pc->eps.epos[0].lxfer, pc->setup.epout0_buf, pc->setup.epout0_bufsize);
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_SG){
		tru_usb_lxfer_register_sgdma_descs(&pc->eps.epis[0].lxfer, pc->setup.epin0_sgdma_descs, 1);
		tru_usb_lxfer_register_sgdma_descs(&pc->eps.epos[0].lxfer, pc->setup.epout0_sgdma_descs, 1);
	}

	//tru_usb_dev_init(&pc->dev, NULL, NULL, NULL, NULL, NULL);
	//tru_usb_configs_init(&pc->configs, NULL, 0, NULL, 0, NULL, 0);
	//tru_usb_strings_init(&pc->strings, NULL, 0);

	// ==============================
	// Process USB device descriptors
	// ==============================

	// Initialise generic device
	tru_usb_dev_init(
		&pc->dev,
		desc_set->hs_dev_desc,
		desc_set->fs_dev_desc,
		desc_set->ls_dev_desc,
		desc_set->hs_dev_qual_desc,
		desc_set->fs_dev_qual_desc
	);
	// Initialise generic configurations, interfaces, endpoints
	tru_usb_configs_init(
		&pc->configs,
		desc_set->hs_config_descs, desc_set->hs_config_descs_num_entries,
		desc_set->fs_config_descs, desc_set->fs_config_descs_num_entries,
		desc_set->ls_config_descs, desc_set->ls_config_descs_num_entries
	);
	// Initialise generic strings
	tru_usb_strings_init(&pc->strings, desc_set->string_descs, desc_set->string_descs_num_entries);

	tru_usb_pc_init_hw(pc);
}

void tru_usb_pc_deinit(tru_usb_pc_t *pc){
	tru_usb_pc_deinit_hw(pc);  // Let the higher level layer (USB device class layer) de-initialise the hardware
	tru_usb_strings_deinit(&pc->strings);
	tru_usb_configs_deinit(&pc->configs);
	tru_usb_dev_deinit(&pc->dev);
	tru_usb_setup_deinit(&pc->setup);
	tru_usb_pc_eps_deinit(&pc->eps);
	pc->dc_info_count = 0;
	pc->num_dc_info = 0;
	free(pc->dc_info);
	pc->dc_info = NULL;
}

// Register device class
void tru_usb_pc_register_dc(tru_usb_pc_t *pc, void *devclass, tru_usb_dc_callbacks_t *callbacks){
	if(pc->dc_info_count < pc->num_dc_info){
		pc->dc_info[pc->dc_info_count].devclass = devclass;
		pc->dc_info[pc->dc_info_count].callbacks = callbacks;
		pc->dc_info_count++;
	}
}

/*
// Weak interrupt handlers
#if defined(TRU_CFG_CPU) && TRU_CFG_CPU == TRU_OPT_CPU_CORTEXA9
	void __attribute__((weak)) tru_usb_pc0_isr(void){
	}
	#define USB0_HANDLER tru_usb_pc0_isr

	void __attribute__((weak)) tru_usb_pc1_isr(void){
	}
	#define USB1_HANDLER tru_usb_pc1_isr
#endif
*/

/*
	Initialise USB controller hardware.

	Notes:
		(1)
			While in reset, the USB controller registers are not mapped, reading
			the register addresses will return zeroes and writing is ignored.
			So this must be done first.  You may also notice this during
			debugging, as an example:

			in break/halt mode, entering this command into the GDB console:
			x 0xffb40040
			gives this result:
			0xffb40040: 0x00000000

			After reset, issuing the same GDB command will give correct result
			(release number of the core (USB controller)):
			0xffb40040: 0x4f54293a

		(2)
			The GLBLINTRMSK register bit is the main USB controller interrupt
			generation enable bit, write 1 to enable.  Once enabled, the
			USB controller will generate interrupts.
*/
void tru_usb_pc_init_hw(tru_usb_pc_t *pc){
#if defined(TRU_CFG_CHIPSET) && TRU_CFG_CHIPSET == TRU_OPT_CHIPSET_C5SOC
	if(tru_usb_pc_periph_reset_held(pc->base_addr)){
		// Put the USB controller peripheral out of reset (SoC FPGA HPS specific register)
		tru_usb_pc_periph_reset_release(pc->base_addr);
	}

	// There must a small delay after a reset release, otherwise reading/writing USB registers do nothing, i.e. read/write with zeroes
	//unsigned int i = 10000;
	//while(i--) __asm__ __volatile__("");
	tru_delay_ms(1);
#endif

	//tru_dwc2_get_hwcfg_snpsid(pc->base_addr, &pc->hwcfg);
	tru_dwc2_get_hwcfg(pc->base_addr, &pc->hwcfg);

	// Setup USB-PHY related configuration settings specific for DE10-Nano development board
	// On the Nucleo-144 board, if the USB ID PIN (PA10) is not used we need to manually switch (force) to device mode
	tru_dwc2_init_gusbcfg_phy(pc->base_addr, &pc->phy, pc->otg_id_pin_supported);

	// Reset the controller after GUSBCFG has been configured
	tru_dwc2_grstctl_reset(pc->base_addr, pc->hwcfg.snpsid);

	// After a reset, wait for the AHB master to become idle before starting any operation
	tru_dwc2_ahb_master_busy_wait(pc->base_addr);

	// GDFIFOCFG[31:16] field is the FIFO total size when this register is at the default value, also this is set back to default when the controller is reset
	pc->hwcfg.fifo_total_size_words = tru_dwc2_get_gdfifocfg(pc->base_addr, &pc->hwcfg);

	// Setup global USB AHB configuration settings
	tru_dwc2_init_gahbcfg(pc->base_addr, pc->dma_mode);

	tru_dwc2_init_intstatus(pc->base_addr, pc->hwcfg.num_dev_in_ep, pc->hwcfg.num_dev_ep);

	tru_dwc2_init_gccfg(pc->base_addr, &pc->phy);

	// Setup device mode configuration settings
	tru_dwc2_init_dcfg(pc->base_addr, &pc->phy, pc->dma_mode, pc->speed_sel);

	// Setup device mode control settings
	tru_dwc2_init_dctl(pc->base_addr);

	tru_dwc2_init_dthrctl(pc->base_addr);

	tru_dwc2_init_pcgcctl(pc->base_addr);

	// Setup up hardware FIFOs
	if(pc->txfifo_mode == TRU_DWC2_TXFIFO_DEDICATED){
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
			pc->eps.epos[15].mps
		};
		tru_dwc2_init_fifo_dedicated(pc->base_addr, epins_mps, epouts_mps, pc->hwcfg.num_dev_in_ep, pc->hwcfg.num_dev_ep, pc->hwcfg.fifo_usable_size_words);
	}else{
		tru_dwc2_init_fifo_shared(pc->base_addr, pc->hwcfg.num_dev_in_ep, pc->hwcfg.num_dev_ep, pc->hwcfg.fifo_usable_size_words);
	}

	tru_dwc2_clr_globalinnak_np(pc->base_addr);
	tru_dwc2_clr_globaloutnak(pc->base_addr);
	//tru_dwc2_snak_epin_wait_all(pc->base_addr);
	//tru_dwc2_snak_epout_wait_all(pc->base_addr);

	// Unmask interrupt status interrupt generation (triggering)
	tru_dwc2_init_intmasks(pc->base_addr, pc->enable_sof_trigger, pc->dma_mode);

#if defined(TRU_CFG_USB_LOG_INIT) && TRU_CFG_USB_LOG_INIT == 1
	tru_dwc2_log_details(pc->base_addr, pc->hwcfg.num_dev_in_ep);
#endif
}

void tru_usb_pc_deinit_hw(tru_usb_pc_t *pc){
	tru_dwc2_deactivate_epin_all(pc->base_addr, pc->hwcfg.num_dev_in_ep);
	tru_dwc2_deactivate_epout_all(pc->base_addr, pc->dma_mode, pc->hwcfg.num_dev_ep);

	// Stop interrupt generation
	tru_dwc2_glob_intr_disable(pc->base_addr);

#if defined(USB0_HANDLER) || defined(USB1_HANDLER)
	// Disable user interrupt handler
	tru_irq_unregister(pc->intr_id);
#endif
}

bool tru_usb_pc_periph_reset_held(void *usb_base_addr){
#if defined(TRU_CFG_CPU) && TRU_CFG_CPU == TRU_OPT_CPU_CORTEXA9
	switch((uint32_t)usb_base_addr){
		case TRU_USB_U0_BASE: return (TRU_HPS_RSTMGR_PERMODRST_REG->bits.usb0 == 1) ? true : false; break;  // Bit2 = 1
		case TRU_USB_U1_BASE: return (TRU_HPS_RSTMGR_PERMODRST_REG->bits.usb1 == 1) ? true : false; break;  // Bit3 = 1
	}
#endif
	return false;
}

/*
	Reset hold USB controller peripheral.
*/
void tru_usb_periph_reset_hold(void *usb_base_addr){
#if defined(TRU_CFG_CPU) && TRU_CFG_CPU == TRU_OPT_CPU_CORTEXA9
	switch((uint32_t)usb_base_addr){
		case TRU_USB_U0_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.usb0 = 1; break;  // Bit2 = 1
		case TRU_USB_U1_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.usb1 = 1; break;  // Bit3 = 1
	}
#endif
}

/*
	Reset release USB controller peripheral.
*/
void tru_usb_pc_periph_reset_release(void *usb_base_addr){
#if defined(TRU_CFG_CPU) && TRU_CFG_CPU == TRU_OPT_CPU_CORTEXA9
	switch((uint32_t)usb_base_addr){
		case TRU_USB_U0_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.usb0 = 0; break;  // Bit2 = 0
		case TRU_USB_U1_BASE: TRU_HPS_RSTMGR_PERMODRST_REG->bits.usb1 = 0; break;  // Bit3 = 0
	}
#endif
}

void tru_usb_pc_start(tru_usb_pc_t *pc){
	//tru_dwc2_init_irq_triggers(pc->base_addr);
	tru_dwc2_glob_intr_enable(pc->base_addr);

#if defined(TRU_CFG_USB_LOG_INIT) && TRU_CFG_USB_LOG_INIT == 1
	LOG("USB INTR STARTED\n");
#endif
}

void tru_usb_pc_is_ahb_master_busy_wait(void *usb_base_addr){
	// Wait for the AHB master to be idle
	while(tru_dwc2_is_ahb_master_busy(usb_base_addr)){
		// To do: implement a cancellation and or timeout
	};
}

void tru_usb_pc_is_flush_txrxfifo_busy_wait(void *usb_base_addr){
	// Wait for flush FIFOs to complete
	while(tru_dwc2_is_flush_rxtxfifo_busy(usb_base_addr)){
		// To do: implement a cancellation and or timeout
	};
}

void tru_usb_pc_setup_sgdma_epin(tru_usb_pc_t *pc, tru_usb_pc_epi_t *epin, bool is_ioc_per_desc){
	uint32_t desc_max_xfer_size = tru_dwc2_sgdma_get_desc_size_limit_epin(epin->flow_type);
	uint32_t xfer_size;
	bool is_last;
	bool is_final = (epin->lxfer.whole.total_xferred_size + epin->lxfer.chunk.total_xfer_size == epin->lxfer.whole.total_xfer_size) ? true : false;
	bool is_short;  // If true, send a ZLP after all data is transmitted

	for(uint32_t i = 0; i < epin->lxfer.chunk.num_registered; i++){
		//xfer_size = (epin->lxfer.chunk.buffers[i].xfer_size_aligned > desc_max_xfer_size) ? desc_max_xfer_size : epin->lxfer.chunk.buffers[i].xfer_size_aligned;
		xfer_size = (epin->lxfer.chunk.buffers[i].xfer_size > desc_max_xfer_size) ? desc_max_xfer_size : epin->lxfer.chunk.buffers[i].xfer_size;
		is_last = (i + 1 == epin->lxfer.chunk.num_registered) ? true : ((epin->lxfer.chunk.buffers[i + 1].xfer_size == 0) ? true : false);
		is_short = (is_final && is_last && (epin->lxfer.whole.total_xfer_size == 0 || (epin->lxfer.whole.total_xfer_size != epin->lxfer.whole.total_exp_size && epin->lxfer.whole.total_xfer_size % epin->mps == 0))) ? true : false;
		tru_dwc2_sgdma_setup_desc_epin(&epin->lxfer.chunk.sgdma, i, epin->lxfer.chunk.buffers[i].buf, xfer_size, is_short, is_last, (is_ioc_per_desc) ? true : is_last, epin->flow_type, epin->mps, 0, 0);
		if(is_last) break;
	}
}

void tru_usb_pc_setup_sgdma_epout(tru_usb_pc_t *pc, tru_usb_pc_epo_t *epout, bool is_ioc_per_desc){
	uint32_t desc_max_xfer_size = tru_dwc2_sgdma_get_desc_size_limit_epout(epout->flow_type, epout->mps);
	uint32_t xfer_size;
	bool is_last;

	for(uint32_t i = 0; i < epout->lxfer.chunk.num_registered; i++){
		xfer_size = (epout->lxfer.chunk.buffers[i].xfer_size_aligned > desc_max_xfer_size) ? desc_max_xfer_size : epout->lxfer.chunk.buffers[i].xfer_size_aligned;
		is_last = (i + 1 == epout->lxfer.chunk.num_registered) ? true : ((epout->lxfer.chunk.buffers[i + 1].xfer_size == 0) ? true : false);
		tru_dwc2_sgdma_setup_desc_epout(&epout->lxfer.chunk.sgdma, i, epout->lxfer.chunk.buffers[i].buf, xfer_size, is_last, (is_ioc_per_desc) ? true : is_last, epout->flow_type, 0);
		if(is_last) break;
	}
}

void tru_usb_pc_enable_xfer_epin(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid){
	tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;
	uint32_t xfer_size = ep->lxfer.chunk.total_xfer_size - ep->lxfer.chunk.total_xferred_size;

	switch(pc->dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			ep->xfer_buf = (uint8_t *)ep->lxfer.chunk.buffers[0].buf + ep->lxfer.chunk.total_xferred_size;
			ep->xfer_size = tru_dwc2_get_max_xfer_epin(pc->base_addr, ep_num, xfer_size, ep->lxfer.ep_xfer_limit, ep->mps, pc->dma_mode, pc->txfifo_mode, &pc->hwcfg);  // Calculate maximum allowed transfer size
			break;
		case TRU_DWC2_DMA_MODE_SB:
			ep->xfer_buf = (uint8_t *)ep->lxfer.chunk.buffers[0].buf + ep->lxfer.chunk.total_xferred_size;
			ep->xfer_size = tru_dwc2_get_max_xfer_epin(pc->base_addr, ep_num, xfer_size, ep->lxfer.ep_xfer_limit, ep->mps, pc->dma_mode, pc->txfifo_mode, &pc->hwcfg);  // Calculate maximum allowed transfer size

			// Cache coherence maintenance before starting the SB DMA operation
			// We clean the data buffer to ensure the DMA controller can see the changes we made
			#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 0
			#if defined(TRU_CFG_L1C) && TRU_CFG_L1C == 1 || defined(TRU_CFG_L2C) && TRU_CFG_L2C == 1
				tru_usb_pc_cache_clean_sbdma_data_epin(ep);  // Clean data buffer
			#endif
			#endif

			break;
		case TRU_DWC2_DMA_MODE_SG:
			ep->xfer_buf = ep->lxfer.chunk.sgdma.dma_descs;
			ep->xfer_size = xfer_size;

			// Cache coherence maintenance before starting the SG DMA operation
			// Seems a hardware bug, for some reason the DMA linked-list must be cleaned + invalidated rather than just clean
			// We clean the data buffers to ensure the DMA controller can see the changes we made
			#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 0
			#if defined(TRU_CFG_L1C) && TRU_CFG_L1C == 1 || defined(TRU_CFG_L2C) && TRU_CFG_L2C == 1
				tru_usb_pc_cache_cleaninv_sgdma_list(&ep->lxfer);  // Clean & invalidate descriptor list
				tru_usb_pc_cache_clean_sgdma_data(&ep->lxfer);     // Clean data buffers
			#endif
			#endif

			break;
	}

	/*
	if(ep_num){
		if(ep->lxfer.whole.total_xferred_size == 0){
			uint32_t limit = (pc->dma_mode == TRU_DWC2_DMA_MODE_SG) ? TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_LIMIT : TRU_DWC2_DSTS_SOFFN_LIMIT;
			pc->frame_num = TRU_DWC2_DSTS_REG(pc->base_addr)->bits.soffn;
			while(tru_usb_pc_epin_frame_target_elapsed(ep, limit, pc->frame_num)){
				tru_usb_pc_epin_inc_frame_target(ep, limit);
				pc->frame_num = TRU_DWC2_DSTS_REG(pc->base_addr)->bits.soffn;
			}
		}
	}
	*/

#if defined(TRU_CFG_USB_LOG_EPENA) && TRU_CFG_USB_LOG_EPENA == 1
	LOG("ENABLE: EI%u %lu bytes\n", ep->num, ep->xfer_size);
#endif

	tru_dwc2_enable_epin(
		pc->base_addr,
		ep_num,
		ep->mps,
		ep->flow_type,
		ep->xfer_buf,
		ep->xfer_size,
		ep->interval,
		&ep->frame_target,
		&ep->lxfer,
		pc->speed,
		pc->dma_mode,
		stall,
		nak,
		dpid
	);

	if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
		if(xfer_size != 0) tru_dwc2_set_intmask_txfifo_empty(pc->base_addr, ep->num);  // If not ZLP, enable interrupt generation from the TX FIFO empty status trigger
	}
}

void tru_usb_pc_enable_xfer_epout(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid){
	tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;
	uint32_t xfer_size = ep->lxfer.chunk.total_xfer_size - ep->lxfer.chunk.total_xferred_size;

	switch(pc->dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			ep->xfer_buf = (uint8_t *)ep->lxfer.chunk.buffers[0].buf + ep->lxfer.chunk.total_xferred_size;
			ep->xfer_size = tru_dwc2_get_max_xfer_epout(pc->base_addr, ep_num, xfer_size, ep->lxfer.ep_xfer_limit, ep->mps, pc->dma_mode, &pc->hwcfg);  // Calculate maximum allowed transfer size
			break;
		case TRU_DWC2_DMA_MODE_SB:
			ep->xfer_buf = (uint8_t *)ep->lxfer.chunk.buffers[0].buf + ep->lxfer.chunk.total_xferred_size;
			ep->xfer_size = tru_dwc2_get_max_xfer_epout(pc->base_addr, ep_num, xfer_size, ep->lxfer.ep_xfer_limit, ep->mps, pc->dma_mode, &pc->hwcfg);  // Calculate maximum allowed transfer size

			// Seems another hardware bug, for some reason we need to invalidate or clean the receive data buffer before we receive data!
			// Normally, you would only need to invalidate after receiving the data.
			// This bug doesn't seem to apply for the SG DMA mode.
			#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 0
			#if defined(TRU_CFG_L1C) && TRU_CFG_L1C == 1 || defined(TRU_CFG_L2C) && TRU_CFG_L2C == 1
				tru_usb_pc_cache_inv_sbdma_data_epout(ep);
			#endif
			#endif
			break;
		case TRU_DWC2_DMA_MODE_SG:
			ep->xfer_buf = ep->lxfer.chunk.sgdma.dma_descs;
			ep->xfer_size = xfer_size;

			// Cache coherence maintenance before the SG DMA operation
			// Seems a hardware bug, for some reason the DMA linked-list must be cleaned + invalidated rather than just clean
			#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 0
			#if defined(TRU_CFG_L1C) && TRU_CFG_L1C == 1 || defined(TRU_CFG_L2C) && TRU_CFG_L2C == 1
				tru_usb_pc_cache_cleaninv_sgdma_list(&ep->lxfer);  // Clean & invalidate descriptor list
				//tru_usb_pc_cache_inv_sgdma_data(&ep->lxfer);  // Invalidate data buffers
			#endif
			#endif

			break;
	}

	/*
	if(ep_num){
		if(ep->lxfer.whole.total_xferred_size == 0){
			uint32_t limit = (pc->dma_mode == TRU_DWC2_DMA_MODE_SG) ? TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_LIMIT : TRU_DWC2_DSTS_SOFFN_LIMIT;
			pc->frame_num = TRU_DWC2_DSTS_REG(pc->base_addr)->bits.soffn;
			while(tru_usb_pc_epout_frame_target_elapsed(ep, limit, pc->frame_num)){
				tru_usb_pc_epout_inc_frame_target(ep, limit);
				pc->frame_num = TRU_DWC2_DSTS_REG(pc->base_addr)->bits.soffn;
			}
		}
	}
	*/

#if defined(TRU_CFG_USB_LOG_EPENA) && TRU_CFG_USB_LOG_EPENA == 1
	LOG("ENABLE: EO%u %lu bytes\n", ep->num, ep->xfer_size);
#endif

	tru_dwc2_enable_epout(
		pc->base_addr,
		ep_num,
		ep->mps,
		ep->flow_type,
		ep->xfer_buf,
		ep->xfer_size,
		ep->interval,
		&ep->frame_target,
		&ep->lxfer,
		pc->speed,
		pc->dma_mode,
		stall,
		nak,
		dpid
	);
}

/*
	Start a new or resume a long transfer on request for the specified IN endpoint (transmit data to host)
*/
void tru_usb_pc_transmit_sr(tru_usb_pc_t *pc, uint8_t ep_num, bool is_first){
	tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;
	tru_dwc2_dpid_eo_t dpid = TRU_DWC2_DPID_EO_AUTO;
	bool enable = true;

	if(is_first){
		ep->lxfer.whole.total_xferred_size = 0;
		ep->lxfer.whole.offset = 0;
	}
	ep->lxfer.chunk.total_xferred_size = 0;
	ep->xferred_size_prezlp = 0;
	ep->xferred_size = 0;

	tru_usb_lxfer_update_chunk_total_xfer_size(&ep->lxfer);
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_SG){
		tru_usb_pc_setup_sgdma_epin(pc, ep, false);
		enable = tru_dwc2_is_epin_activated(pc->base_addr, ep_num) && !tru_dwc2_is_epin_enabled(pc->base_addr, ep_num);
	}

	if(enable){
		#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
			tru_usb_pc_epin_set_deferred(ep, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, dpid);
		#else
			tru_usb_pc_enable_xfer_epin(pc, ep_num, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, dpid);  // Enable the transfer
		#endif
	}
}

/*
	Start a new long transfer request for the specified IN endpoint (transmit data to host)
*/
uint32_t tru_usb_pc_transmit(tru_usb_pc_t *pc, uint8_t ep_num, uint32_t xfer_size, uint32_t exp_size){
	tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;

	if(pc->dma_mode != TRU_DWC2_DMA_MODE_SG){
		if(tru_dwc2_is_epin_busy(pc->base_addr, ep_num) || ep->deferred.is_epena_pending) return TRU_USB_PC_RC_EP_BUSY;  // Exit if EP is busy
	}

	// Setup long transfer size
	ep->lxfer.whole.total_xfer_size = xfer_size;
	ep->lxfer.whole.total_exp_size = exp_size;

	// Setup short transfer size
	//tru_usb_lxfer_zero_chunk_xfer_size_all(&ep->lxfer);
	tru_usb_lxfer_fill_chunk_xfer_size_all_tx(&ep->lxfer, xfer_size, pc->dma_mode);

	tru_usb_pc_transmit_sr(pc, ep_num, true);

	return TRU_USB_PC_RC_OK;
}

/*
	Continue an existing long transfer.

	Because the upper layer transfer request is too big for the USB controller register,
	the request requires multiple small requests, this starts the next register limited small request.
*/
void tru_usb_pc_transmit_resume_ep(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;
	tru_dwc2_dpid_eo_t dpid = TRU_DWC2_DPID_EO_AUTO;
	bool enable = true;

	ep->xferred_size = 0;
	ep->xferred_size_prezlp = 0;

	if(pc->dma_mode == TRU_DWC2_DMA_MODE_SG){
		enable = tru_dwc2_is_epin_activated(pc->base_addr, ep_num) && !tru_dwc2_is_epin_enabled(pc->base_addr, ep_num) && !ep->deferred.is_epena_pending;
	}

	if(enable){
		#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
			tru_usb_pc_epin_set_deferred(ep, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, dpid);
		#else
			tru_usb_pc_enable_xfer_epin(pc, ep_num, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, dpid);  // Enable the next transfer
		#endif
	}
}

/*
	Start a new or resume to complete an existing transfer to transmit a ZLP (Zero Length Packet).
*/
void tru_usb_pc_transmit_zlp_sr(tru_usb_pc_t *pc, uint8_t ep_num, bool is_first){
	tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;
	bool enable = true;

	tru_usb_lxfer_zero_chunk_xfer_size_all(&ep->lxfer);
	if(is_first){
		ep->lxfer.whole.total_xferred_size = 0;
		ep->lxfer.whole.offset = 0;
	}
	ep->lxfer.chunk.total_xfer_size = 0;
	ep->lxfer.chunk.total_xfer_size_aligned = 0;
	ep->lxfer.chunk.total_xferred_size = 0;
	ep->xferred_size_prezlp = 0;
	ep->xferred_size = 0;

	if(pc->dma_mode == TRU_DWC2_DMA_MODE_SG){
		tru_dwc2_sgdma_setup_desc_epin(&ep->lxfer.chunk.sgdma, 0, NULL, 0, true, true, true, ep->flow_type, ep->mps, ep->frame_target, 0);
		enable = tru_dwc2_is_epin_activated(pc->base_addr, ep_num) && !tru_dwc2_is_epin_enabled(pc->base_addr, ep_num) && !ep->deferred.is_epena_pending;
	}

	if(enable){
		#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
			tru_usb_pc_epin_set_deferred(ep, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, TRU_DWC2_DPID_EO_AUTO);
		#else
			tru_usb_pc_enable_xfer_epin(pc, ep_num, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, TRU_DWC2_DPID_EO_AUTO);
		#endif
	}
}

/*
	Start a new transfer to transmit a ZLP (Zero Length Packet).
*/
uint32_t tru_usb_pc_transmit_zlp(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;

	if(pc->dma_mode != TRU_DWC2_DMA_MODE_SG){
		if(tru_dwc2_is_epin_busy(pc->base_addr, ep_num) || ep->deferred.is_epena_pending) return TRU_USB_PC_RC_EP_BUSY;  // Exit if EP is busy
	}

	tru_usb_pc_transmit_zlp_sr(pc, ep_num, true);

	return TRU_USB_PC_RC_OK;
}

/*
	Start a new or resume a long transfer on the specified OUT endpoint (receive data from host)

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
*/
void tru_usb_pc_receive_sr(tru_usb_pc_t *pc, uint8_t ep_num, tru_dwc2_stall_t stall, bool is_first){
	tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;
	tru_dwc2_nak_t nak = TRU_DWC2_NAK_CLEAR;
	tru_dwc2_dpid_eo_t dpid = TRU_DWC2_DPID_EO_AUTO;
	bool enable = true;

	if(ep_num == 0){
		// For the SETUP stage and STATUS OUT stage we must not clear the NAK mode, i.e. enable NAK mode.  NAK mode is cleared only for the DATA OUT stage
		if(pc->eps.epos[0].next_control_stage == TRU_USB_PC_EP_CONTROL_STAGE_SETUP || pc->eps.epos[0].next_control_stage == TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT){
			nak = TRU_DWC2_NAK_SET;  // Enable NAK mode
			//nak = TRU_DWC2_NAK_CURRENT;  // Use current NAK mode
		}
	}

	if(is_first){
		ep->lxfer.whole.total_xferred_size = 0;
		ep->lxfer.whole.offset = 0;
	}
	ep->lxfer.chunk.total_xferred_size = 0;
	ep->xferred_size_prezlp = 0;
	ep->xferred_size = 0;

	tru_usb_lxfer_update_chunk_total_xfer_size(&ep->lxfer);
	if(pc->dma_mode == TRU_DWC2_DMA_MODE_SG){
		tru_usb_pc_setup_sgdma_epout(pc, ep, false);
		enable = !tru_dwc2_is_epout_enabled(pc->base_addr, ep_num);
	}

	if(enable){
		#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
			tru_usb_pc_epout_set_deferred(ep, stall, nak, dpid);
		#else
			tru_usb_pc_enable_xfer_epout(pc, ep_num, stall, nak, dpid);  // Enable the transfer
		#endif
	}
}

/*
	Start a new long transfer request on the specified OUT endpoint (receive data from host)
*/
uint32_t tru_usb_pc_receive(tru_usb_pc_t *pc, uint8_t ep_num, uint32_t xfer_size, uint32_t exp_size){
	tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;

	if(!ep->is_activated) return TRU_USB_PC_RC_EP_NOTACTIVATED;  // Exit if EP is not activated in a SET CONFIG or SET INTERFACE request

	if(pc->dma_mode != TRU_DWC2_DMA_MODE_SG){
		if(tru_dwc2_is_epout_busy(pc->base_addr, ep_num) || ep->deferred.is_epena_pending) return TRU_USB_PC_RC_EP_BUSY;  // Exit if EP is busy
	}

	// Setup long transfer size
	ep->lxfer.whole.total_xfer_size = xfer_size;
	ep->lxfer.whole.total_exp_size = exp_size;

	// Setup short transfer size
	//tru_usb_lxfer_zero_chunk_xfer_size_all(&ep->lxfer);
	tru_usb_lxfer_fill_chunk_xfer_size_all_rx(&ep->lxfer, xfer_size, pc->dma_mode);

	tru_usb_pc_receive_sr(pc, ep_num, TRU_DWC2_STALL_CLEAR, true);

	return TRU_USB_PC_RC_OK;
}

/*
	Continue an existing long transfer.

	Because the upper layer transfer request is too big for the USB controller register,
	the request requires multiple small requests, this starts the next register limited small request.
*/
void tru_usb_pc_receive_resume_ep(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;
	tru_dwc2_dpid_eo_t dpid = TRU_DWC2_DPID_EO_AUTO;
	bool enable = true;

	ep->xferred_size = 0;
	ep->xferred_size_prezlp = 0;

	if(pc->dma_mode == TRU_DWC2_DMA_MODE_SG){
		enable = tru_dwc2_is_epout_activated(pc->base_addr, ep_num) && !tru_dwc2_is_epout_enabled(pc->base_addr, ep_num) && !ep->deferred.is_epena_pending;
	}

	if(enable){
		#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
			tru_usb_pc_epout_set_deferred(ep, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, dpid);
		#else
			tru_usb_pc_enable_xfer_epout(pc, ep_num, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, dpid);  // Enable the next transfer
		#endif
	}
}

/*
	Start a new or resume to complete an existing transfer to receive a ZLP (Zero Length Packet).
*/
void tru_usb_pc_receive_zlp_sr(tru_usb_pc_t *pc, uint8_t ep_num, bool is_first){
	tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;
	bool enable = true;

	tru_usb_lxfer_zero_chunk_xfer_size_all(&ep->lxfer);
	if(is_first){
		ep->lxfer.whole.total_xferred_size = 0;
		ep->lxfer.whole.offset = 0;
	}
	ep->lxfer.chunk.total_xfer_size = 0;
	ep->lxfer.chunk.total_xfer_size_aligned = 0;
	ep->lxfer.chunk.total_xferred_size = 0;
	ep->xferred_size_prezlp = 0;
	ep->xferred_size = 0;

	if(pc->dma_mode == TRU_DWC2_DMA_MODE_SG){
		tru_dwc2_sgdma_setup_desc_epout(&ep->lxfer.chunk.sgdma, 0, NULL, 0, true, true, ep->flow_type, ep->frame_target);
		enable = tru_dwc2_is_epout_activated(pc->base_addr, ep_num) && !tru_dwc2_is_epout_enabled(pc->base_addr, ep_num) && !ep->deferred.is_epena_pending;
	}

	if(enable){
		#if defined(TRU_USB_PC_DEFER_XFER) && TRU_USB_PC_DEFER_XFER == 1
			tru_usb_pc_epout_set_deferred(ep, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, TRU_DWC2_DPID_EO_AUTO);
		#else
			tru_usb_pc_enable_xfer_epout(pc, ep_num, TRU_DWC2_STALL_CLEAR, TRU_DWC2_NAK_CLEAR, TRU_DWC2_DPID_EO_AUTO);
		#endif
	}
}

/*
	Start a new transfer to receive a ZLP (Zero Length Packet).
*/
uint32_t tru_usb_pc_receive_zlp(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;

	if(pc->dma_mode != TRU_DWC2_DMA_MODE_SG){
		if(tru_dwc2_is_epout_busy(pc->base_addr, ep_num) || ep->deferred.is_epena_pending) return TRU_USB_PC_RC_EP_BUSY;  // Exit if EP is busy
	}

	tru_usb_pc_receive_zlp_sr(pc, ep_num, true);

	return TRU_USB_PC_RC_OK;
}

void tru_usb_pc_process_txfifo_write(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epi_t *epin = &pc->eps.epis[ep_num];

	if(epin->lxfer.chunk.total_xferred_size < epin->lxfer.chunk.total_xfer_size){
		uint32_t max_xfer_size = tru_dwc2_get_max_xfer_epin(pc->base_addr, epin->num, epin->lxfer.chunk.total_xfer_size - epin->lxfer.chunk.total_xferred_size, epin->lxfer.ep_xfer_limit, epin->mps, pc->dma_mode, pc->txfifo_mode, &pc->hwcfg);  // Calculate maximum allowed transfer size
		if(max_xfer_size){
			tru_dwc2_write_fifo(pc->base_addr, epin->num, epin->xfer_buf, max_xfer_size);  // Write data to TXFIFO data (push to TXFIFO)
			tru_usb_pc_update_lxfer_details_epin(pc, epin->num);
			if(epin->lxfer.chunk.total_xferred_size >= epin->xfer_size){
				tru_usb_pc_end_lxfer_details_epin(pc, epin->num);
			}
		}
	}
}

void tru_usb_pc_process_txfifo_empty_irq(tru_usb_pc_t *pc){
	tru_usb_pc_epi_t *epin = pc->eps.epis;
	uint32_t diepempmsk = TRU_DWC2_DIEPEMPMSK_REG(pc->base_addr)->val;
	uint32_t daint = TRU_DWC2_DAINT_REG(pc->base_addr)->val & TRU_DWC2_DAINTMSK_REG(pc->base_addr)->val & 0x0000ffff;

	for(uint8_t i = 0; i < pc->hwcfg.num_dev_in_ep; i++){
		if(pc->dma_mode == TRU_DWC2_DMA_MODE_FF){
			if(daint & (1 << epin->num)){
				// Note: the TXFEMP trigger bit is read-only and does not require manual clearing, the USB controller will automatically clear this bit
				if(TRU_DWC2_DIEPINT_REG(pc->base_addr, epin->num)->bits.txfemp){
					if(diepempmsk & (1 << epin->num)){
						tru_usb_pc_process_txfifo_write(pc, epin->num);
					}
				}
			}
		}

		epin++;
	}
}

/*
	Process software deferred transfer requests.
*/
void tru_usb_pc_process_deferred(tru_usb_pc_t *pc){
	//tru_dwc2_glob_intr_disable(pc->base_addr);
	//GIC_DisableIRQ(pc->intr_id);
	//GIC_DisableInterface();
	//irq_mask(0);
	//GIC_ClearPendingIRQ(pc->intr_id);

	tru_usb_pc_epi_t *epin = pc->eps.epis;
	tru_usb_pc_epo_t *epout = pc->eps.epos;

	for(uint8_t i = 0; i < pc->hwcfg.num_dev_in_ep; i++){
		if(epin->deferred.is_epena_pending){
			tru_usb_pc_enable_xfer_epin(pc, epin->num, epin->deferred.stall, epin->deferred.nak, epin->deferred.dpid);
			epin->deferred.is_epena_pending = false;
		}
		epin++;
	}

	for(uint8_t i = 0; i < pc->hwcfg.num_dev_ep; i++){
		if(epout->deferred.is_epena_pending){
			tru_usb_pc_enable_xfer_epout(pc, epout->num, epout->deferred.stall, epout->deferred.nak, epout->deferred.dpid);
			epout->deferred.is_epena_pending = false;
		}
		epout++;
	}

	//irq_mask(1);
	//GIC_EnableInterface();
	//GIC_EnableIRQ(pc->intr_id);
	//tru_dwc2_glob_intr_enable(pc->base_addr);
}

void tru_usb_pc_halt_epin(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_dwc2_stall_epin(pc->base_addr, ep_num);
	pc->eps.epis[ep_num].status |= TRU_USB_STATUS_EP_HALT_MSK_SET;
}

void tru_usb_pc_halt_epout(tru_usb_pc_t *pc, uint8_t ep_num){
	pc->eps.epos[ep_num].status |= TRU_USB_STATUS_EP_HALT_MSK_SET;
	tru_dwc2_stall_epout(pc->base_addr, ep_num);
}

void tru_usb_pc_unhalt_epin(tru_usb_pc_t *pc, uint8_t ep_num){
	pc->eps.epis[ep_num].status &= TRU_USB_STATUS_EP_HALT_MSK_CLR;
	tru_dwc2_stall_clr_epin(pc->base_addr, ep_num, TRU_DWC2_STALL_CLEAR);  // Data toggle is reset to 0 when SetConfiguration, SetInterface or ClearFeature(ENDPOINT_HALT) is requested
}

void tru_usb_pc_unhalt_epout(tru_usb_pc_t *pc, uint8_t ep_num){
	pc->eps.epos[ep_num].status &= TRU_USB_STATUS_EP_HALT_MSK_CLR;
	tru_dwc2_stall_clr_epout(pc->base_addr, ep_num, TRU_DWC2_STALL_CLEAR);  // Data toggle is reset to 0 when SetConfiguration, SetInterface or ClearFeature(ENDPOINT_HALT) is requested
}

void tru_usb_pc_update_lxfer_details_epin(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epi_t *ep = pc->eps.epis + ep_num;

	switch(pc->dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			//ep->xferred_size = ep->xfer_size;
			ep->xferred_size = tru_dwc2_dev_epin_xferred_size(pc->base_addr, ep_num, ep->xfer_size, pc->hwcfg.max_transfer_size);
			ep->lxfer.chunk.buffers[0].xferred_size = ep->xferred_size;
			break;
		case TRU_DWC2_DMA_MODE_SB:
			ep->xferred_size = tru_dwc2_dev_epin_xferred_size(pc->base_addr, ep_num, ep->xfer_size, pc->hwcfg.max_transfer_size);
			ep->lxfer.chunk.buffers[0].xferred_size = ep->xferred_size;

			break;
		case TRU_DWC2_DMA_MODE_SG:
			// Cache coherence maintenance after the SG DMA operation
			// We must invalidate the SG DMA linked-list so we can see the changes (writes) made by the DMA
			#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 0
			#if defined(TRU_CFG_L1C) && TRU_CFG_L1C == 1 || defined(TRU_CFG_L2C) && TRU_CFG_L2C == 1
				tru_usb_pc_cache_inv_sgdma_list(&ep->lxfer);  // Invalidate descriptor list
			#endif
			#endif

			ep->xferred_size = ep->lxfer.chunk.total_xfer_size_aligned - tru_dwc2_sgdma_total_remaining_epin(&ep->lxfer.chunk.sgdma, ep->flow_type) - ep->lxfer.chunk.total_xferred_size;
			ep->lxfer.chunk.buffers[ep->lxfer.chunk.sgdma.curr_index].xferred_size = ep->lxfer.chunk.buffers[ep->lxfer.chunk.sgdma.curr_index].xfer_size_aligned - tru_dwc2_sgdma_curr_remaining_epin(&ep->lxfer.chunk.sgdma, ep->flow_type);
			ep->lxfer.chunk.sgdma.curr_index++;
			break;
	}

	ep->xferred_size_prezlp += ep->xferred_size;
	ep->lxfer.chunk.total_xferred_size += ep->xferred_size;
	ep->lxfer.whole.total_xferred_size += ep->xferred_size;
}

void tru_usb_pc_end_lxfer_details_epin(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_dwc2_clr_intmask_txfifo_empty(pc->base_addr, ep_num);  // Disable interrupt generation from the TX FIFO empty status trigger
}

void tru_usb_pc_update_lxfer_details_epout(tru_usb_pc_t *pc, uint8_t ep_num){
	tru_usb_pc_epo_t *ep = pc->eps.epos + ep_num;

	switch(pc->dma_mode){
		case TRU_DWC2_DMA_MODE_FF:
			ep->xferred_size = ep->rxfifo_xferred_size;
			ep->lxfer.chunk.buffers[0].xferred_size = ep->xferred_size;
			break;
		case TRU_DWC2_DMA_MODE_SB:
			ep->xferred_size = tru_dwc2_dev_epout_xferred_size(pc->base_addr, ep_num, ep->xfer_size, pc->hwcfg.max_transfer_size);
			ep->lxfer.chunk.buffers[0].xferred_size = ep->xferred_size;

			// Cache coherence maintenance after the SB DMA operation
			// We must invalidate the data buffer so we can see the changes (writes) made by the DMA
			#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 0
			#if defined(TRU_CFG_L1C) && TRU_CFG_L1C == 1 || defined(TRU_CFG_L2C) && TRU_CFG_L2C == 1
				tru_usb_pc_cache_inv_sbdma_data_epout(ep);
			#endif
			#endif
			break;
		case TRU_DWC2_DMA_MODE_SG:
			// Cache coherence maintenance after the SG DMA operation
			// We must invalidate the SG DMA linked-list and data buffers so we can see the changes (writes) made by the DMA
			#if defined(TRU_CFG_NONCACHEABLE_SECTION) && TRU_CFG_NONCACHEABLE_SECTION == 0
			#if defined(TRU_CFG_L1C) && TRU_CFG_L1C == 1 || defined(TRU_CFG_L2C) && TRU_CFG_L2C == 1
				tru_usb_pc_cache_inv_sgdma_list(&ep->lxfer);  // Invalidate descriptor list
				tru_usb_pc_cache_inv_sgdma_data(&ep->lxfer);  // Invalidate data buffers
			#endif
			#endif

			ep->xferred_size = ep->lxfer.chunk.total_xfer_size_aligned - tru_dwc2_sgdma_total_remaining_epout(&ep->lxfer.chunk.sgdma, ep->flow_type) - ep->lxfer.chunk.total_xferred_size;
			ep->lxfer.chunk.buffers[ep->lxfer.chunk.sgdma.curr_index].xferred_size = ep->lxfer.chunk.buffers[ep->lxfer.chunk.sgdma.curr_index].xfer_size_aligned - tru_dwc2_sgdma_curr_remaining_epout(&ep->lxfer.chunk.sgdma, ep->flow_type);
			ep->lxfer.chunk.sgdma.curr_index++;
			break;
	}

	ep->xferred_size_prezlp += ep->xferred_size;
	ep->lxfer.chunk.total_xferred_size += ep->xferred_size;
	ep->lxfer.whole.total_xferred_size += ep->xferred_size;
}

void tru_usb_pc_end_lxfer_details_epout(tru_usb_pc_t *pc, uint8_t ep_num){
}

/*
	Handle device setup request error: USB 2.0 specifications says to set stall for next data or status stage.
	Note:
		The stall is automatically cleared by the Synopsys USB2.0 OTG controller so no need to clear it.
 */
void tru_usb_pc_setup_error(tru_usb_pc_t *pc){
#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
	LOG("SETUP: Unknown request, stalling EO0\n");
#endif

	pc->eps.epos[0].control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
	pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;

	tru_usb_lxfer_unregister_buffers(&pc->eps.epis[0].lxfer);
	tru_usb_lxfer_unregister_buffers(&pc->eps.epos[0].lxfer);
	tru_usb_lxfer_register_buffer(&pc->eps.epis[0].lxfer, pc->setup.epin0_buf, pc->setup.epin0_bufsize);
	tru_usb_lxfer_register_buffer(&pc->eps.epos[0].lxfer, pc->setup.epout0_buf, pc->setup.epout0_bufsize);

	pc->eps.epos[0].lxfer.whole.total_exp_size = TRU_USB_SETUP_SIZE;
	pc->eps.epos[0].lxfer.whole.total_xfer_size = TRU_USB_SETUP_SIZE;
	//tru_usb_lxfer_zero_chunk_xfer_size_all(&pc->eps.epos[0].lxfer);
	tru_usb_lxfer_fill_chunk_xfer_size_all_rx(&pc->eps.epos[0].lxfer, TRU_USB_SETUP_SIZE, pc->dma_mode);
	tru_usb_pc_receive_sr(pc, 0, TRU_DWC2_STALL_SET, true);  // Stall and setup USB controller to receive a setup packet on OUT endpoint 0
}

/*
	Activate endpoints in the configuration with the specified configuration number.
*/
void tru_usb_pc_setup_set_config_act_ep(tru_usb_pc_t *pc, tru_usb_config_t *req_config){
	// Activate endpoints in current alternate setting for all interfaces in the configuration
	for(uint16_t i = 0; i < req_config->interfaces.num_interfaces; i++){
#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
		LOG("SETUP: Activating interface %u alt %u\n", TRU_USB_INTERFACE_DESC_PTR(req_config->interfaces.items[i].alt_settings.curr_alt_setting->desc)->interface_num, TRU_USB_INTERFACE_DESC_PTR(req_config->interfaces.items[i].alt_settings.curr_alt_setting->desc)->alt_setting_num);
#endif

		tru_usb_pc_setup_set_interface_act_ep(pc, req_config, req_config->interfaces.items + i, req_config->interfaces.items[i].alt_settings.curr_alt_setting, false);
	}

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

	if(pc->txfifo_mode == TRU_DWC2_TXFIFO_DEDICATED){
		// Reallocate dynamic FIFOs
		tru_dwc2_realloc_fifo(pc->base_addr, pc->dma_mode, epins_mps, epouts_mps, true, pc->hwcfg.num_dev_in_ep, pc->hwcfg.num_dev_ep, pc->hwcfg.fifo_usable_size_words);
	}

	pc->configs.prev_config = pc->configs.curr_config;
	pc->configs.curr_config = req_config;
}

/*
	De-activate endpoints in the current configuration.
*/
void tru_usb_pc_setup_set_config_deact_ep(tru_usb_pc_t *pc){
	//if(pc->configs.curr_spd_configs && pc->configs.curr_config){
	if(pc->configs.curr_config){
		// De-activate endpoints in current alternate setting for all interfaces in the configuration
		for(uint16_t i = 0; i < pc->configs.curr_config->interfaces.num_interfaces; i++){
			tru_usb_pc_setup_set_interface_deact_ep(pc, pc->configs.curr_config->interfaces.items + i);
		}

		pc->configs.prev_config = pc->configs.curr_config;
		//pc->configs.curr_config = NULL;
	}
}

/*
	Activate endpoints in the interface and alternate setting.
*/
void tru_usb_pc_setup_set_interface_act_ep(tru_usb_pc_t *pc, tru_usb_config_t *req_config, tru_usb_interface_t *req_interface, tru_usb_interface_alt_setting_t *req_alt_setting, bool realloc_fifo){
	uint8_t i;
	uint8_t ep_num;
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

	// Extract endpoint information in the interface
	for(i = 0; i < req_alt_setting->eps.num_endpoints; i++){
		// Get endpoint number
		ep_num = req_alt_setting->eps.items[i].desc[2] & 0x0f;

		// Endpoint direction: 0x80 = IN, 0x00 = OUT
		if(req_alt_setting->eps.items[i].desc[2] & 0x80){
			pc->eps.epis[ep_num].status &= TRU_USB_STATUS_EP_HALT_MSK_CLR;  // The endpoint status Halt feature is reset to zero after either a SetConfiguration, SetInterface or ClearFeature(ENDPOINT_HALT) is requested
			pc->eps.epis[ep_num].flow_type = req_alt_setting->eps.items[i].desc[3] & 0x03;
			pc->eps.epis[ep_num].iso_sync_type = req_alt_setting->eps.items[i].desc[3] & 0x0c;
			pc->eps.epis[ep_num].usage_type = req_alt_setting->eps.items[i].desc[3] & 0x30;
			pc->eps.epis[ep_num].mps_limit =  buf_le_to_u16(req_alt_setting->eps.items[i].desc + 4) & 0x7ff;
			pc->eps.epis[ep_num].iso_intr_add_trans = req_alt_setting->eps.items[i].desc[5] & 0x18;
			// Determine interval (in frames)
			switch(pc->eps.epis[ep_num].flow_type){
				case TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS:
					switch(pc->speed){
						case TRU_DWC2_DSTS_ENUMSPD_HS3060:
							pc->eps.epis[ep_num].interval = 0x1 << (req_alt_setting->eps.items[i].desc[6] - 1);
							break;
						case TRU_DWC2_DSTS_ENUMSPD_FS3060:
						case TRU_DWC2_DSTS_ENUMSPD_FS48:
							pc->eps.epis[ep_num].interval = 0x1 << (req_alt_setting->eps.items[i].desc[6] - 1);
							break;
						case TRU_DWC2_DSTS_ENUMSPD_LS6:
						default:
							pc->eps.epis[ep_num].interval = req_alt_setting->eps.items[i].desc[6];  // Isochronous is invalid for low-speed
					}
					break;
				case TRU_USB_EP_TRANSFER_TYPE_INTERRUPT:
					switch(pc->speed){
						case TRU_DWC2_DSTS_ENUMSPD_HS3060:
							pc->eps.epis[ep_num].interval = 0x1 << (req_alt_setting->eps.items[i].desc[6] - 1);
							break;
						case TRU_DWC2_DSTS_ENUMSPD_FS3060:
						case TRU_DWC2_DSTS_ENUMSPD_FS48:
						case TRU_DWC2_DSTS_ENUMSPD_LS6:
						default:
							pc->eps.epis[ep_num].interval = req_alt_setting->eps.items[i].desc[6];
					}
					break;
				default:
					pc->eps.epis[ep_num].interval = req_alt_setting->eps.items[i].desc[6];
			}
			pc->eps.epis[ep_num].frame_target = 0;
			pc->eps.epis[ep_num].iso_incomplete = 0;
			pc->eps.epis[ep_num].mps = tru_dwc2_determine_mps(pc->speed, pc->eps.epis[ep_num].flow_type, pc->eps.epis[ep_num].mps_limit);  // Set the actual allowed max packet size
			epins_mps[ep_num] = pc->eps.epis[ep_num].mps;
		}else{
			pc->eps.epos[ep_num].status &= TRU_USB_STATUS_EP_HALT_MSK_CLR;  // The endpoint status Halt feature is reset to zero after either a SetConfiguration, SetInterface or ClearFeature(ENDPOINT_HALT) is requested
			pc->eps.epos[ep_num].flow_type = req_alt_setting->eps.items[i].desc[3] & 0x03;
			pc->eps.epos[ep_num].iso_sync_type = req_alt_setting->eps.items[i].desc[3] & 0x0c;
			pc->eps.epos[ep_num].usage_type = req_alt_setting->eps.items[i].desc[3] & 0x30;
			pc->eps.epos[ep_num].mps_limit = buf_le_to_u16(req_alt_setting->eps.items[i].desc + 4) & 0x7ff;
			pc->eps.epos[ep_num].iso_intr_add_trans = req_alt_setting->eps.items[i].desc[5] & 0x18;
			// Determine interval (in frames)
			switch(pc->eps.epos[ep_num].flow_type){
				case TRU_USB_EP_TRANSFER_TYPE_ISOCHRONOUS:
					switch(pc->speed){
						case TRU_DWC2_DSTS_ENUMSPD_HS3060:
							pc->eps.epos[ep_num].interval = 0x1 << (req_alt_setting->eps.items[i].desc[6] - 1);
							break;
						case TRU_DWC2_DSTS_ENUMSPD_FS3060:
						case TRU_DWC2_DSTS_ENUMSPD_FS48:
							pc->eps.epos[ep_num].interval = 0x1 << (req_alt_setting->eps.items[i].desc[6] - 1);
							break;
						case TRU_DWC2_DSTS_ENUMSPD_LS6:
						default:
							pc->eps.epis[ep_num].interval = req_alt_setting->eps.items[i].desc[6];  // Isochronous is invalid for low-speed
					}
					break;
				case TRU_USB_EP_TRANSFER_TYPE_INTERRUPT:
					switch(pc->speed){
						case TRU_DWC2_DSTS_ENUMSPD_HS3060:
							pc->eps.epos[ep_num].interval = 0x1 << (req_alt_setting->eps.items[i].desc[6] - 1);
							break;
						case TRU_DWC2_DSTS_ENUMSPD_FS3060:
						case TRU_DWC2_DSTS_ENUMSPD_FS48:
						case TRU_DWC2_DSTS_ENUMSPD_LS6:
						default:
							pc->eps.epos[ep_num].interval = req_alt_setting->eps.items[i].desc[6];
					}
					break;
				default:
					pc->eps.epos[ep_num].interval = req_alt_setting->eps.items[i].desc[6];
			}
			pc->eps.epos[ep_num].frame_target = 0;
			pc->eps.epos[ep_num].iso_incomplete = 0;
			pc->eps.epos[ep_num].mps = tru_dwc2_determine_mps(pc->speed, pc->eps.epos[ep_num].flow_type, pc->eps.epos[ep_num].mps_limit);  // Set the actual allowed max packet size
			pc->eps.epos[ep_num].control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
			epouts_mps[ep_num] = pc->eps.epos[ep_num].mps;
		}
	}

	// Reallocate dynamic FIFOs
	if(realloc_fifo) tru_dwc2_realloc_fifo(pc->base_addr, pc->dma_mode, epins_mps, epouts_mps, true, pc->hwcfg.num_dev_in_ep, pc->hwcfg.num_dev_ep, pc->hwcfg.fifo_usable_size_words);

	// Activate the endpoints in the interfaces
	for(i = 0; i < req_alt_setting->eps.num_endpoints; i++){
		// Get endpoint number
		ep_num = req_alt_setting->eps.items[i].desc[2] & 0x0f;

		// Endpoint direction: 0x80 = IN, 0x00 = OUT
		if(req_alt_setting->eps.items[i].desc[2] & 0x80){
			// Activate the endpoint. Data toggle is reset to 0 when SetConfiguration, SetInterface or ClearFeature(ENDPOINT_HALT) is requested
			tru_dwc2_activate_epin(
				pc->base_addr,
				ep_num,
				pc->eps.epis[ep_num].flow_type,
				pc->eps.epis[ep_num].mps,
				(pc->eps.epis[ep_num].status & TRU_USB_STATUS_EP_HALT_MSK_SET) ? TRU_DWC2_STALL_SET : TRU_DWC2_STALL_CLEAR,
				TRU_DWC2_NAK_SET,
				TRU_DWC2_DPID_EO_D0_OR_EVEN
			);

			pc->eps.epis[ep_num].is_activated = true;
			pc->eps.epis[ep_num].curr_alt_setting = req_alt_setting;
		}else{
			// Activate the endpoint. Data toggle is reset to 0 when SetConfiguration, SetInterface or ClearFeature(ENDPOINT_HALT) is requested
			tru_dwc2_activate_epout(
				pc->base_addr,
				ep_num,
				pc->eps.epos[ep_num].flow_type,
				pc->eps.epos[ep_num].mps,
				(pc->eps.epos[ep_num].status & TRU_USB_STATUS_EP_HALT_MSK_SET) ? TRU_DWC2_STALL_SET : TRU_DWC2_STALL_CLEAR,
				TRU_DWC2_NAK_SET,
				TRU_DWC2_DPID_EO_D0_OR_EVEN
			);

			pc->eps.epos[ep_num].is_activated = true;
			pc->eps.epos[ep_num].curr_alt_setting = req_alt_setting;
		}
	}

	req_interface->alt_settings.prev_alt_setting = req_interface->alt_settings.curr_alt_setting;
	req_interface->alt_settings.curr_alt_setting = req_alt_setting;
}

/*
	De-activate endpoints in the alternate setting.
*/
void tru_usb_pc_setup_set_interface_deact_ep(tru_usb_pc_t *pc, tru_usb_interface_t *req_interface){
	uint8_t i;
	uint8_t ep_num;

	if(req_interface->alt_settings.curr_alt_setting){
		// Iterate endpoints of the interface
		for(i = 0; i < req_interface->alt_settings.curr_alt_setting->eps.num_endpoints; i++){
			// Get endpoint number
			ep_num = req_interface->alt_settings.curr_alt_setting->eps.items[i].desc[2] & 0x0f;

			// Endpoint direction: 0x80 = IN, 0x00 = OUT
			if(req_interface->alt_settings.curr_alt_setting->eps.items[i].desc[2] & 0x80){
				// Disable IN endpoint
				//tru_dwc2_deactivate_epin(pc->base_addr, ep_num);
				tru_dwc2_disable_epin_wait(pc->base_addr, ep_num, false);
				//tru_dwc2_flush_txfifo_wait(pc->base_addr, ep_num);
				tru_usb_pc_end_lxfer_details_epin(pc, ep_num);

				pc->eps.epis[ep_num].status &= TRU_USB_STATUS_EP_HALT_MSK_CLR;  // The endpoint status Halt feature is reset to zero after either a SetConfiguration or SetInterface setup request
				pc->eps.epis[ep_num].mps = 0;
				pc->eps.epis[ep_num].is_activated = false;
				//pc->eps.epis[ep_num].curr_alt_setting = NULL;
			}else{
				// Disable OUT endpoint
				//tru_dwc2_deactivate_epout(pc->base_addr, ep_num, pc->dma_mode);
				//tru_dwc2_disable_epout_forced(pc->base_addr, ep_num, pc->dma_mode, false);
				tru_dwc2_disable_epout_wait(pc->base_addr, ep_num, pc->dma_mode, false);
				tru_usb_pc_end_lxfer_details_epout(pc, ep_num);

				pc->eps.epos[ep_num].status &= TRU_USB_STATUS_EP_HALT_MSK_CLR;  // The endpoint status Halt feature is reset to zero after either a SetConfiguration or SetInterface setup request
				pc->eps.epos[ep_num].mps = 0;
				pc->eps.epos[ep_num].is_activated = false;
				//pc->eps.epos[ep_num].curr_alt_setting = NULL;
			}
		}
		//tru_dwc2_flush_rxfifo_wait(pc->base_addr);

		req_interface->alt_settings.prev_alt_setting = req_interface->alt_settings.curr_alt_setting;
		//req_interface->alt_settings.curr_alt_setting = NULL;
	}
}

/*
	Callback the interface function for the configuration
*/
void tru_usb_pc_setup_callback_config_interfaces(tru_usb_pc_t *pc, tru_usb_config_t *config, bool is_activated){
	for(uint16_t intf_i = 0; intf_i < config->interfaces.num_interfaces; intf_i++){
		// Call back device class functions
		for(uint32_t dc_i = 0; dc_i < pc->num_dc_info; dc_i++){
			if(pc->dc_info[dc_i].callbacks->cb_itf_act){
				pc->dc_info[dc_i].callbacks->cb_itf_act(pc->dc_info[dc_i].devclass, config->interfaces.items[intf_i].alt_settings.curr_alt_setting, is_activated);  // Call the USB device class specific callback function
			}
		}
	}
}

/*
	Callback the activated endpoint function for the configuration
*/
void tru_usb_pc_setup_callback_config_ep_act(tru_usb_pc_t *pc, tru_usb_config_t *config){
	// De-activate or activate endpoints in current alternate setting for all interfaces in the configuration
	for(uint16_t i = 0; i < config->interfaces.num_interfaces; ++i){
		tru_usb_pc_setup_callback_interface_ep_act(pc, config->interfaces.items[i].alt_settings.curr_alt_setting);
	}
}

/*
	Callback the de-activated endpoint function for the configuration
*/
void tru_usb_pc_setup_callback_config_ep_deact(tru_usb_pc_t *pc, tru_usb_config_t *config){
	// De-activate or activate endpoints in current alternate setting for all interfaces in the configuration
	for(uint16_t i = 0; i < config->interfaces.num_interfaces; ++i){
		tru_usb_pc_setup_callback_interface_ep_deact(pc, config->interfaces.items[i].alt_settings.curr_alt_setting);
	}
}

/*
	Callback the activated endpoint functions for the interface
*/
void tru_usb_pc_setup_callback_interface_ep_act(tru_usb_pc_t *pc, tru_usb_interface_alt_setting_t *alt_setting){
	uint8_t i;
	uint8_t ep_num;

	// Iterate endpoints of the interface
	for(i = 0; i < alt_setting->eps.num_endpoints; i++){
		// Get endpoint number
		ep_num = alt_setting->eps.items[i].desc[2] & 0x0f;

		// Endpoint direction: 0x80 = IN, 0x00 = OUT
		if(alt_setting->eps.items[i].desc[2] & 0x80){
			// Call back device class functions
			for(uint32_t i = 0; i < pc->num_dc_info; i++){
				if(pc->dc_info[i].callbacks->cb_epi_act[ep_num]){
					pc->dc_info[i].callbacks->cb_epi_act[ep_num](pc->dc_info[i].devclass, &pc->eps.epis[ep_num], true);  // Call the USB device class specific callback function
				}
			}
		}else{
			// Call back device class functions
			for(uint32_t i = 0; i < pc->num_dc_info; i++){
				if(pc->dc_info[i].callbacks->cb_epo_act[ep_num]){
					pc->dc_info[i].callbacks->cb_epo_act[ep_num](pc->dc_info[i].devclass, &pc->eps.epos[ep_num], true);  // Call the USB device class specific callback function
				}
			}
		}
	}
}

/*
	Callback the de-activated endpoint functions for the interface
*/
void tru_usb_pc_setup_callback_interface_ep_deact(tru_usb_pc_t *pc, tru_usb_interface_alt_setting_t *alt_setting){
	uint8_t i;
	uint8_t ep_num;

	// Iterate endpoints of the interface
	for(i = 0; i < alt_setting->eps.num_endpoints; i++){
		// Get endpoint number
		ep_num = alt_setting->eps.items[i].desc[2] & 0x0f;

		// Endpoint direction: 0x80 = IN, 0x00 = OUT
		if(alt_setting->eps.items[i].desc[2] & 0x80){
			// Call back device class functions
			for(uint32_t i = 0; i < pc->num_dc_info; i++){
				if(pc->dc_info[i].callbacks->cb_epi_act[ep_num]){
					pc->dc_info[i].callbacks->cb_epi_act[ep_num](pc->dc_info[i].devclass, &pc->eps.epis[ep_num], false);  // Call the USB device class specific callback function
				}
			}
		}else{
			// Call back device class functions
			for(uint32_t i = 0; i < pc->num_dc_info; i++){
				if(pc->dc_info[i].callbacks->cb_epo_act[ep_num]){
					pc->dc_info[i].callbacks->cb_epo_act[ep_num](pc->dc_info[i].devclass, &pc->eps.epos[ep_num], false);  // Call the USB device class specific callback function
				}
			}
		}
	}
}

/*
	Handle device setup SET_CONFIG request.

	Notes:
		Does nothing if the requested configuration was already previously set.
		A requested configuration number of zero will de-configure and set state to USB address.
*/
bool tru_usb_pc_setup_set_config(tru_usb_pc_t *pc){
	uint8_t req_config_num = pc->setup.request.value & 0xff;
	tru_usb_config_t *req_config = tru_usb_configs_find_config(pc->configs.curr_spd_configs, req_config_num);

	// USB is already in configured state?
	switch(pc->state){
		case TRU_USB_PC_DEV_STATE_CONFIGURED:
			if(req_config){
				if(req_config_num != pc->configs.curr_config->config_num){
					// De-configure
					tru_usb_pc_setup_set_config_deact_ep(pc);  // Need to deactivate current (now old) configuration endpoints first
					pc->state = TRU_USB_PC_DEV_STATE_ADDRESS;

					// Callbacks
					tru_usb_pc_setup_callback_config_ep_deact(pc, pc->configs.prev_config);  // EP callbacks
					tru_usb_pc_setup_callback_config_interfaces(pc, pc->configs.prev_config, false);  // Interface callbacks
					for(uint32_t i = 0; i < pc->num_dc_info; i++){
						if(pc->dc_info[i].callbacks->cb_cfg_act){
							pc->dc_info[i].callbacks->cb_cfg_act(pc->dc_info[i].devclass, pc->configs.prev_config, false);  // Config callbacks
						}
					}

					// Is configuration number greater than zero?
					if(req_config_num > 0){
						tru_usb_pc_setup_set_config_act_ep(pc, req_config);  // Process requested configuration and activate all its endpoints
						pc->state = TRU_USB_PC_DEV_STATE_CONFIGURED;

						// Callbacks
						for(uint32_t i = 0; i < pc->num_dc_info; i++){
							if(pc->dc_info[i].callbacks->cb_cfg_act){
								pc->dc_info[i].callbacks->cb_cfg_act(pc->dc_info[i].devclass, pc->configs.curr_config, true);  // Config callbacks
							}
						}
						tru_usb_pc_setup_callback_config_interfaces(pc, pc->configs.curr_config, true);  // Interface callbacks
						tru_usb_pc_setup_callback_config_ep_act(pc, pc->configs.curr_config);  // EP callbacks
					}
				}

				return true;
			}

			break;
		case TRU_USB_PC_DEV_STATE_DEFAULT:
		case TRU_USB_PC_DEV_STATE_ADDRESS:
			// Note: There is nothing to de-configure in this USB state

			if(req_config){
				// Is configuration number greater than zero?
				if(req_config_num > 0){
					tru_usb_pc_setup_set_config_act_ep(pc, req_config);  // Process requested configuration and activate all its endpoints
					pc->state = TRU_USB_PC_DEV_STATE_CONFIGURED;

					// Callbacks
					for(uint32_t i = 0; i < pc->num_dc_info; i++){
						if(pc->dc_info[i].callbacks->cb_cfg_act){
							pc->dc_info[i].callbacks->cb_cfg_act(pc->dc_info[i].devclass, pc->configs.curr_config, true);  // Config callbacks
						}
					}
					tru_usb_pc_setup_callback_config_interfaces(pc, pc->configs.curr_config, true);  // Interface callbacks
					tru_usb_pc_setup_callback_config_ep_act(pc, pc->configs.curr_config);  // EP callbacks

					return true;
				}
			}

			break;
		default:
	}

	return false;
}

/*
	Handle device setup SET_INTERFACE request.
*/
bool tru_usb_pc_setup_set_interface(tru_usb_pc_t *pc){
	uint8_t req_interface_num;
	uint8_t req_alt_setting_num;
	tru_usb_interface_t *req_interface;
	tru_usb_interface_alt_setting_t *req_alt_setting;

	// USB was in configured state?
	if(pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
		req_interface_num = pc->setup.request.index & 0xff;
		req_alt_setting_num = pc->setup.request.value & 0xff;
		req_interface = tru_usb_interfaces_find_interface(&pc->configs.curr_config->interfaces, req_interface_num);

		if(req_interface){
			req_alt_setting = tru_usb_interfaces_find_alt_setting(req_interface, req_alt_setting_num);

			if(req_alt_setting){
				tru_usb_pc_setup_set_interface_deact_ep(pc, req_interface);  // Need to deactivate current (now old) interface endpoints first

				tru_usb_pc_setup_callback_interface_ep_deact(pc, req_interface->alt_settings.prev_alt_setting);
				// Call back device class functions
				for(uint32_t dc_i = 0; dc_i < pc->num_dc_info; dc_i++){
					if(pc->dc_info[dc_i].callbacks->cb_itf_act){
						pc->dc_info[dc_i].callbacks->cb_itf_act(pc->dc_info[dc_i].devclass, req_interface->alt_settings.prev_alt_setting, false);  // Call the USB device class specific callback function
					}
				}

				tru_usb_pc_setup_set_interface_act_ep(pc, pc->configs.curr_config, req_interface, req_alt_setting, true);  // Process requested interface and activate all its endpoints

				// Call back device class functions
				for(uint32_t dc_i = 0; dc_i < pc->num_dc_info; dc_i++){
					if(pc->dc_info[dc_i].callbacks->cb_itf_act){
						pc->dc_info[dc_i].callbacks->cb_itf_act(pc->dc_info[dc_i].devclass, req_alt_setting, true);  // Call the USB device class specific callback function
					}
				}
				tru_usb_pc_setup_callback_config_ep_act(pc, pc->configs.curr_config);
			}
		}

		return true;
	}

	return false;
}

/*
	Device standard setup request handler for request type:
		direction: host-to-device
		type     : standard
		recipient: device
*/
bool tru_usb_pc_setup_hd_std_dev(tru_usb_pc_t *pc){
	// Process the setup request
	switch(pc->setup.request.code){
		case TRU_USB_SETUP_REQ_CLEAR_FEATURE:
			switch(pc->setup.request.value){
				case TRU_USB_FEAT_DEV_REMOTE_WAKEUP:
					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: CLEAR_FEATURE REMOTE_WAKEUP\n");
#endif

							if(
								((pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS) && pc->setup.request.index == 0) ||
								pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED
							){
								pc->status &= TRU_USB_STATUS_DEV_REMOTE_WAKEUP_MSK_CLR;  // Clear USB device status remote wakeup bit
								return true;
							}
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
					break;
				case TRU_USB_FEAT_TEST_MODE:
					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: CLEAR_FEATURE TEST_MODE\n");
#endif

							tru_dwc2_clr_dev_test_mode(pc->base_addr);
							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_REQ_SET_FEATURE:
			switch(pc->setup.request.value){
				case TRU_USB_FEAT_DEV_REMOTE_WAKEUP:
					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: SET_FEATURE REMOTE_WAKEUP\n");
#endif

							if(
								((pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS) && pc->setup.request.index == 0) ||
								pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED
							){
								pc->status |= TRU_USB_STATUS_DEV_REMOTE_WAKEUP_MSK_SET;  // Set USB device status remote wakeup bit
								return true;
							}
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
					break;

				case TRU_USB_FEAT_TEST_MODE:
					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: SET_FEATURE TEST_MODE\n");
#endif

							tru_dwc2_set_dev_test_mode(pc->base_addr, pc->setup.request.index >> 8);
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_REQ_SET_ADDRESS:
			switch(pc->eps.epos[0].control_stage){
				case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
					pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
					LOG("REQ: SET_ADDRESS %u\n", pc->setup.request.value);
#endif

					if(pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
						tru_dwc2_set_address(pc->base_addr, pc->setup.request.value);  // Set USB address
						if(pc->setup.request.value != 0) pc->state = TRU_USB_PC_DEV_STATE_ADDRESS;
						return true;
					}
					break;
				case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
					pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
					return true;
					break;
			}
			break;

		case TRU_USB_SETUP_REQ_SET_CONFIGURATION:
			switch(pc->eps.epos[0].control_stage){
				case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
					pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
					LOG("REQ: SET_CONFIGURATION %u\n", pc->setup.request.value & 0xff);
#endif

					// Try to set (select) the configuration
					if(tru_usb_pc_setup_set_config(pc)){
						return true;
					}
					break;
				case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
					pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
					return true;
					break;
			}
			break;
	}

	return false;
}

/*
	Device standard setup request handler for request type:
		direction: host-to-device
		type     : standard
		recipient: interface
*/
bool tru_usb_pc_setup_hd_std_intf(tru_usb_pc_t *pc){
	// Process the setup request
	switch(pc->setup.request.code){
		case TRU_USB_SETUP_REQ_CLEAR_FEATURE:
			// No feature flags defined in standard setup request
			break;

		case TRU_USB_SETUP_REQ_SET_FEATURE:
			// No feature flags defined in standard setup request
			break;

		case TRU_USB_SETUP_REQ_SET_INTERFACE:
			switch(pc->eps.epos[0].control_stage){
				case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
					pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
					LOG("REQ: SET_INTERFACE %u ALT %u\n", pc->setup.request.index & 0xff, pc->setup.request.value & 0xff);
#endif

					// Try to set (select) the interface
					if(tru_usb_pc_setup_set_interface(pc)){
						return true;
					}
					break;
				case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
					pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
					return true;
					break;
			}
			break;
	}

	return false;
}

/*
	Device standard setup request handler for request type:
		direction: host-to-device
		type     : standard
		recipient: endpoint
*/
bool tru_usb_pc_setup_hd_std_ep(tru_usb_pc_t *pc){
	// Process the setup request
	switch(pc->setup.request.code){
		case TRU_USB_SETUP_REQ_CLEAR_FEATURE:
			switch(pc->setup.request.value){
				case TRU_USB_FEAT_EP_HALT:
					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: CLEAR_FEATURE EP_HALT 0x%.2x\n", pc->setup.request.index & 0xff);
#endif

							if(
								((pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS) && pc->setup.request.index == 0) ||
								pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED
							){
								// Endpoint direction: 0x80 = IN, 0x00 = OUT
								if(pc->setup.request.index & 0x80){
									tru_usb_pc_unhalt_epin(pc, pc->setup.request.index & 0x0f);  // Clear halt (clear stall & status flag) on the specified endpoint
								}else{
									tru_usb_pc_unhalt_epout(pc, pc->setup.request.index & 0x0f);  // Clear halt (clear stall & status flag) on the specified endpoint
								}
								pc->status &= TRU_USB_STATUS_EP_HALT_MSK_CLR;  // Clear USB device status HALT bit
								return true;
							}
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_REQ_SET_FEATURE:
			switch(pc->setup.request.value){
				case TRU_USB_FEAT_EP_HALT:
					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: SET_FEATURE EP_HALT 0x%.2x\n", pc->setup.request.index & 0xff);
#endif

							if(
								((pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS) && pc->setup.request.index == 0) ||
								pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED
							){
								// Endpoint direction: 0x80 = IN, 0x00 = OUT
								if(pc->setup.request.index & 0x80){
									tru_usb_pc_halt_epin(pc, pc->setup.request.index & 0x0f);  // Set halt (set stall & status flag) on the specified endpoint
								}else{
									tru_usb_pc_halt_epout(pc, pc->setup.request.index & 0x0f);  // Set halt (set stall & status flag) on the specified endpoint
								}
								pc->status |= TRU_USB_STATUS_EP_HALT_MSK_SET;  // Set USB device status HALT bit
								return true;
							}
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
					break;
			}
			break;
	}

	return false;
}

/*
	Device standard setup request handler for request type:
		direction: device-to-host
		type     : standard
		recipient: device
*/
bool tru_usb_pc_setup_dh_std_dev(tru_usb_pc_t *pc){
	uint8_t req_config_index;
	uint8_t req_desc_type;
	uint8_t *desc;
	uint32_t xfer_size;

	// Process the setup request
	switch(pc->setup.request.code){
		case TRU_USB_SETUP_REQ_GET_STATUS:
			if(pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				if(pc->setup.request.length == 2){
					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

							// Transmit status
							TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, &pc->status, 2);
							tru_usb_pc_transmit(pc, 0, 2, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: GET_STATUS LEN %u\n", pc->setup.request.length);
#endif

							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;

		case TRU_USB_SETUP_REQ_GET_DESCRIPTOR:
			req_desc_type = (pc->setup.request.value >> 8) & 0xff;

			switch(req_desc_type){
				case TRU_USB_DESC_TYPE_DEVICE:
					if(pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
						if(pc->setup.request.length > 0){
							if(pc->dev.curr_spd_dev_desc){
								switch(pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

										// Transmit device descriptor
										xfer_size = (pc->setup.request.length > sizeof(tru_usb_dev_desc_t)) ? sizeof(tru_usb_dev_desc_t) : pc->setup.request.length;
										TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, pc->dev.curr_spd_dev_desc, xfer_size);
										tru_usb_pc_transmit(pc, 0, xfer_size, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
										LOG("REQ: GET_DESCRIPTOR DEVICE LEN %u\n", pc->setup.request.length);
#endif

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
							}
						}
					}
					break;

				case TRU_USB_DESC_TYPE_DEVICE_QUAL:
					if(pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
						if(pc->setup.request.length > 0 && pc->dev.curr_spd_dev_qual_desc){
							if(pc->dev.curr_spd_dev_qual_desc){
								switch(pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

										// Transmit device qualifier descriptor
										xfer_size = (pc->setup.request.length > sizeof(tru_usb_dev_qual_desc_t)) ? sizeof(tru_usb_dev_qual_desc_t) : pc->setup.request.length;
										TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, pc->dev.curr_spd_dev_qual_desc, xfer_size);
										tru_usb_pc_transmit(pc, 0, xfer_size, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
										LOG("REQ: GET_DESCRIPTOR DEVICE_QUALIFIER LEN %u\n", pc->setup.request.length);
#endif

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
							}
						}
					}
					break;

				case TRU_USB_DESC_TYPE_CONFIG:
					if(pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
						if(pc->setup.request.length > 0){
							req_config_index = pc->setup.request.value & 0xff;
							if(req_config_index < pc->configs.curr_spd_configs->num_elements){
								if(pc->configs.curr_spd_configs){
									switch(pc->eps.epos[0].control_stage){
										case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
											pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

											// Transmit configuration descriptor
											xfer_size = (pc->setup.request.length > pc->configs.curr_spd_configs->items[req_config_index].size) ? pc->configs.curr_spd_configs->items[req_config_index].size : pc->setup.request.length;
											TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, pc->configs.curr_spd_configs->items[req_config_index].desc, xfer_size);
											tru_usb_pc_transmit(pc, 0, xfer_size, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
											LOG("REQ: GET_DESCRIPTOR CONFIGURATION LEN %u\n", pc->setup.request.length);
#endif

											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
											pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
											return true;
											break;
										case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
											pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
											return true;
											break;
									}
								}
							}
						}
					}
					break;

				case TRU_USB_DESC_TYPE_OTHER_SPEED_CONFIG:
					if(pc->speed == TRU_DWC2_DSTS_ENUMSPD_HS3060){
						if(pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
							if(pc->setup.request.length > 0){
								req_config_index = pc->setup.request.value & 0xff;
								if(req_config_index < pc->configs.other_spd_configs->num_elements){
									if(pc->configs.other_spd_configs){
										switch(pc->eps.epos[0].control_stage){
											case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
												pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

												// Transmit other speed configuration descriptor
												xfer_size = (pc->setup.request.length > pc->configs.other_spd_configs->items[req_config_index].size) ? pc->configs.other_spd_configs->items[req_config_index].size : pc->setup.request.length;
												TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, pc->configs.other_spd_configs->items[req_config_index].desc, xfer_size);
												w8_unaligned(pc->eps.epis[0].lxfer.chunk.buffers[0].buf + 1, TRU_USB_DESC_TYPE_OTHER_SPEED_CONFIG);
												tru_usb_pc_transmit(pc, 0, xfer_size, pc->setup.request.length);

	#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
												LOG("REQ: GET_DESCRIPTOR OTHER_SPEED_CONFIG LEN %u\n", pc->setup.request.length);
	#endif

												return true;
												break;
											case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
												pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
												return true;
												break;
											case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
												pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
												return true;
												break;
										}
									}
								}
							}
						}
					}
					break;

				case TRU_USB_DESC_TYPE_STRING:
					if(pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS || pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
						if(pc->setup.request.length > 0){
							desc = tru_usb_strings_find_desc(&pc->strings, pc->setup.request.value & 0xff, pc->setup.request.index);
							if(desc){
								switch(pc->eps.epos[0].control_stage){
									case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

										// Transmit string descriptor
										xfer_size = (pc->setup.request.length > desc[0]) ? desc[0] : pc->setup.request.length;
										TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, desc, xfer_size);
										tru_usb_pc_transmit(pc, 0, xfer_size, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
										LOG("REQ: GET_DESCRIPTOR STRING INDEX %u LANGID 0x%.4x LEN %u\n", pc->setup.request.value & 0xff, pc->setup.request.index, pc->setup.request.length);
#endif

										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
										return true;
										break;
									case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
										pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
										return true;
										break;
								}
							}
						}
					}
					break;
			}
			break;

		case TRU_USB_SETUP_REQ_GET_CONFIGURATION:
			if(pc->setup.request.length == 1){
				switch(pc->state){
					case TRU_USB_PC_DEV_STATE_DEFAULT:
					case TRU_USB_PC_DEV_STATE_ADDRESS:
						switch(pc->eps.epos[0].control_stage){
							case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

								// Transmit selected configuration number
								w8_unaligned(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, 0);  // Set configuration number 0 to 32 bit aligned buffer
								tru_usb_pc_transmit(pc, 0, 1, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
								if(pc->eps.epos[0].control_stage == TRU_USB_PC_EP_CONTROL_STAGE_SETUP){
									LOG("REQ: GET_CONFIGURATION LEN %u\n", pc->setup.request.length);
								}
#endif

								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
								return true;
								break;
						}
						break;
					case TRU_USB_PC_DEV_STATE_CONFIGURED:
						switch(pc->eps.epos[0].control_stage){
							case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

								// Transmit selected configuration number
								TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, &TRU_USB_CONFIG_DESC_PTR(pc->configs.curr_config->desc)->config_num, 1);
								tru_usb_pc_transmit(pc, 0, 1, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
								if(pc->eps.epos[0].control_stage == TRU_USB_PC_EP_CONTROL_STAGE_SETUP){
									LOG("REQ: GET_CONFIGURATION LEN %u\n", pc->setup.request.length);
								}
#endif

								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
								return true;
								break;
						}
						break;
					default:
				}
			}
			break;
	}

	return false;
}

/*
	Device standard setup request handler for request type:
		direction: device-to-host
		type     : standard
		recipient: interface
*/
bool tru_usb_pc_setup_dh_std_intf(tru_usb_pc_t *pc){
	uint8_t req_interface_num;
	tru_usb_interface_t *req_interface;

	// Process the setup request
	switch(pc->setup.request.code){
		case TRU_USB_SETUP_REQ_GET_STATUS:
			if(
				((pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS) && pc->setup.request.index == 0) ||
				pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED
			){
				if(pc->setup.request.length == 2){
					req_interface_num = pc->setup.request.index & 0xff;
					req_interface = tru_usb_interfaces_find_interface(&pc->configs.curr_config->interfaces, req_interface_num);

					if(req_interface){
						switch(pc->eps.epos[0].control_stage){
							case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

								// Transmit status
								TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, &req_interface->alt_settings.curr_alt_setting->status, 2);
								tru_usb_pc_transmit(pc, 0, 2, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
								LOG("REQ: GET_STATUS INTERFACE %u ALT %u LEN %u\n", req_interface_num, TRU_USB_INTERFACE_DESC_PTR(req_interface->alt_settings.curr_alt_setting->desc)->alt_setting_num, pc->setup.request.length);
#endif

								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
								return true;
								break;
						}
					}
				}
			}
			break;

		case TRU_USB_SETUP_REQ_GET_INTERFACE:
			if(pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				if(pc->setup.request.length == 1){
					req_interface_num = pc->setup.request.index & 0xff;
					req_interface = tru_usb_interfaces_find_interface(&pc->configs.curr_config->interfaces, req_interface_num);

					if(req_interface){
						switch(pc->eps.epos[0].control_stage){
							case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
								if(pc->eps.epos[0].control_stage == TRU_USB_PC_EP_CONTROL_STAGE_SETUP){
									LOG("REQ: GET_INTERFACE LEN %u\n", pc->setup.request.length);
								}
#endif

								TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, req_interface->alt_settings.curr_alt_setting->desc + 3, 1);  // Copy alternate setting number
								tru_usb_pc_transmit(pc, 0, 1, pc->setup.request.length);
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
								return true;
								break;
						}
					}
				}
			}
			break;
	}

	return false;
}

/*
	Device standard setup request handler for request type:
		direction: device-to-host
		type     : standard
		recipient: endpoint
*/
bool tru_usb_pc_setup_dh_std_ep(tru_usb_pc_t *pc){
	uint8_t ep_num;
	uint16_t frame_num;
	uint32_t xfer_size;

	// Process the setup request
	switch(pc->setup.request.code){
		case TRU_USB_SETUP_REQ_GET_STATUS:
			if(
				((pc->state == TRU_USB_PC_DEV_STATE_DEFAULT || pc->state == TRU_USB_PC_DEV_STATE_ADDRESS) && pc->setup.request.index == 0) ||
				pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED
			){
				if(pc->setup.request.length == 2){
					ep_num = pc->setup.request.index & 0x0f;

					switch(pc->eps.epos[0].control_stage){
						case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

							xfer_size = 2;
							TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, (pc->setup.request.index & 0x80) ? &pc->eps.epis[ep_num].status : &pc->eps.epos[ep_num].status, xfer_size);
							tru_usb_pc_transmit(pc, 0, xfer_size, pc->setup.request.length);

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
							LOG("REQ: GET_STATUS EP %u LEN %u\n", ep_num, pc->setup.request.length);
#endif

							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
							return true;
							break;
						case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
							pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
							return true;
							break;
					}
				}
			}
			break;

		case TRU_USB_SETUP_REQ_SYNCH_FRAME:
			if(pc->state == TRU_USB_PC_DEV_STATE_CONFIGURED){
				if(pc->setup.request.length == 2){
					// SYNCH_FRAME is meant for Isochronous control flow

					// Must be length 2 and IN Endpoint
					if(pc->setup.request.length == 2 && (pc->setup.request.index & 0x80)){
						ep_num = pc->setup.request.index & 0x0f;

						// High-speed?
						if(pc->speed == TRU_DWC2_DSTS_ENUMSPD_HS3060){
							frame_num = pc->eps.epis[ep_num].frame_target / 8;  // Convert microframe to frame
						}else{
							frame_num = pc->eps.epis[ep_num].frame_target;
						}

						switch(pc->eps.epos[0].control_stage){
							case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN;

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
								LOG("REQ: SYNC_FRAME EP %u LEN %u\n", ep_num, pc->setup.request.length);
#endif

								xfer_size = 2;
								TRU_MEMCPY(pc->eps.epis[0].lxfer.chunk.buffers[0].buf, &frame_num, xfer_size);
								tru_usb_pc_transmit(pc, 0, xfer_size, pc->setup.request.length);
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT;
								return true;
								break;
							case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
								pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
								return true;
								break;
						}
					}
				}
			}
			break;
	}

	return false;
}

/*
	Common standard device setup request handler.
*/
void tru_usb_pc_setup_handler(tru_usb_pc_t *pc){
	pc->setup.is_handled = false;

/*
#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
	switch(pc->eps.epos[0].control_stage){
		case TRU_USB_PC_EP_CONTROL_STAGE_SETUP: LOG("STAGE: SETUP\n"); break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN: LOG("STAGE: DATA IN\n");break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT: LOG("STAGE: STATUS OUT\n"); break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT: LOG("STAGE: DATA OUT\n"); break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN: LOG("STAGE: STATUS IN\n"); break;
		default: LOG("STAGE: INVALID\n");
	}
#endif
*/

	// Parse the setup packet request bytes into variable's member fields
	switch(pc->eps.epos[0].control_stage){
		case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
			#if defined(TRU_CFG_USB_LOG_SETUP_BYTES) && TRU_CFG_USB_LOG_SETUP_BYTES == 1
				LOG("REQ: ");
				for(uint32_t i = 0; i < TRU_USB_SETUP_SIZE; i++){
					LOG("%.2x", rd8_unaligned((uint8_t *)pc->eps.epos[0].lxfer.chunk.buffers[0].buf + i));
				}
				LOG("\n");
			#endif

			tru_usb_setup_parse(&pc->setup, pc->eps.epos[0].lxfer.chunk.buffers[0].buf);

			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
			#if defined(TRU_CFG_USB_LOG_SETUP_BYTES) && TRU_CFG_USB_LOG_SETUP_BYTES == 1
				LOG("RXD: ");
				for(uint32_t i = 0; i < pc->eps.epos[0].lxfer.chunk.total_xferred_size; i++){
					LOG("%.2x", rd8_unaligned((uint8_t *)pc->eps.epos[0].lxfer.chunk.buffers[0].buf + i));
				}
				LOG(" LEN: %lu\n", pc->eps.epos[0].lxfer.chunk.total_xferred_size);
			#endif

			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
			#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
				LOG("RXD: ZLP\n");
			#endif

			break;
		default:
	}

	// What request direction?
	switch(pc->setup.request.type.bits.dir){
		case TRU_USB_SETUP_DIR_HOST2DEV:  // Host to device direction
			// What request type?
			switch(pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: pc->setup.is_handled = pc->setup.is_handled | tru_usb_pc_setup_hd_std_dev(pc); break;
						case TRU_USB_SETUP_RCP_INTERFACE: pc->setup.is_handled = pc->setup.is_handled | tru_usb_pc_setup_hd_std_intf(pc); break;
						case TRU_USB_SETUP_RCP_ENDPOINT: pc->setup.is_handled = pc->setup.is_handled | tru_usb_pc_setup_hd_std_ep(pc); break;
					}
					break;
			}
			break;

		case TRU_USB_SETUP_DIR_DEV2HOST:  // Device to host direction
			// What request type?
			switch(pc->setup.request.type.bits.type){
				case TRU_USB_SETUP_TYPE_STANDARD:
					// What recipient?
					switch(pc->setup.request.type.bits.recipient){
						case TRU_USB_SETUP_RCP_DEVICE: pc->setup.is_handled = pc->setup.is_handled | tru_usb_pc_setup_dh_std_dev(pc); break;
						case TRU_USB_SETUP_RCP_INTERFACE: pc->setup.is_handled = pc->setup.is_handled | tru_usb_pc_setup_dh_std_intf(pc); break;
						case TRU_USB_SETUP_RCP_ENDPOINT: pc->setup.is_handled = pc->setup.is_handled | tru_usb_pc_setup_dh_std_ep(pc); break;
					}
					break;
			}
			break;
	}

	// Call back device class functions
	for(uint32_t i = 0; i < pc->num_dc_info; i++){
		if(pc->dc_info[i].callbacks->cb_setup){
			pc->setup.is_handled = pc->setup.is_handled | pc->dc_info[i].callbacks->cb_setup(pc->dc_info[i].devclass);  // Call the USB device class specific setup callback function
		}
	}

	// Success or error?
	if(pc->setup.is_handled){
		tru_usb_pc_setup_enable_transfer(pc);
	}else{
		// Unknown or unsupported setup request
		tru_usb_pc_setup_error(pc);
	}
}

void tru_usb_pc_setup_enable_transfer(tru_usb_pc_t *pc){
	// =======================================================================================
	// Switches to the next stage of the setup.
	// It also configures the OTG controller EP0 (Endpoint 0) for the next stage of the setup
	// =======================================================================================

	uint8_t curr_control_stage = pc->eps.epos[0].control_stage;
	pc->eps.epos[0].control_stage = pc->eps.epos[0].next_control_stage;  // Switch to the next stage

#if TRU_USB_PC_SETUP_EARLY_XFER == 1
	switch(curr_control_stage){
		case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
			switch(pc->eps.epos[0].next_control_stage){
				case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
					// Wait for the DATA IN stage to complete.  A place holder to prevent the default case

					// Prepare early STATUS OUT stage
					// Setup to receive an OUT ZLP from the host after an DATA IN stage
					// Note, the request below is issued early.  It is normally issued after the IN EP0 data stage has transferred, but tests shows early prevents OUT NAKs
#if TRU_USB_PC_SETUP_STRICT_ZLPOUT == 1
					// Setup to receive size to 0 (ZLP)
					tru_usb_pc_receive_zlp_sr(pc, 0, false);
#else
					// We don't actually need to set receive size to 0 (ZLP), it is better to set it to max buffer size to catch non-compliant responses or error cases
					tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
#endif
					break;
				case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
					// Wait for the DATA OUT stage to complete.  A place holder to prevent the default case
					break;
				default:
					if(pc->setup.request.length == 0){
						// Prepare STATUS IN stage
						tru_usb_pc_transmit_zlp_sr(pc, 0, false);  // After the SETUP stage without a data stage (setup request data length = 0), transmit an IN ZLP for the IN or STATUS OUT stage
						pc->eps.epos[0].next_control_stage = (pc->setup.request.type.bits.dir == TRU_USB_EP_DIR_IN) ? TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT : TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;  // Switch to next stage
					}

					// Prepare early SETUP stage
					// Setup to receive the next setup packet, i.e. prepare early for the next SETUP stage
					// Note: this is normally issued after the STATUS IN or STATUS OUT stage, but tests shows early prevents OUT NAKs
					tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
			}
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
			// Nothing to do.  A place holder to prevent the default case
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
			// Prepare SETUP stage
			// Setup to receive the next setup packet
			pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
			tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
			// Prepare STATUS IN stage
			tru_usb_pc_transmit_zlp_sr(pc, 0, false);  // After the DATA OUT stage, transmit an IN ZLP for the STATUS IN stage

			// Prepare early SETUP stage
			// Setup to receive the next setup packet, i.e. prepare early for the next SETUP stage
			// Note: this is normally issued after the STATUS IN or STATUS OUT stage, but tests shows early prevents OUT NAKs
			tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);  // Schedule to receive a setup packet on OUT endpoint 0
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
			// Nothing to do because we did this earlier in the DATA OUT stage.  A place holder to prevent the default case
			pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
			break;
		default:
			// Unexpected state
			// Prepare SETUP stage
			// Setup to receive the next setup packet
			pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
			tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);  // Schedule to receive a setup packet on OUT endpoint 0
	}
#else
	switch(curr_control_stage){
		case TRU_USB_PC_EP_CONTROL_STAGE_SETUP:
			switch(pc->eps.epos[0].next_control_stage){
				case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
					// Wait for the DATA IN stage to complete.  A place holder to prevent the default case
					break;
				case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
					// Wait for the DATA OUT stage to complete.  A place holder to prevent the default case
					break;
				default:
					if(pc->setup.request.length == 0){
						// Prepare STATUS IN stage
						tru_usb_pc_transmit_zlp_sr(pc, 0, true);  // After the SETUP stage without a data stage (setup request data length = 0), transmit an IN ZLP for the IN or STATUS OUT stage
						pc->eps.epos[0].next_control_stage = (pc->setup.request.type.bits.dir == TRU_USB_EP_DIR_IN) ? TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT : TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN;  // Switch to next stage
					}else{
						// Prepare SETUP stage
						// Setup to receive the next setup packet
						pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
						tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
					}
			}
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
			// Prepare STATUS OUT stage
#if TRU_USB_PC_SETUP_STRICT_ZLPOUT == 1
			// Setup to receive an OUT ZLP from the host after an DATA IN stage
			tru_usb_pc_receive_zlp_sr(pc, 0, true);
#else
			// We don't actually need to set receive size to 0 (ZLP), it is better to set it to max buffer size to catch non-compliant responses or error cases
			tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
#endif
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT:
			// Prepare SETUP stage
			// Setup to receive the next setup packet
			pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
			tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT:
			// Prepare STATUS IN stage
			tru_usb_pc_transmit_zlp_sr(pc, 0, true);  // After the DATA OUT stage, transmit an IN ZLP for the STATUS IN stage
			break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN:
			// Prepare SETUP stage
			// Setup to receive the next setup packet
			pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
			tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
			break;
		default:
			// Unexpected state
			// Prepare SETUP stage
			// Setup to receive the next setup packet
			pc->eps.epos[0].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
			tru_usb_pc_receive(pc, 0, TRU_USB_SETUP_SIZE, TRU_USB_SETUP_SIZE);
	}
#endif

#if defined(TRU_CFG_USB_LOG_SETUP_TEXT) && TRU_CFG_USB_LOG_SETUP_TEXT == 1
	switch(pc->eps.epos[0].next_control_stage){
		case TRU_USB_PC_EP_CONTROL_STAGE_SETUP: LOG("STAGE: SETUP\n"); break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN: LOG("STAGE: DATA IN\n");break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT: LOG("STAGE: STATUS OUT\n"); break;
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT: LOG("STAGE: DATA OUT\n"); break;
		case TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN: LOG("STAGE: STATUS IN\n"); LOG("TXD: ZLP\n"); break;
		default: LOG("STAGE: INVALID\n");
	}
#endif

#if defined(TRU_CFG_USB_LOG_SETUP_BYTES) && TRU_CFG_USB_LOG_SETUP_BYTES == 1
	switch(pc->eps.epos[0].next_control_stage){
		case TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN:
			LOG("TXD: ");
			for(uint32_t i = 0; i < pc->eps.epis[0].lxfer.chunk.buffers[0].xfer_size; i++){
				LOG("%.2x", rd8_unaligned((uint8_t *)pc->eps.epis[0].lxfer.chunk.buffers[0].buf + i));
			}
			LOG(" LEN: %lu\n", pc->eps.epis[0].lxfer.chunk.buffers[0].xfer_size);
			break;
		default:
	}
#endif
}
