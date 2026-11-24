/*
	MIT License

	Copyright (c) 2025 Truong Hy

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

	USB 2.0 device class UAC 2.0 definitions.
*/

#ifndef TRU_USB_UAC2_DEF_H
#define TRU_USB_UAC2_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tru_usb_pd_info.h"
#include "usb/tru_usb_ep.h"
#include <stdint.h>

// Audio Function Class Codes
#define TRU_USB_UAC2_FUNC_CLASS TRU_USB_UAC2_ITF_CLASS

// Audio Function Subclass Codes
#define TRU_USB_UAC2_FUNC_SUBCLASS_UNDEFINED 0x00

// Audio Function Protocol Codes
#define TRU_USB_UAC2_FUNC_PROTOCOL_UNDEFINED        0x00
#define TRU_USB_UAC2_FUNC_PROTOCOL_AF_VERSION_02_00 TRU_USB_UAC2_ITF_IP_VERSION_02_00

// Audio Interface Class Code
#define TRU_USB_UAC2_ITF_CLASS 0x01

// Audio Interface Subclass Codes
#define TRU_USB_UAC2_ITF_SUBCLASS_UNDEFINED      0x00
#define TRU_USB_UAC2_ITF_SUBCLASS_AUDIOCONTROL   0x01
#define TRU_USB_UAC2_ITF_SUBCLASS_AUDIOSTREAMING 0x02
#define TRU_USB_UAC2_ITF_SUBCLASS_MIDISTREAMING  0x03

// Audio Interface Protocol Codes
#define TRU_USB_UAC2_ITF_PROTOCOL_UNDEFINED 0x00
#define TRU_USB_UAC2_ITF_IP_VERSION_02_00   0x20

// Audio Function Category Codes
#define TRU_USB_UAC2_FUNC_CAT_UNDEFINED           0x00
#define TRU_USB_UAC2_FUNC_CAT_DESKTOP_SPEAKER     0x01
#define TRU_USB_UAC2_FUNC_CAT_HOME_THEATER        0x02
#define TRU_USB_UAC2_FUNC_CAT_MICROPHONE          0x03
#define TRU_USB_UAC2_FUNC_CAT_HEADSET             0x04
#define TRU_USB_UAC2_FUNC_CAT_TELEPHONE           0x05
#define TRU_USB_UAC2_FUNC_CAT_CONVERTER           0x06
#define TRU_USB_UAC2_FUNC_CAT_VOICESOUND_RECORDER 0x07
#define TRU_USB_UAC2_FUNC_CAT_IO_BOX              0x08
#define TRU_USB_UAC2_FUNC_CAT_MUSICAL_INSTRUMENT  0x09
#define TRU_USB_UAC2_FUNC_CAT_PROAUDIO            0x0A
#define TRU_USB_UAC2_FUNC_CAT_AUDIOVIDEO          0x0B
#define TRU_USB_UAC2_FUNC_CAT_CONTROL_PANEL       0x0C
#define TRU_USB_UAC2_FUNC_CAT_OTHER               0xFF

// Audio Class-Specific Descriptor Types
#define TRU_USB_UAC2_DESC_TYPE_UNDEFINED     0x20
#define TRU_USB_UAC2_DESC_TYPE_DEVICE        0x21
#define TRU_USB_UAC2_DESC_TYPE_CONFIGURATION 0x22
#define TRU_USB_UAC2_DESC_TYPE_STRING        0x23
#define TRU_USB_UAC2_DESC_TYPE_INTERFACE     0x24
#define TRU_USB_UAC2_DESC_TYPE_ENDPOINT      0x25

// Audio Class-Specific AC (AudioControl) Interface Descriptor Subtypes
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_UNDEFINED             0x00
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_HEADER                0x01
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_INPUT_TERMINAL        0x02
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_OUTPUT_TERMINAL       0x03
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_MIXER_UNIT            0x04
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_SELECTOR_UNIT         0x05
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_FEATURE_UNIT          0x06
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_EFFECT_UNIT           0x07
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_PROCESSING_UNIT       0x08
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_EXTENSION_UNIT        0x09
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_CLOCK_SOURCE          0x0A
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_CLOCK_SELECTOR        0x0B
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_CLOCK_MULTIPLIER      0x0C
#define TRU_USB_UAC2_AC_ITF_DESC_SUBTYPE_SAMPLE_RATE_CONVERTOR 0x0D

// Audio Class-Specific AS (AudioStreaming) Interface Descriptor Subtypes
#define TRU_USB_UAC2_AS_ITF_DESC_SUBTYPE_UNDEFINED       0x00
#define TRU_USB_UAC2_AS_ITF_DESC_SUBTYPE_GENERAL         0x01
#define TRU_USB_UAC2_AS_ITF_DESC_SUBTYPE_FORMAT_TYPE     0x02
#define TRU_USB_UAC2_AS_ITF_DESC_SUBTYPE_ENCODER         0x03
#define TRU_USB_UAC2_AS_ITF_DESC_SUBTYPE_DECODER         0x04

// Effect Unit Effect Types
#define TRU_USB_UAC2_EU_TYPE_UNDEFINED       0x00
#define TRU_USB_UAC2_EU_TYPE_AS_GENERAL      0x01
#define TRU_USB_UAC2_EU_TYPE_FORMAT_TYPE     0x02
#define TRU_USB_UAC2_EU_TYPE_FORMAT_SPECIFIC 0x03

// Processing Unit Process Types
#define TRU_USB_UAC2_PU_PROC_TYPE_UNDEFINED       0x00
#define TRU_USB_UAC2_PU_PROC_TYPE_UPDOWNMIX       0x01
#define TRU_USB_UAC2_PU_PROC_TYPE_DOLBY_PROLOGIC  0x02
#define TRU_USB_UAC2_PU_PROC_TYPE_STEREO_EXTENDER 0x03

// Audio Class-Specific Endpoint Descriptor Subtypes
#define TRU_USB_UAC2_EP_DESC_SUBTYPE_UNDEFINED  0x00
#define TRU_USB_UAC2_EP_DESC_SUBTYPE_EP_GENERAL 0x01

// Audio Class-Specific Setup request codes
#define TRU_USB_UAC2_REQ_UNDEFINED 0x00
#define TRU_USB_UAC2_REQ_CUR       0x01
#define TRU_USB_UAC2_REQ_RANGE     0x02
#define TRU_USB_UAC2_REQ_MEM       0x03

// Encoder Type Codes
#define TRU_USB_UAC2_ENC_TYPE_UNDEFINED 0x00
#define TRU_USB_UAC2_ENC_TYPE_OTHER     0x01
#define TRU_USB_UAC2_ENC_TYPE_MPEG      0x02
#define TRU_USB_UAC2_ENC_TYPE_AC3       0x03
#define TRU_USB_UAC2_ENC_TYPE_WMA       0x04
#define TRU_USB_UAC2_ENC_TYPE_DTS       0x05

