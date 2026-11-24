#ifndef TRU_USB_CACHE_H
#define TRU_USB_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_config.h"
#include "usb/tru_usb_lxfer.h"
#include "usb/tru_usb_pc_ep.h"

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1 || defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
void tru_usb_pc_cache_clean_sbdma_data_epin(tru_usb_pc_epi_t *ep);
void tru_usb_pc_cache_inv_sbdma_data_epout(tru_usb_pc_epo_t *ep);
void tru_usb_pc_cache_clean_sbdma_data_epout(tru_usb_pc_epo_t *ep);
void tru_usb_pc_cache_cleaninv_sgdma_list(tru_usb_lxfer_t *lxfer);
void tru_usb_pc_cache_inv_sgdma_list(tru_usb_lxfer_t *lxfer);
void tru_usb_pc_cache_inv_sgdma_data(tru_usb_lxfer_t *lxfer);
void tru_usb_pc_cache_clean_sgdma_data(tru_usb_lxfer_t *lxfer);
#endif

#ifdef __cplusplus
}
#endif

#endif
