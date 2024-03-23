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

    Developer: Truong Hy
    HDL      : Verilog
    Target   : DE10-Nano development kit board (Intel Cyclone V SoC-FPGA)
    Version  : 20230822
    
    FPGA logic design demonstrating two-way interaction between the two separate
		systems: FPGA and HPS.
		
		This is the FPGA logic part of the ledsweep, designed to work with the
		software counterpart.  It provides the logic to support the software on the
		HPS so that it is able to control the 8x FPGA LEDs, and to receive interrupt
		that is triggered by the FPGA input keys 0 and 1.
    
    HPS to FPGA interaction (output LEDs):
        => HPS side : software writes to the L2H bridge using memory addresses
                      mapped to the PIO outputs
        => FPGA side: the PIO retrieves the updated values from the L2H bridge
        => FPGA side: FPGA logic updates LED states according to the PIO
                      outputs
        
    FPGA to HPS interaction (input keys):
        => FPGA side: when a tactile key is pressed, it updates the PIO inputs
                      and also the PIO IRQ output (a simulated IRQ signal)
        => FPGA side: FPGA logic updates the F2H interrupt receiver using the PIO
                      IRQ output signal
        => FPGA side: the F2H interrupt receiver generates a processor interrupt
                      with the pre-assigned IRQ ID
        => HPS side : interrupt handler is triggered, which reads from the L2H
                      bridge using memory addresses mapped to the PIO inputs,
                      the value indicates the key that was pressed, and then
                      updates the animation speed
    
    Synonyms:
        HPS = Hard-Processing System - the ARM Cortex A-9 processor system
        PIO = PIO IP (Parallel I/O IP) - a generic I/O IP that is mappable to a
              memory region
        L2H = Lightweight-to-HPS bridge
        F2H interrupt receiver = IRQ receiver for the FPGA-to-HPS bridge
*/

