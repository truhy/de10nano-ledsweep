/*
	MIT License

	Copyright (c) 2024 Truong Hy

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

	Definitions for Synopsys DWC2 USB 2.0 OTG controller.
*/

#ifndef TRU_DWC2_DEF_H
#define TRU_DWC2_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// =========
// Registers
// =========

#define TRU_DWC2_GOTGCTL_OFFSET              0x0
#define TRU_DWC2_GOTGCTL_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GOTGCTL_OFFSET))
#define TRU_DWC2_GOTGCTL_SESREQSCS_SET_MSK   0x00000001
#define TRU_DWC2_GOTGCTL_HSTNEGSCS_SET_MSK   0x00000100

#define TRU_DWC2_GOTGINT_OFFSET                   0x4
#define TRU_DWC2_GOTGINT_ADDR(usb_base_addr)      ((void *)((char *)usb_base_addr + TRU_DWC2_GOTGINT_OFFSET))
#define TRU_DWC2_GOTGINT_SESENDDET_SET_MSK        0x00000004
#define TRU_DWC2_GOTGINT_SESREQSUCSTSCHNG_SET_MSK 0x00000100
#define TRU_DWC2_GOTGINT_HSTNEGSUCSTSCHNG_SET_MSK 0x00000200
#define TRU_DWC2_GOTGINT_HSTNEGDET_SET_MSK        0x00020000
#define TRU_DWC2_GOTGINT_ADEVTOUTCHG_SET_MSK      0x00040000
#define TRU_DWC2_GOTGINT_DBNCEDONE_SET_MSK        0x00080000

#define TRU_DWC2_GAHBCFG_OFFSET                  0x8
#define TRU_DWC2_GAHBCFG_ADDR(usb_base_addr)     ((void *)((char *)usb_base_addr + TRU_DWC2_GAHBCFG_OFFSET))
#define TRU_DWC2_GAHBCFG_GLBLINTRMSK_SET_MSK     0x00000001
#define TRU_DWC2_GAHBCFG_HBSTLEN_SET_MSK         0x0000001e
#define TRU_DWC2_GAHBCFG_DMAEN_SET_MSK           0x00000020
#define TRU_DWC2_GAHBCFG_NPTXFEMPLVL_SET_MSK     0x00000080
#define TRU_DWC2_GAHBCFG_PTXFEMPLVL_SET_MSK      0x00000100
#define TRU_DWC2_GAHBCFG_REMMEMSUPP_SET_MSK      0x00200000
#define TRU_DWC2_GAHBCFG_NOTIALLDMAWRIT_SET_MSK  0x00400000
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD1ORSINGLE   0x0
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD4ORINCR     0x1
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD8           0x2
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD16ORINCR4   0x3
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD32          0x4
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD64ORINCR8   0x5
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD128         0x6
#define TRU_DWC2_GAHBCFG_HBSTLEN_WORD256ORINCR16 0x7
#define TRU_DWC2_GAHBCFG_HBSTLEN_LSB             1
#define TRU_DWC2_GAHBCFG_DMAEN_SLAVE             0x0
#define TRU_DWC2_GAHBCFG_DMAEN_DMA               0x1
#define TRU_DWC2_GAHBCFG_DMAEN_LSB               5
#define TRU_DWC2_GAHBCFG_NPTXFEMPLVL_HALFEMPTY   0x0
#define TRU_DWC2_GAHBCFG_NPTXFEMPLVL_EMPTY       0x1
#define TRU_DWC2_GAHBCFG_NPTXFEMPLVL_LSB         7
#define TRU_DWC2_GAHBCFG_PTXFEMPLVL_HALFEMPTY    0x0
#define TRU_DWC2_GAHBCFG_PTXFEMPLVL_EMPTY        0x1
#define TRU_DWC2_GAHBCFG_PTXFEMPLVL_LSB          8
#define TRU_DWC2_GAHBCFG_REMMEMSUPP_NOWAIT       0x0
#define TRU_DWC2_GAHBCFG_REMMEMSUPP_WAIT         0x1
#define TRU_DWC2_GAHBCFG_REMMEMSUPP_LSB          21
#define TRU_DWC2_GAHBCFG_NOTIALLDMAWRIT_ALL      0x1
#define TRU_DWC2_GAHBCFG_NOTIALLDMAWRIT_LAST     0x0
#define TRU_DWC2_GAHBCFG_NOTIALLDMAWRIT_LSB      22

#define TRU_DWC2_GUSBCFG_OFFSET              0xc
#define TRU_DWC2_GUSBCFG_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GUSBCFG_OFFSET))
#define TRU_DWC2_GUSBCFG_CORRUPTTXPKT_SET_MSK         0x80000000
#define TRU_DWC2_GUSBCFG_FORCEDEVMODE_SET_MSK         0x40000000
#define TRU_DWC2_GUSBCFG_FORCEHSTMODE_SET_MSK         0x20000000
#define TRU_DWC2_GUSBCFG_TXENDDELAY_SET_MSK           0x10000000
#define TRU_DWC2_GUSBCFG_ULPI_SET_MSK                 0x02000000
#define TRU_DWC2_GUSBCFG_INDICATOR_SET_MSK            0x01000000
#define TRU_DWC2_GUSBCFG_COMPLEMENT_SET_MSK           0x00800000
#define TRU_DWC2_GUSBCFG_TERMSELDLPULSE_SET_MSK       0x00400000
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSINDICATOR_SET_MSK 0x00200000
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSDRV_SET_MSK       0x00100000
#define TRU_DWC2_GUSBCFG_ULPICLKSUSM_SET_MSK          0x00080000
#define TRU_DWC2_GUSBCFG_ULPIAUTORES_SET_MSK          0x00040000
#define TRU_DWC2_GUSBCFG_USBTRDTIM_SET_MSK            0x00003c00
#define TRU_DWC2_GUSBCFG_HNPCAP_SET_MSK               0x00000200
#define TRU_DWC2_GUSBCFG_SRPCAP_SET_MSK               0x00000100
#define TRU_DWC2_GUSBCFG_DDRSEL_SET_MSK               0x00000080
#define TRU_DWC2_GUSBCFG_PHYSEL_SET_MSK               0x00000040
#define TRU_DWC2_GUSBCFG_FSINTF_SET_MSK               0x00000020
#define TRU_DWC2_GUSBCFG_PHYIF_SET_MSK                0x00000008
#define TRU_DWC2_GUSBCFG_TOUTCAL_SET_MSK              0x00000007
#define TRU_DWC2_GUSBCFG_TOUTCAL_LSB                  0
#define TRU_DWC2_GUSBCFG_PHYIF_16BIT                  0x1
#define TRU_DWC2_GUSBCFG_PHYIF_8BIT                   0x0
#define TRU_DWC2_GUSBCFG_PHYIF_LSB                    3
#define TRU_DWC2_GUSBCFG_ULPI_UTMI_SEL_ULPI           0x0
#define TRU_DWC2_GUSBCFG_ULPI_UTMI_SEL_UTMI           0x1
#define TRU_DWC2_GUSBCFG_ULPI_UTMI_SEL_LSB            4
#define TRU_DWC2_GUSBCFG_FSINTF_FS6PIN                0x0
#define TRU_DWC2_GUSBCFG_FSINTF_FS3PIN                0x1
#define TRU_DWC2_GUSBCFG_FSINTF_LSB                   5
#define TRU_DWC2_GUSBCFG_PHYSEL_USB20                 0x0
#define TRU_DWC2_GUSBCFG_PHYSEL_USB11                 0x1
#define TRU_DWC2_GUSBCFG_PHYSEL_LSB                   6
#define TRU_DWC2_GUSBCFG_DDRSEL_SDR                   0x0
#define TRU_DWC2_GUSBCFG_DDRSEL_DDR                   0x1
#define TRU_DWC2_GUSBCFG_DDRSEL_RESERVED              0x1
#define TRU_DWC2_GUSBCFG_DDRSEL_LSB                   7
#define TRU_DWC2_GUSBCFG_SRPCAP_DIS                   0x0
#define TRU_DWC2_GUSBCFG_SRPCAP_ENA                   0x1
#define TRU_DWC2_GUSBCFG_SRPCAP_LSB                   8
#define TRU_DWC2_GUSBCFG_HNPCAP_DIS                   0x0
#define TRU_DWC2_GUSBCFG_HNPCAP_ENA                   0x1
#define TRU_DWC2_GUSBCFG_HNPCAP_LSB                   9
#define TRU_DWC2_GUSBCFG_USBTRDTIM                    0x9
#define TRU_DWC2_GUSBCFG_USBTRDTIM_LSB                10
#define TRU_DWC2_GUSBCFG_ULPIFSLS_DIS                 0x0
#define TRU_DWC2_GUSBCFG_ULPIFSLS_ENA                 0x1
#define TRU_DWC2_GUSBCFG_ULPIFSLS_LSB                 17
#define TRU_DWC2_GUSBCFG_ULPIAUTORES_DIS              0x0
#define TRU_DWC2_GUSBCFG_ULPIAUTORES_ENA              0x1
#define TRU_DWC2_GUSBCFG_ULPIAUTORES_LSB              18
#define TRU_DWC2_GUSBCFG_ULPICLKSUSM_DIS              0x0
#define TRU_DWC2_GUSBCFG_ULPICLKSUSM_ENA              0x1
#define TRU_DWC2_GUSBCFG_ULPICLKSUSM_LSB              19
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSDRV_INT           0x0
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSDRV_EXT           0x1
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSDRV_LSB           20
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSINDICATOR_INT     0x0
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSINDICATOR_EXT     0x1
#define TRU_DWC2_GUSBCFG_ULPIEXTVBUSINDICATOR_LSB     21
#define TRU_DWC2_GUSBCFG_TERMSELDLPULSE_TXVALID       0x0
#define TRU_DWC2_GUSBCFG_TERMSELDLPULSE_TERMSEL       0x1
#define TRU_DWC2_GUSBCFG_TERMSELDLPULSE_LSB           22
#define TRU_DWC2_GUSBCFG_COMPLEMENT_NONINVERT         0x0
#define TRU_DWC2_GUSBCFG_COMPLEMENT_INVERT            0x1
#define TRU_DWC2_GUSBCFG_COMPLEMENT_LSB               23
#define TRU_DWC2_GUSBCFG_INDICATOR_QUALIFIED          0x0
#define TRU_DWC2_GUSBCFG_INDICATOR_NONQUALIFIED       0x1
#define TRU_DWC2_GUSBCFG_INDICATOR_LSB                24
#define TRU_DWC2_GUSBCFG_ULPI_ENA                     0x0
#define TRU_DWC2_GUSBCFG_ULPI_DIS                     0x1
#define TRU_DWC2_GUSBCFG_ULPI_LSB                     25
#define TRU_DWC2_GUSBCFG_TXENDDELAY_NORMAL            0x0
#define TRU_DWC2_GUSBCFG_TXENDDELAY_LSB               28
#define TRU_DWC2_GUSBCFG_FORCEHSTMODE_DIS             0x0
#define TRU_DWC2_GUSBCFG_FORCEHSTMODE_ENA             0x1
#define TRU_DWC2_GUSBCFG_FORCEHSTMODE_LSB             29
#define TRU_DWC2_GUSBCFG_FORCEDEVMODE_DIS             0x0
#define TRU_DWC2_GUSBCFG_FORCEDEVMODE_ENA             0x1
#define TRU_DWC2_GUSBCFG_FORCEDEVMODE_LSB             30
#define TRU_DWC2_GUSBCFG_CORRUPTTXPKT_NORMAL          0x0
#define TRU_DWC2_GUSBCFG_CORRUPTTXPKT_DBG             0x1
#define TRU_DWC2_GUSBCFG_CORRUPTTXPKT_LSB             31

#define TRU_DWC2_GRSTCTL_OFFSET              0x10
#define TRU_DWC2_GRSTCTL_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GRSTCTL_OFFSET))
#define TRU_DWC2_GRSTCTL_AHBIDLE_SET_MSK     0x80000000
#define TRU_DWC2_GRSTCTL_DMAREQ_SET_MSK      0x40000000
#define TRU_DWC2_GRSTCTL_TXFNUM_SET_MSK      0x000007c0
#define TRU_DWC2_GRSTCTL_TXFFLSH_SET_MSK     0x00000020
#define TRU_DWC2_GRSTCTL_RXFFLSH_SET_MSK     0x00000010
#define TRU_DWC2_GRSTCTL_TXFFLSH_DISABLED    0x0
#define TRU_DWC2_GRSTCTL_TXFFLSH             0x1
#define TRU_DWC2_GRSTCTL_TXFFLSH_LSB         5
#define TRU_DWC2_GRSTCTL_RXFFLSH_DISABLED    0x0
#define TRU_DWC2_GRSTCTL_RXFFLSH             0x1
#define TRU_DWC2_GRSTCTL_RXFFLSH_LSB         4
#define TRU_DWC2_GRSTCTL_TXFNUM_0            0x0
#define TRU_DWC2_GRSTCTL_TXFNUM_1            0x1
#define TRU_DWC2_GRSTCTL_TXFNUM_2            0x2
#define TRU_DWC2_GRSTCTL_TXFNUM_15           0xf
#define TRU_DWC2_GRSTCTL_TXFNUM_ALL          0x10
#define TRU_DWC2_GRSTCTL_TXFNUM_LSB          6

