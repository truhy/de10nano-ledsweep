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

#include "usb/tru_usb_pc_ep.h"
#include "usb/tru_usb_ep.h"
#include <stddef.h>

void tru_usb_pc_eps_init(tru_usb_pc_eps_t *eps){
	for(uint8_t ep_num = 0; ep_num < 16; ep_num++){
		eps->epis[ep_num].num = ep_num;
		eps->epis[ep_num].is_activated = (ep_num == 0) ? true : false;  // EP0 is always activated
		eps->epis[ep_num].curr_alt_setting = NULL;
		eps->epis[ep_num].flow_type = TRU_USB_EP_TRANSFER_TYPE_CONTROL;
		eps->epis[ep_num].mps_limit = (ep_num == 0) ? TRU_USB_PC_EP_DEFAULT_EP0_MPS_LIMIT : 0;
		eps->epis[ep_num].mps = (ep_num == 0) ? TRU_USB_PC_EP_DEFAULT_EP0_MPS : 0;
		eps->epis[ep_num].xfer_buf = NULL;
		eps->epis[ep_num].xfer_size = 0;
		eps->epis[ep_num].xferred_size = 0;
		eps->epis[ep_num].xferred_size_prezlp = 0;
		eps->epis[ep_num].usage_type = TRU_USB_EP_USAGE_TYPE_DATA;
		eps->epis[ep_num].iso_sync_type = TRU_USB_EP_ISO_SYNC_TYPE_NONE;
		eps->epis[ep_num].iso_intr_add_trans = TRU_USB_EP_PERIODIC_ADD_TRANS_NONE;
		eps->epis[ep_num].iso_incomplete = 0;
		eps->epis[ep_num].interval = 0;
		eps->epis[ep_num].frame_target = -1;
		eps->epis[ep_num].status = 0;
		eps->epis[ep_num].deferred.is_epena_pending = false;
		eps->epis[ep_num].deferred.stall = TRU_DWC2_STALL_CURRENT;
		eps->epis[ep_num].deferred.nak = TRU_DWC2_NAK_CURRENT;
		eps->epis[ep_num].deferred.dpid = TRU_DWC2_DPID_EO_CURRENT;
		eps->epis[ep_num].ep_ext = NULL;
		tru_usb_lxfer_init(&eps->epis[ep_num].lxfer);

		eps->epos[ep_num].num = ep_num;
		eps->epos[ep_num].is_activated = (ep_num == 0) ? true : false;  // EP0 is always activated
		eps->epos[ep_num].curr_alt_setting = NULL;
		eps->epos[ep_num].flow_type = TRU_USB_EP_TRANSFER_TYPE_CONTROL;
		eps->epos[ep_num].mps_limit = (ep_num == 0) ? TRU_USB_PC_EP_DEFAULT_EP0_MPS_LIMIT : 0;
		eps->epos[ep_num].mps = (ep_num == 0) ? TRU_USB_PC_EP_DEFAULT_EP0_MPS : 0;
		eps->epos[ep_num].xfer_buf = NULL;
		eps->epos[ep_num].xfer_size = 0;
		eps->epos[ep_num].xferred_size = 0;
		eps->epos[ep_num].xferred_size_prezlp = 0;
		eps->epos[ep_num].rxfifo_xferred_size = 0;
		eps->epos[ep_num].usage_type = TRU_USB_EP_USAGE_TYPE_DATA;
		eps->epos[ep_num].iso_sync_type = TRU_USB_EP_ISO_SYNC_TYPE_NONE;
		eps->epos[ep_num].iso_intr_add_trans = TRU_USB_EP_PERIODIC_ADD_TRANS_NONE;
		eps->epos[ep_num].iso_incomplete = 0;
		eps->epos[ep_num].interval = 0;
		eps->epos[ep_num].frame_target = -1;
		eps->epos[ep_num].status = 0;
		eps->epos[ep_num].control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
		eps->epos[ep_num].next_control_stage = TRU_USB_PC_EP_CONTROL_STAGE_SETUP;
		eps->epos[ep_num].deferred.is_epena_pending = false;
		eps->epos[ep_num].deferred.stall = TRU_DWC2_STALL_CURRENT;
		eps->epos[ep_num].deferred.nak = TRU_DWC2_NAK_CURRENT;
		eps->epos[ep_num].deferred.dpid = TRU_DWC2_DPID_EO_CURRENT;
		eps->epos[ep_num].ep_ext = NULL;
		tru_usb_lxfer_init(&eps->epos[ep_num].lxfer);
	}
}

void tru_usb_pc_eps_deinit(tru_usb_pc_eps_t *eps){
	for(uint8_t ep_num = 0; ep_num < 16; ep_num++){
		tru_usb_lxfer_deinit(&eps->epis[ep_num].lxfer);
		tru_usb_lxfer_deinit(&eps->epos[ep_num].lxfer);
	}
}

// Set ep as deferred transfer
void tru_usb_pc_epin_set_deferred(tru_usb_pc_epi_t *ep, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid){
	ep->deferred.stall = stall;
	ep->deferred.nak = nak;
	ep->deferred.dpid = dpid;
	ep->deferred.is_epena_pending = true;
}

// Set ep as deferred transfer
void tru_usb_pc_epout_set_deferred(tru_usb_pc_epo_t *ep, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid){
	ep->deferred.stall = stall;
	ep->deferred.nak = nak;
	ep->deferred.dpid = dpid;
	ep->deferred.is_epena_pending = true;
}

void tru_usb_pc_eps_clear_deferred(tru_usb_pc_eps_t *eps, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep){
	tru_usb_pc_epi_t *epi = eps->epis;
	tru_usb_pc_epo_t *epo = eps->epos;

	for(uint8_t i = 0; i < num_dev_in_ep; i++){
		epi->deferred.is_epena_pending = false;
		epi++;
	}
	for(uint8_t i = 0; i < num_dev_out_ep; i++){
		epo->deferred.is_epena_pending = false;
		epo++;
	}
}
