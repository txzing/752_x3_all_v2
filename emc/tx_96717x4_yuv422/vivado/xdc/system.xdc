set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

# eeprom
## eeprom sda
set_property -dict {PACKAGE_PIN AF10 IOSTANDARD LVCMOS33} [get_ports eeprom_sda_io]
## eeprom scl
set_property -dict {PACKAGE_PIN Y9 IOSTANDARD LVCMOS33} [get_ports eeprom_scl_io]

# MAX96717F_0
## sda
set_property PACKAGE_PIN N6 [get_ports i2c_0_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_0_sda_io]
## scl
set_property PACKAGE_PIN P7 [get_ports i2c_0_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_0_scl_io]

# MAX96717F_1
## sda
set_property PACKAGE_PIN H3 [get_ports i2c_1_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_sda_io]
# scl
set_property PACKAGE_PIN H4 [get_ports i2c_1_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_scl_io]


# MAX96717F_2
set_property PACKAGE_PIN L3 [get_ports i2c_2_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_sda_io]
## scl
set_property PACKAGE_PIN H9 [get_ports i2c_2_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_scl_io]

# MAX96717F_3
set_property PACKAGE_PIN U8 [get_ports i2c_3_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_sda_io]
## scl
set_property PACKAGE_PIN V8 [get_ports i2c_3_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_scl_io]

# ADC
set_property PACKAGE_PIN AE14 [get_ports i2c_4_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_4_sda_io]
## scl
set_property PACKAGE_PIN AE15 [get_ports i2c_4_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_4_scl_io]


# relay_ctrl
set_property -dict {PACKAGE_PIN AF11 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[0]}]

# EEPROM WRITE PROTECT
set_property -dict {PACKAGE_PIN AA8 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[1]}]

#gpio for maxim chips

#0_717_LOCK
set_property -dict {PACKAGE_PIN R8 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[2]}]
#0_717_MFP8
set_property -dict {PACKAGE_PIN L1 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[3]}]
#0_717_PWDNB
set_property -dict {PACKAGE_PIN K1 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[4]}]
#0_717_PCLK
set_property -dict {PACKAGE_PIN N7 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[5]}]

#1_717_LOCK
set_property -dict {PACKAGE_PIN M6 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[6]}]
#1_717_MFP8
set_property -dict {PACKAGE_PIN R7 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[7]}]
#1_717_PWDNB
set_property -dict {PACKAGE_PIN K3 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[8]}]
#1_717_PCLK
set_property -dict {PACKAGE_PIN K4 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[9]}]

#2_717_LOCK
set_property -dict {PACKAGE_PIN K9 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[10]}]
#2_717_MFP8
set_property -dict {PACKAGE_PIN L7 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[11]}]
#2_717_PWDNB
set_property -dict {PACKAGE_PIN L6 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[12]}]
#2_717_PCLK
set_property -dict {PACKAGE_PIN L2 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[13]}]

#3_717_LOCK
set_property -dict {PACKAGE_PIN H1 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[14]}]
#3_717_MFP8
set_property -dict {PACKAGE_PIN M8 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[15]}]
#3_717_PWDNB
set_property -dict {PACKAGE_PIN J4 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[16]}]
#3_717_PCLK
set_property -dict {PACKAGE_PIN J5 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[17]}]

#SFP_TX_DIS
set_property -dict {PACKAGE_PIN AE12 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[18]}]

#SFP_TX_LOSS
set_property -dict {PACKAGE_PIN AF12 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[19]}]