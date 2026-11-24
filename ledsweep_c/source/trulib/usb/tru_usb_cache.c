#include "tru_usb_cache.h"
#include "tru_cache.h"
#include "synopsys/tru_dwc2_sgdma.h"

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1 || defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
void tru_usb_pc_cache_clean_sbdma_data_epin(tru_usb_pc_epi_t *ep){
	uint32_t len;

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
	if(tru_l1_is_dcache_enabled()){
		len = ep->xfer_size;
		tru_l1_data_clean_range(ep->xfer_buf, len);
	}
#endif

#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
	if(tru_l2_is_enabled()){
		len = ep->xfer_size;
		tru_l2_data_clean_range(ep->xfer_buf, len);
	}
#endif
}

void tru_usb_pc_cache_inv_sbdma_data_epout(tru_usb_pc_epo_t *ep){
	uint32_t len;

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
	if(tru_l1_is_dcache_enabled()){
		len = ep->xfer_size;
		tru_l1_data_inv_range(ep->xfer_buf, len);
	}
#endif

void tru_usb_pc_cache_clean_sbdma_data_epout(tru_usb_pc_epo_t *ep){
	uint32_t len;

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
	if(tru_l1_is_dcache_enabled()){
		len = ep->xfer_size;
		tru_l1_data_clean_range(ep->xfer_buf, len);
	}
#endif

#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
	if(tru_l2_is_enabled()){
		len = ep->xfer_size;
		tru_l2_data_clean_range(ep->xfer_buf, len);
	}
#endif
}

#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
	if(tru_l2_is_enabled()){
		len = ep->xfer_size;
		tru_l2_data_inv_range(ep->xfer_buf, len);
	}
#endif
}

void tru_usb_pc_cache_cleaninv_sgdma_list(tru_usb_lxfer_t *lxfer){
	uint32_t len;

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
	if(tru_l1_is_dcache_enabled()){
		len = lxfer->chunk.sgdma.num_desc * sizeof(tru_dwc2_dev_sgdma_desc_reg_t);
		tru_l1_data_cleaninv_range(lxfer->chunk.sgdma.dma_descs, len);
	}
#endif

#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
	if(tru_l2_is_enabled()){
		len = lxfer->chunk.sgdma.num_desc * sizeof(tru_dwc2_dev_sgdma_desc_reg_t);
		tru_l2_data_cleaninv_range(lxfer->chunk.sgdma.dma_descs, len);
	}
#endif
}

void tru_usb_pc_cache_inv_sgdma_list(tru_usb_lxfer_t *lxfer){
	uint32_t len;

#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
	if(tru_l2_is_enabled()){
		len = lxfer->chunk.sgdma.num_desc * sizeof(tru_dwc2_dev_sgdma_desc_reg_t);
		tru_l2_data_inv_range(lxfer->chunk.sgdma.dma_descs, len);
	}
#endif

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
	if(tru_l1_is_dcache_enabled()){
		len = lxfer->chunk.sgdma.num_desc * sizeof(tru_dwc2_dev_sgdma_desc_reg_t);
		tru_l1_data_inv_range(lxfer->chunk.sgdma.dma_descs, len);
	}
#endif
}

void tru_usb_pc_cache_inv_sgdma_data(tru_usb_lxfer_t *lxfer){
	uint32_t len;

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
	if(tru_l1_is_dcache_enabled()){
		tru_usb_lxfer_buf_t *sg_buffers = lxfer->chunk.buffers;
		for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
			len = sg_buffers->xfer_size_aligned;
			tru_l1_data_inv_range(sg_buffers->buf, len);
			sg_buffers++;

			if(lxfer->chunk.buffers[i].xfer_size == 0) break;
		}
	}
#endif

#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
	if(tru_l2_is_enabled()){
		tru_usb_lxfer_buf_t *sg_buffers = lxfer->chunk.buffers;
		for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
			len = sg_buffers->xfer_size_aligned;
			tru_l2_data_inv_range(sg_buffers->buf, len);
			sg_buffers++;

			if(lxfer->chunk.buffers[i].xfer_size == 0) break;
		}
	}
#endif
}

void tru_usb_pc_cache_clean_sgdma_data(tru_usb_lxfer_t *lxfer){
	uint32_t len;

#if defined(TRU_CFG_L1C_PRESENT) && TRU_CFG_L1C_PRESENT == 1
	if(tru_l1_is_dcache_enabled()){
		tru_usb_lxfer_buf_t *sg_buffers = lxfer->chunk.buffers;
		for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
			len = sg_buffers->xfer_size_aligned;
			tru_l1_data_clean_range(sg_buffers->buf, len);
			sg_buffers++;

			if(lxfer->chunk.buffers[i].xfer_size == 0) break;
		}
	}
#endif

#if defined(TRU_CFG_L2C_PRESENT) && TRU_CFG_L2C_PRESENT == 1
	if(tru_l2_is_enabled()){
		tru_usb_lxfer_buf_t *sg_buffers = lxfer->chunk.buffers;
		for(uint32_t i = 0; i < lxfer->chunk.num_registered; i++){
			len = sg_buffers->xfer_size_aligned;
			tru_l2_data_clean_range(sg_buffers->buf, len);
			sg_buffers++;

			if(lxfer->chunk.buffers[i].xfer_size == 0) break;
		}
	}
#endif
}

#endif