#define TRU_DWC2_GINTSTS_OFFSET              0x14
#define TRU_DWC2_GINTSTS_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GINTSTS_OFFSET))
#define TRU_DWC2_GINTSTS_ALL_NORES_SET_MSK      0xf7fefcff
//#define TRU_DWC2_GINTSTS_SERV_ALL_NORES_SET_MSK 0xf7fe7cde
#define TRU_DWC2_GINTSTS_SERV_ALL_NORES_SET_MSK 0xf7fefcde
#define TRU_DWC2_GINTSTS_SERV_DEVICE_SET_MSK    0xd0fe7cde
#define TRU_DWC2_GINTSTS_WKUPINT_SET_MSK        0x80000000
#define TRU_DWC2_GINTSTS_SERV_HOST_SET_MSK      0xf700001e
#define TRU_DWC2_GINTSTS_SESSREQINT_SET_MSK     0x40000000
#define TRU_DWC2_GINTSTS_DISCONNINT_SET_MSK     0x20000000
#define TRU_DWC2_GINTSTS_CONIDSTSCHNG_SET_MSK   0x10000000
#define TRU_DWC2_GINTSTS_PTXFEMP_SET_MSK        0x04000000
#define TRU_DWC2_GINTSTS_HCHINT_SET_MSK         0x02000000
#define TRU_DWC2_GINTSTS_PRTINT_SET_MSK         0x01000000
#define TRU_DWC2_GINTSTS_RSTDET_SET_MSK         0x00800000
#define TRU_DWC2_GINTSTS_FETSUSP_SET_MSK        0x00400000
#define TRU_DWC2_GINTSTS_INCOMPLP_SET_MSK       0x00200000
#define TRU_DWC2_GINTSTS_INCOMPISOIN_SET_MSK    0x00100000
#define TRU_DWC2_GINTSTS_OEPINT_SET_MSK         0x00080000
#define TRU_DWC2_GINTSTS_IEPINT_SET_MSK         0x00040000
#define TRU_DWC2_GINTSTS_EPMIS_SET_MSK          0x00020000
#define TRU_DWC2_GINTSTS_ISOOUTDROP_SET_MSK     0x00004000
#define TRU_DWC2_GINTSTS_ENUMDONE_SET_MSK       0x00002000
#define TRU_DWC2_GINTSTS_USBRST_SET_MSK         0x00001000
#define TRU_DWC2_GINTSTS_USBSUSP_SET_MSK        0x00000800
#define TRU_DWC2_GINTSTS_ERLYSUSP_SET_MSK       0x00000400
#define TRU_DWC2_GINTSTS_GOUTNAKEFF_SET_MSK     0x00000080
#define TRU_DWC2_GINTSTS_GINNAKEFF_SET_MSK      0x00000040
#define TRU_DWC2_GINTSTS_RXFLVL_SET_MSK         0x00000010
#define TRU_DWC2_GINTSTS_SOF_SET_MSK            0x00000008
#define TRU_DWC2_GINTSTS_OTGINT_SET_MSK         0x00000004
#define TRU_DWC2_GINTSTS_MODMIS_SET_MSK         0x00000002
#define TRU_DWC2_GINTSTS_CURMOD_SET_MSK         0x00000001
#define TRU_DWC2_GINTSTS_CURMOD_DEVICE          0x0
#define TRU_DWC2_GINTSTS_CURMOD_HOST            0x1
#define TRU_DWC2_GINTSTS_CURMOD_LSB             0

#define TRU_DWC2_GINTMSK_OFFSET              0x18
#define TRU_DWC2_GINTMSK_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GINTMSK_OFFSET))
#define TRU_DWC2_GINTMSK_ALL_NORES_SET_MSK          0xf7fefcff
#define TRU_DWC2_GINTMSK_SERV_ALL_NORES_SET_MSK     0xf7fefcde
#define TRU_DWC2_GINTMSK_SERV_NOFIFO_SET_MSK        0xf3fe7cce
#define TRU_DWC2_GINTMSK_SERV_DEVICE_SET_MSK        0xd0fe7cde
#define TRU_DWC2_GINTMSK_SERV_DEVICE_NOFIFO_SET_MSK 0xd0fe7cce
#define TRU_DWC2_GINTMSK_SERV_HOST_SET_MSK          0xf700001e
#define TRU_DWC2_GINTMSK_SERV_HOST_NOFIFO_SET_MSK   0xf300000e
#define TRU_DWC2_GINTMSK_SERV_FIFO_SET_MSK          0x04000010
#define TRU_DWC2_GINTMSK_IEPINT_SET_MSK             0x00040000
#define TRU_DWC2_GINTMSK_OEPINT_SET_MSK             0x00080000
#define TRU_DWC2_GINTMSK_XEPINT_SET_MSK             0x000c0000
#define TRU_DWC2_GINTMSK_GOUTNAKEFFMSK_SET_MSK      0x00000080

#define TRU_DWC2_GRXSTSR_OFFSET         0x1c
#define TRU_DWC2_GRXSTSR_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GRXSTSR_OFFSET))
#define TRU_DWC2_GRXSTSR_CHNUM_SET_MSK  0x0000000f
#define TRU_DWC2_GRXSTSR_BCNT_SET_MSK   0x00007ff0
#define TRU_DWC2_GRXSTSR_BCNT_LSB       4
#define TRU_DWC2_GRXSTSR_DPID_SET_MSK   0x00018000
#define TRU_DWC2_GRXSTSR_DPID_LSB       15
#define TRU_DWC2_GRXSTSR_PKTSTS_SET_MSK 0x001e0000
#define TRU_DWC2_GRXSTSR_PKTSTS_LSB     17

#define TRU_DWC2_GRXSTSP_OFFSET         0x20
#define TRU_DWC2_GRXSTSP_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GRXSTSP_OFFSET))
#define TRU_DWC2_GRXSTSP_CHNUM_SET_MSK  0x0000000f
#define TRU_DWC2_GRXSTSP_BCNT_SET_MSK   0x00007ff0
#define TRU_DWC2_GRXSTSP_BCNT_LSB       4
#define TRU_DWC2_GRXSTSP_DPID_SET_MSK   0x00018000
#define TRU_DWC2_GRXSTSP_DPID_LSB       15
#define TRU_DWC2_GRXSTSP_PKTSTS_SET_MSK 0x001e0000
#define TRU_DWC2_GRXSTSP_PKTSTS_LSB     17
#define TRU_DWC2_GRXSTSP_FN_SET_MSK     0x01e00000
#define TRU_DWC2_GRXSTSP_FN_LSB         21

// Packet status values for GRXSTSR and GRXSTSP
#define TRU_DWC2_GRXSTS_PKTSTS_GLOBALOUTNAK    1
#define TRU_DWC2_GRXSTS_PKTSTS_OUTRX           2
#define TRU_DWC2_GRXSTS_PKTSTS_HCHIN           2
#define TRU_DWC2_GRXSTS_PKTSTS_OUTDONE         3
#define TRU_DWC2_GRXSTS_PKTSTS_HCHIN_XFER_COMP 3
#define TRU_DWC2_GRXSTS_PKTSTS_SETUPDONE       4
#define TRU_DWC2_GRXSTS_PKTSTS_DATATOGGLEERR   5
#define TRU_DWC2_GRXSTS_PKTSTS_SETUPRX         6
#define TRU_DWC2_GRXSTS_PKTSTS_HCHHALTED       7

#define TRU_DWC2_GRXFSIZ_OFFSET              0x24
#define TRU_DWC2_GRXFSIZ_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GRXFSIZ_OFFSET))
#define TRU_DWC2_GRXFSIZ_RXFDEP_SET_MSK      0x00003fff

#define TRU_DWC2_GNPTXFSIZ_OFFSET              0x28
#define TRU_DWC2_GNPTXFSIZ_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GNPTXFSIZ_OFFSET))
#define TRU_DWC2_GNPTXFSIZ_NPTXFSTADDR_SET_MSK 0x00003fff
#define TRU_DWC2_GNPTXFSIZ_NPTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_GNPTXFSIZ_NPTXFDEP_LSB        16

#define TRU_DWC2_GNPTXSTS_OFFSET                0x2c
#define TRU_DWC2_GNPTXSTS_ADDR(usb_base_addr)   ((void *)((char *)usb_base_addr + TRU_DWC2_GNPTXSTS_OFFSET))
#define TRU_DWC2_GNPTXSTS_NPTXFSPCAVAIL_SET_MSK 0x0000ffff
#define TRU_DWC2_GNPTXSTS_NPTXQSPXAVAIL_SET_MSK 0x00ff0000
#define TRU_DWC2_GNPTXSTS_NPTXQTOP_SET_MSK      0x3f000000
#define TRU_DWC2_GNPTXSTS_NPTXQSPXAVAIL_LSB     16
#define TRU_DWC2_GNPTXSTS_NPTXQTOP_LSB          24

#define TRU_DWC2_GPVNDCTL_OFFSET              0x34
#define TRU_DWC2_GPVNDCTL_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GPVNDCTL_OFFSET))

#define TRU_DWC2_GCCFG_OFFSET              0x38
#define TRU_DWC2_GCCFG_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GCCFG_OFFSET))

#define TRU_DWC2_CID_OFFSET              0x3c
#define TRU_DWC2_CID_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_CID_OFFSET))

#define TRU_DWC2_GSNPSID_OFFSET              0x40
#define TRU_DWC2_GSNPSID_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GSNPSID_OFFSET))
#define TRU_DWC2_GSNPSID_REV_MASK            0x0000ffff
#define TRU_DWC2_GSNPSID_CORE_REV_2_93A      0x4f54293a
#define TRU_DWC2_GSNPSID_CORE_REV_3_30A      0x4f54330a
#define TRU_DWC2_GSNPSID_CORE_REV_4_20A      0x4f54420a

#define TRU_DWC2_GHWCFG1_OFFSET 0x44
#define TRU_DWC2_GHWCFG1_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GHWCFG1_OFFSET))

#define TRU_DWC2_GHWCFG2_OFFSET                               0x48
#define TRU_DWC2_GHWCFG2_ADDR(usb_base_addr)                  ((void *)((char *)usb_base_addr + TRU_DWC2_GHWCFG2_OFFSET))
#define TRU_DWC2_GHWCFG2_DMA_NONE                             0
#define TRU_DWC2_GHWCFG2_DMA_EXTERNAL                         1
#define TRU_DWC2_GHWCFG2_DMA_INTERNAL                         2
#define TRU_DWC2_GHWCFG2_HNP_SRP_CAPABLE_OTG_HOST_DEV         0
#define TRU_DWC2_GHWCFG2_SRP_HNP_SRP_CAPABLE_OTG_HOST_DEV     1
#define TRU_DWC2_GHWCFG2_NON_HNP_NON_SRP_CAPABLE_OTG_HOST_DEV 2
#define TRU_DWC2_GHWCFG2_SRP_CAPABLE_DEV                      3
#define TRU_DWC2_GHWCFG2_NON_OTG_DEV                          4
#define TRU_DWC2_GHWCFG2_SRP_CAPABLE_HOST                     5
#define TRU_DWC2_GHWCFG2_NON_OTG_HOST                         6

#define TRU_DWC2_GHWCFG3_OFFSET 0x4c
#define TRU_DWC2_GHWCFG3_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GHWCFG3_OFFSET))

#define TRU_DWC2_GHWCFG4_OFFSET 0x50
#define TRU_DWC2_GHWCFG4_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GHWCFG4_OFFSET))

#define TRU_DWC2_GLPMCFG_OFFSET              0x54
#define TRU_DWC2_GLPMCFG_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GLPMCFG_OFFSET))

#define TRU_DWC2_GPWRDN_OFFSET              0x58
#define TRU_DWC2_GPWRDN_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GPWRDN_OFFSET))

#define TRU_DWC2_GDFIFOCFG_OFFSET              0x5c
#define TRU_DWC2_GDFIFOCFG_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_GDFIFOCFG_OFFSET))
#define TRU_DWC2_GDFIFOCFG_EPINFOBASEADDR_LSB  16

#define TRU_DWC2_HPTXFSIZ_OFFSET              0x100
#define TRU_DWC2_HPTXFSIZ_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_HPTXFSIZ_OFFSET))

#define TRU_DWC2_DIEPTXF1_OFFSET                 0x104
#define TRU_DWC2_DIEPTXF1_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF1_OFFSET))
#define TRU_DWC2_DIEPTXF1_INEPNTXFSTADDR_SET_MSK 0x00007fff
#define TRU_DWC2_DIEPTXF1_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF1_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF2_OFFSET                 0x108
#define TRU_DWC2_DIEPTXF2_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF2_OFFSET))
#define TRU_DWC2_DIEPTXF2_INEPNTXFSTADDR_SET_MSK 0x00007fff
#define TRU_DWC2_DIEPTXF2_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF2_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF3_OFFSET                 0x10c
#define TRU_DWC2_DIEPTXF3_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF3_OFFSET))
#define TRU_DWC2_DIEPTXF3_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF3_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF3_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF4_OFFSET                 0x110
#define TRU_DWC2_DIEPTXF4_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF4_OFFSET))
#define TRU_DWC2_DIEPTXF4_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF4_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF4_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF5_OFFSET                 0x114
#define TRU_DWC2_DIEPTXF5_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF5_OFFSET))
#define TRU_DWC2_DIEPTXF5_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF5_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF5_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF6_OFFSET                 0x118
#define TRU_DWC2_DIEPTXF6_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF6_OFFSET))
#define TRU_DWC2_DIEPTXF6_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF6_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF6_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF7_OFFSET                 0x11c
#define TRU_DWC2_DIEPTXF7_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF7_OFFSET))
#define TRU_DWC2_DIEPTXF7_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF7_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF7_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF8_OFFSET                 0x120
#define TRU_DWC2_DIEPTXF8_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF8_OFFSET))
#define TRU_DWC2_DIEPTXF8_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF8_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF8_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF9_OFFSET                 0x124
#define TRU_DWC2_DIEPTXF9_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF9_OFFSET))
#define TRU_DWC2_DIEPTXF9_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF9_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF9_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF10_OFFSET                 0x128
#define TRU_DWC2_DIEPTXF10_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF10_OFFSET))
#define TRU_DWC2_DIEPTXF10_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF10_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF10_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF11_OFFSET                 0x12c
#define TRU_DWC2_DIEPTXF11_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF11_OFFSET))
#define TRU_DWC2_DIEPTXF11_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF11_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF11_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF12_OFFSET                 0x130
#define TRU_DWC2_DIEPTXF12_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF12_OFFSET))
#define TRU_DWC2_DIEPTXF12_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF12_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF12_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF13_OFFSET                 0x134
#define TRU_DWC2_DIEPTXF13_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF13_OFFSET))
#define TRU_DWC2_DIEPTXF13_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF13_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF13_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF14_OFFSET                 0x138
#define TRU_DWC2_DIEPTXF14_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF14_OFFSET))
#define TRU_DWC2_DIEPTXF14_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF14_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF14_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXF15_OFFSET                 0x13c
#define TRU_DWC2_DIEPTXF15_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF15_OFFSET))
#define TRU_DWC2_DIEPTXF15_INEPNTXFSTADDR_SET_MSK 0x0000ffff
#define TRU_DWC2_DIEPTXF15_INEPNTXFDEP_SET_MSK    0x3fff0000
#define TRU_DWC2_DIEPTXF15_INEPNTXFDEP_LSB        16

