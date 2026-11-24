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

	USB 2.0 peripheral controller mode end-point support.
*/

#ifndef TRU_USB_PC_EP_H
#define TRU_USB_PC_EP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"
#include "usb/tru_usb_lxfer.h"
#include "synopsys/tru_dwc2.h"
#include <stdint.h>

#define TRU_USB_PC_EP_CONTROL_STAGE_SETUP      0
#define TRU_USB_PC_EP_CONTROL_STAGE_DATA_IN    1
#define TRU_USB_PC_EP_CONTROL_STAGE_STATUS_OUT 2
#define TRU_USB_PC_EP_CONTROL_STAGE_DATA_OUT   3
#define TRU_USB_PC_EP_CONTROL_STAGE_STATUS_IN  4

#define TRU_USB_PC_EP_DEFAULT_EP0_MPS       64
#define TRU_USB_PC_EP_DEFAULT_EP0_MPS_LIMIT 64

typedef struct{
	bool is_epena_pending;
	tru_dwc2_stall_t stall;
	tru_dwc2_nak_t nak;
	tru_dwc2_dpid_eo_t dpid;
}tru_usb_pc_ep_deferred;

// USB peripheral controller in endpoint
typedef struct{
	uint8_t num;
	bool is_activated;
	void *curr_alt_setting;
	uint8_t flow_type;
	uint16_t mps;                          // Current MPS (Max Packet Size)
	uint16_t mps_limit;                    // MPS found in the device descriptor
	void *xfer_buf;                        // Endpoint transfer buffer
	uint32_t xfer_size;                    // Endpoint transfer size
	uint32_t xferred_size;                 // Endpoint actual transferred size.  This will be zero when transferring a ZLP
	uint32_t xferred_size_prezlp;          // Endpoint actual transferred size before a ZLP
	uint8_t usage_type;
	uint8_t iso_sync_type;                 // Isochronous synchronization type
	uint8_t iso_intr_add_trans;            // Isochronous & Interrupt additional transaction(s) per microframe
	uint32_t iso_incomplete;               // Isochronous incomplete transfer count
	uint32_t interval;                     // Periodic interval in (micro)frames, i.e. frames or microframes depending on the current USB speed
	uint32_t frame_target;                 // Target (micro)frame number of when to transfer, i.e. the first opportunity periodic transfer
	uint16_t status;                       // For GET_STATUS, CLEAR_FEATURE, SET_FEATURE
	tru_usb_pc_ep_deferred deferred;       // Deferred EP transfer
	tru_usb_lxfer_t lxfer;                 // Long transfer
	void *ep_ext;                          // Class EP extension
}tru_usb_pc_epi_t;

// USB peripheral controller out endpoint
typedef struct{
	uint8_t num;
	bool is_activated;
	void *curr_alt_setting;
	uint8_t flow_type;
	uint16_t mps;                          // Current MPS (Max Packet Size)
	uint16_t mps_limit;                    // MPS found in the device descriptor
	void *xfer_buf;                        // Endpoint transfer buffer
	uint32_t xfer_size;                    // Endpoint transfer size
	uint32_t xferred_size;                 // Endpoint actual transferred size.  This will be zero when transferring a ZLP
	uint32_t xferred_size_prezlp;          // Endpoint actual transferred size before a ZLP
	uint32_t rxfifo_xferred_size;
	uint8_t usage_type;
	uint8_t iso_sync_type;                 // Isochronous synchronization type
	uint8_t iso_intr_add_trans;            // Isochronous & Interrupt additional transaction(s) per microframe
	uint32_t iso_incomplete;               // Isochronous incomplete transfer count
	uint32_t interval;                     // Periodic interval in frames or microframes
	uint32_t frame_target;                 // Target (micro)frame number of when to transfer, i.e. the first opportunity periodic transfer
	uint16_t status;                       // For GET_STATUS, CLEAR_FEATURE, SET_FEATURE
	uint8_t control_stage;                 // Current stage of the data control flow
	uint8_t next_control_stage;            // Next stage of the data control flow
	tru_usb_pc_ep_deferred deferred;       // Deferred EP transfer
	tru_usb_lxfer_t lxfer;                 // Long transfer
	void *ep_ext;                          // Class EP extension
}tru_usb_pc_epo_t;

// All USB peripheral controller endpoints
typedef struct{
	tru_usb_pc_epi_t epis[16];  // Array of software model of peripheral mode in endpoints
	tru_usb_pc_epo_t epos[16];  // Array of software model of peripheral mode out endpoints
}tru_usb_pc_eps_t;

void tru_usb_pc_eps_init(tru_usb_pc_eps_t *eps);
void tru_usb_pc_eps_deinit(tru_usb_pc_eps_t *eps);
void tru_usb_pc_epin_set_deferred(tru_usb_pc_epi_t *ep, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid);
void tru_usb_pc_epout_set_deferred(tru_usb_pc_epo_t *ep, tru_dwc2_stall_t stall, tru_dwc2_nak_t nak, tru_dwc2_dpid_eo_t dpid);
void tru_usb_pc_eps_clear_deferred(tru_usb_pc_eps_t *eps, uint32_t num_dev_in_ep, uint32_t num_dev_out_ep);

#ifdef __cplusplus
}
#endif

#endif