// Decoder Type Codes
#define TRU_USB_UAC2_DEC_TYPE_UNDEFINED     0x00
#define TRU_USB_UAC2_DEC_TYPE_OTHER         0x01
#define TRU_USB_UAC2_DEC_TYPE_MPEG          0x02
#define TRU_USB_UAC2_DEC_TYPE_AC3           0x03
#define TRU_USB_UAC2_DEC_TYPE_WMA           0x04
#define TRU_USB_UAC2_DEC_TYPE_DTS           0x05

// ===================================
// Control Selector (CS) Request Codes
// ===================================

// Clock Source Control Selectors
#define TRU_USB_UAC2_CS_CS_UNDEFINED   0x00
#define TRU_USB_UAC2_CS_CS_SAM_FREQ    0x01
#define TRU_USB_UAC2_CS_CS_CLOCK_VALID 0x02

// Clock Selector Control Selectors
#define TRU_USB_UAC2_CS_CX_UNDEFINED 0x00
#define TRU_USB_UAC2_CS_CX_CLOCK     0x01

// Clock Multiplier Control Selectors
#define TRU_USB_UAC2_CS_CM_UNDEFINED   0x00
#define TRU_USB_UAC2_CS_CM_NUMERATOR   0x01
#define TRU_USB_UAC2_CS_CM_DENOMINATOR 0x02

// Terminal Control Selectors
#define TRU_USB_UAC2_CS_TE_UNDEFINED    0x00
#define TRU_USB_UAC2_CS_TE_COPY_PROTECT 0x01
#define TRU_USB_UAC2_CS_TE_CONNECTOR    0x02
#define TRU_USB_UAC2_CS_TE_OVERLOAD     0x03
#define TRU_USB_UAC2_CS_TE_CLUSTER      0x04
#define TRU_USB_UAC2_CS_TE_UNDERFLOW    0x05
#define TRU_USB_UAC2_CS_TE_OVERFLOW     0x06
#define TRU_USB_UAC2_CS_TE_LATENCY      0x07

// Mixer Unit Control Selectors
#define TRU_USB_UAC2_CS_MU_UNDEFINED 0x00
#define TRU_USB_UAC2_CS_MU_MIXER     0x01
#define TRU_USB_UAC2_CS_MU_CLUSTER   0x02
#define TRU_USB_UAC2_CS_MU_UNDERFLOW 0x03
#define TRU_USB_UAC2_CS_MU_OVERFLOW  0x04
#define TRU_USB_UAC2_CS_MU_LATENCY   0x05

// Selector Unit Control Selectors
#define TRU_USB_UAC2_CS_SU_UNDEFINED 0x00
#define TRU_USB_UAC2_CS_SU_SELECTOR  0x01
#define TRU_USB_UAC2_CS_SU_LATENCY   0x02

// Feature Unit Control Selectors
#define TRU_USB_UAC2_CS_FU_UNDEFINED         0x00
#define TRU_USB_UAC2_CS_FU_MUTE              0x01
#define TRU_USB_UAC2_CS_FU_VOLUME            0x02
#define TRU_USB_UAC2_CS_FU_BASS              0x03
#define TRU_USB_UAC2_CS_FU_MID               0x04
#define TRU_USB_UAC2_CS_FU_TREBLE            0x05
#define TRU_USB_UAC2_CS_FU_GRAPHIC_EQUALIZER 0x06
#define TRU_USB_UAC2_CS_FU_AUTOMATIC_GAIN    0x07
#define TRU_USB_UAC2_CS_FU_DELAY             0x08
#define TRU_USB_UAC2_CS_FU_BASS_BOOST        0x09
#define TRU_USB_UAC2_CS_FU_LOUDNESS          0x0A
#define TRU_USB_UAC2_CS_FU_INPUT_GAIN        0x0B
#define TRU_USB_UAC2_CS_FU_INPUT_GAIN_PAD    0x0C
#define TRU_USB_UAC2_CS_FU_PHASE_INVERTER    0x0D
#define TRU_USB_UAC2_CS_FU_UNDERFLOW         0x0E
#define TRU_USB_UAC2_CS_FU_OVERFLOW          0x0F
#define TRU_USB_UAC2_CS_FU_LATENCY           0x10

// Effect Unit Control Selectors
// Parametric Equalizer Section Effect Unit Control Selectors
#define TRU_USB_UAC2_CS_EU_PE_UNDEFINED        0x00
#define TRU_USB_UAC2_CS_EU_PE_ENABLE           0x01
#define TRU_USB_UAC2_CS_EU_PE_CENTERFREQ       0x02
#define TRU_USB_UAC2_CS_EU_PE_QFACTOR          0x03
#define TRU_USB_UAC2_CS_EU_PE_GAIN             0x04
#define TRU_USB_UAC2_CS_EU_PE_UNDERFLOW        0x05
#define TRU_USB_UAC2_CS_EU_PE_OVERFLOW         0x06
#define TRU_USB_UAC2_CS_EU_PE_LATENCY          0x07
// Reverberation Effect Unit Control Selectors
#define TRU_USB_UAC2_CS_EU_RV_UNDEFINED        0x00
#define TRU_USB_UAC2_CS_EU_RV_ENABLE           0x01
#define TRU_USB_UAC2_CS_EU_RV_TYPE             0x02
#define TRU_USB_UAC2_CS_EU_RV_LEVEL            0x03
#define TRU_USB_UAC2_CS_EU_RV_TIME             0x04
#define TRU_USB_UAC2_CS_EU_RV_FEEDBACK         0x05
#define TRU_USB_UAC2_CS_EU_RV_PREDELAY         0x06
#define TRU_USB_UAC2_CS_EU_RV_DENSITY          0x07
#define TRU_USB_UAC2_CS_EU_RV_HIFREQ_ROLLOFF   0x08
#define TRU_USB_UAC2_CS_EU_RV_UNDERFLOW        0x09
#define TRU_USB_UAC2_CS_EU_RV_OVERFLOW         0x0A
#define TRU_USB_UAC2_CS_EU_RV_LATENCY          0x0B
// Modulation Delay Effect Unit Control Selectors
#define TRU_USB_UAC2_CS_EU_MD_UNDEFINED        0x00
#define TRU_USB_UAC2_CS_EU_MD_ENABLE           0x01
#define TRU_USB_UAC2_CS_EU_MD_BALANCE          0x02
#define TRU_USB_UAC2_CS_EU_MD_RATE             0x03
#define TRU_USB_UAC2_CS_EU_MD_DEPTH            0x04
#define TRU_USB_UAC2_CS_EU_MD_TIME             0x05
#define TRU_USB_UAC2_CS_EU_MD_FEEDBACK         0x06
#define TRU_USB_UAC2_CS_EU_MD_UNDERFLOW        0x07
#define TRU_USB_UAC2_CS_EU_MD_OVERFLOW         0x08
#define TRU_USB_UAC2_CS_EU_MD_LATENCY          0x09
// Dynamic Range Compressor Effect Unit Control Selectors
#define TRU_USB_UAC2_CS_EU_DR_UNDEFINED        0x00
#define TRU_USB_UAC2_CS_EU_DR_ENABLE           0x01
#define TRU_USB_UAC2_CS_EU_DR_COMPRESSION_RATE 0x02
#define TRU_USB_UAC2_CS_EU_DR_MAXAMPL          0x03
#define TRU_USB_UAC2_CS_EU_DR_THRESHOLD        0x04
#define TRU_USB_UAC2_CS_EU_DR_ATTACK_TIME      0x05
#define TRU_USB_UAC2_CS_EU_DR_RELEASE_TIME     0x06
#define TRU_USB_UAC2_CS_EU_DR_UNDERFLOW        0x07
#define TRU_USB_UAC2_CS_EU_DR_OVERFLOW         0x08
#define TRU_USB_UAC2_CS_EU_DR_LATENCY          0x09

