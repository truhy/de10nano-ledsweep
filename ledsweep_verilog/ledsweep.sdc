#**************************************************************
# This .sdc file is created by Terasic Tool.
# Users are recommended to modify this file to match users logic.
#**************************************************************

#**************************************************************
# Create Clock
#**************************************************************
create_clock -period "50.0 MHz" [get_ports FPGA_CLK1_50]
create_clock -period "50.0 MHz" [get_ports FPGA_CLK2_50]
create_clock -period "50.0 MHz" [get_ports FPGA_CLK3_50]

# for enhancing USB BlasterII to be reliable, 25MHz
create_clock -name {altera_reserved_tck} -period 40 {altera_reserved_tck}
set_input_delay -clock altera_reserved_tck -clock_fall 3 [get_ports altera_reserved_tdi]
set_input_delay -clock altera_reserved_tck -clock_fall 3 [get_ports altera_reserved_tms]
set_output_delay -clock altera_reserved_tck 3 [get_ports altera_reserved_tdo]

#**************************************************************
# Create Generated Clock
#**************************************************************
derive_pll_clocks



#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************
derive_clock_uncertainty



#**************************************************************
# Set Input Delay
#**************************************************************



#**************************************************************
# Set Output Delay
#**************************************************************



#**************************************************************
# Set Clock Groups
#**************************************************************



#**************************************************************
# Set False Path
#**************************************************************

