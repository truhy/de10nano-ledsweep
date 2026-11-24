/*
	MIT License

	Copyright (c) 2023 Truong Hy

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

	USB 2.0 device class UAC 1.0 definitions.
*/

#ifndef TRU_USB_UAC_DEF_H
#define TRU_USB_UAC_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_pd_info.h"
#include "usb/tru_usb_ep.h"
#include <stdint.h>

// Audio Interface Class Code
#define TRU_USB_UAC_ITF_CLASS 0x01

// Audio Interface Subclass Codes
#define TRU_USB_UAC_ITF_SUBCLASS_UNDEFINED      0x00
#define TRU_USB_UAC_ITF_SUBCLASS_AUDIOCONTROL   0x01
#define TRU_USB_UAC_ITF_SUBCLASS_AUDIOSTREAMING 0x02
#define TRU_USB_UAC_ITF_SUBCLASS_MIDISTREAMING  0x03

// Audio Interface Protocol Codes
#define TRU_USB_UAC_ITF_PROTOCOL_UNDEFINED 0x00

// Audio Class-Specific Descriptor Types
#define TRU_USB_UAC_DESC_TYPE_UNDEFINED     0x20
#define TRU_USB_UAC_DESC_TYPE_DEVICE        0x21
#define TRU_USB_UAC_DESC_TYPE_CONFIGURATION 0x22
#define TRU_USB_UAC_DESC_TYPE_STRING        0x23
#define TRU_USB_UAC_DESC_TYPE_INTERFACE     0x24
#define TRU_USB_UAC_DESC_TYPE_ENDPOINT      0x25

// Audio Class-Specific AC (AudioControl) Interface Descriptor Subtypes
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_UNDEFINED       0x00
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_HEADER          0x01
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_INPUT_TERMINAL  0x02
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_OUTPUT_TERMINAL 0x03
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_MIXER_UNIT      0x04
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_SELECTOR_UNIT   0x05
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_FEATURE_UNIT    0x06
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_PROCESSING_UNIT 0x07
#define TRU_USB_UAC_AC_ITF_DESC_SUBTYPE_EXTENSION_UNIT  0x08

// Audio Class-Specific AS (AudioStreaming) Interface Descriptor Subtypes
#define TRU_USB_UAC_AS_ITF_DESC_SUBTYPE_UNDEFINED       0x00
#define TRU_USB_UAC_AS_ITF_DESC_SUBTYPE_GENERAL         0x01
#define TRU_USB_UAC_AS_ITF_DESC_SUBTYPE_FORMAT_TYPE     0x02
#define TRU_USB_UAC_AS_ITF_DESC_SUBTYPE_FORMAT_SPECIFIC 0x03

// Processing Unit Process Types
#define TRU_USB_UAC_PU_PROC_TYPE_UNDEFINED          0x00
#define TRU_USB_UAC_PU_PROC_TYPE_UPDOWNMIX          0x01
#define TRU_USB_UAC_PU_PROC_TYPE_DOLBY_PROLOGIC     0x02
#define TRU_USB_UAC_PU_PROC_TYPE_3D_STEREO_EXTENDER 0x03
#define TRU_USB_UAC_PU_PROC_TYPE_REVERBERATION      0x04
#define TRU_USB_UAC_PU_PROC_TYPE_CHORUS             0x05
#define TRU_USB_UAC_PU_PROC_TYPE_DYN_RANGE_COMP     0x06

// Audio Class-Specific Endpoint Descriptor Subtypes
#define TRU_USB_UAC_EP_DESC_SUBTYPE_UNDEFINED  0x00
#define TRU_USB_UAC_EP_DESC_SUBTYPE_EP_GENERAL 0x01

// Audio Class-Specific Setup request codes
#define TRU_USB_UAC_REQ_UNDEFINED 0x00
#define TRU_USB_UAC_REQ_SET_CUR   0x01
#define TRU_USB_UAC_REQ_GET_CUR   0x81
#define TRU_USB_UAC_REQ_SET_MIN   0x02
#define TRU_USB_UAC_REQ_GET_MIN   0x82
#define TRU_USB_UAC_REQ_SET_MAX   0x03
#define TRU_USB_UAC_REQ_GET_MAX   0x83
#define TRU_USB_UAC_REQ_SET_RES   0x04
#define TRU_USB_UAC_REQ_GET_RES   0x84
#define TRU_USB_UAC_REQ_SET_MEM   0x05
#define TRU_USB_UAC_REQ_GET_MEM   0x85
#define TRU_USB_UAC_REQ_GET_STAT  0xFF