// Processing Unit Control Selectors
// Up/Down-mix Control Selectors
#define TRU_USB_UAC2_CS_PU_UD_UNDEFINED        0x00
#define TRU_USB_UAC2_CS_PU_UD_ENABLE           0x01
#define TRU_USB_UAC2_CS_PU_UD_MODE_SELECT      0x02
#define TRU_USB_UAC2_CS_PU_UD_CLUSTER          0x03
#define TRU_USB_UAC2_CS_PU_UD_UNDERFLOW        0x04
#define TRU_USB_UAC2_CS_PU_UD_OVERFLOW         0x05
#define TRU_USB_UAC2_CS_PU_UD_LATENCY          0x06
// Dolby Prologic Processing Unit Control Selectors
#define TRU_USB_UAC2_CS_PU_DP_UNDEFINED        0x00
#define TRU_USB_UAC2_CS_PU_DP_ENABLE           0x01
#define TRU_USB_UAC2_CS_PU_DP_MODE_SELECT      0x02
#define TRU_USB_UAC2_CS_PU_DP_CLUSTER          0x03
#define TRU_USB_UAC2_CS_PU_DP_UNDERFLOW        0x04
#define TRU_USB_UAC2_CS_PU_DP_OVERFLOW         0x05
#define TRU_USB_UAC2_CS_PU_DP_LATENCY          0x06
// Stereo Extender Processing Unit Control Selectors
#define TRU_USB_UAC2_CS_PU_ST_EXT_UNDEFINED    0x00
#define TRU_USB_UAC2_CS_PU_ST_EXT_ENABLE       0x01
#define TRU_USB_UAC2_CS_PU_ST_EXT_WIDTH        0x02
#define TRU_USB_UAC2_CS_PU_ST_EXT_UNDERFLOW    0x03
#define TRU_USB_UAC2_CS_PU_ST_EXT_OVERFLOW     0x04
#define TRU_USB_UAC2_CS_PU_ST_EXT_LATENCY      0x05

// Extension Unit Control Selectors
#define TRU_USB_UAC2_CS_XU_UNDEFINED 0x00
#define TRU_USB_UAC2_CS_XU_ENABLE    0x01
#define TRU_USB_UAC2_CS_XU_CLUSTER   0x02
#define TRU_USB_UAC2_CS_XU_UNDERFLOW 0x03
#define TRU_USB_UAC2_CS_XU_OVERFLOW  0x04
#define TRU_USB_UAC2_CS_XU_LATENCY   0x05

// AudioStreaming Interface Control Selectors
#define TRU_USB_UAC2_CS_AS_UNDEFINED         0x00
#define TRU_USB_UAC2_CS_AS_ACT_ALT_SETTING   0x01
#define TRU_USB_UAC2_CS_AS_VAL_ALT_SETTING   0x02
#define TRU_USB_UAC2_CS_AS_AUDIO_DATA_FORMAT 0x03

// Encoder Control Selectors
#define TRU_USB_UAC2_CS_EN_UNDEFINED     0x00
#define TRU_USB_UAC2_CS_EN_BIT_RATE      0x01
#define TRU_USB_UAC2_CS_EN_QUALITY       0x02
#define TRU_USB_UAC2_CS_EN_VBR           0x03
#define TRU_USB_UAC2_CS_EN_TYPE          0x04
#define TRU_USB_UAC2_CS_EN_UNDERFLOW     0x05
#define TRU_USB_UAC2_CS_EN_OVERFLOW      0x06
#define TRU_USB_UAC2_CS_EN_ENCODER_ERROR 0x07
#define TRU_USB_UAC2_CS_EN_PARAM1        0x08
#define TRU_USB_UAC2_CS_EN_PARAM2        0x09
#define TRU_USB_UAC2_CS_EN_PARAM3        0x0A
#define TRU_USB_UAC2_CS_EN_PARAM4        0x0B
#define TRU_USB_UAC2_CS_EN_PARAM5        0x0C
#define TRU_USB_UAC2_CS_EN_PARAM6        0x0D
#define TRU_USB_UAC2_CS_EN_PARAM7        0x0E
#define TRU_USB_UAC2_CS_EN_PARAM8        0x0F

// Decoder Control Selectors
// MPEG Decoder Control Selectors
#define TRU_USB_UAC2_CS_DE_MD_UNDEFINED     0x00
#define TRU_USB_UAC2_CS_DE_MD_DUAL_CHANNEL  0x01
#define TRU_USB_UAC2_CS_DE_MD_SECOND_STEREO 0x02
#define TRU_USB_UAC2_CS_DE_MD_MULTILINGUAL  0x03
#define TRU_USB_UAC2_CS_DE_MD_DYN_RANGE     0x04
#define TRU_USB_UAC2_CS_DE_MD_SCALING       0x05
#define TRU_USB_UAC2_CS_DE_MD_HILO_SCALING  0x06
#define TRU_USB_UAC2_CS_DE_MD_UNDERFLOW     0x07
#define TRU_USB_UAC2_CS_DE_MD_OVERFLOW      0x08
#define TRU_USB_UAC2_CS_DE_MD_DECODER_ERROR 0x09
// AC-3 Decoder Control Selectors
#define TRU_USB_UAC2_CS_DE_AD_UNDEFINED     0x00
#define TRU_USB_UAC2_CS_DE_AD_MODE          0x01
#define TRU_USB_UAC2_CS_DE_AD_DYN_RANGE     0x02
#define TRU_USB_UAC2_CS_DE_AD_SCALING       0x03
#define TRU_USB_UAC2_CS_DE_AD_HILO_SCALING  0x04
#define TRU_USB_UAC2_CS_DE_AD_UNDERFLOW     0x05
#define TRU_USB_UAC2_CS_DE_AD_OVERFLOW      0x06
#define TRU_USB_UAC2_CS_DE_AD_DECODER_ERROR 0x07
// WMA Decoder Control Selectors
#define TRU_USB_UAC2_CS_DE_WD_UNDEFINED     0x00
#define TRU_USB_UAC2_CS_DE_WD_UNDERFLOW     0x01
#define TRU_USB_UAC2_CS_DE_WD_OVERFLOW      0x02
#define TRU_USB_UAC2_CS_DE_WD_DECODER_ERROR 0x03
// DTS Decoder Control Selectors
#define TRU_USB_UAC2_CS_DE_DD_UNDEFINED     0x00
#define TRU_USB_UAC2_CS_DE_DD_UNDERFLOW     0x01
#define TRU_USB_UAC2_CS_DE_DD_OVERFLOW      0x02
#define TRU_USB_UAC2_CS_DE_DD_DECODER_ERROR 0x03

