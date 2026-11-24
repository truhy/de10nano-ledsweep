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

	CDC (Communications Device Class) definitions.

	References:
		Document: Universal Serial Bus Class Definitions for Communications Devices Revision 1.2
		File: CDC1.2_WMC1.1_012011.zip
		  Document: Universal Serial Bus Class Definitions for Communications Devices Revision 1.2 (Errata 1)
		  File: CDC120-20101103-track.pdf
		  Document: Universal Serial Bus Communications Class Subclass Specification for PSTN Devices Revision 1.2
		  File: PSTN120.pdf
*/

#ifndef TRU_USB_CDC_DEF_H
#define TRU_USB_CDC_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usb/tru_usb_setup.h"
#include "tru_usb_pd_info.h"
#include <stdint.h>

// CDC interface class code
#define TRU_USB_CDC_INTERFACE_CLASS 0x02

// CDC interface subclass code
#define TRU_USB_CDC_INTERFACE_SUBCLASS_RES    0x00
#define TRU_USB_CDC_INTERFACE_SUBCLASS_DLCM   0x01
#define TRU_USB_CDC_INTERFACE_SUBCLASS_ACM    0x02
#define TRU_USB_CDC_INTERFACE_SUBCLASS_TCM    0x03
#define TRU_USB_CDC_INTERFACE_SUBCLASS_MCCM   0x04
#define TRU_USB_CDC_INTERFACE_SUBCLASS_CAPICM 0x05
#define TRU_USB_CDC_INTERFACE_SUBCLASS_ENCM   0x06
#define TRU_USB_CDC_INTERFACE_SUBCLASS_ATMNCM 0x07
#define TRU_USB_CDC_INTERFACE_SUBCLASS_WHCM   0x08
#define TRU_USB_CDC_INTERFACE_SUBCLASS_DMM    0x09
#define TRU_USB_CDC_INTERFACE_SUBCLASS_MDLM   0x0a
#define TRU_USB_CDC_INTERFACE_SUBCLASS_OBEX   0x0b
#define TRU_USB_CDC_INTERFACE_SUBCLASS_EEM    0x0c

// CDC interface protocol code
#define TRU_USB_CDC_INTERFACE_PROTOCOL_UNDEFINED      0x00
#define TRU_USB_CDC_INTERFACE_PROTOCOL_AT_V250        0x01
#define TRU_USB_CDC_INTERFACE_PROTOCOL_AT_PCCA101     0x02
#define TRU_USB_CDC_INTERFACE_PROTOCOL_AT_PCCA101_AO  0x03
#define TRU_USB_CDC_INTERFACE_PROTOCOL_AT_GSM_0707    0x04
#define TRU_USB_CDC_INTERFACE_PROTOCOL_AT_3GPP_27_007 0x05
#define TRU_USB_CDC_INTERFACE_PROTOCOL_AT_CS_0017_0   0x06
#define TRU_USB_CDC_INTERFACE_PROTOCOL_USB_EEM        0x07
#define TRU_USB_CDC_INTERFACE_PROTOCOL_EXTERNAL       0xfe
#define TRU_USB_CDC_INTERFACE_PROTOCOL_VENDOR         0xff

// CDC data interface class code
#define TRU_USB_CDC_DATA_INTERFACE_CLASS 0x0a

// CDC data interface subclass code
#define TRU_USB_CDC_DATA_INTERFACE_SUBCLASS_UNDEFINED 0x00

// CDC data interface class protocol code
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_UNDEFINED      0x00
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_I430           0x30
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_IEC_3309_1993  0x31
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_TRANSPARENT    0x32
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_Q921M          0x50
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_Q921           0x51
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_Q921TM         0x52
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_V42BIS         0x90
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_Q931_EURO_ISDN 0x91
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_V120           0x92
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_CAPI20         0x93
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_HOST           0xfd
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_CDC            0xfe
#define TRU_USB_CDC_DATA_INTERFACE_CLASS_PROTOCOL_VENDOR         0xff