#define TRU_DWC2_DIEPTXFN_ADDR(usb_base_addr, epnum) ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTXF1_OFFSET + 4 * (epnum - 1)))
#define TRU_DWC2_DIEPTXFN_INEPNTXFSTADDR_SET_MSK     0x0000ffff
#define TRU_DWC2_DIEPTXFN_INEPNTXFSTADDR_LSB         0
#define TRU_DWC2_DIEPTXFN_INEPNTXFDEP_SET_MSK        0x3fff0000
#define TRU_DWC2_DIEPTXFN_INEPNTXFDEP_LSB            16

#define TRU_DWC2_DCFG_OFFSET               0x800
#define TRU_DWC2_DCFG_ADDR(usb_base_addr)  ((void *)((char *)usb_base_addr + TRU_DWC2_DCFG_OFFSET))
#define TRU_DWC2_DCFG_DEVADDR_SET_MSK      0x000007f0
#define TRU_DWC2_DCFG_DEVSPD_SET_MSK       0x00000003
#define TRU_DWC2_DCFG_NZSTSOUTHSHK_SET_MSK 0x00000004
#define TRU_DWC2_DCFG_ENA32KHZSUSP_SET_MSK 0x00000008
#define TRU_DWC2_DCFG_DEVADDR_SET_MSK      0x000007f0
#define TRU_DWC2_DCFG_PERFRINT_SET_MSK     0x00001800
#define TRU_DWC2_DCFG_ENDEVOUTNAK_SET_MSK  0x00002000
#define TRU_DWC2_DCFG_DESCDMA_SET_MSK      0x00800000
#define TRU_DWC2_DCFG_PERSCHINTVL_SET_MSK  0x03000000
#define TRU_DWC2_DCFG_RESVALID_SET_MSK     0xfc000000
#define TRU_DWC2_DCFG_DEVSPD_USBHS20       0x0
#define TRU_DWC2_DCFG_DEVSPD_USBFS20       0x1
#define TRU_DWC2_DCFG_DEVSPD_USBLS116      0x2
#define TRU_DWC2_DCFG_DEVSPD_USBLS1148     0x3
#define TRU_DWC2_DCFG_DEVSPD_LSB           0
#define TRU_DWC2_DCFG_NZSTSOUTHSHK_ZLP     0x0
#define TRU_DWC2_DCFG_NZSTSOUTHSHK_STALL   0x1
#define TRU_DWC2_DCFG_NZSTSOUTHSHK_LSB     2
#define TRU_DWC2_DCFG_ENA32KHZSUSP_DIS     0x0
#define TRU_DWC2_DCFG_ENA32KHZSUSP_ENA     0x1
#define TRU_DWC2_DCFG_ENA32KHZSUSP_LSB     3
#define TRU_DWC2_DCFG_DEVADDR_RESET        0x0
#define TRU_DWC2_DCFG_DEVADDR_LSB          4
#define TRU_DWC2_DCFG_PERFRINT_EOPF80      0x0
#define TRU_DWC2_DCFG_PERFRINT_EOPF85      0x1
#define TRU_DWC2_DCFG_PERFRINT_EOPF90      0x2
#define TRU_DWC2_DCFG_PERFRINT_EOPF95      0x3
#define TRU_DWC2_DCFG_PERFRINT_LSB         11
#define TRU_DWC2_DCFG_ENDEVOUTNAK_DIS      0x0
#define TRU_DWC2_DCFG_ENDEVOUTNAK_ENA      0x1
#define TRU_DWC2_DCFG_ENDEVOUTNAK_LSB      13
#define TRU_DWC2_DCFG_DESCDMA_DIS          0x0
#define TRU_DWC2_DCFG_DESCDMA_ENA          0x1
#define TRU_DWC2_DCFG_DESCDMA_LSB          23
#define TRU_DWC2_DCFG_PERSCHINTVL_MF25     0x0
#define TRU_DWC2_DCFG_PERSCHINTVL_MF50     0x1
#define TRU_DWC2_DCFG_PERSCHINTVL_MF75     0x2
#define TRU_DWC2_DCFG_PERSCHINTVL_RESERVED 0x3
#define TRU_DWC2_DCFG_PERSCHINTVL_LSB      24
#define TRU_DWC2_DCFG_RESVALID_RESET       0x2
#define TRU_DWC2_DCFG_RESVALID_LSB         26

#define TRU_DWC2_DCTL_OFFSET              0x804
#define TRU_DWC2_DCTL_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DCTL_OFFSET))
#define TRU_DWC2_DCTL_IGNRFRMNUM_SET_MSK  0x00008000
#define TRU_DWC2_DCTL_GMC_SET_MSK         0x00006000
#define TRU_DWC2_DCTL_CGOUTNAK_SET_MSK    0x00000400
#define TRU_DWC2_DCTL_SGOUTNAK_SET_MSK    0x00000200
#define TRU_DWC2_DCTL_CGNPINNAK_SET_MSK   0x00000100
#define TRU_DWC2_DCTL_SGNPINNAK_SET_MSK   0x00000080
#define TRU_DWC2_DCTL_TSTCTL_SET_MSK      0x00000070
#define TRU_DWC2_DCTL_RMTWKUPSIG_SET_MSK  0x00000001
#define TRU_DWC2_DCTL_IGNRFRMNUM_DIS      0x0
#define TRU_DWC2_DCTL_IGNRFRMNUM_ENA      0x1
#define TRU_DWC2_DCTL_IGNRFRMNUM_LSB      15
#define TRU_DWC2_DCTL_GMC_INVALID         0x0
#define TRU_DWC2_DCTL_GMC_ONEPKT          0x1
#define TRU_DWC2_DCTL_GMC_TWOPKT          0x2
#define TRU_DWC2_DCTL_GMC_THREEPKT        0x3
#define TRU_DWC2_DCTL_GMC_LSB             13
#define TRU_DWC2_DCTL_CGOUTNAK_DIS        0x0
#define TRU_DWC2_DCTL_CGOUTNAK_ENA        0x1
#define TRU_DWC2_DCTL_CGOUTNAK_LSB        10
#define TRU_DWC2_DCTL_SGOUTNAK_DIS        0x0
#define TRU_DWC2_DCTL_SGOUTNAK_ENA        0x1
#define TRU_DWC2_DCTL_SGOUTNAK_LSB        9
#define TRU_DWC2_DCTL_CGNPINNAK_DIS       0x0
#define TRU_DWC2_DCTL_CGNPINNAK_ENA       0x1
#define TRU_DWC2_DCTL_CGNPINNAK_LSB       8
#define TRU_DWC2_DCTL_SGNPINNAK_DIS       0x0
#define TRU_DWC2_DCTL_SGNPINNAK_ENA       0x1
#define TRU_DWC2_DCTL_SGNPINNAK_LSB       7
#define TRU_DWC2_DCTL_TSTCTL_DIS          0x0
#define TRU_DWC2_DCTL_TSTCTL_J            0x1
#define TRU_DWC2_DCTL_TSTCTL_K            0x2
#define TRU_DWC2_DCTL_TSTCTL_SN           0x3
#define TRU_DWC2_DCTL_TSTCTL_P            0x4
#define TRU_DWC2_DCTL_TSTCTL_FE           0x5
#define TRU_DWC2_DCTL_TSTCTL_LSB          0x4

#define TRU_DWC2_DSTS_OFFSET              0x808
#define TRU_DWC2_DSTS_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DSTS_OFFSET))
#define TRU_DWC2_DSTS_ENUMSPD_SET_MSK     0x00000006
#define TRU_DWC2_DSTS_SOFFN_SET_MSK       0x003fff00
#define TRU_DWC2_DSTS_ENUMSPD_HS3060      0x0
#define TRU_DWC2_DSTS_ENUMSPD_FS3060      0x1
#define TRU_DWC2_DSTS_ENUMSPD_LS6         0x2
#define TRU_DWC2_DSTS_ENUMSPD_FS48        0x3
#define TRU_DWC2_DSTS_ENUMSPD_LSB         1
#define TRU_DWC2_DSTS_SOFFN_LSB           8
//#define TRU_DWC2_DSTS_SOFFN_LIMIT         0x3fff
#define TRU_DWC2_DSTS_SOFFN_LIMIT         0x7ff  // Although SOFFN field is 14-bits, the counter value itself is only 12-bits

#define TRU_DWC2_DIEPMSK_OFFSET                 0x810
#define TRU_DWC2_DIEPMSK_ADDR(usb_base_addr)    ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPMSK_OFFSET))
#define TRU_DWC2_DIEPMSK_SERV_ALL_SET_MSK       0x0000237f
#define TRU_DWC2_DIEPMSK_SERV_NOFIFO_SET_MSK    0x0000226f
#define TRU_DWC2_DIEPMSK_INTKNTXFEMPMSK_SET_MSK 0x00000010
#define TRU_DWC2_DIEPMSK_NAKMSK_SET_MSK         0x00002000

#define TRU_DWC2_DOEPMSK_OFFSET              0x814
#define TRU_DWC2_DOEPMSK_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DOEPMSK_OFFSET))
#define TRU_DWC2_DOEPMSK_SERV_ALL_SET_MSK    0x0000735f

#define TRU_DWC2_DAINT_OFFSET              0x818
#define TRU_DWC2_DAINT_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DAINT_OFFSET))
#define TRU_DWC2_DAINT_SERV_ALL_SET_MSK    0xffffffff
#define TRU_DWC2_DAINT_EPIN_MSK            0x0000ffff
#define TRU_DWC2_DAINT_EPOUT_MSK           0xffff0000

#define TRU_DWC2_DAINTMSK_OFFSET              0x81c
#define TRU_DWC2_DAINTMSK_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DAINTMSK_OFFSET))
#define TRU_DWC2_DAINTMSK_INEPMSK0_SET_MSK    0x00000001
#define TRU_DWC2_DAINTMSK_OUTEPMSK0_SET_MSK   0x00010000

#define TRU_DWC2_DVBUSDIS_OFFSET              0x828
#define TRU_DWC2_DVBUSDIS_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DVBUSDIS_OFFSET))

#define TRU_DWC2_DVBUSPULSE_OFFSET              0x82c
#define TRU_DWC2_DVBUSPULSE_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DVBUSPULSE_OFFSET))

#define TRU_DWC2_DTHRCTL_OFFSET              0x830
#define TRU_DWC2_DTHRCTL_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DTHRCTL_OFFSET))

#define TRU_DWC2_DIEP_OFFSET 0x20

#define TRU_DWC2_DIEPEMPMSK_OFFSET              0x834
#define TRU_DWC2_DIEPEMPMSK_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPEMPMSK_OFFSET))

#define TRU_DWC2_DIEPCTL0_OFFSET                     0x900
#define TRU_DWC2_DIEPCTL_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPCTL0_OFFSET + ep_num * TRU_DWC2_DIEP_OFFSET))
#define TRU_DWC2_DIEPCTL0_MPS_SET_MSK                0x00000003
#define TRU_DWC2_DIEPCTL_MPS_SET_MSK                 0x000007ff
#define TRU_DWC2_DIEPCTL_EPTYPE_SET_MSK              0x000c0000
#define TRU_DWC2_DIEPCTL_USBACTEP_SET_MSK            0x00008000
#define TRU_DWC2_DIEPCTL_NAKSTS_SET_MSK              0x00020000
#define TRU_DWC2_DIEPCTL_STALL_SET_MSK               0x00200000
#define TRU_DWC2_DIEPCTL_TXFNUM_SET_MSK              0x03c00000
#define TRU_DWC2_DIEPCTL_CNAK_SET_MSK                0x04000000
#define TRU_DWC2_DIEPCTL_SNAK_SET_MSK                0x08000000
#define TRU_DWC2_DIEPCTL_EPDIS_SET_MSK               0x40000000
#define TRU_DWC2_DIEPCTL_SETD0PID_SET_MSK            0x10000000
#define TRU_DWC2_DIEPCTL_SETD1PID_SET_MSK            0x20000000
#define TRU_DWC2_DIEPCTL_EPENA_SET_MSK               0x80000000
#define TRU_DWC2_DIEPCTL_EPTYPE_LSB                  18
#define TRU_DWC2_DIEPCTL_TXFNUM_SET_LSB              22

#define TRU_DWC2_DIEPINT0_OFFSET                     0x908
#define TRU_DWC2_DIEPINT_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPINT0_OFFSET + ep_num * TRU_DWC2_DIEP_OFFSET))
#define TRU_DWC2_DIEPINT_ALL_NORES_SET_MSK           0x00007bff
#define TRU_DWC2_DIEPINT_XFERCOMPL_SET_MSK           0x00000001
#define TRU_DWC2_DIEPINT_EPDISBLD_SET_MSK            0x00000002
#define TRU_DWC2_DIEPINT_INTKNTXFEMP_SET_MSK         0x00000010
#define TRU_DWC2_DIEPINT_TXFEMP_SET_MSK              0x00000080
#define TRU_DWC2_DIEPINT_BNAINTR_SET_MSK             0x00000200
#define TRU_DWC2_DIEPINT_NAKINTRPT_SET_MSK           0x00002000

#define TRU_DWC2_DIEPTSIZ0_OFFSET                     0x910
#define TRU_DWC2_DIEPTSIZ_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPTSIZ0_OFFSET + ep_num * TRU_DWC2_DIEP_OFFSET))
#define TRU_DWC2_DIEPTSIZ0_XFERSIZE_SET_MSK           0x0000007f
#define TRU_DWC2_DIEPTSIZ_XFERSIZE_SET_MSK            0x0007ffff
#define TRU_DWC2_DIEPTSIZ0_PKTCNT_SET_MSK             0x00180000
#define TRU_DWC2_DIEPTSIZ_PKTCNT_SET_MSK              0x1ff80000
#define TRU_DWC2_DIEPTSIZ_MC_SET_MSK                  0x60000000
#define TRU_DWC2_DIEPTSIZ_XFERSIZE_LSB                0
#define TRU_DWC2_DIEPTSIZ_PKTCNT_LSB                  19
#define TRU_DWC2_DIEPTSIZ_MC_LSB                      29

#define TRU_DWC2_DIEPDMA0_OFFSET                     0x914
#define TRU_DWC2_DIEPDMA_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPDMA0_OFFSET + ep_num * TRU_DWC2_DIEP_OFFSET))