// Endpoint Control Selectors
#define TRU_USB_UAC2_CS_EP_UNDEFINED     0x00
#define TRU_USB_UAC2_CS_EP_PITCH         0x01
#define TRU_USB_UAC2_CS_EP_DATA_OVERRUN  0x02
#define TRU_USB_UAC2_CS_EP_DATA_UNDERRUN 0x03

// ====================================
// Control Selector Request Data Values
// ====================================

// Terminal control selector request copy protect control values
#define TRU_USB_UAC2_CS_TE_COPY_PROTECT_CPL0 0x00
#define TRU_USB_UAC2_CS_TE_COPY_PROTECT_CPL1 0x01
#define TRU_USB_UAC2_CS_TE_COPY_PROTECT_CPL2 0x02

// =======
// Bitmaps
// =======

// Channel config bitmap values
#define TRU_USB_UAC2_CHCFG_FL   0x00000001
#define TRU_USB_UAC2_CHCFG_FR   0x00000002
#define TRU_USB_UAC2_CHCFG_FC   0x00000004
#define TRU_USB_UAC2_CHCFG_LFE  0x00000008
#define TRU_USB_UAC2_CHCFG_BL   0x00000010
#define TRU_USB_UAC2_CHCFG_BR   0x00000020
#define TRU_USB_UAC2_CHCFG_FLC  0x00000040
#define TRU_USB_UAC2_CHCFG_FRC  0x00000080
#define TRU_USB_UAC2_CHCFG_BC   0x00000100
#define TRU_USB_UAC2_CHCFG_SL   0x00000200
#define TRU_USB_UAC2_CHCFG_SR   0x00000400
#define TRU_USB_UAC2_CHCFG_TC   0x00000800
#define TRU_USB_UAC2_CHCFG_TFL  0x00001000
#define TRU_USB_UAC2_CHCFG_TFC  0x00002000
#define TRU_USB_UAC2_CHCFG_TFR  0x00004000
#define TRU_USB_UAC2_CHCFG_TBL  0x00008000
#define TRU_USB_UAC2_CHCFG_TBC  0x00010000
#define TRU_USB_UAC2_CHCFG_TBR  0x00020000
#define TRU_USB_UAC2_CHCFG_TFLC 0x00040000
#define TRU_USB_UAC2_CHCFG_TFRC 0x00080000
#define TRU_USB_UAC2_CHCFG_LLFE 0x00100000
#define TRU_USB_UAC2_CHCFG_RLFE 0x00200000
#define TRU_USB_UAC2_CHCFG_TSL  0x00400000
#define TRU_USB_UAC2_CHCFG_TSR  0x00800000
#define TRU_USB_UAC2_CHCFG_BOC  0x01000000
#define TRU_USB_UAC2_CHCFG_BLC  0x02000000
#define TRU_USB_UAC2_CHCFG_BRC  0x04000000
#define TRU_USB_UAC2_CHCFG_RD   0x80000000

// Generic control bitmap values
#define TRU_USB_UAC2_CONTROL_NA 0x00
#define TRU_USB_UAC2_CONTROL_R  0x01
#define TRU_USB_UAC2_CONTROL_RW 0x03

// Clock source attribute bitmap values
#define TRU_USB_UAC2_CLOCK_SOURCE_ATR_EXTERNAL  0x00
#define TRU_USB_UAC2_CLOCK_SOURCE_ATR_INT_FIXED 0x01
#define TRU_USB_UAC2_CLOCK_SOURCE_ATR_INT_VAR   0x02
#define TRU_USB_UAC2_CLOCK_SOURCE_ATR_INT_PROG  0x03
#define TRU_USB_UAC2_CLOCK_SOURCE_ATR_SYNC_SOF  0x04

// Clock source control bitmap bit positions
#define TRU_USB_UAC2_CLOCK_SOURCE_CTL_FREQUENCY_POS 0
#define TRU_USB_UAC2_CLOCK_SOURCE_CTL_VALIDITY_POS  2

// Clock selector control bitmap bit positions
#define TRU_USB_UAC2_CLOCK_SEL_CTL_SELECTOR_POS 0

// Terminal control bitmap bit positions
#define TRU_USB_UAC2_TE_CTL_COPY_PROTECT_POS 0
#define TRU_USB_UAC2_TE_CTL_CONNECTOR_POS    2
#define TRU_USB_UAC2_TE_CTL_OVERLOAD_POS     4
#define TRU_USB_UAC2_TE_CTL_CLUSTER_POS      6
#define TRU_USB_UAC2_TE_CTL_UNDERFLOW_POS    8
#define TRU_USB_UAC2_TE_CTL_OVERFLOW_POS     10

// Feature unit control bitmap bit positions
#define TRU_USB_UAC2_FE_CTL_MUTE_POS           0
#define TRU_USB_UAC2_FE_CTL_VOLUME_POS         2
#define TRU_USB_UAC2_FE_CTL_BASS_POS           4
#define TRU_USB_UAC2_FE_CTL_MID_POS            6
#define TRU_USB_UAC2_FE_CTL_TREBLE_POS         8
#define TRU_USB_UAC2_FE_CTL_EQUALIZER_POS      10
#define TRU_USB_UAC2_FE_CTL_AGC_POS            12
#define TRU_USB_UAC2_FE_CTL_DELAY_POS          14
#define TRU_USB_UAC2_FE_CTL_BASS_BOOST_POS     16
#define TRU_USB_UAC2_FE_CTL_LOUDNESS_POS       18
#define TRU_USB_UAC2_FE_CTL_INPUT_GAIN_POS     20
#define TRU_USB_UAC2_FE_CTL_INPUT_GAIN_PAD_POS 22
#define TRU_USB_UAC2_FE_CTL_PHASE_INVERTER_POS 24
#define TRU_USB_UAC2_FE_CTL_UNDERFLOW_POS      26
#define TRU_USB_UAC2_FE_CTL_OVERFLOW_POS       28