# FPGA IO port constraints
# Inputs
set_false_path -from [get_ports {ADC_SDO}] -to *
set_false_path -from [get_ports {ARDUINO_IO[0]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[1]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[2]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[3]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[4]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[5]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[6]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[7]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[8]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[9]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[10]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[11]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[12]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[13]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[14]}] -to *
set_false_path -from [get_ports {ARDUINO_IO[15]}] -to *
set_false_path -from [get_ports {ARDUINO_RESET_N}] -to *
set_false_path -from [get_ports {GPIO_0[0]}] -to *
set_false_path -from [get_ports {GPIO_0[1]}] -to *
set_false_path -from [get_ports {GPIO_0[2]}] -to *
set_false_path -from [get_ports {GPIO_0[3]}] -to *
set_false_path -from [get_ports {GPIO_0[4]}] -to *
set_false_path -from [get_ports {GPIO_0[5]}] -to *
set_false_path -from [get_ports {GPIO_0[6]}] -to *
set_false_path -from [get_ports {GPIO_0[7]}] -to *
set_false_path -from [get_ports {GPIO_0[8]}] -to *
set_false_path -from [get_ports {GPIO_0[9]}] -to *
set_false_path -from [get_ports {GPIO_0[10]}] -to *
set_false_path -from [get_ports {GPIO_0[11]}] -to *
set_false_path -from [get_ports {GPIO_0[12]}] -to *
set_false_path -from [get_ports {GPIO_0[13]}] -to *
set_false_path -from [get_ports {GPIO_0[14]}] -to *
set_false_path -from [get_ports {GPIO_0[15]}] -to *
set_false_path -from [get_ports {GPIO_0[16]}] -to *
set_false_path -from [get_ports {GPIO_0[17]}] -to *
set_false_path -from [get_ports {GPIO_0[18]}] -to *
set_false_path -from [get_ports {GPIO_0[19]}] -to *
set_false_path -from [get_ports {GPIO_0[20]}] -to *
set_false_path -from [get_ports {GPIO_0[21]}] -to *
set_false_path -from [get_ports {GPIO_0[22]}] -to *
set_false_path -from [get_ports {GPIO_0[23]}] -to *
set_false_path -from [get_ports {GPIO_0[24]}] -to *
set_false_path -from [get_ports {GPIO_0[25]}] -to *
set_false_path -from [get_ports {GPIO_0[26]}] -to *
set_false_path -from [get_ports {GPIO_0[27]}] -to *
set_false_path -from [get_ports {GPIO_0[28]}] -to *
set_false_path -from [get_ports {GPIO_0[29]}] -to *
set_false_path -from [get_ports {GPIO_0[30]}] -to *
set_false_path -from [get_ports {GPIO_0[31]}] -to *
set_false_path -from [get_ports {GPIO_0[32]}] -to *
set_false_path -from [get_ports {GPIO_0[33]}] -to *
set_false_path -from [get_ports {GPIO_0[34]}] -to *
set_false_path -from [get_ports {GPIO_0[35]}] -to *
set_false_path -from [get_ports {GPIO_1[0]}] -to *
set_false_path -from [get_ports {GPIO_1[1]}] -to *
set_false_path -from [get_ports {GPIO_1[2]}] -to *
set_false_path -from [get_ports {GPIO_1[3]}] -to *
set_false_path -from [get_ports {GPIO_1[4]}] -to *
set_false_path -from [get_ports {GPIO_1[5]}] -to *
set_false_path -from [get_ports {GPIO_1[6]}] -to *
set_false_path -from [get_ports {GPIO_1[7]}] -to *
set_false_path -from [get_ports {GPIO_1[8]}] -to *
set_false_path -from [get_ports {GPIO_1[9]}] -to *
set_false_path -from [get_ports {GPIO_1[10]}] -to *
set_false_path -from [get_ports {GPIO_1[11]}] -to *
set_false_path -from [get_ports {GPIO_1[12]}] -to *
set_false_path -from [get_ports {GPIO_1[13]}] -to *
set_false_path -from [get_ports {GPIO_1[14]}] -to *
set_false_path -from [get_ports {GPIO_1[15]}] -to *
set_false_path -from [get_ports {GPIO_1[16]}] -to *
set_false_path -from [get_ports {GPIO_1[17]}] -to *
set_false_path -from [get_ports {GPIO_1[18]}] -to *
set_false_path -from [get_ports {GPIO_1[19]}] -to *
set_false_path -from [get_ports {GPIO_1[20]}] -to *
set_false_path -from [get_ports {GPIO_1[21]}] -to *
set_false_path -from [get_ports {GPIO_1[22]}] -to *
set_false_path -from [get_ports {GPIO_1[23]}] -to *
set_false_path -from [get_ports {GPIO_1[24]}] -to *
set_false_path -from [get_ports {GPIO_1[25]}] -to *
set_false_path -from [get_ports {GPIO_1[26]}] -to *
set_false_path -from [get_ports {GPIO_1[27]}] -to *
set_false_path -from [get_ports {GPIO_1[28]}] -to *
set_false_path -from [get_ports {GPIO_1[29]}] -to *
set_false_path -from [get_ports {GPIO_1[30]}] -to *
set_false_path -from [get_ports {GPIO_1[31]}] -to *
set_false_path -from [get_ports {GPIO_1[32]}] -to *
set_false_path -from [get_ports {GPIO_1[33]}] -to *
set_false_path -from [get_ports {GPIO_1[34]}] -to *
set_false_path -from [get_ports {GPIO_1[35]}] -to *
set_false_path -from [get_ports {HDMI_I2C_SCL}] -to *
set_false_path -from [get_ports {HDMI_I2C_SDA}] -to *
set_false_path -from [get_ports {HDMI_I2S}] -to *
set_false_path -from [get_ports {HDMI_LRCLK}] -to *
set_false_path -from [get_ports {HDMI_MCLK}] -to *
set_false_path -from [get_ports {HDMI_SCLK}] -to *
set_false_path -from [get_ports {HDMI_TX_INT}] -to *
set_false_path -from [get_ports {KEY[0]}] -to *
set_false_path -from [get_ports {KEY[1]}] -to *
set_false_path -from [get_ports {SW[0]}] -to *
set_false_path -from [get_ports {SW[1]}] -to *
set_false_path -from [get_ports {SW[2]}] -to *
set_false_path -from [get_ports {SW[3]}] -to *
# Outputs
set_false_path -from * -to [get_ports {ADC_CONVST}] 
set_false_path -from * -to [get_ports {ADC_SCK}] 
set_false_path -from * -to [get_ports {ADC_SDI}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[0]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[1]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[2]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[3]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[4]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[5]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[6]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[7]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[8]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[9]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[10]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[11]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[12]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[13]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[14]}] 
set_false_path -from * -to [get_ports {ARDUINO_IO[15]}] 
set_false_path -from * -to [get_ports {ARDUINO_RESET_N}] 
set_false_path -from * -to [get_ports {GPIO_0[0]}] 
set_false_path -from * -to [get_ports {GPIO_0[1]}] 
set_false_path -from * -to [get_ports {GPIO_0[2]}] 
set_false_path -from * -to [get_ports {GPIO_0[3]}] 
set_false_path -from * -to [get_ports {GPIO_0[4]}] 
set_false_path -from * -to [get_ports {GPIO_0[5]}] 
set_false_path -from * -to [get_ports {GPIO_0[6]}] 
set_false_path -from * -to [get_ports {GPIO_0[7]}] 
set_false_path -from * -to [get_ports {GPIO_0[8]}] 
set_false_path -from * -to [get_ports {GPIO_0[9]}] 
set_false_path -from * -to [get_ports {GPIO_0[10]}] 
set_false_path -from * -to [get_ports {GPIO_0[11]}] 
set_false_path -from * -to [get_ports {GPIO_0[12]}] 
set_false_path -from * -to [get_ports {GPIO_0[13]}] 
set_false_path -from * -to [get_ports {GPIO_0[14]}] 
set_false_path -from * -to [get_ports {GPIO_0[15]}] 
set_false_path -from * -to [get_ports {GPIO_0[16]}] 
set_false_path -from * -to [get_ports {GPIO_0[17]}] 
set_false_path -from * -to [get_ports {GPIO_0[18]}] 
set_false_path -from * -to [get_ports {GPIO_0[19]}] 
set_false_path -from * -to [get_ports {GPIO_0[20]}] 
set_false_path -from * -to [get_ports {GPIO_0[21]}] 
set_false_path -from * -to [get_ports {GPIO_0[22]}] 
set_false_path -from * -to [get_ports {GPIO_0[23]}] 
set_false_path -from * -to [get_ports {GPIO_0[24]}] 
set_false_path -from * -to [get_ports {GPIO_0[25]}] 
set_false_path -from * -to [get_ports {GPIO_0[26]}] 
set_false_path -from * -to [get_ports {GPIO_0[27]}] 
set_false_path -from * -to [get_ports {GPIO_0[28]}] 
set_false_path -from * -to [get_ports {GPIO_0[29]}] 
set_false_path -from * -to [get_ports {GPIO_0[30]}] 
set_false_path -from * -to [get_ports {GPIO_0[31]}] 
set_false_path -from * -to [get_ports {GPIO_0[32]}] 
set_false_path -from * -to [get_ports {GPIO_0[33]}] 
set_false_path -from * -to [get_ports {GPIO_0[34]}] 
set_false_path -from * -to [get_ports {GPIO_0[35]}] 
set_false_path -from * -to [get_ports {GPIO_1[0]}] 
set_false_path -from * -to [get_ports {GPIO_1[1]}] 
set_false_path -from * -to [get_ports {GPIO_1[2]}] 
set_false_path -from * -to [get_ports {GPIO_1[3]}] 
set_false_path -from * -to [get_ports {GPIO_1[4]}] 
set_false_path -from * -to [get_ports {GPIO_1[5]}] 
set_false_path -from * -to [get_ports {GPIO_1[6]}] 
set_false_path -from * -to [get_ports {GPIO_1[7]}] 
set_false_path -from * -to [get_ports {GPIO_1[8]}] 
set_false_path -from * -to [get_ports {GPIO_1[9]}] 
set_false_path -from * -to [get_ports {GPIO_1[10]}] 
set_false_path -from * -to [get_ports {GPIO_1[11]}] 
set_false_path -from * -to [get_ports {GPIO_1[12]}] 
set_false_path -from * -to [get_ports {GPIO_1[13]}] 
set_false_path -from * -to [get_ports {GPIO_1[14]}] 
set_false_path -from * -to [get_ports {GPIO_1[15]}] 
set_false_path -from * -to [get_ports {GPIO_1[16]}] 
set_false_path -from * -to [get_ports {GPIO_1[17]}] 
set_false_path -from * -to [get_ports {GPIO_1[18]}] 
set_false_path -from * -to [get_ports {GPIO_1[19]}] 
set_false_path -from * -to [get_ports {GPIO_1[20]}] 
set_false_path -from * -to [get_ports {GPIO_1[21]}] 
set_false_path -from * -to [get_ports {GPIO_1[22]}] 
set_false_path -from * -to [get_ports {GPIO_1[23]}] 
set_false_path -from * -to [get_ports {GPIO_1[24]}] 
set_false_path -from * -to [get_ports {GPIO_1[25]}] 
set_false_path -from * -to [get_ports {GPIO_1[26]}] 
set_false_path -from * -to [get_ports {GPIO_1[27]}] 
set_false_path -from * -to [get_ports {GPIO_1[28]}] 
set_false_path -from * -to [get_ports {GPIO_1[29]}] 
set_false_path -from * -to [get_ports {GPIO_1[30]}] 
set_false_path -from * -to [get_ports {GPIO_1[31]}] 
set_false_path -from * -to [get_ports {GPIO_1[32]}] 
set_false_path -from * -to [get_ports {GPIO_1[33]}] 
set_false_path -from * -to [get_ports {GPIO_1[34]}] 
set_false_path -from * -to [get_ports {GPIO_1[35]}]
set_false_path -from * -to [get_ports {HDMI_I2C_SCL}] 
set_false_path -from * -to [get_ports {HDMI_I2C_SDA}] 
set_false_path -from * -to [get_ports {HDMI_I2S}] 
set_false_path -from * -to [get_ports {HDMI_LRCLK}] 
set_false_path -from * -to [get_ports {HDMI_MCLK}] 
set_false_path -from * -to [get_ports {HDMI_SCLK}] 
set_false_path -from * -to [get_ports {HDMI_TX_CLK}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[0]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[1]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[2]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[3]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[4]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[5]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[6]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[7]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[8]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[9]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[10]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[11]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[12]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[13]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[14]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[15]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[16]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[17]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[18]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[19]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[20]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[21]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[22]}] 
set_false_path -from * -to [get_ports {HDMI_TX_D[23]}] 
set_false_path -from * -to [get_ports {HDMI_TX_DE}] 
set_false_path -from * -to [get_ports {HDMI_TX_HS}] 
set_false_path -from * -to [get_ports {HDMI_TX_VS}] 
set_false_path -from * -to [get_ports {LED[0]}]
set_false_path -from * -to [get_ports {LED[1]}]
set_false_path -from * -to [get_ports {LED[2]}]
set_false_path -from * -to [get_ports {LED[3]}]
set_false_path -from * -to [get_ports {LED[4]}]
set_false_path -from * -to [get_ports {LED[5]}]
set_false_path -from * -to [get_ports {LED[6]}]
set_false_path -from * -to [get_ports {LED[7]}]

