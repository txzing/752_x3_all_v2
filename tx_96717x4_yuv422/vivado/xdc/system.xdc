set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

set_property PACKAGE_PIN AG13 [get_ports uart_0_rxd]
set_property IOSTANDARD LVCMOS33 [get_ports uart_0_rxd]
set_property PACKAGE_PIN W11 [get_ports uart_0_txd]
set_property IOSTANDARD LVCMOS33 [get_ports uart_0_txd]
set_property PACKAGE_PIN AH13 [get_ports {uart_0_txen[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {uart_0_txen[0]}]

# MAX96717_0
## sda
set_property PACKAGE_PIN G3 [get_ports i2c_0_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_0_sda_io]
## scl
set_property PACKAGE_PIN F3 [get_ports i2c_0_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_0_scl_io]

# MAX96717_1
## sda
set_property PACKAGE_PIN D9 [get_ports i2c_1_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_sda_io]
# scl
set_property PACKAGE_PIN E9 [get_ports i2c_1_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_scl_io]


# MAX96717_2
set_property PACKAGE_PIN E4 [get_ports i2c_2_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_sda_io]
## scl
set_property PACKAGE_PIN E3 [get_ports i2c_2_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_scl_io]

# MAX96717_3
set_property PACKAGE_PIN C4 [get_ports i2c_3_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_sda_io]
## scl
set_property PACKAGE_PIN D4 [get_ports i2c_3_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_scl_io]

# ADC
set_property PACKAGE_PIN AC11 [get_ports i2c_4_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports i2c_4_sda_io]
## scl
set_property PACKAGE_PIN AB11 [get_ports i2c_4_scl_io]
set_property IOSTANDARD LVCMOS33 [get_ports i2c_4_scl_io]

# eeprom
## eeprom sda
set_property PACKAGE_PIN W14 [get_ports eeprom_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports eeprom_sda_io]
## eeprom scl
set_property PACKAGE_PIN AB15 [get_ports eeprom_scl_io]
set_property IOSTANDARD LVCMOS33 [get_ports eeprom_scl_io]

# FPGA reset
set_property PACKAGE_PIN AG11 [get_ports {gpio_tri_io[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[0]}]

#PL_LOCK_LED
set_property PACKAGE_PIN AF11 [get_ports {gpio_tri_io[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[1]}]

## eeprom wp: (lo: wr; hi: ro)
set_property PACKAGE_PIN AB14 [get_ports {gpio_tri_io[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[2]}]

# ADC ALERT
set_property PACKAGE_PIN AG14 [get_ports {gpio_tri_io[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[3]}]

# NULL 
set_property PACKAGE_PIN AB9 [get_ports {gpio_tri_io[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[4]}]

#gpio for maxim chips

#0_717_LOCK
set_property PACKAGE_PIN F2 [get_ports {gpio_tri_io[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[5]}]
#0_717_MFP8
set_property PACKAGE_PIN F5 [get_ports {gpio_tri_io[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[6]}]
#0_717_PWDNB
set_property PACKAGE_PIN G8 [get_ports {gpio_tri_io[7]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[7]}]
#0_717_PCLK
set_property PACKAGE_PIN F7 [get_ports {gpio_tri_io[8]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[8]}]

#1_717_LOCK
set_property PACKAGE_PIN D5 [get_ports {gpio_tri_io[9]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[9]}]
#1_717_MFP8
set_property PACKAGE_PIN B5 [get_ports {gpio_tri_io[10]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[10]}]
#1_717_PWDNB
set_property PACKAGE_PIN C9 [get_ports {gpio_tri_io[11]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[11]}]
#1_717_PCLK
set_property PACKAGE_PIN B9 [get_ports {gpio_tri_io[12]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[12]}]

#2_717_LOCK
set_property PACKAGE_PIN A3 [get_ports {gpio_tri_io[13]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[13]}]
#2_717_MFP8
set_property PACKAGE_PIN B1 [get_ports {gpio_tri_io[14]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[14]}]
#2_717_PWDNB
set_property PACKAGE_PIN B6 [get_ports {gpio_tri_io[15]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[15]}]
#2_717_PCLK
set_property PACKAGE_PIN C6 [get_ports {gpio_tri_io[16]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[16]}]

#3_717_LOCK
set_property PACKAGE_PIN E8 [get_ports {gpio_tri_io[17]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[17]}]
#3_717_MFP8
set_property PACKAGE_PIN A1 [get_ports {gpio_tri_io[18]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[18]}]
#3_717_PWDNB
set_property PACKAGE_PIN D7 [get_ports {gpio_tri_io[19]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[19]}]
#3_717_PCLK
set_property PACKAGE_PIN D6 [get_ports {gpio_tri_io[20]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[20]}]