// Class Isochronous EndPoint control bitmap bit positions
#define TRU_USB_UAC2_CLS_ISO_EP_CTL_PITCH_POS         0
#define TRU_USB_UAC2_CLS_ISO_EP_CTL_DATA_OVERRUN_POS  2
#define TRU_USB_UAC2_CLS_ISO_EP_CTL_DATA_UNDERRUN_POS 4

typedef union{
	uint8_t val;
	struct{
		uint8_t latency :2;
		uint8_t res2_7  :6;
	}bits;
}tru_usb_uac2_cls_ac_itf_hdr_controls_t;

typedef union{
	uint8_t val;
	struct{
		uint8_t clock_type  :2;
		uint8_t sync_to_sof :1;
		uint8_t res3_7      :5;
	}bits;
}tru_usb_uac2_clk_src_attributes_t;

typedef union{
	uint8_t val;
	struct{
		uint8_t clock_freq     :2;
		uint8_t clock_validity :2;
		uint8_t res4_7         :4;
	}bits;
}tru_usb_uac2_clk_src_controls_t;

typedef union{
	uint8_t val;
	struct{
		uint8_t clock_sel :2;
		uint8_t res2_7    :6;
	}bits;
}tru_usb_uac2_clk_sel_controls_t;

typedef union{
	uint16_t val;
	struct{
		uint16_t copy_protect :2;
		uint16_t connector    :2;
		uint16_t overload     :2;
		uint16_t cluster      :2;
		uint16_t underflow    :2;
		uint16_t overflow     :2;
		uint16_t res12_15     :4;
	}bits;
}tru_usb_uac_te_controls_t;

typedef union{
	uint32_t val;
	struct{
		uint32_t mute           :2;
		uint32_t volume         :2;
		uint32_t bass           :2;
		uint32_t mid            :2;
		uint32_t treble         :2;
		uint32_t equalizer      :2;
		uint32_t agc            :2;
		uint32_t delay          :2;
		uint32_t bass_boost     :2;
		uint32_t loudness       :2;
		uint32_t input_gain     :2;
		uint32_t input_gain_pad :2;
		uint32_t phase_inverter :2;
		uint32_t underflow      :2;
		uint32_t overflow       :2;
		uint32_t res30_31       :2;
	}bits;
}tru_usb_uac2_fu_controls_t;

// Class-Specific AC (AudioControl) Interface Header Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint16_t adc;
	uint8_t category;
	uint16_t total_length;
	tru_usb_uac2_cls_ac_itf_hdr_controls_t controls;
}tru_usb_uac2_ac_itf_hdr_desc_t;

// Clock Source Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t clock_id;
	tru_usb_uac2_clk_src_attributes_t attributes;
	tru_usb_uac2_clk_src_controls_t controls;
	uint8_t assoc_terminal;
	uint8_t clock_source;
}tru_usb_uac2_clock_source_desc_t;

/*
// Clock Selector Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t clock_id;
	uint8_t num_in_pins;
	uint8_t *c_source_id;
	uint8_t controls;
	uint8_t clock_sel;
}tru_usb_uac2_clock_sel_desc_t;
*/

// Clock Multiplier Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t clock_id;
	uint8_t c_source_id;
	uint8_t controls;
	uint8_t clock_mul;
}tru_usb_uac2_clock_mul_desc_t;

// Input Terminal Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t terminal_id;
	uint16_t terminal_type;
	uint8_t assoc_terminal;
	uint8_t c_source_id;
	uint8_t num_channels;
	uint32_t channel_config;
	uint8_t channel_names;
	tru_usb_uac_te_controls_t controls;
	uint8_t terminal;
}tru_usb_uac2_input_terminal_desc_t;

// Output Terminal Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t terminal_id;
	uint16_t terminal_type;
	uint8_t assoc_terminal;
	uint8_t source_id;
	uint8_t c_source_id;
	tru_usb_uac_te_controls_t controls;
	uint8_t terminal;
}tru_usb_uac2_output_terminal_desc_t;

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
	uint32_t channel_config;
	uint8_t channel_names;
	uint8_t *mixer_controls;
	uint8_t controls;
	uint8_t mixer;
}tru_usb_uac2_mu_desc_t;
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
	uint8_t controls;
	uint8_t selector;
}tru_usb_uac2_su_desc_t;
*/

// Feature Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint8_t source_id;
	uint32_t *ma_controls;
	uint8_t feature;
}tru_usb_uac2_fu_desc_t;
*/

// Sampling Rate Converter Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint8_t source_id;
	uint8_t c_source_id;
	uint8_t c_source_out_id;
	uint8_t src;
}tru_usb_uac2_samp_rate_conv_desc_t;

// Effect Unit Descriptor
/*
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t effect_type;
	uint8_t source_id;
	uint32_t *ma_controls;
	uint8_t *effects;
}tru_usb_uac2_eu_desc_t;
*/

/*
// Parametric Equalizer Section Effect Unit Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t effect_type;
	uint8_t source_id;
	uint32_t *ma_controls;
	uint8_t *effects;
}tru_usb_uac2_eu_pe_desc_t;

// Reverberation Effect Unit Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t effect_type;
	uint8_t source_id;
	uint32_t *ma_controls;
	uint8_t *effects;
}tru_usb_uac2_eu_rv_desc_t;

// Modulation Delay Effect Unit Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t effect_type;
	uint8_t source_id;
	uint32_t *ma_controls;
	uint8_t *effects;
}tru_usb_uac2_eu_md_desc_t;

// Dynamic Range Compressor Effect Unit Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t effect_type;
	uint8_t source_id;
	uint32_t *ma_controls;
	uint8_t *effects;
}tru_usb_uac2_eu_dr_desc_t;
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
	uint32_t channel_config;
	uint8_t channel_names;
	uint16_t controls;
	uint8_t processing;
	uint8_t *process_specific;
}tru_usb_uac2_pu_desc_t;
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
	uint32_t channel_config;
	uint8_t channel_names;
	uint16_t controls;
	uint8_t processing;
	uint8_t num_modes;
	uint32_t *modes;
}tru_usb_uac2_pu_ud_desc_t;
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
	uint32_t channel_config;
	uint8_t channel_names;
	uint16_t controls;
	uint8_t processing;
	uint8_t num_modes;
	uint32_t *modes;
}tru_usb_uac2_pu_dp_desc_t;
*/

// Stereo Extender Processing Unit Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t unit_id;
	uint16_t process_type;
	uint8_t num_in_pins;
	uint8_t source_id;
	uint8_t num_channels;
	uint32_t channel_config;
	uint8_t channel_names;
	uint16_t controls;
	uint8_t processing;
}tru_usb_uac2_pu_st_ext_desc_t;

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
	uint32_t channel_config;
	uint8_t channel_names;
	uint8_t controls;
	uint8_t extension;
}tru_usb_uac2_xu_desc_t;
*/