// CDC descriptor type
#define TRU_USB_CDC_DESC_TYPE_INTERFACE 0x24
#define TRU_USB_CDC_DESC_TYPE_ENDPOINT  0x25

// CDC descriptor sub-type
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_HDR              0x00
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_CALL_MANAGEMENT  0x01
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_ACM              0x02
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_DLM              0x03
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_TEL_RINGER       0x04
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_TEL_CAPABILITIES 0x05
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_UNION            0x06
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_COUNTRY_SEL      0x07
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_TEL_OP_MODES     0x08
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_USB_TERMINAL     0x09
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_NET_CH_TERMINAL  0x0a
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_PROTOCOL_UNIT    0x0b
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_EXTENSION_UNIT   0x0c
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_MCCM             0x0d
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_CAPICM           0x0e
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_ENCM             0x0f
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_ATMCM            0x10
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_WHCM             0x11
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_MDLM             0x12
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_MDLM_DETAIL      0x13
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_DMM              0x14
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_OBEX             0x15
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_CMD_SET          0x16
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_CMD_SET_DETAIL   0x17
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_TCM              0x18
#define TRU_USB_CDC_DESC_SUBTYPE_FUNC_OBEX_SERVICE_ID  0x19

// CDC specific setup request
// See chapter 6. Communications Class Specific Messages from CDC120-20101103-track.pdf
// See chapter 6.3 PSTN Subclass Specific Requests from PSTN120.pdf
#define TRU_USB_CDC_REQ_SEND_ENCAPSULATED_COMMAND                    0x00
#define TRU_USB_CDC_REQ_GET_ENCAPSULATED_RESPONSE                    0x01
#define TRU_USB_CDC_REQ_SET_COMM_FEATURE                             0x02
#define TRU_USB_CDC_REQ_GET_COMM_FEATURE                             0x03
#define TRU_USB_CDC_REQ_CLEAR_COMM_FEATURE                           0x04
#define TRU_USB_CDC_REQ_SET_AUX_LINE_STATE                           0x10
#define TRU_USB_CDC_REQ_SET_HOOK_STATE                               0x11
#define TRU_USB_CDC_REQ_PULSE_SETUP                                  0x12
#define TRU_USB_CDC_REQ_SEND_PULSE                                   0x13
#define TRU_USB_CDC_REQ_SET_PULSE_TIME                               0x14
#define TRU_USB_CDC_REQ_RING_AUX_JACK                                0x15
#define TRU_USB_CDC_REQ_SET_LINE_CODING                              0x20
#define TRU_USB_CDC_REQ_GET_LINE_CODING                              0x21
#define TRU_USB_CDC_REQ_SET_CONTROL_LINE_STATE                       0x22
#define TRU_USB_CDC_REQ_SEND_BREAK                                   0x23
#define TRU_USB_CDC_REQ_SET_RINGER_PARMS                             0x30
#define TRU_USB_CDC_REQ_GET_RINGER_PARMS                             0x31
#define TRU_USB_CDC_REQ_SET_OPERATION_PARMS                          0x32
#define TRU_USB_CDC_REQ_GET_OPERATION_PARMS                          0x33
#define TRU_USB_CDC_REQ_SET_LINE_PARMS                               0x34
#define TRU_USB_CDC_REQ_GET_LINE_PARMS                               0x35
#define TRU_USB_CDC_REQ_DIAL_DIGITS                                  0x36
#define TRU_USB_CDC_REQ_SET_UNIT_PARAMETER                           0x37
#define TRU_USB_CDC_REQ_GET_UNIT_PARAMETER                           0x38
#define TRU_USB_CDC_REQ_CLEAR_UNIT_PARAMETER                         0x39
#define TRU_USB_CDC_REQ_GET_PROFILE                                  0x3A
#define TRU_USB_CDC_REQ_SET_ETHERNET_MULTICAST_FILTERS               0x40
#define TRU_USB_CDC_REQ_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x41
#define TRU_USB_CDC_REQ_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x42
#define TRU_USB_CDC_REQ_SET_ETHERNET_PACKET_FILTER                   0x43
#define TRU_USB_CDC_REQ_GET_ETHERNET_STATISTIC                       0x44
#define TRU_USB_CDC_REQ_SET_ATM_DATA_FORMAT                          0x50
#define TRU_USB_CDC_REQ_GET_ATM_DEVICE_STATISTICS                    0x51
#define TRU_USB_CDC_REQ_SET_ATM_DEFAULT_VC                           0x52
#define TRU_USB_CDC_REQ_GET_ATM_VC_STATISTICS                        0x53

