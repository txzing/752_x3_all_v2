set_property CFGBVS VCCO [current_design]
#set_property CFGBVS GND [current_design]

set_property CONFIG_VOLTAGE 3.3 [current_design]
#set_property CONFIG_VOLTAGE 2.5 [current_design]
#set_property CONFIG_VOLTAGE 1.8 [current_design]

set_property BITSTREAM.CONFIG.UNUSEDPIN Pullnone [current_design]
#set_property BITSTREAM.CONFIG.UNUSEDPIN Pulldown [current_design]
#set_property BITSTREAM.CONFIG.UNUSEDPIN Pullup [current_design]

set_property BITSTREAM.GENERAL.COMPRESS true [current_design]

set_property CONFIG_MODE SPIx4 [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 50 [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]
set_property BITSTREAM.CONFIG.SPI_FALL_EDGE YES [current_design]
set_property BITSTREAM.CONFIG.SPI_32BIT_ADDR YES [current_design]
#set_property BITSTREAM.CONFIG.TIMER_CFG 32'h01FFFFFF [current_design]
#set_property BITSTREAM.CONFIG.TIMER_CFG 0x01FFFFFF [current_design]
set_property BITSTREAM.CONFIG.CONFIGFALLBACK ENABLE [current_design]


set_property -dict {PACKAGE_PIN AK17 IOSTANDARD DIFF_SSTL12} [get_ports sys_clk_p]

#set_property -dict {PACKAGE_PIN U7   IOSTANDARD LVCMOS33} [get_ports {qspi_flash_ss_io[0]}]
#set_property -dict {PACKAGE_PIN AC7  IOSTANDARD LVCMOS33} [get_ports {qspi_flash_io0_io}]
#set_property -dict {PACKAGE_PIN AB7  IOSTANDARD LVCMOS33} [get_ports {qspi_flash_io1_io}]
#set_property -dict {PACKAGE_PIN AA7  IOSTANDARD LVCMOS33} [get_ports {qspi_flash_io2_io}]
#set_property -dict {PACKAGE_PIN Y7   IOSTANDARD LVCMOS33} [get_ports {qspi_flash_io3_io}]

#rs232
set_property -dict {PACKAGE_PIN P26 IOSTANDARD LVCMOS33} [get_ports uart_0_rxd]
set_property -dict {PACKAGE_PIN R25 IOSTANDARD LVCMOS33} [get_ports uart_0_txd]
set_property UNAVAILABLE_DURING_CALIBRATION true [get_ports uart_0_rxd]


# EEPROM
## sda
set_property -dict {PACKAGE_PIN M22 IOSTANDARD LVCMOS33} [get_ports i2c_0_sda_io]
set_property UNAVAILABLE_DURING_CALIBRATION true [get_ports i2c_0_sda_io]
## scl
set_property -dict {PACKAGE_PIN N22 IOSTANDARD LVCMOS33} [get_ports i2c_0_scl_io]

# 856
## sda
set_property -dict {PACKAGE_PIN AG34 IOSTANDARD LVCMOS18} [get_ports i2c_1_sda_io]
## scl
set_property -dict {PACKAGE_PIN AF33 IOSTANDARD LVCMOS18} [get_ports i2c_1_scl_io]

# 772
## sda
set_property -dict {PACKAGE_PIN Y30 IOSTANDARD LVCMOS18} [get_ports i2c_2_sda_io]
## scl
set_property -dict {PACKAGE_PIN W30 IOSTANDARD LVCMOS18} [get_ports i2c_2_scl_io]

#
## sda
set_property -dict {PACKAGE_PIN N24 IOSTANDARD LVCMOS33} [get_ports i2c_3_sda_io]
## scl
set_property -dict {PACKAGE_PIN M24 IOSTANDARD LVCMOS33} [get_ports i2c_3_scl_io]

#
## sda
set_property -dict {PACKAGE_PIN N21 IOSTANDARD LVCMOS33} [get_ports i2c_4_sda_io]
## scl
set_property -dict {PACKAGE_PIN M21 IOSTANDARD LVCMOS33} [get_ports i2c_4_scl_io]



# relay_ctrl
set_property -dict {PACKAGE_PIN R26 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[0]}]
# EEPROM WRITE PROTECT
set_property -dict {PACKAGE_PIN N26 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[1]}]

#SFP_TX_DIS
set_property -dict {PACKAGE_PIN K25 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[2]}]