#define TRU_DWC2_DTXFSTS0_OFFSET                     0x918
#define TRU_DWC2_DTXFSTS_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DTXFSTS0_OFFSET + ep_num * TRU_DWC2_DIEP_OFFSET))
#define TRU_DWC2_DTXFSTS_INEPTXFSPCAVAIL_SET_MSK     0x0000ffff

#define TRU_DWC2_DIEPDMAB0_OFFSET                     0x91c
#define TRU_DWC2_DIEPDMAB_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DIEPDMAB0_OFFSET + ep_num * TRU_DWC2_DIEP_OFFSET))

#define TRU_DWC2_DOEP_OFFSET 0x20

#define TRU_DWC2_DOEPCTL0_OFFSET                     0xb00
#define TRU_DWC2_DOEPCTL_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DOEPCTL0_OFFSET + ep_num * TRU_DWC2_DOEP_OFFSET))
#define TRU_DWC2_DOEPCTL_DPID_SET_MSK                0x00010000
#define TRU_DWC2_DOEPCTL0_MPS_SET_MSK                0x00000003
#define TRU_DWC2_DOEPCTL_MPS_SET_MSK                 0x000007ff
#define TRU_DWC2_DOEPCTL_EPTYPE_SET_MSK              0x000c0000
#define TRU_DWC2_DOEPCTL_USBACTEP_SET_MSK            0x00008000
#define TRU_DWC2_DOEPCTL_NAKSTS_SET_MSK              0x00020000
#define TRU_DWC2_DOEPCTL_STALL_SET_MSK               0x00200000
#define TRU_DWC2_DOEPCTL_CNAK_SET_MSK                0x04000000
#define TRU_DWC2_DOEPCTL_SNAK_SET_MSK                0x08000000
#define TRU_DWC2_DOEPCTL_EPDIS_SET_MSK               0x40000000
#define TRU_DWC2_DOEPCTL_SETD0PID_SET_MSK            0x10000000
#define TRU_DWC2_DOEPCTL_SETD1PID_SET_MSK            0x20000000
#define TRU_DWC2_DOEPCTL_EPENA_SET_MSK               0x80000000
#define TRU_DWC2_DOEPCTL_EPTYPE_LSB                  18

#define TRU_DWC2_DOEPINT0_OFFSET                     0xb08
#define TRU_DWC2_DOEPINT_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DOEPINT0_OFFSET + ep_num * TRU_DWC2_DOEP_OFFSET))
#define TRU_DWC2_DOEPINT_ALL_NORES_SET_MSK           0x00007b7f
#define TRU_DWC2_DOEPINT_XFERCOMPL_SET_MSK           0x00000001
#define TRU_DWC2_DOEPINT_EPDISBLD_SET_MSK            0x00000002
#define TRU_DWC2_DOEPINT_SETUP_SET_MSK               0x00000008
#define TRU_DWC2_DOEPINT_OUTTKNEPDIS_SET_MSK         0x00000010
#define TRU_DWC2_DOEPINT_STSPHSERCVD_SET_MSK         0x00000020
#define TRU_DWC2_DOEPINT_BACK2BACKSETUP_SET_MSK      0x00000040
#define TRU_DWC2_DOEPINT_OUTPKTERR_SET_MSK           0x00000100
#define TRU_DWC2_DOEPINT_BNAINTR_SET_MSK             0x00000200
#define TRU_DWC2_DOEPINT_BBLEERR_SET_MSK             0x00001000
#define TRU_DWC2_DOEPINT_NAKINTRPT_SET_MSK           0x00002000
#define TRU_DWC2_DOEPINT_NYETINTRPT_SET_MSK          0x00004000

#define TRU_DWC2_DOEPTSIZ0_OFFSET                     0xb10
#define TRU_DWC2_DOEPTSIZ_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DOEPTSIZ0_OFFSET + ep_num * TRU_DWC2_DOEP_OFFSET))
#define TRU_DWC2_DOEPTSIZ0_XFERSIZE_SET_MSK           0x0000007f
#define TRU_DWC2_DOEPTSIZ_XFERSIZE_SET_MSK            0x0007ffff
#define TRU_DWC2_DOEPTSIZ0_PKTCNT_SET_MSK             0x00180000
#define TRU_DWC2_DOEPTSIZ_PKTCNT_SET_MSK              0x1ff80000
#define TRU_DWC2_DOEPTSIZ0_SUPCNT_SET_MSK             0x60000000
#define TRU_DWC2_DOEPTSIZ_XFERSIZE_LSB                0
#define TRU_DWC2_DOEPTSIZ_PKTCNT_LSB                  19
#define TRU_DWC2_DOEPTSIZ0_SUPCNT_LSB                 29

#define TRU_DWC2_DOEPDMA0_OFFSET                     0xb14
#define TRU_DWC2_DOEPDMA_ADDR(usb_base_addr, ep_num) ((void *)((char *)usb_base_addr + TRU_DWC2_DOEPDMA0_OFFSET + ep_num * TRU_DWC2_DOEP_OFFSET))

#define TRU_DWC2_PCGCCTL_OFFSET              0xe00
#define TRU_DWC2_PCGCCTL_ADDR(usb_base_addr) ((void *)((char *)usb_base_addr + TRU_DWC2_PCGCCTL_OFFSET))

// =============================================================
// USB OTG device mode Scatter Gather DMA descriptor definitions
// =============================================================

#define TRU_DWC2_SGDMA_ALIGNMENT_BYTES 4
#define TRU_DWC2_SGDMA_ALIGNMENT_BITS  32

// Buffer status definitions.  Bits 30 and 31
// Read flags
#define TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_READY 0
#define TRU_DWC2_DEV_SGDMA_STS_BUF_DMA_BUSY   1
#define TRU_DWC2_DEV_SGDMA_STS_BUF_DMA_DONE   2
#define TRU_DWC2_DEV_SGDMA_STS_BUF_HOST_BUSY  3
#define TRU_DWC2_DEV_SGDMA_STS_BUF_SHIFT      30
#define TRU_DWC2_DEV_SGDMA_STS_BUF_SET_MASK   0xc0000000
#define TRU_DWC2_DEV_SGDMA_STS_BUF_CLR_MASK   0x3fffffff

// Transfer status definitions.  Bits 28 and 29
#define TRU_DWC2_DEV_SGDMA_STS_XFER_SUCCESS  0
#define TRU_DWC2_DEV_SGDMA_STS_XFER_BUFFLUSH 1
#define TRU_DWC2_DEV_SGDMA_STS_XFER_RESERVED 2
#define TRU_DWC2_DEV_SGDMA_STS_XFER_BUFERR   3
#define TRU_DWC2_DEV_SGDMA_STS_XFER_SHIFT    28
#define TRU_DWC2_DEV_SGDMA_STS_XFER_SET_MASK 0x30000000
#define TRU_DWC2_DEV_SGDMA_STS_XFER_CLR_MASK 0xcfffffff

/*
	My own notes
	============

	Short packet register flag: TRU_DWC2_DEV_SGDMA_SHORT

	For IN EP (transmit), from tests I found out that this is a write bit,
	which indicates whether the OTG controller will send a ZLP after the
	transfer.  From the USB specification a ZLP is for indicating the end
	of a transfer under certain conditions on Control, Interrupt and Bulk
	data flows. The SHORT bit usage is:
		0 = do not send a ZLP after the transfer
		1 = send a ZLP after the transfer

	For OUT EP (receive) the SHORT bit is a read bit, which indicates that
	a transfer completed with shorter size than the MPS (Maximum Packet
	Size).
*/

// Other status definitions.  Bits 23 to 27
// LAST  = Write flag.  Indicates descriptor is the last one. Write 1 for the last descriptor
// SHORT = Write flag.  Sends a ZLP after the transfer.  Write 1 to include a ZLP
// IOC   = Write flag.  Interrupt On Complete.  Write 1 to trigger an interrupt at the end of transfer for this descriptor.  Typically, you set this for the last descriptor
// SETUP = Read flag.  Setup packet received, applies only to OUT EPs. The OTG controller sets this to 1 when a setup packet is received
// MTRF  = Multiple Transfer Flag, applies only to OUT EPs.  I could not find any details on this
#define TRU_DWC2_DEV_SGDMA_LAST_SET_MASK  0x08000000
#define TRU_DWC2_DEV_SGDMA_SHORT_SET_MASK 0x04000000
#define TRU_DWC2_DEV_SGDMA_IOC_SET_MASK   0x02000000
#define TRU_DWC2_DEV_SGDMA_SETUP_SET_MASK 0x01000000
#define TRU_DWC2_DEV_SGDMA_MTRF_SET_MASK  0x00800000

// Isochronous flow PID to start from.  Bits 23 to 24
#define TRU_DWC2_DEV_SGDMA_ISOC_PID_DATA0    0
#define TRU_DWC2_DEV_SGDMA_ISOC_PID_DATA2    1
#define TRU_DWC2_DEV_SGDMA_ISOC_PID_DATA1    2
#define TRU_DWC2_DEV_SGDMA_ISOC_PID_MDATA    3
#define TRU_DWC2_DEV_SGDMA_ISOC_PID_SHIFT    23
#define TRU_DWC2_DEV_SGDMA_ISOC_PID_SET_MASK 0x01800000
#define TRU_DWC2_DEV_SGDMA_ISOC_PID_CLR_MASK 0xfe7fffff

// Isochronous flow microframe or frame number to transmit from.  Bits 12 to 22
#define TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_LIMIT 0x000007ff
#define TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_MASK  0x007ff000
#define TRU_DWC2_DEV_SGDMA_ISOC_FRNUM_SHIFT 12

// Isochronous flow buffer size definitions.  Bits 0 to 10
#define TRU_DWC2_DEV_SGDMA_ISOC_TX_NBYTES_LIMIT 0x00000fff
#define TRU_DWC2_DEV_SGDMA_ISOC_TX_NBYTES_MASK  0x00000fff
#define TRU_DWC2_DEV_SGDMA_ISOC_RX_NBYTES_LIMIT 0x000007ff
#define TRU_DWC2_DEV_SGDMA_ISOC_RX_NBYTES_MASK  0x000007ff

// Non-Isochronous flow buffer size definitions.  Bits 0 to 15
#define TRU_DWC2_DEV_SGDMA_NBYTES_LIMIT 0x0000ffff
#define TRU_DWC2_DEV_SGDMA_NBYTES_MASK  0x0000ffff

// Buffer size limits aligned down to defined alignment
#define TRU_DWC2_DEV_SGDMA_ISOC_TX_NBYTES_LIMIT_ALIGNED_DOWN (TRU_DWC2_DEV_SGDMA_ISOC_TX_NBYTES_LIMIT - TRU_DWC2_DEV_SGDMA_ISOC_TX_NBYTES_LIMIT % TRU_DWC2_SGDMA_ALIGNMENT_BYTES)
#define TRU_DWC2_DEV_SGDMA_ISOC_RX_NBYTES_LIMIT_ALIGNED_DOWN (TRU_DWC2_DEV_SGDMA_ISOC_RX_NBYTES_LIMIT - TRU_DWC2_DEV_SGDMA_ISOC_RX_NBYTES_LIMIT % TRU_DWC2_SGDMA_ALIGNMENT_BYTES)
#define TRU_DWC2_DEV_SGDMA_NBYTES_LIMIT_ALIGNED_DOWN (TRU_DWC2_DEV_SGDMA_NBYTES_LIMIT - TRU_DWC2_DEV_SGDMA_NBYTES_LIMIT % TRU_DWC2_SGDMA_ALIGNMENT_BYTES)

#define TRU_DWC2_DEV_EP_EP0_MAX_PACKET_COUNT 1

// Push and pop registers for data FIFOs
// Note, these are not FIFO RAM addresses, they are registers.
// Writing to them pushes data into the FIFO for the corresponding endpoint
// Reading from them pops data from the FIFO for the corresponding endpoint
// Normally, we would need only one push-pop register for each endpoint, so
// 16 registers is all that is needed, but there are more registers.  The extra
// registers are optional, but they make it possible to push or pop more bytes
// at the same time for pipelining the USB controller transfer process.  It
// should result in a faster transfer.  The mapping of registers to endpoint
// number (or FIFO number) depends on the transmit FIFO mode - whether shared or
// dedicated.  In dedicated mode, the registers are divided into blocks of
// 0x1000 (4096 bytes) and mapped to a specific dedicated FIFO number.  In
// shared mode, the registers are used in round-robin fashion, i.e. endpoints
// are serviced one at a time.  The registers are treated as a single block and
// mapped to the shared FIFO number 0, which services all endpoints 0 to 15, one
// at a time.
#define TRU_DWC2_DEV_DFIFO_OFFSET                         0x1000
#define TRU_DWC2_DEV_DFIFO_PER_SIZE                       0x1000
#define TRU_DWC2_DEV_DFIFO_L2_WRITE_BOUNDARY              0x1000
#define TRU_DWC2_DEV_DFIFO_RX_ADDR(usb_base_addr)         ((volatile uint32_t *const)((char *)usb_base_addr + TRU_DWC2_DEV_DFIFO_OFFSET))
#define TRU_DWC2_DEV_DFIFO_TX_ADDR(usb_base_addr, fifonum) ((volatile uint32_t *const)((char *)usb_base_addr + TRU_DWC2_DEV_DFIFO_OFFSET + fifonum * TRU_DWC2_DEV_DFIFO_PER_SIZE))

// Scatter Gather DMA descriptor status
typedef union{
	volatile uint32_t val;
	// Non ISO transfer
	struct{
		volatile uint32_t xfersize      :16;
		volatile uint32_t reserved16_22 :7;
		volatile uint32_t mtrf          :1;
		volatile uint32_t rxsetup       :1;
		volatile uint32_t ioc           :1;
		volatile uint32_t zlp           :1;
		volatile uint32_t last          :1;
		volatile uint32_t xfersts       :2;
		volatile uint32_t bufsts        :2;
	}bits;
	// ISO out transfer
	struct{
		volatile uint32_t rxsize     :11;
		volatile uint32_t reserved11 :1;
		volatile uint32_t framenum   :11;
		volatile uint32_t pid        :2;
		volatile uint32_t ioc        :1;
		volatile uint32_t zlp        :1;
		volatile uint32_t last       :1;
		volatile uint32_t xfersts    :2;
		volatile uint32_t bufsts     :2;
	}bits_iso_out;
	// ISO in transfer
	struct{
		volatile uint32_t txsize   :12;
		volatile uint32_t framenum :11;
		volatile uint32_t pid      :2;
		volatile uint32_t ioc      :1;
		volatile uint32_t zlp      :1;
		volatile uint32_t last     :1;
		volatile uint32_t xfersts  :2;
		volatile uint32_t bufsts   :2;
	}bits_iso_in;
}tru_dwc2_dev_sgdma_desc_sts_reg_t;