// Linecoding value
#define TRU_USB_CDC_LINECODING_STOPBITS_ONE     0
#define TRU_USB_CDC_LINECODING_STOPBITS_ONEHALF 1
#define TRU_USB_CDC_LINECODING_STOPBITS_TWO     2
#define TRU_USB_CDC_LINECODING_PARITY_NONE      0
#define TRU_USB_CDC_LINECODING_PARITY_ODD       1
#define TRU_USB_CDC_LINECODING_PARITY_EVEN      2
#define TRU_USB_CDC_LINECODING_PARITY_MARK      3
#define TRU_USB_CDC_LINECODING_PARITY_SPACE     4

// PSTN Subclass Specific Notification (see Chapter 6.5 from PSTN120.pdf)
#define TRU_USB_CDC_NTF_NETWORK_CONNECTION      0x00
#define TRU_USB_CDC_NTF_RESPONSE_AVAILABLE      0x01
#define TRU_USB_CDC_NTF_AUX_JACK_HOOK_STATE     0x08
#define TRU_USB_CDC_NTF_RING_DETECT             0x09
#define TRU_USB_CDC_NTF_SERIAL_STATE            0x20
#define TRU_USB_CDC_NTF_CALL_STATE_CHANGE       0x28
#define TRU_USB_CDC_NTF_LINE_STATE_CHANGE       0x23
#define TRU_USB_CDC_NTF_CONNECTION_SPEED_CHANGE 0x2a

// Network notification value
#define TRU_USB_CDC_NTF_NETWORK_CONNECTION_DISCONNECTED 0
#define TRU_USB_CDC_NTF_NETWORK_CONNECTION_CONNECTED    1

#define TRU_USB_CDC_NTF_MAX_DATA_SIZE 8

typedef struct __attribute__((packed)){
	uint32_t dte_rate;
	uint8_t char_format;
	uint8_t parity_type;
	uint8_t data_bits;
}tru_usb_cdc_linecoding_t;

#define TRU_USB_CDC_LINECODING_PTR(ptr) ((tru_usb_cdc_linecoding_t *)(ptr))

typedef union{
    uint16_t val;
    struct{
    	uint16_t dtr   :1;
        uint16_t rts   :1;
        uint16_t res2  :1;
        uint16_t res3  :1;
        uint16_t res4  :1;
        uint16_t res5  :1;
        uint16_t res6  :1;
        uint16_t res7  :1;
        uint16_t res8  :1;
        uint16_t res9  :1;
        uint16_t res10 :1;
        uint16_t res11 :1;
        uint16_t res12 :1;
        uint16_t res13 :1;
        uint16_t res14 :1;
        uint16_t res15 :1;
    }bits;
}tru_usb_cdc_linecontrol_t;

#define TRU_USB_CDC_LINECONTROL_PTR(ptr) ((tru_usb_cdc_linecontrol_t *)(ptr))

typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint16_t release_num;
}tru_usb_cdc_func_desc_hdr_t;

// ACM capabilities
#define TRU_USB_CDC_FUNC_ACM_CAP_SUPPORT_NONE         0x00
#define TRU_USB_CDC_FUNC_ACM_CAP_SUPPORT_COMM_FEATURE 0x01
#define TRU_USB_CDC_FUNC_ACM_CAP_SUPPORT_LINE_CONTROL 0x02
#define TRU_USB_CDC_FUNC_ACM_CAP_SUPPORT_SEND_BREAK   0x04
#define TRU_USB_CDC_FUNC_ACM_CAP_SUPPORT_NET_NOTIF    0x08

