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

#include "usb/tru_usb_setup.h"
#include "tru_iom.h"

NONCACHEABLE_SECTION static uint8_t epin0_buffer[TRU_USB_SETUP_DATA_XFER_SIZE_LIMIT];
NONCACHEABLE_SECTION static uint8_t epout0_buffer[TRU_USB_SETUP_DATA_XFER_SIZE_LIMIT];
NONCACHEABLE_SECTION static tru_dwc2_dev_sgdma_desc_reg_t epin0_sgdma_descs[1];
NONCACHEABLE_SECTION static tru_dwc2_dev_sgdma_desc_reg_t epout0_sgdma_descs[1];

void tru_usb_setup_init(tru_usb_setup_t *setup, tru_usb_setup_callback_t callback_setup){
	setup->epin0_buf = epin0_buffer;
	setup->epin0_bufsize = sizeof(epin0_buffer);
	setup->epout0_buf = epout0_buffer;
	setup->epout0_bufsize = sizeof(epout0_buffer);
	setup->epin0_sgdma_descs = epin0_sgdma_descs;
	setup->epout0_sgdma_descs = epout0_sgdma_descs;
	setup->is_handled = false;
}

void tru_usb_setup_deinit(tru_usb_setup_t *setup){
}

void tru_usb_setup_parse(tru_usb_setup_t *setup, void *xfer_buf){
	setup->request.type.val = ((uint8_t *)xfer_buf)[0];
	setup->request.code = ((uint8_t *)xfer_buf)[1];
	setup->request.value = buf_le_to_u16(((uint8_t *)xfer_buf) + 2);
	setup->request.index = buf_le_to_u16(((uint8_t *)xfer_buf) + 4);
	setup->request.length = buf_le_to_u16(((uint8_t *)xfer_buf) + 6);
}