// ===================================
// Control Selector (CS) Request Codes
// ===================================

// Terminal Control Selectors
#define TRU_USB_UAC_CS_TE_UNDEFINED    0x00
#define TRU_USB_UAC_CS_TE_COPY_PROTECT 0x01

// Feature Unit Control Selectors
#define TRU_USB_UAC_CS_FU_UNDEFINED         0x00
#define TRU_USB_UAC_CS_FU_MUTE              0x01
#define TRU_USB_UAC_CS_FU_VOLUME            0x02
#define TRU_USB_UAC_CS_FU_BASS              0x03
#define TRU_USB_UAC_CS_FU_MID               0x04
#define TRU_USB_UAC_CS_FU_TREBLE            0x05
#define TRU_USB_UAC_CS_FU_GRAPHIC_EQUALIZER 0x06
#define TRU_USB_UAC_CS_FU_AUTOMATIC_GAIN    0x07
#define TRU_USB_UAC_CS_FU_DELAY             0x08
#define TRU_USB_UAC_CS_FU_BASS_BOOST        0x09
#define TRU_USB_UAC_CS_FU_LOUDNESS          0x0A

// Processing Unit Control Selectors
// Up/Down-mix Control Selectors
#define TRU_USB_UAC_CS_PU_UD_UNDEFINED        0x00
#define TRU_USB_UAC_CS_PU_UD_ENABLE           0x01
#define TRU_USB_UAC_CS_PU_UD_MODE_SELECT      0x02
// Dolby Prologic Processing Unit Control Selectors
#define TRU_USB_UAC_CS_PU_DP_UNDEFINED        0x00
#define TRU_USB_UAC_CS_PU_DP_ENABLE           0x01
#define TRU_USB_UAC_CS_PU_DP_MODE_SELECT      0x02
// 3D Stereo Extender Processing Unit Control Selectors
#define TRU_USB_UAC_CS_PU_3D_UNDEFINED        0x00
#define TRU_USB_UAC_CS_PU_3D_ENABLE           0x01
#define TRU_USB_UAC_CS_PU_3D_SPACIOUSNESS     0x03
// Reverberation Processing Unit Control Selectors
#define TRU_USB_UAC_CS_PU_RV_UNDEFINED        0x00
#define TRU_USB_UAC_CS_PU_RV_ENABLE           0x01
#define TRU_USB_UAC_CS_PU_RV_LEVEL            0x02
#define TRU_USB_UAC_CS_PU_RV_TIME             0x03
#define TRU_USB_UAC_CS_PU_RV_FEEDBACK         0x04
// Chorus Processing Unit Control Selectors
#define TRU_USB_UAC_CS_PU_CH_UNDEFINED        0x00
#define TRU_USB_UAC_CS_PU_CH_ENABLE           0x01
#define TRU_USB_UAC_CS_PU_CH_LEVEL            0x02
#define TRU_USB_UAC_CS_PU_CH_RATE             0x03
#define TRU_USB_UAC_CS_PU_CH_DEPTH            0x04
// Dynamic Range Compressor Processing Unit Control Selectors
#define TRU_USB_UAC_CS_PU_DR_UNDEFINED        0x00
#define TRU_USB_UAC_CS_PU_DR_ENABLE           0x01
#define TRU_USB_UAC_CS_PU_DR_COMPRESSION_RATE 0x02
#define TRU_USB_UAC_CS_PU_DR_MAXAMPL          0x03
#define TRU_USB_UAC_CS_PU_DR_THRESHOLD        0x04
#define TRU_USB_UAC_CS_PU_DR_ATTACK_TIME      0x05
#define TRU_USB_UAC_CS_PU_DR_RELEASE_TIME     0x06