// Scatter Gather DMA descriptor
typedef struct{
	volatile tru_dwc2_dev_sgdma_desc_sts_reg_t status;
	uint32_t buf;
}tru_dwc2_dev_sgdma_desc_reg_t;

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t sesreqscs    :1;
		volatile uint32_t sesreq       :1;
		volatile uint32_t vbvalidoven  :1;
		volatile uint32_t vbvalidovval :1;
		volatile uint32_t avalidoven   :1;
		volatile uint32_t avalidovval  :1;
		volatile uint32_t bvalidoven   :1;
		volatile uint32_t bvalidovval  :1;
		volatile uint32_t hstnegscs    :1;
		volatile uint32_t hnpreq       :1;
		volatile uint32_t hstsethnpen  :1;
		volatile uint32_t devhnpen     :1;
		volatile uint32_t ehen         :1;
		volatile uint32_t res13_15     :3;
		volatile uint32_t conidsts     :1;
		volatile uint32_t dbnctime     :1;
		volatile uint32_t asesvld      :1;
		volatile uint32_t bsesvld      :1;
		volatile uint32_t otgver       :1;
		volatile uint32_t curmod       :1;
		volatile uint32_t res22_31     :10;
	}bits;
}tru_dwc2_gotgctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t sesreqscs    :1;
		uint32_t sesreq       :1;
		uint32_t vbvalidoven  :1;
		uint32_t vbvalidovval :1;
		uint32_t avalidoven   :1;
		uint32_t avalidovval  :1;
		uint32_t bvalidoven   :1;
		uint32_t bvalidovval  :1;
		uint32_t hstnegscs    :1;
		uint32_t hnpreq       :1;
		uint32_t hstsethnpen  :1;
		uint32_t devhnpen     :1;
		uint32_t ehen         :1;
		uint32_t res13_15     :3;
		uint32_t conidsts     :1;
		uint32_t dbnctime     :1;
		uint32_t asesvld      :1;
		uint32_t bsesvld      :1;
		uint32_t otgver       :1;
		uint32_t curmod       :1;
		uint32_t res22_31     :10;
	}bits;
}tru_dwc2_gotgctl_t;

#define TRU_DWC2_GOTGCTL_REG(usb_base_addr) ((volatile tru_dwc2_gotgctl_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GOTGCTL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t res0_1           :2;
		volatile uint32_t sesenddet        :1;
		volatile uint32_t res3_7           :5;
		volatile uint32_t sesreqsucstschng :1;
		volatile uint32_t hstnegsucstschng :1;
		volatile uint32_t res10_16         :7;
		volatile uint32_t hstnegdet        :1;
		volatile uint32_t adevtoutchg      :1;
		volatile uint32_t dbncedone        :1;
		volatile uint32_t res20_31         :12;
	}bits;
}tru_dwc2_gotgint_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t res0_1           :2;
		uint32_t sesenddet        :1;
		uint32_t res3_7           :5;
		uint32_t sesreqsucstschng :1;
		uint32_t hstnegsucstschng :1;
		uint32_t res10_16         :7;
		uint32_t hstnegdet        :1;
		uint32_t adevtoutchg      :1;
		uint32_t dbncedone        :1;
		uint32_t res20_31         :12;
	}bits;
}tru_dwc2_gotgint_t;

#define TRU_DWC2_GOTGINT_REG(usb_base_addr) ((volatile tru_dwc2_gotgint_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GOTGINT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t glblintrmsk    :1;
		volatile uint32_t hbstlen        :4;
		volatile uint32_t dmaen          :1;
		volatile uint32_t res6           :1;
		volatile uint32_t nptxfemplvl    :1;
		volatile uint32_t ptxfemplvl     :1;
		volatile uint32_t res9_20        :12;
		volatile uint32_t remmemsupp     :1;
		volatile uint32_t notialldmawrit :1;
		volatile uint32_t res23_31       :9;
	}bits;
}tru_dwc2_gahbcfg_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t glblintrmsk    :1;
		uint32_t hbstlen        :4;
		uint32_t dmaen          :1;
		uint32_t res6           :1;
		uint32_t nptxfemplvl    :1;
		uint32_t ptxfemplvl     :1;
		uint32_t res9_20        :12;
		uint32_t remmemsupp     :1;
		uint32_t notialldmawrit :1;
		uint32_t res23_31       :9;
	}bits;
}tru_dwc2_gahbcfg_t;

#define TRU_DWC2_GAHBCFG_REG(usb_base_addr) ((volatile tru_dwc2_gahbcfg_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GAHBCFG_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t toutcal              :3;
		volatile uint32_t phyif                :1;
		volatile uint32_t ulpi_utmi_sel        :1;
		volatile uint32_t fsintf               :1;
		volatile uint32_t physel               :1;
		volatile uint32_t ddrsel               :1;
		volatile uint32_t srpcap               :1;
		volatile uint32_t hnpcap               :1;
		volatile uint32_t usbtrdtim            :4;
		volatile uint32_t res14                :1;
		volatile uint32_t phylpc               :1;
		volatile uint32_t res16                :1;
		volatile uint32_t ulpifsl              :1;
		volatile uint32_t ulpiautores          :1;
		volatile uint32_t ulpiclksusm          :1;
		volatile uint32_t ulpiextvbusdrv       :1;
		volatile uint32_t ulpiextvbusindicator :1;
		volatile uint32_t termseldlpulse       :1;
		volatile uint32_t complement           :1;
		volatile uint32_t indicator            :1;
		volatile uint32_t ulpi                 :1;
		volatile uint32_t res26_27             :2;
		volatile uint32_t txenddelay           :1;
		volatile uint32_t forcehstmode         :1;
		volatile uint32_t forcedevmode         :1;
		volatile uint32_t corrupttxpkt         :1;
	}bits;
}tru_dwc2_gusbcfg_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t toutcal              :3;
		uint32_t phyif                :1;
		uint32_t ulpi_utmi_sel        :1;
		uint32_t fsintf               :1;
		uint32_t physel               :1;
		uint32_t ddrsel               :1;
		uint32_t srpcap               :1;
		uint32_t hnpcap               :1;
		uint32_t usbtrdtim            :4;
		uint32_t res14                :1;
		uint32_t phylpc               :1;
		uint32_t res16                :1;
		uint32_t ulpifsl              :1;
		uint32_t ulpiautores          :1;
		uint32_t ulpiclksusm          :1;
		uint32_t ulpiextvbusdrv       :1;
		uint32_t ulpiextvbusindicator :1;
		uint32_t termseldlpulse       :1;
		uint32_t complement           :1;
		uint32_t indicator            :1;
		uint32_t ulpi                 :1;
		uint32_t res26_27             :2;
		uint32_t txenddelay           :1;
		uint32_t forcehstmode         :1;
		uint32_t forcedevmode         :1;
		uint32_t corrupttxpkt         :1;
	}bits;
}tru_dwc2_gusbcfg_t;

#define TRU_DWC2_GUSBCFG_REG(usb_base_addr) ((volatile tru_dwc2_gusbcfg_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GUSBCFG_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t csftrst      :1;
		volatile uint32_t psrst        :1;
		volatile uint32_t frmcntrrst   :1;
		volatile uint32_t res3         :1;
		volatile uint32_t rxfflsh      :1;
		volatile uint32_t txfflsh      :1;
		volatile uint32_t txfnum       :5;
		volatile uint32_t res11_28     :18;
		volatile uint32_t csftrst_done :1;  // Supported from revision >= 4_20a
		volatile uint32_t dmareq       :1;
		volatile uint32_t ahbidle      :1;
	}bits;
}tru_dwc2_grstctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t csftrst      :1;
		uint32_t psrst        :1;
		uint32_t frmcntrrst   :1;
		uint32_t res3         :1;
		uint32_t rxfflsh      :1;
		uint32_t txfflsh      :1;
		uint32_t txfnum       :5;
		uint32_t res11_28     :18;
		uint32_t csftrst_done :1;  // Supported from revision >= 4_20a
		uint32_t dmareq       :1;
		uint32_t ahbidle      :1;
	}bits;
}tru_dwc2_grstctl_t;

#define TRU_DWC2_GRSTCTL_REG(usb_base_addr) ((volatile tru_dwc2_grstctl_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GRSTCTL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t curmod       :1;
		volatile uint32_t modemis      :1;
		volatile uint32_t otgint       :1;
		volatile uint32_t sof          :1;
		volatile uint32_t rxflvl       :1;
		volatile uint32_t nptxfem      :1;
		volatile uint32_t ginnakeff    :1;
		volatile uint32_t goutnakeff   :1;
		volatile uint32_t res8_9       :2;
		volatile uint32_t erlysusp     :1;
		volatile uint32_t usbsusp      :1;
		volatile uint32_t usbrst       :1;
		volatile uint32_t enumdone     :1;
		volatile uint32_t isooutdrop   :1;
		volatile uint32_t eopf         :1;
		volatile uint32_t res16        :1;
		volatile uint32_t epmis        :1;
		volatile uint32_t iepint       :1;
		volatile uint32_t oepint       :1;
		volatile uint32_t incompisoin  :1;
		volatile uint32_t incomplp     :1;
		volatile uint32_t fetsusp      :1;
		volatile uint32_t resetdet     :1;
		volatile uint32_t prtint       :1;
		volatile uint32_t hchint       :1;
		volatile uint32_t ptxfemp      :1;
		volatile uint32_t lpmint       :1;
		volatile uint32_t conidstschng :1;
		volatile uint32_t disconnint   :1;
		volatile uint32_t sessreqint   :1;
		volatile uint32_t wkupint      :1;
	}bits;
}tru_dwc2_gintsts_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t curmod       :1;
		uint32_t modemis      :1;
		uint32_t otgint       :1;
		uint32_t sof          :1;
		uint32_t rxflvl       :1;
		uint32_t nptxfem      :1;
		uint32_t ginnakeff    :1;
		uint32_t goutnakeff   :1;
		uint32_t res8_9       :2;
		uint32_t erlysusp     :1;
		uint32_t usbsusp      :1;
		uint32_t usbrst       :1;
		uint32_t enumdone     :1;
		uint32_t isooutdrop   :1;
		uint32_t eopf         :1;
		uint32_t res16        :1;
		uint32_t epmis        :1;
		uint32_t iepint       :1;
		uint32_t oepint       :1;
		uint32_t incompisoin  :1;
		uint32_t incomplp     :1;
		uint32_t fetsusp      :1;
		uint32_t resetdet     :1;
		uint32_t prtint       :1;
		uint32_t hchint       :1;
		uint32_t ptxfemp      :1;
		uint32_t lpmint       :1;
		uint32_t conidstschng :1;
		uint32_t disconnint   :1;
		uint32_t sessreqint   :1;
		uint32_t wkupint      :1;
	}bits;
}tru_dwc2_gintsts_t;

#define TRU_DWC2_GINTSTS_REG(usb_base_addr) ((volatile tru_dwc2_gintsts_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GINTSTS_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t res0            :1;
		volatile uint32_t modemismsk      :1;
		volatile uint32_t otgintmsk       :1;
		volatile uint32_t sofmsk          :1;
		volatile uint32_t rxflvlmsk       :1;
		volatile uint32_t nptxfem         :1;
		volatile uint32_t ginnakeffmsk    :1;
		volatile uint32_t goutnakeffmsk   :1;
		volatile uint32_t res8_9          :2;
		volatile uint32_t erlysuspmsk     :1;
		volatile uint32_t usbsuspmsk      :1;
		volatile uint32_t usbrstmsk       :1;
		volatile uint32_t enumdonemsk     :1;
		volatile uint32_t isooutdropmsk   :1;
		volatile uint32_t eopfmsk         :1;
		volatile uint32_t res16           :1;
		volatile uint32_t epmismsk        :1;
		volatile uint32_t iepintmsk       :1;
		volatile uint32_t oepintmsk       :1;
		volatile uint32_t incompisoinmsk  :1;
		volatile uint32_t incomplpmsk     :1;
		volatile uint32_t fetsuspmsk      :1;
		volatile uint32_t resetdetmsk     :1;
		volatile uint32_t prtintmsk       :1;
		volatile uint32_t hchintmsk       :1;
		volatile uint32_t ptxfempmsk      :1;
		volatile uint32_t lpmint          :1;
		volatile uint32_t conidstschngmsk :1;
		volatile uint32_t disconnintmsk   :1;
		volatile uint32_t sessreqintmsk   :1;
		volatile uint32_t wkupintmsk      :1;
	}bits;
}tru_dwc2_gintmsk_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t res0            :1;
		uint32_t modemismsk      :1;
		uint32_t otgintmsk       :1;
		uint32_t sofmsk          :1;
		uint32_t rxflvlmsk       :1;
		uint32_t nptxfem         :1;
		uint32_t ginnakeffmsk    :1;
		uint32_t goutnakeffmsk   :1;
		uint32_t res8_9          :2;
		uint32_t erlysuspmsk     :1;
		uint32_t usbsuspmsk      :1;
		uint32_t usbrstmsk       :1;
		uint32_t enumdonemsk     :1;
		uint32_t isooutdropmsk   :1;
		uint32_t eopfmsk         :1;
		uint32_t res16           :1;
		uint32_t epmismsk        :1;
		uint32_t iepintmsk       :1;
		uint32_t oepintmsk       :1;
		uint32_t incompisoinmsk  :1;
		uint32_t incomplpmsk     :1;
		uint32_t fetsuspmsk      :1;
		uint32_t resetdetmsk     :1;
		uint32_t prtintmsk       :1;
		uint32_t hchintmsk       :1;
		uint32_t ptxfempmsk      :1;
		uint32_t lpmint          :1;
		uint32_t conidstschngmsk :1;
		uint32_t disconnintmsk   :1;
		uint32_t sessreqintmsk   :1;
		uint32_t wkupintmsk      :1;
	}bits;
}tru_dwc2_gintmsk_t;