module top(
	// FPGA clock
	input FPGA_CLK1_50,
	input FPGA_CLK2_50,
	input FPGA_CLK3_50,
	
	// FPGA ADC. FPGA pins wired to LTC2308 (SPI)
	output ADC_CONVST,
	output ADC_SCK,
	output ADC_SDI,
	input  ADC_SDO,
	
	// FPGA Arduino IO. FPGA pins wired to female headers
	inout [15:0] ARDUINO_IO,
	inout        ARDUINO_RESET_N,
	
	// FPGA GPIO. FPGA pins wired to 2x male headers
	inout [35:0] GPIO_0,
	inout [35:0] GPIO_1,
	
	// FPGA HDMI. FPGA pins to HDMI transmitter chip (Analog Devices ADV7513BSWZ)
	inout          HDMI_I2C_SCL,
	inout          HDMI_I2C_SDA,
	inout          HDMI_I2S,
	inout          HDMI_LRCLK,
	inout          HDMI_MCLK,
	inout          HDMI_SCLK,
	output         HDMI_TX_CLK,
	output [23: 0] HDMI_TX_D,
	output         HDMI_TX_DE,
	output         HDMI_TX_HS,
	input          HDMI_TX_INT,
	output         HDMI_TX_VS,

	// FPGA push buttons, LEDs and slide switches
	input  [1:0] KEY,  // FPGA pins to tactile switches
	output [7:0] LED,  // FPGA pins to LEDs
	input  [3:0] SW,   // FPGA pins to slide switches
	
	// HPS user key and LED
	inout HPS_KEY,
	inout HPS_LED,

	// HPS DDR-3 SDRAM. HPS pins to DDR3 1GB = 2x512MB chips
	output [14:0] HPS_DDR3_ADDR,
	output [2:0]  HPS_DDR3_BA,
	output        HPS_DDR3_CAS_N,
	output        HPS_DDR3_CKE,
	output        HPS_DDR3_CK_N,
	output        HPS_DDR3_CK_P,
	output        HPS_DDR3_CS_N,
	output [3:0]  HPS_DDR3_DM,
	inout  [31:0] HPS_DDR3_DQ,
	inout  [3:0]  HPS_DDR3_DQS_N,
	inout  [3:0]  HPS_DDR3_DQS_P,
	output        HPS_DDR3_ODT,
	output        HPS_DDR3_RAS_N,
	output        HPS_DDR3_RESET_N,
	input         HPS_DDR3_RZQ,
	output        HPS_DDR3_WE_N,
	
	// HPS SD-CARD. HPS pins wired to micro SD card slot
	output      HPS_SD_CLK,
	inout       HPS_SD_CMD,
	inout [3:0] HPS_SD_DATA,

	// HPS UART (UART-USB). HPS pins wired to chip FTDI FT232R
	input  HPS_UART_RX,
	output HPS_UART_TX,
	inout  HPS_CONV_USB_N,
	
	// HPS USB OTG. HPS pins wired to the USB PHY chip (Microchip USB3300)
	input       HPS_USB_CLKOUT,
	inout [7:0] HPS_USB_DATA,
	input       HPS_USB_DIR,
	input       HPS_USB_NXT,
	output      HPS_USB_STP,
	
	// HPS EMAC. HPS pins wired to the gigabit ethernet PHY chip (Microchip KSZ9031RNX)
	output       HPS_ENET_GTX_CLK,
	output       HPS_ENET_MDC,
	inout        HPS_ENET_MDIO,
	input        HPS_ENET_RX_CLK,
	input [3:0]  HPS_ENET_RX_DATA,
	input        HPS_ENET_RX_DV,
	output [3:0] HPS_ENET_TX_DATA,
	output       HPS_ENET_TX_EN,
	inout        HPS_ENET_INT_N,

	// HPS SPI. HPS pins wired to the LTC 2x7 connector
	output HPS_SPIM_CLK,
	input  HPS_SPIM_MISO,
	output HPS_SPIM_MOSI,
	inout  HPS_SPIM_SS,

	// HPS GPIO. HPS pin wired to the LTC 2x7 connector (note, this cannot be used because the 0ohm resistor is not populated)
	inout HPS_LTC_GPIO,
	
	// HPS I2C 1. HPS pins wired to the LTC 2x7 connector
	inout HPS_I2C1_SCLK,
	inout HPS_I2C1_SDAT,
	
	// HPS I2C 0. HPS pins wired to the ADXL345 accelerometer
	inout HPS_I2C0_SCLK,
	inout HPS_I2C0_SDAT,
	inout HPS_GSENSOR_INT  // ADXL345 interrupt 1 output (pin 9)
);
	// ================
	// HPS (SoC) module
	// ================

	// PLL clock and reset
	wire pll_0_clock;
	wire pll_0_locked;
	wire hps_reset_n;
	wire master_reset_n = hps_reset_n;
	//wire master_reset_n = hps_reset_n | pll_0_locked;  // Stay in reset if pll is not locked
	
	// F2H interrupt receiver
	wire [31:0] f2h_irq0;  // IRQs: 72 to 103  (bit 0 = 72, bit1 = 73, etc)
	wire [31:0] f2h_irq1;  // IRQs: 104 to 135 (bit 0 = 104, bit1 = 105, etc)
	
	// PIO IP
	wire [31:0] pio_0_in;
	wire [31:0] pio_0_out;
	wire        pio_0_irq;
	
	// HPS (SoC) instance
	soc_system u0(
		// Clock
		.clk_clk(FPGA_CLK1_50),
		.clock_bridge_0_out_clk_clk(pll_0_clock),
		.pll_0_locked_export(pll_0_locked),

		// HPS DDR-3 SDRAM pin connections
		.memory_mem_a(HPS_DDR3_ADDR),
		.memory_mem_ba(HPS_DDR3_BA),
		.memory_mem_ck(HPS_DDR3_CK_P),
		.memory_mem_ck_n(HPS_DDR3_CK_N),
		.memory_mem_cke(HPS_DDR3_CKE),
		.memory_mem_cs_n(HPS_DDR3_CS_N),
		.memory_mem_ras_n(HPS_DDR3_RAS_N),
		.memory_mem_cas_n(HPS_DDR3_CAS_N),
		.memory_mem_we_n(HPS_DDR3_WE_N),
		.memory_mem_reset_n(HPS_DDR3_RESET_N),
		.memory_mem_dq(HPS_DDR3_DQ),
		.memory_mem_dqs(HPS_DDR3_DQS_P),
		.memory_mem_dqs_n(HPS_DDR3_DQS_N),
		.memory_mem_odt(HPS_DDR3_ODT),
		.memory_mem_dm(HPS_DDR3_DM),
		.memory_oct_rzqin(HPS_DDR3_RZQ),
		
		// HPS SD-card pin connections
		.hps_io_hps_io_sdio_inst_CMD(HPS_SD_CMD),
		.hps_io_hps_io_sdio_inst_D0(HPS_SD_DATA[0]),
		.hps_io_hps_io_sdio_inst_D1(HPS_SD_DATA[1]),
		.hps_io_hps_io_sdio_inst_CLK(HPS_SD_CLK),
		.hps_io_hps_io_sdio_inst_D2(HPS_SD_DATA[2]),
		.hps_io_hps_io_sdio_inst_D3(HPS_SD_DATA[3]),
		
		// HPS UART0 (UART-USB) pin connections
		.hps_io_hps_io_uart0_inst_RX(HPS_UART_RX),
		.hps_io_hps_io_uart0_inst_TX(HPS_UART_TX),
		
		// HPS EMAC1 (Ethernet) pin connections
		.hps_io_hps_io_emac1_inst_TX_CLK(HPS_ENET_GTX_CLK),
		.hps_io_hps_io_emac1_inst_TXD0(HPS_ENET_TX_DATA[0]),
		.hps_io_hps_io_emac1_inst_TXD1(HPS_ENET_TX_DATA[1]),
		.hps_io_hps_io_emac1_inst_TXD2(HPS_ENET_TX_DATA[2]),
		.hps_io_hps_io_emac1_inst_TXD3(HPS_ENET_TX_DATA[3]),
		.hps_io_hps_io_emac1_inst_RXD0(HPS_ENET_RX_DATA[0]),
		.hps_io_hps_io_emac1_inst_MDIO(HPS_ENET_MDIO),
		.hps_io_hps_io_emac1_inst_MDC(HPS_ENET_MDC),
		.hps_io_hps_io_emac1_inst_RX_CTL(HPS_ENET_RX_DV),
		.hps_io_hps_io_emac1_inst_TX_CTL(HPS_ENET_TX_EN),
		.hps_io_hps_io_emac1_inst_RX_CLK(HPS_ENET_RX_CLK),
		.hps_io_hps_io_emac1_inst_RXD1(HPS_ENET_RX_DATA[1]),
		.hps_io_hps_io_emac1_inst_RXD2(HPS_ENET_RX_DATA[2]),
		.hps_io_hps_io_emac1_inst_RXD3(HPS_ENET_RX_DATA[3]),

		// HPS USB1 2.0 OTG pin connections
		.hps_io_hps_io_usb1_inst_D0(HPS_USB_DATA[0]),
		.hps_io_hps_io_usb1_inst_D1(HPS_USB_DATA[1]),
		.hps_io_hps_io_usb1_inst_D2(HPS_USB_DATA[2]),
		.hps_io_hps_io_usb1_inst_D3(HPS_USB_DATA[3]),
		.hps_io_hps_io_usb1_inst_D4(HPS_USB_DATA[4]),
		.hps_io_hps_io_usb1_inst_D5(HPS_USB_DATA[5]),
		.hps_io_hps_io_usb1_inst_D6(HPS_USB_DATA[6]),
		.hps_io_hps_io_usb1_inst_D7(HPS_USB_DATA[7]),
		.hps_io_hps_io_usb1_inst_CLK(HPS_USB_CLKOUT),
		.hps_io_hps_io_usb1_inst_STP(HPS_USB_STP),
		.hps_io_hps_io_usb1_inst_DIR(HPS_USB_DIR),
		.hps_io_hps_io_usb1_inst_NXT(HPS_USB_NXT),
		
		// HPS SPI1 pin connections
		.hps_io_hps_io_spim1_inst_CLK(HPS_SPIM_CLK),
		.hps_io_hps_io_spim1_inst_MOSI(HPS_SPIM_MOSI),
		.hps_io_hps_io_spim1_inst_MISO(HPS_SPIM_MISO),
		.hps_io_hps_io_spim1_inst_SS0(HPS_SPIM_SS),
		
		// HPS I2C0 pin connections
		.hps_io_hps_io_i2c0_inst_SDA(HPS_I2C0_SDAT),
		.hps_io_hps_io_i2c0_inst_SCL(HPS_I2C0_SCLK),
		
		// HPS I2C1 pin connections
		.hps_io_hps_io_i2c1_inst_SDA(HPS_I2C1_SDAT),
		.hps_io_hps_io_i2c1_inst_SCL(HPS_I2C1_SCLK),
		
		// HPS GPIO pin connections
		.hps_io_hps_io_gpio_inst_GPIO09(HPS_CONV_USB_N),
		.hps_io_hps_io_gpio_inst_GPIO35(HPS_ENET_INT_N),
		.hps_io_hps_io_gpio_inst_GPIO40(HPS_LTC_GPIO),
		.hps_io_hps_io_gpio_inst_GPIO53(HPS_LED),
		.hps_io_hps_io_gpio_inst_GPIO54(HPS_KEY),
		.hps_io_hps_io_gpio_inst_GPIO61(HPS_GSENSOR_INT),
		
		// F2H interrupt receiver
		.hps_0_f2h_irq0_irq(f2h_irq0),  // IRQs 72 to 103
		.hps_0_f2h_irq1_irq(f2h_irq1),  // IRQs 104 to 135
		
		// PIO IP mapped to L2F memory region
		.pio_0_external_connection_in_port(pio_0_in),
		.pio_0_external_connection_out_port(pio_0_out),
		.pio_0_irq_irq(pio_0_irq),
		
		// Reset
		.hps_0_h2f_reset_reset_n(hps_reset_n),
		.reset_reset_n(hps_reset_n)
	);
	
	// ===================
	// Debounce input keys
	// ===================
	
	wire debounced_key0;
	wire debounced_key1;
	debounce #(
		.CLK_CNT_WIDTH(21),
		.SW_WIDTH(2)
	)
	debounce_0(
		.rst_n(master_reset_n),
		.clk(FPGA_CLK1_50),
		.div(1250000),
		.sw_in({ ~KEY[1], ~KEY[0] }),
		.sw_out({ debounced_key1, debounced_key0 })
	);
	
	// ====================================
	// Configure logic between FPGA and HPS
	// ====================================

	// Create friendly names
	wire [1:0] fpga_keys = { debounced_key1, debounced_key0 };  // Represents the FPGA input tactile keys
	wire [7:0] fpga_leds = LED[7:0];                            // Represents the FPGA output LEDs
	
	// Configure logic for interrupt receiver
	assign f2h_irq0 = { {31{1'b0}}, pio_0_irq };  // Wire PIO output to IRQ ID 72.  Remaining IDs 73 to 103 are unused
	assign f2h_irq1 = 0;                          // IRQ IDs 104 to 135 are unused
	
	// Configure logic for keys and LEDs
	assign pio_0_in[1:0]  = fpga_keys;       // Wire PIO inputs to FPGA input keys
	assign fpga_leds[7:0] = pio_0_out[7:0];  // Wire PIO outputs to FPGA output LEDs
endmodule