// Extension Unit Control Selectors
#define TRU_USB_UAC_CS_XU_UNDEFINED 0x00
#define TRU_USB_UAC_CS_XU_ENABLE    0x01

// Endpoint Control Selectors
#define TRU_USB_UAC_CS_EP_UNDEFINED      0x00
#define TRU_USB_UAC_CS_EP_SAMPLING_FREQ  0x01
#define TRU_USB_UAC_CS_EP_SAMPLING_PITCH 0x02

// ====================================
// Control Selector Request Data Values
// ====================================

// Terminal control selector request copy protect control values
#define TRU_USB_UAC_CS_TE_COPY_PROTECT_CPL0 0x00
#define TRU_USB_UAC_CS_TE_COPY_PROTECT_CPL1 0x01
#define TRU_USB_UAC_CS_TE_COPY_PROTECT_CPL2 0x02

// =======
// Bitmaps
// =======

// Channel config bitmap values
#define TRU_USB_UAC_CHCFG_L   0x00000001
#define TRU_USB_UAC_CHCFG_R   0x00000002
#define TRU_USB_UAC_CHCFG_C   0x00000004
#define TRU_USB_UAC_CHCFG_LFE 0x00000008
#define TRU_USB_UAC_CHCFG_LS  0x00000010
#define TRU_USB_UAC_CHCFG_RS  0x00000020
#define TRU_USB_UAC_CHCFG_LC  0x00000040
#define TRU_USB_UAC_CHCFG_RC  0x00000080
#define TRU_USB_UAC_CHCFG_S   0x00000100
#define TRU_USB_UAC_CHCFG_SL  0x00000200
#define TRU_USB_UAC_CHCFG_SR  0x00000400
#define TRU_USB_UAC_CHCFG_T   0x00000800

// Feature unit control bitmap values
#define TRU_USB_UAC_FE_CTL_MUTE       0x1
#define TRU_USB_UAC_FE_CTL_VOLUME     0x2
#define TRU_USB_UAC_FE_CTL_BASS       0x4
#define TRU_USB_UAC_FE_CTL_MID        0x8
#define TRU_USB_UAC_FE_CTL_TREBLE     0x10
#define TRU_USB_UAC_FE_CTL_EQUALIZER  0x20
#define TRU_USB_UAC_FE_CTL_AGC        0x40
#define TRU_USB_UAC_FE_CTL_DELAY      0x80
#define TRU_USB_UAC_FE_CTL_BASS_BOOST 0x100
#define TRU_USB_UAC_FE_CTL_LOUDNESS   0x200

typedef union{
	uint8_t val;
	struct{
		uint8_t mute      :1;
		uint8_t volume    :1;
		uint8_t bass      :1;
		uint8_t mid       :1;
		uint8_t treble    :1;
		uint8_t equalizer :1;
		uint8_t agc       :1;
		uint8_t delay     :1;
	}bits;
}tru_usb_uac_fu_controls_uint8_t;

typedef union{
	uint16_t val;
	struct{
		uint16_t mute       :1;
		uint16_t volume     :1;
		uint16_t bass       :1;
		uint16_t mid        :1;
		uint16_t treble     :1;
		uint16_t equalizer  :1;
		uint16_t agc        :1;
		uint16_t delay      :1;
		uint16_t bass_boost :1;
		uint16_t loudness   :1;
		uint16_t res10_15   :6;
	}bits;
}tru_usb_uac_fu_controls_uint16_t;

// Class-Specific AC (AudioControl) Interface Header Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint16_t adc;
	uint16_t total_length;
	uint8_t in_collection;
	uint8_t *interface_num;
}tru_usb_uac_ac_itf_hdr_desc_t;
*/

// Input Terminal Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t terminal_id;
	uint16_t terminal_type;
	uint8_t assoc_terminal;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t terminal;
}tru_usb_uac_input_terminal_desc_t;

// Output Terminal Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t terminal_id;
	uint16_t terminal_type;
	uint8_t assoc_terminal;
	uint8_t source_id;
	uint8_t terminal;
}tru_usb_uac_output_terminal_desc_t;

// Mixer Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint8_t num_in_pins;
	uint8_t *source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t *controls;
	uint8_t mixer;
}tru_usb_uac_mu_desc_t;
*/