#define TRU_DWC2_GINTMSK_REG(usb_base_addr) ((volatile tru_dwc2_gintmsk_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GINTMSK_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t chnum    :4;
		volatile uint32_t bcnt     :11;
		volatile uint32_t dpid     :2;
		volatile uint32_t pktsts   :4;
		volatile uint32_t fn       :4;
		volatile uint32_t res25_26 :2;
		volatile uint32_t stsphst  :1;
		volatile uint32_t res28_31 :4;
	}bits;
}tru_dwc2_grxstsr_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t chnum    :4;
		uint32_t bcnt     :11;
		uint32_t dpid     :2;
		uint32_t pktsts   :4;
		uint32_t fn       :4;
		uint32_t res25_26 :2;
		uint32_t stsphst  :1;
		uint32_t res28_31 :4;
	}bits;
}tru_dwc2_grxstsr_t;

#define TRU_DWC2_GRXSTSR_REG(usb_base_addr) ((volatile tru_dwc2_grxstsr_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GRXSTSR_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t chnum    :4;
		volatile uint32_t bcnt     :11;
		volatile uint32_t dpid     :2;
		volatile uint32_t pktsts   :4;
		volatile uint32_t fn       :4;
		volatile uint32_t res25_26 :2;
		volatile uint32_t stsphst  :1;
		volatile uint32_t res28_31 :4;
	}bits;
}tru_dwc2_grxstsp_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t chnum    :4;
		uint32_t bcnt     :11;
		uint32_t dpid     :2;
		uint32_t pktsts   :4;
		uint32_t fn       :4;
		uint32_t res25_26 :2;
		uint32_t stsphst  :1;
		uint32_t res28_31 :4;
	}bits;
}tru_dwc2_grxstsp_t;

#define TRU_DWC2_GRXSTSP_REG(usb_base_addr) ((volatile tru_dwc2_grxstsp_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GRXSTSP_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t rxfdep   :16;
		volatile uint32_t res16_31 :16;
	}bits;
}tru_dwc2_grxfsiz_reg_t;

#define TRU_DWC2_GRXFSIZ_REG(usb_base_addr) ((volatile tru_dwc2_grxfsiz_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GRXFSIZ_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t nptxfstaddr :16;
		volatile uint32_t nptxfdep    :16;
	}bits;
}tru_dwc2_gnptxfsiz_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t nptxfstaddr :16;
		uint32_t nptxfdep    :16;
	}bits;
}tru_dwc2_gnptxfsiz_t;

#define TRU_DWC2_GNPTXFSIZ_REG(usb_base_addr) ((volatile tru_dwc2_gnptxfsiz_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GNPTXFSIZ_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t nptxfspcavail :16;
		volatile uint32_t nptxqspcavail :8;
		volatile uint32_t nptxqtop      :7;
		volatile uint32_t res31         :1;
	}bits;
}tru_dwc2_gnptxsts_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t nptxfspcavail :16;
		uint32_t nptxqspcavail :8;
		uint32_t nptxqtop      :7;
		uint32_t res31         :1;
	}bits;
}tru_dwc2_gnptxsts_t;

#define TRU_DWC2_GNPTXSTS_REG(usb_base_addr) ((volatile tru_dwc2_gnptxsts_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GNPTXSTS_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t regdata     :8;
		volatile uint32_t vctrl       :8;
		volatile uint32_t regaddr     :6;
		volatile uint32_t regwr       :1;
		volatile uint32_t res23_24    :2;
		volatile uint32_t newregreq   :1;
		volatile uint32_t vstsbsy     :1;
		volatile uint32_t vstsdone    :1;
		volatile uint32_t res28_30    :3;
		volatile uint32_t disulpidrvr :1;
	}bits;
}tru_dwc2_gpvndctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t regdata     :8;
		uint32_t vctrl       :8;
		uint32_t regaddr     :6;
		uint32_t regwr       :1;
		uint32_t res23_24    :2;
		uint32_t newregreq   :1;
		uint32_t vstsbsy     :1;
		uint32_t vstsdone    :1;
		uint32_t res28_30    :3;
		uint32_t disulpidrvr :1;
	}bits;
}tru_dwc2_gpvndctl_t;

#define TRU_DWC2_GPVNDCTL_REG(usb_base_addr) ((volatile tru_dwc2_gpvndctl_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GPVNDCTL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t dcdet    :1;
		volatile uint32_t pdet     :1;
		volatile uint32_t sdet     :1;
		volatile uint32_t ps2det   :1;
		volatile uint32_t res4_15  :12;
		volatile uint32_t pwrdwn   :1;
		volatile uint32_t bcden    :1;
		volatile uint32_t dcden    :1;
		volatile uint32_t pden     :1;
		volatile uint32_t sden     :1;
		volatile uint32_t vbden    :1;
		volatile uint32_t res22_31 :10;
	}bits;
}tru_dwc2_gccfg_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t dcdet    :1;
		uint32_t pdet     :1;
		uint32_t sdet     :1;
		uint32_t ps2det   :1;
		uint32_t res4_15  :12;
		uint32_t pwrdwn   :1;
		uint32_t bcden    :1;
		uint32_t dcden    :1;
		uint32_t pden     :1;
		uint32_t sden     :1;
		uint32_t vbden    :1;
		uint32_t res22_31 :10;
	}bits;
}tru_dwc2_gccfg_t;

#define TRU_DWC2_GCCFG_REG(usb_base_addr) ((volatile tru_dwc2_gccfg_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GCCFG_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t id      :16;
		volatile uint32_t id_copy :16;
	}bits;
}tru_dwc2_cid_reg_t;

#define TRU_DWC2_CID_REG(usb_base_addr) ((volatile tru_dwc2_cid_reg_t *const)((char *)usb_base_addr + TRU_DWC2_CID_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t gsnpsid      :16;
		volatile uint32_t gsnpsid_copy :16;
	}bits;
}tru_dwc2_gsnpsid_reg_t;

#define TRU_DWC2_GSNPSID_REG(usb_base_addr) ((volatile tru_dwc2_gsnpsid_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GSNPSID_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t epdir0  :2;
		volatile uint32_t epdir1  :2;
		volatile uint32_t epdir2  :2;
		volatile uint32_t epdir3  :2;
		volatile uint32_t epdir4  :2;
		volatile uint32_t epdir5  :2;
		volatile uint32_t epdir6  :2;
		volatile uint32_t epdir7  :2;
		volatile uint32_t epdir8  :2;
		volatile uint32_t epdir9  :2;
		volatile uint32_t epdir10 :2;
		volatile uint32_t epdir11 :2;
		volatile uint32_t epdir12 :2;
		volatile uint32_t epdir13 :2;
		volatile uint32_t epdir14 :2;
		volatile uint32_t epdir15 :2;
	}bits;
}tru_dwc2_ghwcfg1_reg_t;

#define TRU_DWC2_GHWCFG1_REG(usb_base_addr) ((volatile tru_dwc2_ghwcfg1_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GHWCFG1_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t otgmode         :3;
		volatile uint32_t dmaarch         :2;
		volatile uint32_t singpnt         :1;
		volatile uint32_t hsphytype       :2;
		volatile uint32_t fsphytype       :2;
		volatile uint32_t numdeveps       :4;
		volatile uint32_t numhstchnl      :4;
		volatile uint32_t periosupport    :1;
		volatile uint32_t dynfifosizing   :1;
		volatile uint32_t multiprocintrpt :1;
		volatile uint32_t res1            :1;
		volatile uint32_t nptxqdepth      :2;
		volatile uint32_t ptxqdepth       :2;
		volatile uint32_t tknqdepth       :5;
		volatile uint32_t res2            :1;
	}bits;
}tru_dwc2_ghwcfg2_reg_t;

#define TRU_DWC2_GHWCFG2_REG(usb_base_addr) ((volatile tru_dwc2_ghwcfg2_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GHWCFG2_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfersizewidth :4;
		volatile uint32_t pktsizewidth  :3;
		volatile uint32_t otgen         :1;
		volatile uint32_t i2cintsel     :1;
		volatile uint32_t vndctlsupt    :1;
		volatile uint32_t optfeature    :1;
		volatile uint32_t rsttype       :1;
		volatile uint32_t adpsupport    :1;
		volatile uint32_t hsicmode      :1;
		volatile uint32_t bcsupport     :1;
		volatile uint32_t lpmmode       :1;
		volatile uint32_t dfifodepth    :16;
	}bits;
}tru_dwc2_ghwcfg3_reg_t;

#define TRU_DWC2_GHWCFG3_REG(usb_base_addr) ((volatile tru_dwc2_ghwcfg3_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GHWCFG3_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t numdevperioeps    :4;
		volatile uint32_t partialpwrdn      :1;
		volatile uint32_t ahbfreq           :1;
		volatile uint32_t hibernation       :1;
		volatile uint32_t res1              :7;
		volatile uint32_t phydatawidth      :2;
		volatile uint32_t numctleps         :4;
		volatile uint32_t iddgfltr          :1;
		volatile uint32_t vbusvalidfltr     :1;
		volatile uint32_t avalidfltr        :1;
		volatile uint32_t bvalidfltr        :1;
		volatile uint32_t sessendfltr       :1;
		volatile uint32_t dedfifomode       :1;
		volatile uint32_t ineps             :4;
		volatile uint32_t dma_configuration :1;
		volatile uint32_t dma               :1;
	}bits;
}tru_dwc2_ghwcfg4_reg_t;

#define TRU_DWC2_GHWCFG4_REG(usb_base_addr) ((volatile tru_dwc2_ghwcfg4_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GHWCFG4_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t gdfifocfg      :16;
		volatile uint32_t epinfobaseaddr :16;
	}bits;
}tru_dwc2_gdfifocfg_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t gdfifocfg      :16;
		uint32_t epinfobaseaddr :16;
	}bits;
}tru_dwc2_gdfifocfg_t;

#define TRU_DWC2_GDFIFOCFG_REG(usb_base_addr) ((volatile tru_dwc2_gdfifocfg_reg_t *const)((char *)usb_base_addr + TRU_DWC2_GDFIFOCFG_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t inepntxfstaddr :16;
		volatile uint32_t inepntxfdep    :16;
	}bits;
}tru_dwc2_dieptxf_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t inepntxfstaddr :16;
		uint32_t inepntxfdep    :16;
	}bits;
}tru_dwc2_dieptxf_t;

#define TRU_DWC2_DIEPTXF_REG(usb_base_addr, fifonum) ((volatile tru_dwc2_dieptxf_reg_t *const)((char *)usb_base_addr + (4 * (fifonum - 1)) + TRU_DWC2_DIEPTXF1_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t devspd       :2;
		volatile uint32_t nzstsouthshk :1;
		volatile uint32_t ena32khzsusp :1;
		volatile uint32_t devaddr      :7;
		volatile uint32_t perfrint     :2;
		volatile uint32_t endevoutnak  :1;
		volatile uint32_t xcvrdly      :1;
		volatile uint32_t erratim      :1;
		volatile uint32_t res16_22     :7;
		volatile uint32_t descdma      :1;
		volatile uint32_t perschintvl  :2;
		volatile uint32_t resvalid     :6;
	}bits;
}tru_dwc2_dcfg_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t devspd       :2;
		uint32_t nzstsouthshk :1;
		uint32_t ena32khzsusp :1;
		uint32_t devaddr      :7;
		uint32_t perfrint     :2;
		uint32_t endevoutnak  :1;
		uint32_t xcvrdly      :1;
		uint32_t erratim      :1;
		uint32_t res16_22     :7;
		uint32_t descdma      :1;
		uint32_t perschintvl  :2;
		uint32_t resvalid     :6;
	}bits;
}tru_dwc2_dcfg_t;

#define TRU_DWC2_DCFG_REG(usb_base_addr) ((volatile tru_dwc2_dcfg_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DCFG_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t rmtwkupsig   :1;
		volatile uint32_t sftdiscon    :1;
		volatile uint32_t gnpinnaksts  :1;
		volatile uint32_t goutnaksts   :1;
		volatile uint32_t tstctl       :3;
		volatile uint32_t sgnpinnak    :1;
		volatile uint32_t cgnpinnak    :1;
		volatile uint32_t sgoutnak     :1;
		volatile uint32_t cgoutnak     :1;
		volatile uint32_t pwronprgdone :1;
		volatile uint32_t res12        :1;
		volatile uint32_t gmc          :2;
		volatile uint32_t ignrfrmnum   :1;
		volatile uint32_t nakonbbl     :1;
		volatile uint32_t encontonbna  :1;
		volatile uint32_t dsbeslrjct   :1;
		volatile uint32_t res19_31     :13;
	}bits;
}tru_dwc2_dctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t rmtwkupsig   :1;
		uint32_t sftdiscon    :1;
		uint32_t gnpinnaksts  :1;
		uint32_t goutnaksts   :1;
		uint32_t tstctl       :3;
		uint32_t sgnpinnak    :1;
		uint32_t cgnpinnak    :1;
		uint32_t sgoutnak     :1;
		uint32_t cgoutnak     :1;
		uint32_t pwronprgdone :1;
		uint32_t res12        :1;
		uint32_t gmc          :2;
		uint32_t ignrfrmnum   :1;
		uint32_t nakonbbl     :1;
		uint32_t encontonbna  :1;
		uint32_t dsbeslrjct   :1;
		uint32_t res19_31     :13;
	}bits;
}tru_dwc2_dctl_t;

#define TRU_DWC2_DCTL_REG(usb_base_addr) ((volatile tru_dwc2_dctl_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DCTL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t suspsts   :1;
		volatile uint32_t enumspd   :2;
		volatile uint32_t errticerr :1;
		volatile uint32_t res4_7    :4;
		volatile uint32_t soffn     :14;
		volatile uint32_t devlnsts  :2;
		volatile uint32_t res24_31  :8;
	}bits;
}tru_dwc2_dsts_reg_t;

typedef union{
	volatile uint32_t val;
	struct{
		uint32_t suspsts   :1;
		uint32_t enumspd   :2;
		uint32_t errticerr :1;
		uint32_t res4_7    :4;
		uint32_t soffn     :14;
		uint32_t devlnsts  :2;
		uint32_t res24_31  :8;
	}bits;
}tru_dwc2_dsts_t;

