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

	USB 2.0 Setup feature status support.
*/

#ifndef TRU_USB_FEATURE_STATUS_H
#define TRU_USB_FEATURE_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

// Standard feature selectors
#define TRU_USB_FEAT_DEV_REMOTE_WAKEUP 0x1
#define TRU_USB_FEAT_EP_HALT           0x0
#define TRU_USB_FEAT_TEST_MODE         0x2

// Status bit masks
#define TRU_USB_STATUS_DEV_SELF_POWERED_MSK_SET  0x1
#define TRU_USB_STATUS_DEV_SELF_POWERED_MSK_CLR  0xfffe
#define TRU_USB_STATUS_DEV_REMOTE_WAKEUP_MSK_SET 0x2
#define TRU_USB_STATUS_DEV_REMOTE_WAKEUP_MSK_CLR 0xfffd
#define TRU_USB_STATUS_EP_HALT_MSK_SET           0x1
#define TRU_USB_STATUS_EP_HALT_MSK_CLR           0xfffe

// Test mode selectors
#define TRU_USB_TEST_MODE_TEST_DISABLED      0x00
#define TRU_USB_TEST_MODE_TEST_J             0x01
#define TRU_USB_TEST_MODE_TEST_K             0x02
#define TRU_USB_TEST_MODE_TEST_SE0_NAK       0x03
#define TRU_USB_TEST_MODE_TEST_PACKET        0x04
#define TRU_USB_TEST_MODE_TEST_FORCE_ENABLE  0x05

// Test mode selectors as high byte of 16 bit word with length = 0
#define TRU_USB_TEST_MODE_TEST_DISABLED_MSB     0x0000
#define TRU_USB_TEST_MODE_TEST_J_MSB            0x0100
#define TRU_USB_TEST_MODE_TEST_K_MSB            0x0200
#define TRU_USB_TEST_MODE_TEST_SE0_NAK_MSB      0x0300
#define TRU_USB_TEST_MODE_TEST_PACKET_MSB       0x0400
#define TRU_USB_TEST_MODE_TEST_FORCE_ENABLE_MSB 0x0500

#ifdef __cplusplus
}
#endif

#endif