// Selector Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint8_t num_in_pins;
	uint8_t *source_id;
	uint8_t selector;
}tru_usb_uac_su_desc_t;
*/

// Feature Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint8_t source_id;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t feature;
}tru_usb_uac_fu_desc_t;
*/

// Processing Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t *source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t processing;
	uint8_t *process_specific;
}tru_usb_uac_pu_desc_t;
*/

// Up/Down-mix Processing Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t processing;
	uint8_t num_modes;
	uint16_t *modes;
}tru_usb_uac_pu_ud_desc_t;
*/

// Dolby Prologic Processing Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t processing;
	uint8_t num_modes;
	uint16_t *modes;
}tru_usb_uac_pu_dp_desc_t;
*/

// 3D-Stereo Extender Processing Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t processing;
}tru_usb_uac_pu_3d_desc_t;
*/

// Reverberation Processing Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t *source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t processing;
}tru_usb_uac_pu_rv_desc_t;
*/

// Chorus Processing Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t *source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t processing;
}tru_usb_uac_pu_ch_desc_t;
*/

// Dynamic Range Compressor Processing Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t *source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t processing;
}tru_usb_uac_pu_dr_desc_t;
*/

// Extension Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t extension_code;
	uint8_t num_in_pins;
	uint8_t *source_id;
	uint8_t num_channels;
	uint16_t channel_config;
	uint8_t channel_names;
	uint8_t control_size;
	uint8_t *controls;
	uint8_t extension;
}tru_usb_uac_xu_desc_t;
*/

// Associated Interface Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t interface_num;
	uint8_t *association_specific;
}tru_usb_uac_assoc_itf_desc_t;
*/

// ============
// AudioControl
// ============

// Standard AC (AudioControl) Interrupt Endpoint Descriptor
// Note, despite it being named standard, this is not the same as the standard USB endpoint descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	tru_usb_ep_desc_address_t address;
	tru_usb_ep_desc_attributes_t attributes;
	uint16_t mps;
	uint8_t interval;
	uint8_t refresh;
	uint8_t sync_address;
}tru_usb_uac_ac_std_int_ep_desc_t;

// ==============
// AudioStreaming
// ==============

// Class-Specific AS (AudioStreaming) Interface Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t terminal_link;
	uint8_t delay;
	uint16_t format_tag;
}tru_usb_uac_as_cls_itf_desc_t;

// Standard AS (AudioStreaming) Isochronous Audio Data Endpoint Descriptor
// Note, despite it being named standard, this is not the same as the standard USB endpoint descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	tru_usb_ep_desc_address_t address;
	tru_usb_ep_desc_attributes_t attributes;
	uint16_t mps;
	uint8_t interval;
	uint8_t refresh;
	uint8_t sync_address;
}tru_usb_uac_as_std_iso_ep_desc_t;

typedef union{
	uint8_t val;
	struct{
		uint8_t sampling_freq    :1;
		uint8_t pitch            :1;
		uint8_t res2_6           :5;
		uint8_t max_packets_only :1;
	}bits;
}tru_usb_uac_as_cls_iso_ep_attributes_t;

// Class-Specific AS (AudioStreaming) Isochronous Audio Data Endpoint Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	tru_usb_uac_as_cls_iso_ep_attributes_t attributes;
	uint8_t lock_delay_units;
	uint16_t lock_delay;
}tru_usb_uac_as_cls_iso_ep_desc_t;

// =================
// UAC Audio Formats
// =================

// Audio Data Format Type I Codes (Format Tag)
#define TRU_USB_UAC_AF_TYPE1_UNDEFINED  0x0000
#define TRU_USB_UAC_AF_TYPE1_PCM        0x0001
#define TRU_USB_UAC_AF_TYPE1_PCM8       0x0002
#define TRU_USB_UAC_AF_TYPE1_IEEE_FLOAT 0x0003
#define TRU_USB_UAC_AF_TYPE1_ALAW       0x0004
#define TRU_USB_UAC_AF_TYPE1_MULAW      0x0005

// Audio Data Format Type II Codes (Format Tag)
#define TRU_USB_UAC_AF_TYPE2_UNDEFINED 0x1000
#define TRU_USB_UAC_AF_TYPE2_MPEG      0x1001
#define TRU_USB_UAC_AF_TYPE2_AC3       0x1002