#define TRU_DWC2_DSTS_REG(usb_base_addr) ((volatile tru_dwc2_dsts_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DSTS_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfercomplmsk   :1;
		volatile uint32_t epdisbldmsk    :1;
		volatile uint32_t ahberrmsk      :1;
		volatile uint32_t timeoutmsk     :1;
		volatile uint32_t intkntxfempmsk :1;
		volatile uint32_t intknepmismsk  :1;
		volatile uint32_t inepnakeffmsk  :1;
		volatile uint32_t res7           :1;
		volatile uint32_t txfifoundrnmsk :1;
		volatile uint32_t bnamsk         :1;
		volatile uint32_t res10_12       :3;
		volatile uint32_t nakmsk         :1;
		volatile uint32_t res14_31       :18;
	}bits;
}tru_dwc2_diepmsk_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfercomplmsk   :1;
		uint32_t epdisbldmsk    :1;
		uint32_t ahberrmsk      :1;
		uint32_t timeoutmsk     :1;
		uint32_t intkntxfempmsk :1;
		uint32_t intknepmismsk  :1;
		uint32_t inepnakeffmsk  :1;
		uint32_t res7           :1;
		uint32_t txfifoundrnmsk :1;
		uint32_t bnamsk         :1;
		uint32_t res10_12       :3;
		uint32_t nakmsk         :1;
		uint32_t res14_31       :18;
	}bits;
}tru_dwc2_diepmsk_t;

#define TRU_DWC2_DIEPMSK_REG(usb_base_addr) ((volatile tru_dwc2_diepmsk_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DIEPMSK_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfercomplmsk   :1;
		volatile uint32_t epdisbldmsk    :1;
		volatile uint32_t ahberrmsk      :1;
		volatile uint32_t setupmsk       :1;
		volatile uint32_t outtknepdismsk :1;
		volatile uint32_t stsphsrxmsk    :1;
		volatile uint32_t b2bsetupmsk    :1;
		volatile uint32_t res7           :1;
		volatile uint32_t outpkterrmsk   :1;
		volatile uint32_t bnamsk         :1;
		volatile uint32_t res10_11       :2;
		volatile uint32_t bbleerrmsk     :1;
		volatile uint32_t nakmsk         :1;
		volatile uint32_t nyetmsk        :1;
		volatile uint32_t res15_31       :17;
	}bits;
}tru_dwc2_doepmsk_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfercomplmsk   :1;
		uint32_t epdisbldmsk    :1;
		uint32_t ahberrmsk      :1;
		uint32_t setupmsk       :1;
		uint32_t outtknepdismsk :1;
		uint32_t stsphsrxmsk    :1;
		uint32_t b2bsetupmsk    :1;
		uint32_t res7           :1;
		uint32_t outpkterrmsk   :1;
		uint32_t bnamsk         :1;
		uint32_t res10_11       :2;
		uint32_t bbleerrmsk     :1;
		uint32_t nakmsk         :1;
		uint32_t nyetmsk        :1;
		uint32_t stpktrxmsk     :1;
		uint32_t res16_31       :16;
	}bits;
}tru_dwc2_doepmsk_t;

#define TRU_DWC2_DOEPMSK_REG(usb_base_addr) ((volatile tru_dwc2_doepmsk_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DOEPMSK_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t inepint0   :1;
		volatile uint32_t inepint1   :1;
		volatile uint32_t inepint2   :1;
		volatile uint32_t inepint3   :1;
		volatile uint32_t inepint4   :1;
		volatile uint32_t inepint5   :1;
		volatile uint32_t inepint6   :1;
		volatile uint32_t inepint7   :1;
		volatile uint32_t inepint8   :1;
		volatile uint32_t inepint9   :1;
		volatile uint32_t inepint10  :1;
		volatile uint32_t inepint11  :1;
		volatile uint32_t inepint12  :1;
		volatile uint32_t inepint13  :1;
		volatile uint32_t inepint14  :1;
		volatile uint32_t inepint15  :1;
		volatile uint32_t outepint0  :1;
		volatile uint32_t outepint1  :1;
		volatile uint32_t outepint2  :1;
		volatile uint32_t outepint3  :1;
		volatile uint32_t outepint4  :1;
		volatile uint32_t outepint5  :1;
		volatile uint32_t outepint6  :1;
		volatile uint32_t outepint7  :1;
		volatile uint32_t outepint8  :1;
		volatile uint32_t outepint9  :1;
		volatile uint32_t outepint10 :1;
		volatile uint32_t outepint11 :1;
		volatile uint32_t outepint12 :1;
		volatile uint32_t outepint13 :1;
		volatile uint32_t outepint14 :1;
		volatile uint32_t outepint15 :1;
	}bits;
}tru_dwc2_daint_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t inepint0   :1;
		uint32_t inepint1   :1;
		uint32_t inepint2   :1;
		uint32_t inepint3   :1;
		uint32_t inepint4   :1;
		uint32_t inepint5   :1;
		uint32_t inepint6   :1;
		uint32_t inepint7   :1;
		uint32_t inepint8   :1;
		uint32_t inepint9   :1;
		uint32_t inepint10  :1;
		uint32_t inepint11  :1;
		uint32_t inepint12  :1;
		uint32_t inepint13  :1;
		uint32_t inepint14  :1;
		uint32_t inepint15  :1;
		uint32_t outepint0  :1;
		uint32_t outepint1  :1;
		uint32_t outepint2  :1;
		uint32_t outepint3  :1;
		uint32_t outepint4  :1;
		uint32_t outepint5  :1;
		uint32_t outepint6  :1;
		uint32_t outepint7  :1;
		uint32_t outepint8  :1;
		uint32_t outepint9  :1;
		uint32_t outepint10 :1;
		uint32_t outepint11 :1;
		uint32_t outepint12 :1;
		uint32_t outepint13 :1;
		uint32_t outepint14 :1;
		uint32_t outepint15 :1;
	}bits;
}tru_dwc2_daint_t;

#define TRU_DWC2_DAINT_REG(usb_base_addr) ((volatile tru_dwc2_daint_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DAINT_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t inepmsk0   :1;
		volatile uint32_t inepmsk1   :1;
		volatile uint32_t inepmsk2   :1;
		volatile uint32_t inepmsk3   :1;
		volatile uint32_t inepmsk4   :1;
		volatile uint32_t inepmsk5   :1;
		volatile uint32_t inepmsk6   :1;
		volatile uint32_t inepmsk7   :1;
		volatile uint32_t inepmsk8   :1;
		volatile uint32_t inepmsk9   :1;
		volatile uint32_t inepmsk10  :1;
		volatile uint32_t inepmsk11  :1;
		volatile uint32_t inepmsk12  :1;
		volatile uint32_t inepmsk13  :1;
		volatile uint32_t inepmsk14  :1;
		volatile uint32_t inepmsk15  :1;
		volatile uint32_t outepmsk0  :1;
		volatile uint32_t outepmsk1  :1;
		volatile uint32_t outepmsk2  :1;
		volatile uint32_t outepmsk3  :1;
		volatile uint32_t outepmsk4  :1;
		volatile uint32_t outepmsk5  :1;
		volatile uint32_t outepmsk6  :1;
		volatile uint32_t outepmsk7  :1;
		volatile uint32_t outepmsk8  :1;
		volatile uint32_t outepmsk9  :1;
		volatile uint32_t outepmsk10 :1;
		volatile uint32_t outepmsk11 :1;
		volatile uint32_t outepmsk12 :1;
		volatile uint32_t outepmsk13 :1;
		volatile uint32_t outepmsk14 :1;
		volatile uint32_t outepmsk15 :1;
	}bits;
}tru_dwc2_daintmsk_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t inepmsk0   :1;
		uint32_t inepmsk1   :1;
		uint32_t inepmsk2   :1;
		uint32_t inepmsk3   :1;
		uint32_t inepmsk4   :1;
		uint32_t inepmsk5   :1;
		uint32_t inepmsk6   :1;
		uint32_t inepmsk7   :1;
		uint32_t inepmsk8   :1;
		uint32_t inepmsk9   :1;
		uint32_t inepmsk10  :1;
		uint32_t inepmsk11  :1;
		uint32_t inepmsk12  :1;
		uint32_t inepmsk13  :1;
		uint32_t inepmsk14  :1;
		uint32_t inepmsk15  :1;
		uint32_t outepmsk0  :1;
		uint32_t outepmsk1  :1;
		uint32_t outepmsk2  :1;
		uint32_t outepmsk3  :1;
		uint32_t outepmsk4  :1;
		uint32_t outepmsk5  :1;
		uint32_t outepmsk6  :1;
		uint32_t outepmsk7  :1;
		uint32_t outepmsk8  :1;
		uint32_t outepmsk9  :1;
		uint32_t outepmsk10 :1;
		uint32_t outepmsk11 :1;
		uint32_t outepmsk12 :1;
		uint32_t outepmsk13 :1;
		uint32_t outepmsk14 :1;
		uint32_t outepmsk15 :1;
	}bits;
}tru_dwc2_daintmsk_t;

#define TRU_DWC2_DAINTMSK_REG(usb_base_addr) ((volatile tru_dwc2_daintmsk_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DAINTMSK_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t dvbusdis :16;
		volatile uint32_t res16_31 :16;
	}bits;
}tru_dwc2_dvbusdis_reg_t;

#define TRU_DWC2_DVBUSDIS_REG(usb_base_addr) ((volatile tru_dwc2_dvbusdis_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DVBUSDIS_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t dvbuspulse :16;
		volatile uint32_t res16_31   :16;
	}bits;
}tru_dwc2_dvbuspulse_reg_t;

#define TRU_DWC2_DVBUSPULSE_REG(usb_base_addr) ((volatile tru_dwc2_dvbuspulse_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DVBUSPULSE_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t nonisothren :1;
		volatile uint32_t isothren    :1;
		volatile uint32_t txthrlen    :9;
		volatile uint32_t ahbthrratio :2;
		volatile uint32_t res13_15    :3;
		volatile uint32_t rxthren     :1;
		volatile uint32_t rxthrlen    :9;
		volatile uint32_t res26       :1;
		volatile uint32_t arbprken    :1;
		volatile uint32_t res28_31    :4;
	}bits;
}tru_dwc2_dthrctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t nonisothren :1;
		uint32_t isothren    :1;
		uint32_t txthrlen    :9;
		uint32_t ahbthrratio :2;
		uint32_t res13_15    :3;
		uint32_t rxthren     :1;
		uint32_t rxthrlen    :9;
		uint32_t res26       :1;
		uint32_t arbprken    :1;
		uint32_t res28_31    :4;
	}bits;
}tru_dwc2_dthrctl_t;

#define TRU_DWC2_DTHRCTL_REG(usb_base_addr) ((volatile tru_dwc2_dthrctl_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DTHRCTL_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ineptxfempmsk0  :1;
		volatile uint32_t ineptxfempmsk1  :1;
		volatile uint32_t ineptxfempmsk2  :1;
		volatile uint32_t ineptxfempmsk3  :1;
		volatile uint32_t ineptxfempmsk4  :1;
		volatile uint32_t ineptxfempmsk5  :1;
		volatile uint32_t ineptxfempmsk6  :1;
		volatile uint32_t ineptxfempmsk7  :1;
		volatile uint32_t ineptxfempmsk8  :1;
		volatile uint32_t ineptxfempmsk9  :1;
		volatile uint32_t ineptxfempmsk10 :1;
		volatile uint32_t ineptxfempmsk11 :1;
		volatile uint32_t ineptxfempmsk12 :1;
		volatile uint32_t ineptxfempmsk13 :1;
		volatile uint32_t ineptxfempmsk14 :1;
		volatile uint32_t ineptxfempmsk15 :1;
		volatile uint32_t res16_31        :16;
	}bits;
}tru_dwc2_diepempmsk_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t ineptxfempmsk0  :1;
		uint32_t ineptxfempmsk1  :1;
		uint32_t ineptxfempmsk2  :1;
		uint32_t ineptxfempmsk3  :1;
		uint32_t ineptxfempmsk4  :1;
		uint32_t ineptxfempmsk5  :1;
		uint32_t ineptxfempmsk6  :1;
		uint32_t ineptxfempmsk7  :1;
		uint32_t ineptxfempmsk8  :1;
		uint32_t ineptxfempmsk9  :1;
		uint32_t ineptxfempmsk10 :1;
		uint32_t ineptxfempmsk11 :1;
		uint32_t ineptxfempmsk12 :1;
		uint32_t ineptxfempmsk13 :1;
		uint32_t ineptxfempmsk14 :1;
		uint32_t ineptxfempmsk15 :1;
		uint32_t res16_31        :16;
	}bits;
}tru_dwc2_diepempmsk_t;

#define TRU_DWC2_DIEPEMPMSK_REG(usb_base_addr) ((volatile tru_dwc2_diepempmsk_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DIEPEMPMSK_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t mps      :2;
		volatile uint32_t res2_14  :13;
		volatile uint32_t usbactep :1;
		volatile uint32_t res16    :1;
		volatile uint32_t naksts   :1;
		volatile uint32_t eptype   :2;
		volatile uint32_t res20    :1;
		volatile uint32_t stall    :1;
		volatile uint32_t txfnum   :4;
		volatile uint32_t cnak     :1;
		volatile uint32_t snak     :1;
		volatile uint32_t res28_29 :2;
		volatile uint32_t epdis    :1;
		volatile uint32_t epena    :1;
	}bits;
}tru_dwc2_diepctl0_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t mps      :2;
		uint32_t res2_14  :13;
		uint32_t usbactep :1;
		uint32_t res16    :1;
		uint32_t naksts   :1;
		uint32_t eptype   :2;
		uint32_t res20    :1;
		uint32_t stall    :1;
		uint32_t txfnum   :4;
		uint32_t cnak     :1;
		uint32_t snak     :1;
		uint32_t res28_29 :2;
		uint32_t epdis    :1;
		uint32_t epena    :1;
	}bits;
}tru_dwc2_diepctl0_t;

#define TRU_DWC2_DIEPCTL0_REG(usb_base_addr) ((volatile tru_dwc2_diepctl0_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DIEPCTL0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfersize :7;
		volatile uint32_t res7_18  :12;
		volatile uint32_t pktcnt   :2;
		volatile uint32_t res21_31 :11;
	}bits;
}tru_dwc2_dieptsiz0_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfersize :7;
		uint32_t res7_18  :12;
		uint32_t pktcnt   :2;
		uint32_t res21_31 :11;
	}bits;
}tru_dwc2_dieptsiz0_t;