#SFP_TX_LOSS
set_property -dict {PACKAGE_PIN L25 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[3]}]

#SFP_TX_DIS
set_property -dict {PACKAGE_PIN L27 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[4]}]

#SFP_TX_LOSS
set_property -dict {PACKAGE_PIN M27 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[5]}]

#862_F_DIS_REM
set_property -dict {PACKAGE_PIN AD31 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[6]}]

#862_LOCK
set_property -dict {PACKAGE_PIN AD30 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[7]}]

#862_POWERDOWN
set_property -dict {PACKAGE_PIN AE32 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[8]}]

#772_F_MS
set_property -dict {PACKAGE_PIN AC28 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[9]}]
#772_LOCK
set_property -dict {PACKAGE_PIN AE33 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[10]}]
#772_POWERDOWN
set_property -dict {PACKAGE_PIN AF27 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[11]}]




#SFP
set_property PACKAGE_PIN P6 [get_ports mgt_clk_0_clk_p]
set_property PACKAGE_PIN M2 [get_ports sfp_0_rxp]
set_property PACKAGE_PIN N4 [get_ports sfp_0_txp]

## Misc
##rx
set_property PACKAGE_PIN AF5 [get_ports mgtrefclk0_pad_n_in_0]
set_property PACKAGE_PIN AF6 [get_ports mgtrefclk0_pad_p_in_0]
set_property PACKAGE_PIN AD5 [get_ports mgtrefclk1_pad_n_in_0]
set_property PACKAGE_PIN AD6 [get_ports mgtrefclk1_pad_p_in_0]


# DisplayPort RX
# MCDP6000
set_property -dict {PACKAGE_PIN T24 IOSTANDARD LVCMOS33} [get_ports MCDP6000_IIC_0_scl_io]
set_property -dict {PACKAGE_PIN T25 IOSTANDARD LVCMOS33} [get_ports MCDP6000_IIC_0_sda_io]
set_property PACKAGE_PIN AG31 [get_ports rx_hpd_0]
set_property IOSTANDARD LVCMOS18 [get_ports rx_hpd_0]
set_property PULLDOWN true [get_ports rx_hpd_0]

#set_property -dict {PACKAGE_PIN J25  IOSTANDARD LVCMOS33} [get_ports {aux_rx_data_out_0}]
#set_property -dict {PACKAGE_PIN R22  IOSTANDARD LVCMOS33} [get_ports {aux_rx_data_in_0}]
#set_property -dict {PACKAGE_PIN J24  IOSTANDARD LVCMOS33} [get_ports {aux_rx_data_en_0}]
#set_property -dict {PACKAGE_PIN AD29  IOSTANDARD LVDS_25} [get_ports {aux_rx_io_p_0}]
#set_property -dict {PACKAGE_PIN AD29  IOSTANDARD Sub-LVDS} [get_ports {aux_rx_io_p_0}]
set_property IOSTANDARD DIFF_SSTL18_I [get_ports aux_rx_io_n_0]
set_property -dict {PACKAGE_PIN AD29 IOSTANDARD DIFF_SSTL18_I} [get_ports aux_rx_io_p_0]

set_property PACKAGE_PIN AJ3 [get_ports {lnk_rx_lane_n_0[3]}]
set_property PACKAGE_PIN AK1 [get_ports {lnk_rx_lane_n_0[2]}]
set_property PACKAGE_PIN AM1 [get_ports {lnk_rx_lane_n_0[1]}]
set_property PACKAGE_PIN AP1 [get_ports {lnk_rx_lane_n_0[0]}]



## Misc
#rx
set_property PACKAGE_PIN K5 [get_ports mgtrefclk0_pad_n_in_1]
set_property PACKAGE_PIN K6 [get_ports mgtrefclk0_pad_p_in_1]
set_property PACKAGE_PIN H5 [get_ports mgtrefclk1_pad_n_in_1]
set_property PACKAGE_PIN H6 [get_ports mgtrefclk1_pad_p_in_1]


# DisplayPort RX
# MCDP6000
set_property -dict {PACKAGE_PIN P23 IOSTANDARD LVCMOS33} [get_ports MCDP6000_IIC_1_scl_io]
set_property -dict {PACKAGE_PIN R23 IOSTANDARD LVCMOS33} [get_ports MCDP6000_IIC_1_sda_io]
set_property PACKAGE_PIN AF34 [get_ports rx_hpd_1]
set_property IOSTANDARD LVCMOS18 [get_ports rx_hpd_1]
set_property PULLDOWN true [get_ports rx_hpd_1]