# HPS peripherals port false path setting to workaround the unconstraint path (setting false_path for hps_0 ports will not affect the routing as it is hard silicon)
# Inputs
set_false_path -from [get_ports {HPS_KEY}] -to *
set_false_path -from [get_ports {HPS_LED}] -to *
set_false_path -from [get_ports {HPS_ENET_INT_N}] -to *
set_false_path -from [get_ports {HPS_ENET_MDIO}] -to *
set_false_path -from [get_ports {HPS_ENET_RX_DV}] -to *
set_false_path -from [get_ports {HPS_ENET_RX_CLK}] -to *
set_false_path -from [get_ports {HPS_ENET_RX_DATA[0]}] -to *
set_false_path -from [get_ports {HPS_ENET_RX_DATA[1]}] -to *
set_false_path -from [get_ports {HPS_ENET_RX_DATA[2]}] -to *
set_false_path -from [get_ports {HPS_ENET_RX_DATA[3]}] -to *
set_false_path -from [get_ports {HPS_SD_CMD}] -to *
set_false_path -from [get_ports {HPS_SD_DATA[0]}] -to *
set_false_path -from [get_ports {HPS_SD_DATA[1]}] -to *
set_false_path -from [get_ports {HPS_SD_DATA[2]}] -to *
set_false_path -from [get_ports {HPS_SD_DATA[3]}] -to *
set_false_path -from [get_ports {HPS_SPIM_MISO}] -to *
set_false_path -from [get_ports {HPS_UART_RX}] -to *
set_false_path -from [get_ports {HPS_CONV_USB_N}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[0]}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[1]}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[2]}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[3]}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[4]}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[5]}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[6]}] -to *
set_false_path -from [get_ports {HPS_USB_DATA[7]}] -to *
set_false_path -from [get_ports {HPS_USB_CLKOUT}] -to *
set_false_path -from [get_ports {HPS_USB_DIR}] -to *
set_false_path -from [get_ports {HPS_USB_NXT}] -to *
set_false_path -from [get_ports {HPS_I2C0_SCLK}] -to *
set_false_path -from [get_ports {HPS_I2C0_SDAT}] -to *
set_false_path -from [get_ports {HPS_GSENSOR_INT}] -to *
set_false_path -from [get_ports {HPS_I2C1_SCLK}] -to *
set_false_path -from [get_ports {HPS_I2C1_SDAT}] -to *
# Outputs
set_false_path -from * -to [get_ports {HPS_KEY}] 
set_false_path -from * -to [get_ports {HPS_LED}] 
set_false_path -from * -to [get_ports {HPS_ENET_INT_N}] 
set_false_path -from * -to [get_ports {HPS_ENET_MDIO}] 
set_false_path -from * -to [get_ports {HPS_ENET_MDC}] 
set_false_path -from * -to [get_ports {HPS_ENET_GTX_CLK}] 
set_false_path -from * -to [get_ports {HPS_ENET_TX_DATA[0]}] 
set_false_path -from * -to [get_ports {HPS_ENET_TX_DATA[1]}] 
set_false_path -from * -to [get_ports {HPS_ENET_TX_DATA[2]}] 
set_false_path -from * -to [get_ports {HPS_ENET_TX_DATA[3]}] 
set_false_path -from * -to [get_ports {HPS_ENET_TX_EN}] 
set_false_path -from * -to [get_ports {HPS_SD_CMD}] 
set_false_path -from * -to [get_ports {HPS_SD_DATA[0]}] 
set_false_path -from * -to [get_ports {HPS_SD_DATA[1]}] 
set_false_path -from * -to [get_ports {HPS_SD_DATA[2]}] 
set_false_path -from * -to [get_ports {HPS_SD_DATA[3]}] 
set_false_path -from * -to [get_ports {HPS_SD_CLK}] 
set_false_path -from * -to [get_ports {HPS_SPIM_CLK}] 
set_false_path -from * -to [get_ports {HPS_SPIM_MOSI}] 
set_false_path -from * -to [get_ports {HPS_SPIM_SS}] 
set_false_path -from * -to [get_ports {HPS_UART_TX}]
set_false_path -from * -to [get_ports {HPS_USB_DATA[0]}] 
set_false_path -from * -to [get_ports {HPS_USB_DATA[1]}] 
set_false_path -from * -to [get_ports {HPS_USB_DATA[2]}] 
set_false_path -from * -to [get_ports {HPS_USB_DATA[3]}] 
set_false_path -from * -to [get_ports {HPS_USB_DATA[4]}] 
set_false_path -from * -to [get_ports {HPS_USB_DATA[5]}] 
set_false_path -from * -to [get_ports {HPS_USB_DATA[6]}] 
set_false_path -from * -to [get_ports {HPS_USB_DATA[7]}] 
set_false_path -from * -to [get_ports {HPS_USB_STP}] 
set_false_path -from * -to [get_ports {HPS_I2C0_SCLK}] 
set_false_path -from * -to [get_ports {HPS_I2C0_SDAT}] 
set_false_path -from * -to [get_ports {HPS_GSENSOR_INT}] 
set_false_path -from * -to [get_ports {HPS_I2C1_SCLK}] 
set_false_path -from * -to [get_ports {HPS_I2C1_SDAT}] 

# create unused clock constraint to avoid misleading unconstraint clock reporting in TimeQuest
create_clock -period "1 MHz" [get_ports HPS_I2C0_SCLK]
create_clock -period "1 MHz" [get_ports HPS_I2C1_SCLK]
create_clock -period "48 MHz" [get_ports HPS_USB_CLKOUT]

#**************************************************************
# Set Multicycle Path
#**************************************************************



#**************************************************************
# Set Maximum Delay
#**************************************************************



#**************************************************************
# Set Minimum Delay
#**************************************************************



#**************************************************************
# Set Input Transition
#**************************************************************



#**************************************************************
# Set Load
#**************************************************************