#define TRU_DWC2_DIEPTSIZ0_REG(usb_base_addr) ((volatile tru_dwc2_dieptsiz0_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DIEPTSIZ0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t mps        :11;
		volatile uint32_t res11_14   :4;
		volatile uint32_t usbactep   :1;
		volatile uint32_t dpid_eonum :1;
		volatile uint32_t naksts     :1;
		volatile uint32_t eptype     :2;
		volatile uint32_t res20      :1;
		volatile uint32_t stall      :1;
		volatile uint32_t txfnum     :4;
		volatile uint32_t cnak       :1;
		volatile uint32_t snak       :1;
		volatile uint32_t d0pid_even :1;
		volatile uint32_t d1pid_odd  :1;
		volatile uint32_t epdis      :1;
		volatile uint32_t epena      :1;
	}bits;
}tru_dwc2_diepctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t mps        :11;
		uint32_t res11_14   :4;
		uint32_t usbactep   :1;
		uint32_t dpid_eonum :1;
		uint32_t naksts     :1;
		uint32_t eptype     :2;
		uint32_t res20      :1;
		uint32_t stall      :1;
		uint32_t txfnum     :4;
		uint32_t cnak       :1;
		uint32_t snak       :1;
		uint32_t d0pid_even :1;
		uint32_t d1pid_odd  :1;
		uint32_t epdis      :1;
		uint32_t epena      :1;
	}bits;
}tru_dwc2_diepctl_t;

#define TRU_DWC2_DIEPCTL_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_diepctl_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DIEPCTL0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfercompl   :1;
		volatile uint32_t epdisbld    :1;
		volatile uint32_t ahberr      :1;
		volatile uint32_t timeout     :1;
		volatile uint32_t intkntxfem  :1;
		volatile uint32_t intknepmis  :1;
		volatile uint32_t inepnakeff  :1;
		volatile uint32_t txfemp      :1;
		volatile uint32_t txfifoundrn :1;
		volatile uint32_t bna         :1;
		volatile uint32_t res10       :1;
		volatile uint32_t pktdrpsts   :1;
		volatile uint32_t bbleerr     :1;
		volatile uint32_t nakintrpt   :1;
		volatile uint32_t nyetintrpt  :1;
		volatile uint32_t res16_31    :17;
	}bits;
}tru_dwc2_diepint_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfercompl   :1;
		uint32_t epdisbld    :1;
		uint32_t ahberr      :1;
		uint32_t timeout     :1;
		uint32_t intkntxfem  :1;
		uint32_t intknepmis  :1;
		uint32_t inepnakeff  :1;
		uint32_t txfemp      :1;
		uint32_t txfifoundrn :1;
		uint32_t bna         :1;
		uint32_t res10       :1;
		uint32_t pktdrpsts   :1;
		uint32_t bbleerr     :1;
		uint32_t nakintrpt   :1;
		uint32_t nyetintrpt  :1;
		uint32_t res16_31    :17;
	}bits;
}tru_dwc2_diepint_t;

#define TRU_DWC2_DIEPINT_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_diepint_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DIEPINT0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfersize :19;
		volatile uint32_t pktcnt   :10;
		volatile uint32_t mc       :2;
		volatile uint32_t res31    :1;
	}bits;
}tru_dwc2_dieptsiz_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfersize :19;
		uint32_t pktcnt   :10;
		uint32_t mc       :2;
		uint32_t res31    :1;
	}bits;
}tru_dwc2_dieptsiz_t;

#define TRU_DWC2_DIEPTSIZ_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_dieptsiz_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DIEPTSIZ0_OFFSET))

typedef struct{
	volatile uint32_t buf;
}tru_dwc2_diepdma_reg_t;

#define TRU_DWC2_DIEPDMA_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_diepdma_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DIEPDMA0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t ineptxfspcavail :16;
		volatile uint32_t res16_31        :16;
	}bits;
}tru_dwc2_dtxfsts_reg_t;

#define TRU_DWC2_DTXFSTS_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_dtxfsts_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DTXFSTS0_OFFSET))

typedef struct{
	volatile uint32_t buf;
}tru_dwc2_diepdmab_reg_t;

#define TRU_DWC2_DIEPDMAB_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_diepdmab_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DIEPDMAB0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t mps      :2;
		volatile uint32_t res2_14  :13;
		volatile uint32_t usbactep :1;
		volatile uint32_t res16    :1;
		volatile uint32_t naksts   :1;
		volatile uint32_t eptype   :2;
		volatile uint32_t snp      :1;
		volatile uint32_t stall    :1;
		volatile uint32_t res22_25 :4;
		volatile uint32_t cnak     :1;
		volatile uint32_t snak     :1;
		volatile uint32_t res28_29 :2;
		volatile uint32_t epdis    :1;
		volatile uint32_t epena    :1;
	}bits;
}tru_dwc2_doepctl0_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t mps      :2;
		uint32_t res2_14  :13;
		uint32_t usbactep :1;
		uint32_t res16    :1;
		uint32_t naksts   :1;
		uint32_t eptype   :2;
		uint32_t snp      :1;
		uint32_t stall    :1;
		uint32_t res22_25 :4;
		uint32_t cnak     :1;
		uint32_t snak     :1;
		uint32_t res28_29 :2;
		uint32_t epdis    :1;
		uint32_t epena    :1;
	}bits;
}tru_dwc2_doepctl0_t;

#define TRU_DWC2_DOEPCTL0_REG(usb_base_addr) ((volatile tru_dwc2_doepctl0_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DOEPCTL0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfersize :7;
		volatile uint32_t res7_18  :12;
		volatile uint32_t pktcnt   :1;
		volatile uint32_t res20_38 :9;
		volatile uint32_t supcnt   :2;
		volatile uint32_t res31    :1;
	}bits;
}tru_dwc2_doeptsiz0_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfersize :7;
		uint32_t res7_18  :12;
		uint32_t pktcnt   :1;
		uint32_t res20_38 :9;
		uint32_t supcnt   :2;
		uint32_t res31    :1;
	}bits;
}tru_dwc2_doeptsiz0_t;

#define TRU_DWC2_DOEPTSIZ0_REG(usb_base_addr) ((volatile tru_dwc2_doeptsiz0_reg_t *const)((char *)usb_base_addr + TRU_DWC2_DOEPTSIZ0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t mps        :11;
		volatile uint32_t res11_14   :4;
		volatile uint32_t usbactep   :1;
		volatile uint32_t dpid_eonum :1;
		volatile uint32_t naksts     :1;
		volatile uint32_t eptype     :2;
		volatile uint32_t snp        :1;
		volatile uint32_t stall      :1;
		volatile uint32_t res22_25   :4;
		volatile uint32_t cnak       :1;
		volatile uint32_t snak       :1;
		volatile uint32_t d0pid_even :1;
		volatile uint32_t d1pid_odd  :1;
		volatile uint32_t epdis      :1;
		volatile uint32_t epena      :1;
	}bits;
}tru_dwc2_doepctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t mps        :11;
		uint32_t res11_14   :4;
		uint32_t usbactep   :1;
		uint32_t dpid_eonum :1;
		uint32_t naksts     :1;
		uint32_t eptype     :2;
		uint32_t snp        :1;
		uint32_t stall      :1;
		uint32_t res22_25   :4;
		uint32_t cnak       :1;
		uint32_t snak       :1;
		uint32_t d0pid_even :1;
		uint32_t d1pid_odd  :1;
		uint32_t epdis      :1;
		uint32_t epena      :1;
	}bits;
}tru_dwc2_doepctl_t;

#define TRU_DWC2_DOEPCTL_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_doepctl_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DOEPCTL0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfercompl   :1;
		volatile uint32_t epdisbld    :1;
		volatile uint32_t ahberr      :1;
		volatile uint32_t setup       :1;
		volatile uint32_t outtknepdis :1;
		volatile uint32_t stsphsrx    :1;
		volatile uint32_t b2bsetup    :1;
		volatile uint32_t res7        :1;
		volatile uint32_t outpkterr   :1;
		volatile uint32_t bna         :1;
		volatile uint32_t res10       :1;
		volatile uint32_t pktdrpsts   :1;
		volatile uint32_t bbleerr     :1;
		volatile uint32_t nak         :1;
		volatile uint32_t nyet        :1;
		volatile uint32_t stpktrx     :1;
		volatile uint32_t res16_31    :16;
	}bits;
}tru_dwc2_doepint_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfercompl   :1;
		uint32_t epdisbld    :1;
		uint32_t ahberr      :1;
		uint32_t setup       :1;
		uint32_t outtknepdis :1;
		uint32_t stsphsrx    :1;
		uint32_t b2bsetup    :1;
		uint32_t res7        :1;
		uint32_t outpkterr   :1;
		uint32_t bna         :1;
		uint32_t res10       :1;
		uint32_t pktdrpsts   :1;
		uint32_t bbleerr     :1;
		uint32_t nak         :1;
		uint32_t nyet        :1;
		uint32_t stpktrx     :1;
		uint32_t res16_31    :16;
	}bits;
}tru_dwc2_doepint_t;

#define TRU_DWC2_DOEPINT_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_doepint_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DOEPINT0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t xfersize :19;
		volatile uint32_t pktcnt   :10;
		volatile uint32_t rxdpid   :2;
		volatile uint32_t res31    :1;
	}bits;
}tru_dwc2_doeptsiz_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t xfersize :19;
		uint32_t pktcnt   :10;
		uint32_t rxdpid   :2;
		uint32_t res31    :1;
	}bits;
}tru_dwc2_doeptsiz_t;

#define TRU_DWC2_DOEPTSIZ_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_doeptsiz_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DOEPTSIZ0_OFFSET))

typedef struct{
	volatile uint32_t buf;
}tru_dwc2_doepdma_reg_t;

#define TRU_DWC2_DOEPDMA_REG(usb_base_addr, ep_num) ((volatile tru_dwc2_doepdma_reg_t *const)((char *)usb_base_addr + (0x20 * ep_num) + TRU_DWC2_DOEPDMA0_OFFSET))

typedef union{
	volatile uint32_t val;
	struct{
		volatile uint32_t stppclk  :1;
		volatile uint32_t gatehclk :1;
		volatile uint32_t res2_3   :2;
		volatile uint32_t physusp  :1;
		volatile uint32_t enl1gtg  :1;
		volatile uint32_t physleep :1;
		volatile uint32_t susp     :1;
		volatile uint32_t res8_31  :24;
	}bits;
}tru_dwc2_pcgcctl_reg_t;

// Non-volatile version
typedef union{
	uint32_t val;
	struct{
		uint32_t stppclk  :1;
		uint32_t gatehclk :1;
		uint32_t res2_3   :2;
		uint32_t physusp  :1;
		uint32_t enl1gtg  :1;
		uint32_t physleep :1;
		uint32_t susp     :1;
		uint32_t res8_31  :24;
	}bits;
}tru_dwc2_pcgcctl_t;

#define TRU_DWC2_PCGCCTL_REG(usb_base_addr) ((volatile tru_dwc2_pcgcctl_reg_t *const)((char *)usb_base_addr + TRU_DWC2_PCGCCTL_OFFSET))

// =================
// Other definitions
// =================

typedef enum{
	TRU_DWC2_PHYSPEED_HS,
	TRU_DWC2_PHYSPEED_FS
}tru_dwc2_physpeed_t;

typedef enum{
	TRU_DWC2_PHYIFTYPE_INTERNAL,
	TRU_DWC2_PHYIFTYPE_ULPI,
	TRU_DWC2_PHYIFTYPE_UTMI
}tru_dwc2_phyiftype_t;

typedef enum{
	TRU_DWC2_PHYRATE_SDR,
	TRU_DWC2_PHYRATE_DDR
}tru_dwc2_phyrate_t;

typedef enum{
	TRU_DWC2_PHYWIDTH_8BIT,
	TRU_DWC2_PHYWIDTH_16BIT
}tru_dwc2_phywidth_t;

typedef struct{
	tru_dwc2_physpeed_t speed;
	tru_dwc2_phyiftype_t iftype;
	tru_dwc2_phyrate_t rate;
	tru_dwc2_phywidth_t width;
}tru_dwc2_phy_t;

typedef enum{
	TRU_DWC2_DMA_MODE_FF,  // FIFO slave mode, i.e. DMA off (aka slave mode)
	TRU_DWC2_DMA_MODE_SB,  // DMA Single Buffer mode (non scatter gather mode)
	TRU_DWC2_DMA_MODE_SG   // DMA Scatter Gather mode (multiple buffers using a list of descriptors)
}tru_dwc2_dma_mode_t;

// Macro versions of DMA modes for use with preprocessor directives, such as #if
#define _TRU_DWC2_DMA_MODE_FF 0
#define _TRU_DWC2_DMA_MODE_SB 1
#define _TRU_DWC2_DMA_MODE_SG 2

typedef enum{
	TRU_DWC2_TXFIFO_SHARED,
	TRU_DWC2_TXFIFO_DEDICATED
}tru_dwc2_txfifo_mode_t;

typedef enum{
	TRU_DWC2_STALL_CLEAR,   // Set stall = 0
	TRU_DWC2_STALL_SET,     // Set stall = 1
	TRU_DWC2_STALL_CURRENT  // Use the current value
}tru_dwc2_stall_t;

typedef enum{
	TRU_DWC2_NAK_CLEAR,   // Set cnak = 1
	TRU_DWC2_NAK_SET,     // Set snak = 1
	TRU_DWC2_NAK_CURRENT  // Use the current value
}tru_dwc2_nak_t;

// DATA0/DATA1 PID (Bulk) or Even/Odd parity (Isochronous)
typedef enum{
	TRU_DWC2_DPID_EO_D0_OR_EVEN,  // Set setd0pid = 1 (DATA0 or even parity)
	TRU_DWC2_DPID_EO_D1_OR_ODD,   // Set setd1pid = 1 (DATA1 or odd parity)
	TRU_DWC2_DPID_EO_CURRENT,     // Use the current value
	TRU_DWC2_DPID_EO_AUTO         // Decided by library
}tru_dwc2_dpid_eo_t;

typedef struct{
	uint32_t snpsid;
	uint32_t num_dev_ep;
	uint32_t num_dev_in_ep;
	uint32_t fifo_total_size_words;
	uint32_t fifo_usable_size_words;
	uint32_t tknqdepth;
	uint32_t ptxqdepth;
	uint32_t nptxqdepth;
	uint32_t max_transfer_size;
	uint32_t max_packet_count;
	uint32_t dma_arch;
	bool dma_capable;
	bool dma_desc_capable;
}tru_dwc2_hwcfg_t;

#ifdef __cplusplus
}
#endif

#endif