// ============
// AudioControl
// ============

// ==============
// AudioStreaming
// ==============

typedef union{
	uint8_t val;
	struct{
		uint8_t active_alt_setting :2;
		uint8_t valid_alt_settings :2;
		uint8_t res4_7             :4;
	}bits;
}tru_usb_uac2_as_cls_itf_controls_t;

// Class-Specific AS (AudioStreaming) Interface Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t terminal_link;
	tru_usb_uac2_as_cls_itf_controls_t controls;
	uint8_t format_type;
	uint32_t formats;
	uint8_t num_channels;
	uint32_t channel_config;
	uint8_t channel_names;
}tru_usb_uac2_as_cls_itf_desc_t;

// Encoder Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t encoder_id;
	uint8_t encoder_type;
	uint32_t controls;
	uint8_t param1;
	uint8_t param2;
	uint8_t param3;
	uint8_t param4;
	uint8_t param5;
	uint8_t param6;
	uint8_t param7;
	uint8_t param8;
	uint8_t encoder;
}tru_usb_uac2_as_encoder_desc_t;

// MPEG Decoder Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t decoder_id;
	uint8_t decoder_type;
	uint16_t mpeg_capabilities;
	uint8_t mpeg_features;
	uint8_t controls;
	uint8_t decoder;
}tru_usb_uac2_as_mpeg_decoder_desc_t;

// AC-3 Decoder Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t decoder_id;
	uint8_t decoder_type;
	uint32_t bsid;
	uint8_t ac3_features;
	uint8_t controls;
	uint8_t decoder;
}tru_usb_uac2_as_ac3_decoder_desc_t;

// WMA Decoder Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t decoder_id;
	uint8_t decoder_type;
	uint16_t wma_profile;
	uint8_t controls;
	uint8_t decoder;
}tru_usb_uac2_as_wma_decoder_desc_t;

// DTS Decoder Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t decoder_id;
	uint8_t decoder_type;
	uint8_t capabilities;
	uint8_t controls;
	uint8_t decoder;
}tru_usb_uac2_as_dts_decoder_desc_t;

typedef union{
	uint8_t val;
	struct{
		uint8_t res0_6           :7;
		uint8_t max_packets_only :1;
	}bits;
}tru_usb_uac2_as_cls_iso_ep_attributes_t;

typedef union{
	uint8_t val;
	struct{
		uint8_t pitch         :2;
		uint8_t data_overrun  :2;
		uint8_t data_underrun :2;
		uint8_t res6_7        :2;
	}bits;
}tru_usb_uac_as_cls_iso_ep_controls_t;

// Class-Specific AS (AudioStreaming) Isochronous Audio Data Endpoint Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	tru_usb_uac2_as_cls_iso_ep_attributes_t attributes;
	tru_usb_uac_as_cls_iso_ep_controls_t controls;
	uint8_t lock_delay_units;
	uint16_t lock_delay;
}tru_usb_uac2_as_cls_iso_ep_desc_t;

// Layout 1 Control Current Parameter Block
typedef struct{
	uint8_t cur;
}tru_usb_uac2_layout1_cur_t;

// Single Layout 1 Control Range
typedef struct{
	uint8_t min;
	uint8_t max;
	uint8_t res;
}tru_usb_uac2_layout1_range_t;

// Layout 2 Control Current Parameter Block
typedef struct{
	uint16_t cur;
}tru_usb_uac2_layout2_cur_t;

// Single Layout 2 Control Range
typedef struct{
	uint16_t min;
	uint16_t max;
	uint16_t res;
}tru_usb_uac2_layout2_range_t;

// Layout 3 Control Current Parameter Block
typedef struct{
	uint32_t cur;
}tru_usb_uac2_layout3_cur_t;

// Single Layout 3 Control Range
typedef struct{
	uint32_t min;
	uint32_t max;
	uint32_t res;
}tru_usb_uac2_layout3_range_t;

// ==================
// UAC2 Audio Formats
// ==================

// Format Type Codes
#define TRU_USB_UAC2_AF_TYPE_UNDEFINED 0x00
#define TRU_USB_UAC2_AF_TYPE1          0x01
#define TRU_USB_UAC2_AF_TYPE2          0x02
#define TRU_USB_UAC2_AF_TYPE3          0x03
#define TRU_USB_UAC2_AF_TYPE4          0x04
#define TRU_USB_UAC2_AF_EXT_TYPE1      0x81
#define TRU_USB_UAC2_AF_EXT_TYPE2      0x82
#define TRU_USB_UAC2_AF_EXT_TYPE3      0x83

// Audio Data Format Type I Bit Allocations
#define TRU_USB_UAC2_AF_TYPE1_PCM        0x00000001
#define TRU_USB_UAC2_AF_TYPE1_PCM8       0x00000002
#define TRU_USB_UAC2_AF_TYPE1_IEEE_FLOAT 0x00000004
#define TRU_USB_UAC2_AF_TYPE1_ALAW       0x00000008
#define TRU_USB_UAC2_AF_TYPE1_MULAW      0x00000010
#define TRU_USB_UAC2_AF_TYPE1_RAW_DATA   0x80000000

// Audio Data Format Type II Bit Allocations
#define TRU_USB_UAC2_AF_TYPE2_MPEG     0x00000001
#define TRU_USB_UAC2_AF_TYPE2_AC3      0x00000002
#define TRU_USB_UAC2_AF_TYPE2_WMA      0x00000004
#define TRU_USB_UAC2_AF_TYPE2_DTS      0x00000008
#define TRU_USB_UAC2_AF_TYPE2_RAW_DATA 0x80000000

// Audio Data Format Type III Bit Allocations
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_AC3                    0x00000001
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_MPEG1_L1               0x00000002
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_MPEG1_L2L3_MPEG2_NOEXT 0x00000004
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_MPEG2_EXT              0x00000008
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_MPEG2_AAC_ADTS         0x00000010
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_MPEG2_L1LS             0x00000020
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_MPEG2_L2L3LS           0x00000040
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_DTS1                   0x00000080
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_DTS2                   0x00000100
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_DTS3                   0x00000200
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_ATRAC                  0x00000400
#define TRU_USB_UAC2_AF_TYPE3_IEC1937_ATRAC2_3               0x00000800
#define TRU_USB_UAC2_AF_TYPE3_WMA                            0x00001000