#set_property -dict {PACKAGE_PIN L20  IOSTANDARD LVCMOS33} [get_ports {aux_rx_data_out_1}]
#set_property -dict {PACKAGE_PIN M20  IOSTANDARD LVCMOS33} [get_ports {aux_rx_data_in_1}]
#set_property -dict {PACKAGE_PIN R21  IOSTANDARD LVCMOS33} [get_ports {aux_rx_data_en_1}]
#set_property -dict {PACKAGE_PIN V31  IOSTANDARD LVDS_25} [get_ports {aux_rx_io_p_1}]
#set_property -dict {PACKAGE_PIN V31  IOSTANDARD Sub-LVDS} [get_ports {aux_rx_io_p_1}]
set_property IOSTANDARD DIFF_SSTL18_I [get_ports aux_rx_io_n_1]
set_property -dict {PACKAGE_PIN V31 IOSTANDARD DIFF_SSTL18_I} [get_ports aux_rx_io_p_1]

set_property PACKAGE_PIN A3 [get_ports {lnk_rx_lane_n_1[3]}]
set_property PACKAGE_PIN B1 [get_ports {lnk_rx_lane_n_1[2]}]
set_property PACKAGE_PIN D1 [get_ports {lnk_rx_lane_n_1[1]}]
set_property PACKAGE_PIN E3 [get_ports {lnk_rx_lane_n_1[0]}]

# PL_LOCK_LED
set_property -dict {PACKAGE_PIN L24 IOSTANDARD LVCMOS33} [get_ports c0_init_calib_complete]