// Audio Data Format Type III Codes (Format Tag)
#define TRU_USB_UAC_AF_TYPE3_UNDEFINED                      0x2000
#define TRU_USB_UAC_AF_TYPE3_IEC1937_AC3                    0x2001
#define TRU_USB_UAC_AF_TYPE3_IEC1937_MPEG1_L1               0x2002
#define TRU_USB_UAC_AF_TYPE3_IEC1937_MPEG1_L2L3_MPEG2_NOEXT 0x2003
#define TRU_USB_UAC_AF_TYPE3_IEC1937_MPEG2_EXT              0x2004
#define TRU_USB_UAC_AF_TYPE3_IEC1937_MPEG2_L1LS             0x2005
#define TRU_USB_UAC_AF_TYPE3_IEC1937_MPEG2_L2L3LS           0x2006

// Format Type Codes
#define TRU_USB_UAC_AF_TYPE_UNDEFINED 0x00
#define TRU_USB_UAC_AF_TYPE1          0x01
#define TRU_USB_UAC_AF_TYPE2          0x02
#define TRU_USB_UAC_AF_TYPE3          0x03

// MPEG Control Selectors
#define TRU_USB_UAC_AF_MPEG_CTL_SEL_UNDEFINED             0x00
#define TRU_USB_UAC_AF_MPEG_CTL_SEL_DUAL_CHANNEL_CONTROL  0x01
#define TRU_USB_UAC_AF_MPEG_CTL_SEL_SECOND_STEREO_CONTROL 0x02
#define TRU_USB_UAC_AF_MPEG_CTL_SEL_MULTILINGUAL_CONTROL  0x03
#define TRU_USB_UAC_AF_MPEG_CTL_SEL_DYN_RANGE_CONTROL     0x04
#define TRU_USB_UAC_AF_MPEG_CTL_SEL_SCALING_CONTROL       0x05
#define TRU_USB_UAC_AF_MPEG_CTL_SEL_HILO_SCALING_CONTROL  0x06

// Type I Format Type Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint8_t num_channels;
	uint8_t subframe_size;
	uint8_t bit_resolution;
	uint8_t samp_freq_type;
	uint8_t *samp_frequencies;
}tru_usb_uac_af_type1_desc_t;

// Type II Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint16_t max_bit_rate;
	uint16_t samples_per_frame;
	uint8_t samp_freq_type;
	uint8_t *samp_frequencies;
}tru_usb_uac_af_type2_desc_t;
*/

// MPEG Format-Specific Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint16_t format_tag;
	uint16_t mpeg_capabilities;
	uint16_t mpeg_features;
}tru_usb_uac_af_mpeg_desc_t;

// AC-3 Format-Specific Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint16_t format_tag;
	uint32_t bsid;
	uint8_t ac3_features;
}tru_usb_uac_af_ac3_desc_t;

// Type III Format Type Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint8_t num_channels;
	uint8_t subframe_size;
	uint8_t bit_resolution;
	uint8_t samp_freq_type;
	uint8_t *samp_frequencies;
}tru_usb_uac_af_type3_desc_t;
*/

// ==================
// USB Terminal Types
// ==================

// USB Terminal Types
#define TRU_USB_UAC_TERMT_USB_UNDEFINED                     0x0100
#define TRU_USB_UAC_TERMT_USB_STREAMING                     0x0101
#define TRU_USB_UAC_TERMT_USB_VENDOR                        0x01ff

// Input Terminal Types
#define TRU_USB_UAC_TERMT_INPUT_UNDEFINED                   0x0200
#define TRU_USB_UAC_TERMT_INPUT_MICROPHONE                  0x0201
#define TRU_USB_UAC_TERMT_INPUT_DESKTOP_MICROPHONE          0x0202
#define TRU_USB_UAC_TERMT_INPUT_PERSONAL_MICROPHONE         0x0203
#define TRU_USB_UAC_TERMT_INPUT_OMNIDIR_MICROPHONE          0x0204
#define TRU_USB_UAC_TERMT_INPUT_MICROPHONE_ARRAY            0x0205
#define TRU_USB_UAC_TERMT_INPUT_PROCESSING_MICROPHONE_ARRAY 0x0206

