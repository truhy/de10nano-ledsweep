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

#include "usb/tru_usb_dev.h"
#include "synopsys/tru_dwc2.h"
#include <stddef.h>

void tru_usb_dev_init(
	tru_usb_dev_t *dev,
	tru_usb_dev_desc_t *hs_dev_desc,
	tru_usb_dev_desc_t *fs_dev_desc,
	tru_usb_dev_desc_t *ls_dev_desc,
	tru_usb_dev_qual_desc_t *hs_dev_qual_desc,
	tru_usb_dev_qual_desc_t *fs_dev_qual_desc
){
	dev->hs_dev_desc = hs_dev_desc;
	dev->fs_dev_desc = fs_dev_desc;
	dev->ls_dev_desc = ls_dev_desc;
	dev->hs_dev_qual_desc = hs_dev_qual_desc;
	dev->fs_dev_qual_desc = fs_dev_qual_desc;
	dev->hs_dev_ext = NULL;
	dev->fs_dev_ext = NULL;
	dev->ls_dev_ext = NULL;
	dev->curr_spd_dev_desc = NULL;
	dev->curr_spd_dev_qual_desc = NULL;
	dev->curr_spd_dev_ext = NULL;
}

void tru_usb_dev_deinit(tru_usb_dev_t *dev){
	dev->hs_dev_desc = NULL;
	dev->fs_dev_desc = NULL;
	dev->ls_dev_desc = NULL;
	dev->hs_dev_qual_desc = NULL;
	dev->fs_dev_qual_desc = NULL;
	dev->hs_dev_ext = NULL;
	dev->fs_dev_ext = NULL;
	dev->ls_dev_ext = NULL;
	dev->curr_spd_dev_desc = NULL;
	dev->curr_spd_dev_qual_desc = NULL;
	dev->curr_spd_dev_ext = NULL;
}

/*
	This should be called after the USB speed is detected.
	This selects the current pointers to the correct device and device qualifier descriptors from the current speed.
*/
void tru_usb_dev_spd_init(tru_usb_dev_t *dev, uint8_t speed){
	switch(speed){
		case TRU_DWC2_DSTS_ENUMSPD_HS3060:  // High speed
			if(dev->hs_dev_desc){
				dev->curr_spd_dev_desc = dev->hs_dev_desc;
				dev->curr_spd_dev_qual_desc = dev->fs_dev_qual_desc;
				dev->curr_spd_dev_ext = dev->hs_dev_ext;
			}else if(dev->fs_dev_desc){
				dev->curr_spd_dev_desc = dev->fs_dev_desc;
				dev->curr_spd_dev_qual_desc = NULL;
				dev->curr_spd_dev_ext = dev->fs_dev_ext;
			}else if(dev->ls_dev_desc){
				dev->curr_spd_dev_desc = dev->ls_dev_desc;
				dev->curr_spd_dev_qual_desc = NULL;
				dev->curr_spd_dev_ext = dev->ls_dev_ext;
			}else{
				dev->curr_spd_dev_desc = NULL;
				dev->curr_spd_dev_qual_desc = NULL;
				dev->curr_spd_dev_ext = NULL;
			}
			break;
		case TRU_DWC2_DSTS_ENUMSPD_FS3060:  // Full speed
		case TRU_DWC2_DSTS_ENUMSPD_FS48:
			dev->curr_spd_dev_desc = dev->fs_dev_desc;
			dev->curr_spd_dev_qual_desc = dev->hs_dev_qual_desc;
			dev->curr_spd_dev_ext = dev->fs_dev_ext;
			break;
		case TRU_DWC2_DSTS_ENUMSPD_LS6:  // Low speed
			dev->curr_spd_dev_desc = dev->ls_dev_desc;
			dev->curr_spd_dev_qual_desc = NULL;  // There is no device qualifier for low speed
			dev->curr_spd_dev_ext = dev->ls_dev_ext;
			break;
		default:  // Invalid speed
			dev->curr_spd_dev_desc = NULL;
			dev->curr_spd_dev_qual_desc = NULL;
			dev->curr_spd_dev_ext = NULL;
	}
}

void tru_usb_dev_spd_deinit(tru_usb_dev_t *dev){
	dev->curr_spd_dev_desc = NULL;
	dev->curr_spd_dev_qual_desc = NULL;
	dev->curr_spd_dev_ext = NULL;
}