// Audio Data Format Type IV Bit Allocations
#define TRU_USB_UAC2_AF_TYPE4_PCM                            0x00000001
#define TRU_USB_UAC2_AF_TYPE4_PCM8                           0x00000002
#define TRU_USB_UAC2_AF_TYPE4_IEEE_FLOAT                     0x00000004
#define TRU_USB_UAC2_AF_TYPE4_ALAW                           0x00000008
#define TRU_USB_UAC2_AF_TYPE4_MULAW                          0x00000010
#define TRU_USB_UAC2_AF_TYPE4_MPEG                           0x00000020
#define TRU_USB_UAC2_AF_TYPE4_AC3                            0x00000040
#define TRU_USB_UAC2_AF_TYPE4_WMA                            0x00000080
#define TRU_USB_UAC2_AF_TYPE4_IEC61937_AC3                   0x00000100
#define TRU_USB_UAC2_AF_TYPE4_IEC61937_MPEG1_L1              0x00000200
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_MPEG1_L2L3_MPEG2_NOEXT 0x00000400
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_MPEG2_EXT              0x00000800
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_MPEG2_AAC_ADTS         0x00001000
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_MPEG2_L1LS             0x00002000
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_MPEG2_L2L3LS           0x00004000
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_DTS1                   0x00008000
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_DTS2                   0x00010000
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_DTS3                   0x00020000
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_ATRAC                  0x00040000
#define TRU_USB_UAC2_AF_TYPE4_IEC1937_ATRAC2_3               0x00080000
#define TRU_USB_UAC2_AF_TYPE4_TYPE_III_WMA                   0x00100000
#define TRU_USB_UAC2_AF_TYPE4_IEC60958_PCM                   0x00200000

// Side Band Protocol Codes
#define TRU_USB_UAC2_AF_SB_PROTOCOL_UNDEFINED      0x00
#define TRU_USB_UAC2_AF_SB_PROTOCOL_PRES_TIMESTAMP 0x01

// Type I Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint8_t subslot;
	uint8_t bit_resolution;
}tru_usb_uac2_af_type1_desc_t;

// Type II Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint16_t max_bit_rate;
	uint16_t slots_per_frame;
}tru_usb_uac2_af_type2_desc_t;

// Type III Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint8_t subslot_size;
	uint8_t bit_resolution;
}tru_usb_uac2_af_type3_desc_t;

// Type IV Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
}tru_usb_uac2_af_type4_desc_t;

// Extended Type I Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint8_t subslot_size;
	uint8_t bit_resolution;
	uint8_t header_length;
	uint8_t control_size;
	uint8_t sideband_protocol;
}tru_usb_uac2_af_ext_type1_desc_t;

// Extended Type II Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint16_t max_bit_rate;
	uint16_t samples_per_frame;
	uint8_t header_length;
	uint8_t sideband_protocol;
}tru_usb_uac2_af_ext_type2_desc_t;

// Extended Type III Format Type Descriptor
typedef struct __attribute__((packed)){
	uint8_t length;
	uint8_t desc_type;
	uint8_t desc_subtype;
	uint8_t format_type;
	uint8_t subslot_size;
	uint8_t bit_resolution;
	uint8_t header_length;
	uint8_t sideband_protocol;
}tru_usb_uac2_af_ext_type3_desc_t;

// Presentation Timestamp Side Band Protocol
typedef struct __attribute__((packed)){
	uint32_t flags;
	uint32_t nanoseconds[2];
}tru_usb_uac2_af_hires_ptsb_desc_t;

// ==================
// USB Terminal Types
// ==================

// USB Terminal Types
#define TRU_USB_UAC2_TERMT_USB_UNDEFINED                     0x0100
#define TRU_USB_UAC2_TERMT_USB_STREAMING                     0x0101
#define TRU_USB_UAC2_TERMT_USB_VENDOR                        0x01ff

// Input Terminal Types
#define TRU_USB_UAC2_TERMT_INPUT_UNDEFINED                   0x0200
#define TRU_USB_UAC2_TERMT_INPUT_MICROPHONE                  0x0201
#define TRU_USB_UAC2_TERMT_INPUT_DESKTOP_MICROPHONE          0x0202
#define TRU_USB_UAC2_TERMT_INPUT_PERSONAL_MICROPHONE         0x0203
#define TRU_USB_UAC2_TERMT_INPUT_OMNIDIR_MICROPHONE          0x0204
#define TRU_USB_UAC2_TERMT_INPUT_MICROPHONE_ARRAY            0x0205
#define TRU_USB_UAC2_TERMT_INPUT_PROCESSING_MICROPHONE_ARRAY 0x0206

// Output Terminal Types
#define TRU_USB_UAC2_TERMT_OUTPUT_UNDEFINED                  0x0300
#define TRU_USB_UAC2_TERMT_OUTPUT_SPEAKER                    0x0301
#define TRU_USB_UAC2_TERMT_OUTPUT_HEADPHONES                 0x0302
#define TRU_USB_UAC2_TERMT_OUTPUT_HEAD_MOUNTED_DISPLAY_AUDIO 0x0303
#define TRU_USB_UAC2_TERMT_OUTPUT_DESKTOP_SPEAKER            0x0304
#define TRU_USB_UAC2_TERMT_OUTPUT_ROOM_SPEAKER               0x0305
#define TRU_USB_UAC2_TERMT_OUTPUT_COMMUNICATION_SPEAKER      0x0306
#define TRU_USB_UAC2_TERMT_OUTPUT_LOW_FREQ_EFFECTS_SPEAKER   0x0307

// Bi-directional Terminal Types
#define TRU_USB_UAC2_TERMT_BI_UNDEFINED                      0x0400
#define TRU_USB_UAC2_TERMT_BI_HANDSET                        0x0401
#define TRU_USB_UAC2_TERMT_BI_HEADSET                        0x0402
#define TRU_USB_UAC2_TERMT_BI_SPEAKERPHONE_NO_ECHO_R         0x0403
#define TRU_USB_UAC2_TERMT_BI_ECHO_SUPPRESSING_SPEAKERPHONE  0x0404
#define TRU_USB_UAC2_TERMT_BI_ECHO_CANCELING_SPEAKERPHONE    0x0405

// Telephony Terminal Types
#define TRU_USB_UAC2_TERMT_TP_UNDEFINED                      0x0500
#define TRU_USB_UAC2_TERMT_TP_PHONELINE                      0x0501
#define TRU_USB_UAC2_TERMT_TP_TELPHONE                       0x0502
#define TRU_USB_UAC2_TERMT_TP_DOWN_LINE_PHONE                0x0503