// Output Terminal Types
#define TRU_USB_UAC_TERMT_OUTPUT_UNDEFINED                  0x0300
#define TRU_USB_UAC_TERMT_OUTPUT_SPEAKER                    0x0301
#define TRU_USB_UAC_TERMT_OUTPUT_HEADPHONES                 0x0302
#define TRU_USB_UAC_TERMT_OUTPUT_HEAD_MOUNTED_DISPLAY_AUDIO 0x0303
#define TRU_USB_UAC_TERMT_OUTPUT_DESKTOP_SPEAKER            0x0304
#define TRU_USB_UAC_TERMT_OUTPUT_ROOM_SPEAKER               0x0305
#define TRU_USB_UAC_TERMT_OUTPUT_COMMUNICATION_SPEAKER      0x0306
#define TRU_USB_UAC_TERMT_OUTPUT_LOW_FREQ_EFFECTS_SPEAKER   0x0307

// Bi-directional Terminal Types
#define TRU_USB_UAC_TERMT_BI_UNDEFINED                      0x0400
#define TRU_USB_UAC_TERMT_BI_HANDSET                        0x0401
#define TRU_USB_UAC_TERMT_BI_HEADSET                        0x0402
#define TRU_USB_UAC_TERMT_BI_SPEAKERPHONE_NO_ECHO_R         0x0403
#define TRU_USB_UAC_TERMT_BI_ECHO_SUPPRESSING_SPEAKERPHONE  0x0404
#define TRU_USB_UAC_TERMT_BI_ECHO_CANCELING_SPEAKERPHONE    0x0405

// Telephony Terminal Types
#define TRU_USB_UAC_TERMT_TP_UNDEFINED                      0x0500
#define TRU_USB_UAC_TERMT_TP_PHONELINE                      0x0501
#define TRU_USB_UAC_TERMT_TP_TELPHONE                       0x0502
#define TRU_USB_UAC_TERMT_TP_DOWN_LINE_PHONE                0x0503

// External Terminal Types
#define TRU_USB_UAC_TERMT_EXT_UNDEFINED                     0x0600
#define TRU_USB_UAC_TERMT_EXT_ANALOG_CONNECTOR              0x0601
#define TRU_USB_UAC_TERMT_EXT_DIGITAL_AUDIO_INTERFACE       0x0602
#define TRU_USB_UAC_TERMT_EXT_LINE_CONNECTOR                0x0603
#define TRU_USB_UAC_TERMT_EXT_LEGACY_AUDIO_CONNECTOR        0x0604
#define TRU_USB_UAC_TERMT_EXT_SPDIF                         0x0605
#define TRU_USB_UAC_TERMT_EXT_1394_DA_STREAM                0x0606
#define TRU_USB_UAC_TERMT_EXT_1394_DV_STREAM_SOUND_TRACK    0x0607

// Embedded Terminal Types
#define TRU_USB_UAC_TERMT_EMB_UNDEFINED                      0x0700
#define TRU_USB_UAC_TERMT_EMB_LEVEL_CALIBRATION_NOISE_SOURCE 0x0701
#define TRU_USB_UAC_TERMT_EMB_EQUALIZATION_NOISE             0x0702
#define TRU_USB_UAC_TERMT_EMB_CD_PLAYER                      0x0703
#define TRU_USB_UAC_TERMT_EMB_DAT                            0x0704
#define TRU_USB_UAC_TERMT_EMB_DCC                            0x0705
#define TRU_USB_UAC_TERMT_EMB_MINIDISK                       0x0706
#define TRU_USB_UAC_TERMT_EMB_ANALOG_TAPE                    0x0707
#define TRU_USB_UAC_TERMT_EMB_PHONOGRAPH                     0x0708
#define TRU_USB_UAC_TERMT_EMB_VCR_AUDIO                      0x0709
#define TRU_USB_UAC_TERMT_EMB_VIDEO_DISC_AUDIO               0x070a
#define TRU_USB_UAC_TERMT_EMB_DVD_AUDIO                      0x070b
#define TRU_USB_UAC_TERMT_EMB_TV_TUNER_AUDIO                 0x070c
#define TRU_USB_UAC_TERMT_EMB_SATELLITE_TV_AUDIO             0x070d
#define TRU_USB_UAC_TERMT_EMB_CABLE_TUNER_AUDIO              0x070e
#define TRU_USB_UAC_TERMT_EMB_DSS_AUDIO                      0x070f
#define TRU_USB_UAC_TERMT_EMB_RADIO_RECEIVER                 0x0710
#define TRU_USB_UAC_TERMT_EMB_RADIO_TRANSMITTER              0x0711
#define TRU_USB_UAC_TERMT_EMB_MULTITRACK_RECORDER            0x0712
#define TRU_USB_UAC_TERMT_EMB_SYNTHESIZER                    0x0713