// Abstract Control Management Functional Descriptor
// See USB PSTN (Public Switched Telephone Network) specification (used in CDC)
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t capabilites;
}tru_usb_cdc_func_desc_acm_t;

typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t ctrl_interface_num;
	uint8_t sub_interface_num;
}tru_usb_cdc_func_desc_union_t;

#define TRU_USB_CDC_FUNC_CM_CAP_NONE                 0x00
#define TRU_USB_CDC_FUNC_CM_CAP_HANDLES_CM           0x01
#define TRU_USB_CDC_FUNC_CM_CAP_DATA_CLASS_INTERFACE 0x02

// Call Management Functional Descriptor
// See USB PSTN (Public Switched Telephone Network) specification (used in CDC)
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t capabilites;
	uint8_t interface_num;
}tru_usb_cdc_func_desc_call_management_t;

// General notification format
typedef struct{
	tru_usb_request_t header;
	uint8_t data[TRU_USB_CDC_NTF_MAX_DATA_SIZE];
}tru_usb_cdc_ntf_t;

#define TRU_USB_CDC_NTF_PTR(ptr) ((tru_usb_cdc_ntf_t *)(ptr))

// Call management SERIAL_STATE notification data of UART state bitmap
// Note, there is no CTS bit because the current USB CDC-ACM specification does not support it
typedef union{
    uint16_t val;
    struct{
        uint16_t dcd         :1;
        uint16_t dsr         :1;
        uint16_t break_err   :1;
        uint16_t ri          :1;
        uint16_t frame_err   :1;
        uint16_t parity_err  :1;
        uint16_t overrun_err :1;
        uint16_t res7        :1;
        uint16_t res8        :1;
        uint16_t res9        :1;
        uint16_t res10       :1;
        uint16_t res11       :1;
        uint16_t res12       :1;
        uint16_t res13       :1;
        uint16_t res14       :1;
        uint16_t res15       :1;
    }bits;
}tru_usb_cdc_uartstate_t;

#define TRU_USB_CDC_UARTSTATE_PTR(ptr) ((tru_usb_cdc_uartstate_t *)(ptr))

// =============================
// Programming model definitions
// =============================

// =======================================================
// Programming model class interface extension definitions
// =======================================================

typedef struct{
	uint8_t *encap_cmd;
	uint32_t encap_cmd_buf_size;
	uint8_t *encap_resp;
	uint32_t encap_resp_buf_size;
}tru_usb_cdc_encap_t;

// CDC class interface extension (programming model)
typedef struct{
	tru_usb_interface_ext_hdr_t ext_hdr;
	tru_usb_cdc_linecoding_t *linecoding;
	tru_usb_cdc_linecontrol_t *linecontrol;
	tru_usb_cdc_encap_t *encap;
	tru_usb_cdc_uartstate_t *uartstate;
}tru_usb_cdc_interface_ext_t;

// A macro to cast a pointer to a CDC class interface extension pointer
#define TO_CDC_INTERFACE_EXT_PTR(ptr) ((tru_usb_cdc_interface_ext_t *)ptr)

// CDC peripheral device descriptor set
typedef struct{
	// ====================================
	// Common peripheral device descriptors
	// ====================================
	tru_usb_pd_info_t *pd;

	// =========================================
	// CDC class interface extension descriptors
	// =========================================

	tru_usb_cdc_interface_ext_t *hs_cdc_interface_exts;
	uint16_t hs_cdc_interface_exts_num_entries;

	tru_usb_cdc_interface_ext_t *fs_cdc_interface_exts;
	uint16_t fs_cdc_interface_exts_num_entries;

	tru_usb_cdc_interface_ext_t *ls_cdc_interface_exts;
	uint16_t ls_cdc_interface_exts_num_entries;
}tru_usb_cdc_pd_info_t;

#ifdef __cplusplus
}
#endif

#endif