// External Terminal Types
#define TRU_USB_UAC2_TERMT_EXT_UNDEFINED                     0x0600
#define TRU_USB_UAC2_TERMT_EXT_ANALOG_CONNECTOR              0x0601
#define TRU_USB_UAC2_TERMT_EXT_DIGITAL_AUDIO_INTERFACE       0x0602
#define TRU_USB_UAC2_TERMT_EXT_LINE_CONNECTOR                0x0603
#define TRU_USB_UAC2_TERMT_EXT_LEGACY_AUDIO_CONNECTOR        0x0604
#define TRU_USB_UAC2_TERMT_EXT_SPDIF                         0x0605
#define TRU_USB_UAC2_TERMT_EXT_1394_DA_STREAM                0x0606
#define TRU_USB_UAC2_TERMT_EXT_1394_DV_STREAM_SOUND_TRACK    0x0607
#define TRU_USB_UAC2_TERMT_EXT_ADAT_LIGHTPIPE                0x0608
#define TRU_USB_UAC2_TERMT_EXT_TDIF                          0x0609
#define TRU_USB_UAC2_TERMT_EXT_MADI                          0x060A

// Embedded Terminal Types
#define TRU_USB_UAC2_TERMT_EMB_UNDEFINED                      0x0700
#define TRU_USB_UAC2_TERMT_EMB_LEVEL_CALIBRATION_NOISE_SOURCE 0x0701
#define TRU_USB_UAC2_TERMT_EMB_EQUALIZATION_NOISE             0x0702
#define TRU_USB_UAC2_TERMT_EMB_CD_PLAYER                      0x0703
#define TRU_USB_UAC2_TERMT_EMB_DAT                            0x0704
#define TRU_USB_UAC2_TERMT_EMB_DCC                            0x0705
#define TRU_USB_UAC2_TERMT_EMB_MINIDISK                       0x0706
#define TRU_USB_UAC2_TERMT_EMB_ANALOG_TAPE                    0x0707
#define TRU_USB_UAC2_TERMT_EMB_PHONOGRAPH                     0x0708
#define TRU_USB_UAC2_TERMT_EMB_VCR_AUDIO                      0x0709
#define TRU_USB_UAC2_TERMT_EMB_VIDEO_DISC_AUDIO               0x070a
#define TRU_USB_UAC2_TERMT_EMB_DVD_AUDIO                      0x070b
#define TRU_USB_UAC2_TERMT_EMB_TV_TUNER_AUDIO                 0x070c
#define TRU_USB_UAC2_TERMT_EMB_SATELLITE_TV_AUDIO             0x070d
#define TRU_USB_UAC2_TERMT_EMB_CABLE_TUNER_AUDIO              0x070e
#define TRU_USB_UAC2_TERMT_EMB_DSS_AUDIO                      0x070f
#define TRU_USB_UAC2_TERMT_EMB_RADIO_RECEIVER                 0x0710
#define TRU_USB_UAC2_TERMT_EMB_RADIO_TRANSMITTER              0x0711
#define TRU_USB_UAC2_TERMT_EMB_MULTITRACK_RECORDER            0x0712
#define TRU_USB_UAC2_TERMT_EMB_SYNTHESIZER                    0x0713
#define TRU_USB_UAC2_TERMT_EMB_PIANO                          0x0714
#define TRU_USB_UAC2_TERMT_EMB_GUITAR                         0x0715
#define TRU_USB_UAC2_TERMT_EMB_DRUMS_RHYTHM                   0x0716
#define TRU_USB_UAC2_TERMT_EMB_OTHER_MUSICAL_INSTRUMENT       0x0717

// =============================
// Programming model definitions
// =============================

// =======================================================
// Programming model class interface extension definitions
// =======================================================

// Related Audio Control or Audio Streaming interfaces forming the Audio Function
typedef struct{
	uint8_t itf_num;
}tru_usb_uac2_related_itf_t;

typedef struct{
	uint8_t entity_id;
	uint8_t channel_num;
	uint32_t *cur;
	void *range;
}tru_usb_uac2_clk_frq_t;

typedef struct{
	uint8_t entity_id;
	uint8_t channel_num;
	uint8_t *cur;
}tru_usb_uac2_clk_sel_t;

typedef struct{
	uint8_t entity_id;
	uint8_t *cur;
}tru_usb_uac2_copyprotect_t;

typedef struct{
	uint8_t entity_id;
	uint8_t channel_num;
	uint8_t *cur;
}tru_usb_uac2_mute_t;

typedef struct{
	uint8_t entity_id;
	uint8_t channel_num;
	uint16_t *cur;
	void *range;
}tru_usb_uac2_volume_t;

typedef struct{
	uint8_t num_channels;
	uint8_t subslot_size;
	uint8_t bit_resolution;
}tru_usb_uac2_af_t;

// UAC class interface extension (programming model)
typedef struct{
	tru_usb_interface_ext_hdr_t ext_hdr;
	bool is_audio_control;
	tru_usb_uac2_related_itf_t *related_itfs;
	uint32_t num_related_itfs;
	tru_usb_uac2_clk_frq_t *clk_frqs;
	uint32_t num_clk_frqs;
	tru_usb_uac2_clk_sel_t *clk_sels;
	uint32_t num_clk_sels;
	tru_usb_uac2_copyprotect_t *copyprotects;
	uint32_t num_copyprotects;
	tru_usb_uac2_mute_t *mutes;
	uint32_t num_mutes;
	tru_usb_uac2_volume_t *volumes;
	uint32_t num_volumes;
	tru_usb_uac2_af_t af;
}tru_usb_uac2_interface_ext_t;

typedef struct{
	uint8_t *cur;
}tru_usb_uac2_pitch_t;

// UAC class endpoint extension (programming model)
typedef struct{
	uint8_t ep_addr;
	tru_usb_uac2_pitch_t *pitch;
}tru_usb_uac2_ep_ext_t;

// A macro to cast a pointer to a class interface extension pointer
#define TO_UAC2_INTERFACE_EXT_PTR(ptr) ((tru_usb_uac2_interface_ext_t *)ptr)

// UAC peripheral device descriptor set
typedef struct{
	// ====================================
	// Common peripheral device descriptors
	// ====================================
	tru_usb_pd_info_t *pd;

	// =====================================
	// Class interface extension descriptors
	// =====================================

	tru_usb_uac2_interface_ext_t *hs_uac2_interface_exts;
	uint16_t hs_uac2_interface_exts_num_entries;

	tru_usb_uac2_interface_ext_t *fs_uac2_interface_exts;
	uint16_t fs_uac2_interface_exts_num_entries;

	tru_usb_uac2_interface_ext_t *ls_uac2_interface_exts;
	uint16_t ls_uac2_interface_exts_num_entries;

	tru_usb_uac2_ep_ext_t *hs_uac2_ep_exts;
	uint8_t hs_uac2_ep_exts_num_entries;

	tru_usb_uac2_ep_ext_t *fs_uac2_ep_exts;
	uint8_t fs_uac2_ep_exts_num_entries;

	tru_usb_uac2_ep_ext_t *ls_uac2_ep_exts;
	uint8_t ls_uac2_ep_exts_num_entries;
}tru_usb_uac2_pd_info_t;

#ifdef __cplusplus
}
#endif

#endif