// =============================
// Programming model definitions
// =============================

// =======================================================
// Programming model class interface extension definitions
// =======================================================

// Related Audio Control or Audio Streaming interfaces forming the Audio Function
typedef struct{
	uint8_t itf_num;
}tru_usb_uac_related_itf_t;

typedef struct{
	uint8_t entity_id;
	uint8_t *cur;
}tru_usb_uac_copyprotect_t;

typedef struct{
	uint8_t entity_id;
	uint8_t channel_num;
	uint8_t *cur;
}tru_usb_uac_mute_t;

typedef struct{
	uint8_t entity_id;
	uint8_t channel_num;
	uint16_t *cur;
	uint16_t *min;
	uint16_t *max;
	uint16_t *res;
}tru_usb_uac_volume_t;

typedef struct{
	uint8_t num_channels;
	uint8_t subframe_size;
	uint8_t bit_resolution;
}tru_usb_uac_af_t;

// UAC class interface extension (programming model)
typedef struct{
	tru_usb_interface_ext_hdr_t ext_hdr;
	bool is_audio_control;
	tru_usb_uac_related_itf_t *related_itfs;
	uint32_t num_related_itfs;
	tru_usb_uac_copyprotect_t *copyprotects;
	uint32_t num_copyprotects;
	tru_usb_uac_mute_t *mutes;
	uint32_t num_mutes;
	tru_usb_uac_volume_t *volumes;
	uint32_t num_volumes;
	tru_usb_uac_af_t af;
}tru_usb_uac_interface_ext_t;

typedef struct{
	uint8_t cur[3];
	uint8_t min[3];
	uint8_t max[3];
	uint8_t res[3];
}tru_usb_uac_samp_freq_t;

typedef struct{
	uint8_t cur;
}tru_usb_uac_pitch_t;

// UAC class endpoint extension (programming model)
typedef struct{
	uint8_t ep_addr;
	tru_usb_uac_samp_freq_t *samp_freq;
	tru_usb_uac_pitch_t *pitch;
}tru_usb_uac_ep_ext_t;

// A macro to cast a pointer to a class interface extension pointer
#define TO_UAC_INTERFACE_EXT_PTR(ptr) ((tru_usb_uac_interface_ext_t *)ptr)

// UAC peripheral device descriptor set
typedef struct{
	// ====================================
	// Common peripheral device descriptors
	// ====================================
	tru_usb_pd_info_t *pd;

	// =====================================
	// Class interface extension descriptors
	// =====================================

	tru_usb_uac_interface_ext_t *hs_uac_interface_exts;
	uint16_t hs_uac_interface_exts_num_entries;

	tru_usb_uac_interface_ext_t *fs_uac_interface_exts;
	uint16_t fs_uac_interface_exts_num_entries;

	tru_usb_uac_interface_ext_t *ls_uac_interface_exts;
	uint16_t ls_uac_interface_exts_num_entries;

	tru_usb_uac_ep_ext_t *hs_uac_ep_exts;
	uint8_t hs_uac_ep_exts_num_entries;

	tru_usb_uac_ep_ext_t *fs_uac_ep_exts;
	uint8_t fs_uac_ep_exts_num_entries;

	tru_usb_uac_ep_ext_t *ls_uac_ep_exts;
	uint8_t ls_uac_ep_exts_num_entries;
}tru_usb_uac_pd_info_t;

#ifdef __cplusplus
}
#endif

#endif