set_property PACKAGE_PIN AG21 [get_ports {C0_DDR4_dqs_t[0]}]
set_property PACKAGE_PIN AH21 [get_ports {C0_DDR4_dqs_c[0]}]
set_property PACKAGE_PIN AH24 [get_ports {C0_DDR4_dqs_t[1]}]
set_property PACKAGE_PIN AJ25 [get_ports {C0_DDR4_dqs_c[1]}]
set_property PACKAGE_PIN AJ20 [get_ports {C0_DDR4_dqs_t[2]}]
set_property PACKAGE_PIN AK20 [get_ports {C0_DDR4_dqs_c[2]}]
set_property PACKAGE_PIN AP20 [get_ports {C0_DDR4_dqs_t[3]}]
set_property PACKAGE_PIN AP21 [get_ports {C0_DDR4_dqs_c[3]}]
set_property PACKAGE_PIN AL27 [get_ports {C0_DDR4_dqs_t[4]}]
set_property PACKAGE_PIN AL28 [get_ports {C0_DDR4_dqs_c[4]}]
set_property PACKAGE_PIN AN29 [get_ports {C0_DDR4_dqs_t[5]}]
set_property PACKAGE_PIN AP30 [get_ports {C0_DDR4_dqs_c[5]}]
set_property PACKAGE_PIN AH33 [get_ports {C0_DDR4_dqs_t[6]}]
set_property PACKAGE_PIN AJ33 [get_ports {C0_DDR4_dqs_c[6]}]
set_property PACKAGE_PIN AN34 [get_ports {C0_DDR4_dqs_t[7]}]
set_property PACKAGE_PIN AP34 [get_ports {C0_DDR4_dqs_c[7]}]
set_property PACKAGE_PIN AD21 [get_ports {C0_DDR4_dm_n[0]}]
set_property PACKAGE_PIN AE25 [get_ports {C0_DDR4_dm_n[1]}]
set_property PACKAGE_PIN AJ21 [get_ports {C0_DDR4_dm_n[2]}]
set_property PACKAGE_PIN AM21 [get_ports {C0_DDR4_dm_n[3]}]
set_property PACKAGE_PIN AH26 [get_ports {C0_DDR4_dm_n[4]}]
set_property PACKAGE_PIN AN26 [get_ports {C0_DDR4_dm_n[5]}]
set_property PACKAGE_PIN AJ29 [get_ports {C0_DDR4_dm_n[6]}]
set_property PACKAGE_PIN AL32 [get_ports {C0_DDR4_dm_n[7]}]
set_property PACKAGE_PIN AE20 [get_ports {C0_DDR4_dq[0]}]
set_property PACKAGE_PIN AG20 [get_ports {C0_DDR4_dq[1]}]
set_property PACKAGE_PIN AF20 [get_ports {C0_DDR4_dq[2]}]
set_property PACKAGE_PIN AE22 [get_ports {C0_DDR4_dq[3]}]
set_property PACKAGE_PIN AD20 [get_ports {C0_DDR4_dq[4]}]
set_property PACKAGE_PIN AG22 [get_ports {C0_DDR4_dq[5]}]
set_property PACKAGE_PIN AF22 [get_ports {C0_DDR4_dq[6]}]
set_property PACKAGE_PIN AE23 [get_ports {C0_DDR4_dq[7]}]
set_property PACKAGE_PIN AJ24 [get_ports {C0_DDR4_dq[8]}]
set_property PACKAGE_PIN AG24 [get_ports {C0_DDR4_dq[9]}]
set_property PACKAGE_PIN AJ23 [get_ports {C0_DDR4_dq[10]}]
set_property PACKAGE_PIN AF23 [get_ports {C0_DDR4_dq[11]}]
set_property PACKAGE_PIN AH23 [get_ports {C0_DDR4_dq[12]}]
set_property PACKAGE_PIN AF24 [get_ports {C0_DDR4_dq[13]}]
set_property PACKAGE_PIN AH22 [get_ports {C0_DDR4_dq[14]}]
set_property PACKAGE_PIN AG25 [get_ports {C0_DDR4_dq[15]}]
set_property PACKAGE_PIN AK22 [get_ports {C0_DDR4_dq[16]}]
set_property PACKAGE_PIN AL22 [get_ports {C0_DDR4_dq[17]}]
set_property PACKAGE_PIN AM20 [get_ports {C0_DDR4_dq[18]}]
set_property PACKAGE_PIN AL23 [get_ports {C0_DDR4_dq[19]}]
set_property PACKAGE_PIN AK23 [get_ports {C0_DDR4_dq[20]}]
set_property PACKAGE_PIN AL25 [get_ports {C0_DDR4_dq[21]}]
set_property PACKAGE_PIN AL20 [get_ports {C0_DDR4_dq[22]}]
set_property PACKAGE_PIN AL24 [get_ports {C0_DDR4_dq[23]}]
set_property PACKAGE_PIN AM24 [get_ports {C0_DDR4_dq[24]}]
set_property PACKAGE_PIN AN23 [get_ports {C0_DDR4_dq[25]}]
set_property PACKAGE_PIN AN24 [get_ports {C0_DDR4_dq[26]}]
set_property PACKAGE_PIN AP23 [get_ports {C0_DDR4_dq[27]}]
set_property PACKAGE_PIN AP25 [get_ports {C0_DDR4_dq[28]}]
set_property PACKAGE_PIN AN22 [get_ports {C0_DDR4_dq[29]}]
set_property PACKAGE_PIN AP24 [get_ports {C0_DDR4_dq[30]}]
set_property PACKAGE_PIN AM22 [get_ports {C0_DDR4_dq[31]}]
set_property PACKAGE_PIN AM26 [get_ports {C0_DDR4_dq[32]}]
set_property PACKAGE_PIN AJ28 [get_ports {C0_DDR4_dq[33]}]
set_property PACKAGE_PIN AM27 [get_ports {C0_DDR4_dq[34]}]
set_property PACKAGE_PIN AK28 [get_ports {C0_DDR4_dq[35]}]
set_property PACKAGE_PIN AH27 [get_ports {C0_DDR4_dq[36]}]
set_property PACKAGE_PIN AH28 [get_ports {C0_DDR4_dq[37]}]
set_property PACKAGE_PIN AK26 [get_ports {C0_DDR4_dq[38]}]
set_property PACKAGE_PIN AK27 [get_ports {C0_DDR4_dq[39]}]
set_property PACKAGE_PIN AN28 [get_ports {C0_DDR4_dq[40]}]
set_property PACKAGE_PIN AM30 [get_ports {C0_DDR4_dq[41]}]
set_property PACKAGE_PIN AP28 [get_ports {C0_DDR4_dq[42]}]
set_property PACKAGE_PIN AM29 [get_ports {C0_DDR4_dq[43]}]
set_property PACKAGE_PIN AN27 [get_ports {C0_DDR4_dq[44]}]
set_property PACKAGE_PIN AL30 [get_ports {C0_DDR4_dq[45]}]
set_property PACKAGE_PIN AL29 [get_ports {C0_DDR4_dq[46]}]
set_property PACKAGE_PIN AP29 [get_ports {C0_DDR4_dq[47]}]
set_property PACKAGE_PIN AK31 [get_ports {C0_DDR4_dq[48]}]
set_property PACKAGE_PIN AH34 [get_ports {C0_DDR4_dq[49]}]
set_property PACKAGE_PIN AK32 [get_ports {C0_DDR4_dq[50]}]
set_property PACKAGE_PIN AJ31 [get_ports {C0_DDR4_dq[51]}]
set_property PACKAGE_PIN AJ30 [get_ports {C0_DDR4_dq[52]}]
set_property PACKAGE_PIN AH31 [get_ports {C0_DDR4_dq[53]}]
set_property PACKAGE_PIN AJ34 [get_ports {C0_DDR4_dq[54]}]
set_property PACKAGE_PIN AH32 [get_ports {C0_DDR4_dq[55]}]
set_property PACKAGE_PIN AN31 [get_ports {C0_DDR4_dq[56]}]
set_property PACKAGE_PIN AL34 [get_ports {C0_DDR4_dq[57]}]
set_property PACKAGE_PIN AN32 [get_ports {C0_DDR4_dq[58]}]
set_property PACKAGE_PIN AN33 [get_ports {C0_DDR4_dq[59]}]
set_property PACKAGE_PIN AM32 [get_ports {C0_DDR4_dq[60]}]
set_property PACKAGE_PIN AM34 [get_ports {C0_DDR4_dq[61]}]
set_property PACKAGE_PIN AP31 [get_ports {C0_DDR4_dq[62]}]
set_property PACKAGE_PIN AP33 [get_ports {C0_DDR4_dq[63]}]
set_property PACKAGE_PIN AG14 [get_ports {C0_DDR4_adr[0]}]
set_property PACKAGE_PIN AF17 [get_ports {C0_DDR4_adr[1]}]
set_property PACKAGE_PIN AF15 [get_ports {C0_DDR4_adr[2]}]
set_property PACKAGE_PIN AJ14 [get_ports {C0_DDR4_adr[3]}]
set_property PACKAGE_PIN AD18 [get_ports {C0_DDR4_adr[4]}]
set_property PACKAGE_PIN AG17 [get_ports {C0_DDR4_adr[5]}]
set_property PACKAGE_PIN AE17 [get_ports {C0_DDR4_adr[6]}]
set_property PACKAGE_PIN AK18 [get_ports {C0_DDR4_adr[7]}]
set_property PACKAGE_PIN AD16 [get_ports {C0_DDR4_adr[8]}]
set_property PACKAGE_PIN AH18 [get_ports {C0_DDR4_adr[9]}]
set_property PACKAGE_PIN AD19 [get_ports {C0_DDR4_adr[10]}]
set_property PACKAGE_PIN AD15 [get_ports {C0_DDR4_adr[11]}]
set_property PACKAGE_PIN AH16 [get_ports {C0_DDR4_adr[12]}]
set_property PACKAGE_PIN AL17 [get_ports {C0_DDR4_adr[13]}]
set_property PACKAGE_PIN AL15 [get_ports {C0_DDR4_adr[14]}]
set_property PACKAGE_PIN AL19 [get_ports {C0_DDR4_adr[15]}]
set_property PACKAGE_PIN AM19 [get_ports {C0_DDR4_adr[16]}]
set_property PACKAGE_PIN AG15 [get_ports {C0_DDR4_ba[0]}]
set_property PACKAGE_PIN AL18 [get_ports {C0_DDR4_ba[1]}]
set_property PACKAGE_PIN AJ15 [get_ports {C0_DDR4_bg[0]}]
set_property PACKAGE_PIN AG19 [get_ports {C0_DDR4_odt[0]}]
set_property PACKAGE_PIN AJ16 [get_ports {C0_DDR4_cke[0]}]
set_property PACKAGE_PIN AE16 [get_ports {C0_DDR4_ck_t[0]}]
set_property PACKAGE_PIN AE15 [get_ports {C0_DDR4_ck_c[0]}]
set_property PACKAGE_PIN AG16 [get_ports C0_DDR4_reset_n]
set_property PACKAGE_PIN AF18 [get_ports C0_DDR4_act_n]
set_property PACKAGE_PIN AE18 [get_ports {C0_DDR4_cs_n[0]}]

create_clock -period 5.000 [get_ports sys_clk_p]

