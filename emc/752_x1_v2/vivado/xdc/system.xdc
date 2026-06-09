set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]


## eeprom sda
set_property -dict {PACKAGE_PIN AF10 IOSTANDARD LVCMOS33} [get_ports eeprom_sda_io]
## eeprom scl
set_property -dict {PACKAGE_PIN Y9 IOSTANDARD LVCMOS33} [get_ports eeprom_scl_io]

# unused
## sda
set_property PACKAGE_PIN E9 [get_ports i2c_0_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_0_sda_io]
## scl
set_property PACKAGE_PIN D9 [get_ports i2c_0_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_0_scl_io]


# ch_0
## sda
set_property PACKAGE_PIN A9 [get_ports i2c_1_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_sda_io]
## scl
set_property PACKAGE_PIN B8 [get_ports i2c_1_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_scl_io]

# ch_1
## sda
set_property PACKAGE_PIN R7 [get_ports i2c_2_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_sda_io]
## scl
set_property PACKAGE_PIN K3 [get_ports i2c_2_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_scl_io]

# unused
# ch_2
## sda
set_property PACKAGE_PIN F3 [get_ports i2c_3_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_sda_io]
## scl
set_property PACKAGE_PIN G3 [get_ports i2c_3_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_scl_io]

# unused
# ch_3
## sda
set_property PACKAGE_PIN E8 [get_ports i2c_4_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_4_sda_io]
## scl
set_property PACKAGE_PIN F8 [get_ports i2c_4_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_4_scl_io]


# relay_ctrl
set_property -dict {PACKAGE_PIN AF11 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[0]}]

# EEPROM WRITE PROTECT
set_property -dict {PACKAGE_PIN AA8 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[1]}]

#752_0 ERR
set_property -dict {PACKAGE_PIN A8 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[2]}]

#752_0 lock
set_property -dict {PACKAGE_PIN C9 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[3]}]

#752_0 PWDNB
set_property -dict {PACKAGE_PIN B9 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[4]}]

#752_1 ERR
set_property -dict {PACKAGE_PIN N6 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[5]}]

#752_1 lock
set_property -dict {PACKAGE_PIN P7 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[6]}]

#752_1 PWDNB
set_property -dict {PACKAGE_PIN P6 IOSTANDARD LVCMOS18} [get_ports {gpio_tri_io[7]}]

#SFP_TX_DIS
set_property -dict {PACKAGE_PIN AE12 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[8]}]

#SFP_TX_LOSS
set_property -dict {PACKAGE_PIN AF12 IOSTANDARD LVCMOS33} [get_ports {gpio_tri_io[9]}]

################################################################################################

################################################################################################
################################################################################################
create_clock -period 12.000 [get_ports ch0_clkin0_p]
create_clock -period 12.000 [get_ports ch0_clkin1_p]

set_property ODT RTT_48 [get_ports ch0_clkin0_p]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch0_clkin0_p]
set_property PACKAGE_PIN C3 [get_ports ch0_clkin0_p]
set_property PACKAGE_PIN C2 [get_ports ch0_clkin0_n]
set_property ODT RTT_48 [get_ports ch0_clkin0_n]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch0_clkin0_n]

set_property ODT RTT_48 [get_ports {ch0_datain0_p[0]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_p[0]}]
set_property PACKAGE_PIN E4 [get_ports {ch0_datain0_p[0]}]
set_property PACKAGE_PIN E3 [get_ports {ch0_datain0_n[0]}]
set_property ODT RTT_48 [get_ports {ch0_datain0_n[0]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_n[0]}]
set_property ODT RTT_48 [get_ports {ch0_datain0_p[1]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_p[1]}]
set_property PACKAGE_PIN E1 [get_ports {ch0_datain0_p[1]}]
set_property PACKAGE_PIN D1 [get_ports {ch0_datain0_n[1]}]
set_property ODT RTT_48 [get_ports {ch0_datain0_n[1]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_n[1]}]
set_property ODT RTT_48 [get_ports {ch0_datain0_p[2]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_p[2]}]
set_property PACKAGE_PIN C6 [get_ports {ch0_datain0_p[2]}]
set_property PACKAGE_PIN B6 [get_ports {ch0_datain0_n[2]}]
set_property ODT RTT_48 [get_ports {ch0_datain0_n[2]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_n[2]}]
set_property ODT RTT_48 [get_ports {ch0_datain0_p[3]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_p[3]}]
set_property PACKAGE_PIN B3 [get_ports {ch0_datain0_p[3]}]
set_property PACKAGE_PIN A3 [get_ports {ch0_datain0_n[3]}]
set_property ODT RTT_48 [get_ports {ch0_datain0_n[3]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain0_n[3]}]

set_property ODT RTT_48 [get_ports ch0_clkin1_p]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch0_clkin1_p]
set_property PACKAGE_PIN D4 [get_ports ch0_clkin1_p]
set_property PACKAGE_PIN C4 [get_ports ch0_clkin1_n]
set_property ODT RTT_48 [get_ports ch0_clkin1_n]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch0_clkin1_n]

set_property ODT RTT_48 [get_ports {ch0_datain1_p[0]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_p[0]}]
set_property PACKAGE_PIN G6 [get_ports {ch0_datain1_p[0]}]
set_property PACKAGE_PIN F6 [get_ports {ch0_datain1_n[0]}]
set_property ODT RTT_48 [get_ports {ch0_datain1_n[0]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_n[0]}]
set_property ODT RTT_48 [get_ports {ch0_datain1_p[1]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_p[1]}]
set_property PACKAGE_PIN G8 [get_ports {ch0_datain1_p[1]}]
set_property PACKAGE_PIN F7 [get_ports {ch0_datain1_n[1]}]
set_property ODT RTT_48 [get_ports {ch0_datain1_n[1]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_n[1]}]
set_property ODT RTT_48 [get_ports {ch0_datain1_p[2]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_p[2]}]
set_property PACKAGE_PIN G1 [get_ports {ch0_datain1_p[2]}]
set_property PACKAGE_PIN F1 [get_ports {ch0_datain1_n[2]}]
set_property ODT RTT_48 [get_ports {ch0_datain1_n[2]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_n[2]}]
set_property ODT RTT_48 [get_ports {ch0_datain1_p[3]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_p[3]}]
set_property PACKAGE_PIN F2 [get_ports {ch0_datain1_p[3]}]
set_property PACKAGE_PIN E2 [get_ports {ch0_datain1_n[3]}]
set_property ODT RTT_48 [get_ports {ch0_datain1_n[3]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch0_datain1_n[3]}]




##lvds71_in_ch0
#create_clock -period 20.000 [get_ports ch1_clkin0_p]
#create_clock -period 20.000 [get_ports ch1_clkin1_p]


#set_property ODT RTT_48 [get_ports ch1_clkin0_p]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch1_clkin0_p]
#set_property PACKAGE_PIN L7 [get_ports ch1_clkin0_p]
#set_property PACKAGE_PIN L6 [get_ports ch1_clkin0_n]
#set_property ODT RTT_48 [get_ports ch1_clkin0_n]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch1_clkin0_n]

#set_property ODT RTT_48 [get_ports {ch1_datain0_p[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_p[0]}]
#set_property PACKAGE_PIN U8 [get_ports {ch1_datain0_p[0]}]
#set_property PACKAGE_PIN V8 [get_ports {ch1_datain0_n[0]}]
#set_property ODT RTT_48 [get_ports {ch1_datain0_n[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_n[0]}]
#set_property ODT RTT_48 [get_ports {ch1_datain0_p[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_p[1]}]
#set_property PACKAGE_PIN J5 [get_ports {ch1_datain0_p[1]}]
#set_property PACKAGE_PIN J4 [get_ports {ch1_datain0_n[1]}]
#set_property ODT RTT_48 [get_ports {ch1_datain0_n[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_n[1]}]
#set_property ODT RTT_48 [get_ports {ch1_datain0_p[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_p[2]}]
#set_property PACKAGE_PIN J1 [get_ports {ch1_datain0_p[2]}]
#set_property PACKAGE_PIN H1 [get_ports {ch1_datain0_n[2]}]
#set_property ODT RTT_48 [get_ports {ch1_datain0_n[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_n[2]}]
#set_property ODT RTT_48 [get_ports {ch1_datain0_p[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_p[3]}]
#set_property PACKAGE_PIN H9 [get_ports {ch1_datain0_p[3]}]
#set_property PACKAGE_PIN H8 [get_ports {ch1_datain0_n[3]}]
#set_property ODT RTT_48 [get_ports {ch1_datain0_n[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain0_n[3]}]

#set_property ODT RTT_48 [get_ports ch1_clkin1_p]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch1_clkin1_p]
#set_property PACKAGE_PIN L3 [get_ports ch1_clkin1_p]
#set_property PACKAGE_PIN L2 [get_ports ch1_clkin1_n]
#set_property ODT RTT_48 [get_ports ch1_clkin1_n]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch1_clkin1_n]

#set_property ODT RTT_48 [get_ports {ch1_datain1_p[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_p[0]}]
#set_property PACKAGE_PIN K8 [get_ports {ch1_datain1_p[0]}]
#set_property PACKAGE_PIN K7 [get_ports {ch1_datain1_n[0]}]
#set_property ODT RTT_48 [get_ports {ch1_datain1_n[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_n[0]}]
#set_property ODT RTT_48 [get_ports {ch1_datain1_p[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_p[1]}]
#set_property PACKAGE_PIN J6 [get_ports {ch1_datain1_p[1]}]
#set_property PACKAGE_PIN H6 [get_ports {ch1_datain1_n[1]}]
#set_property ODT RTT_48 [get_ports {ch1_datain1_n[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_n[1]}]
#set_property ODT RTT_48 [get_ports {ch1_datain1_p[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_p[2]}]
#set_property PACKAGE_PIN N9 [get_ports {ch1_datain1_p[2]}]
#set_property PACKAGE_PIN N8 [get_ports {ch1_datain1_n[2]}]
#set_property ODT RTT_48 [get_ports {ch1_datain1_n[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_n[2]}]
#set_property ODT RTT_48 [get_ports {ch1_datain1_p[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_p[3]}]
#set_property PACKAGE_PIN K2 [get_ports {ch1_datain1_p[3]}]
#set_property PACKAGE_PIN J2 [get_ports {ch1_datain1_n[3]}]
#set_property ODT RTT_48 [get_ports {ch1_datain1_n[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch1_datain1_n[3]}]

#################################################################################################
#create_clock -period 20.000 [get_ports ch2_clkin0_p]
#create_clock -period 20.000 [get_ports ch2_clkin1_p]

#set_property ODT RTT_48 [get_ports ch2_clkin0_p]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch2_clkin0_p]
#set_property PACKAGE_PIN L3 [get_ports ch2_clkin0_p]
#set_property PACKAGE_PIN L2 [get_ports ch2_clkin0_n]
#set_property ODT RTT_48 [get_ports ch2_clkin0_n]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch2_clkin0_n]

#set_property ODT RTT_48 [get_ports {ch2_datain0_p[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_p[0]}]
#set_property PACKAGE_PIN U8 [get_ports {ch2_datain0_p[0]}]
#set_property PACKAGE_PIN V8 [get_ports {ch2_datain0_n[0]}]
#set_property ODT RTT_48 [get_ports {ch2_datain0_n[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_n[0]}]
#set_property ODT RTT_48 [get_ports {ch2_datain0_p[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_p[1]}]
#set_property PACKAGE_PIN J5 [get_ports {ch2_datain0_p[1]}]
#set_property PACKAGE_PIN J4 [get_ports {ch2_datain0_n[1]}]
#set_property ODT RTT_48 [get_ports {ch2_datain0_n[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_n[1]}]
#set_property ODT RTT_48 [get_ports {ch2_datain0_p[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_p[2]}]
#set_property PACKAGE_PIN J1 [get_ports {ch2_datain0_p[2]}]
#set_property PACKAGE_PIN H1 [get_ports {ch2_datain0_n[2]}]
#set_property ODT RTT_48 [get_ports {ch2_datain0_n[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_n[2]}]
#set_property ODT RTT_48 [get_ports {ch2_datain0_p[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_p[3]}]
#set_property PACKAGE_PIN H9 [get_ports {ch2_datain0_p[3]}]
#set_property PACKAGE_PIN H8 [get_ports {ch2_datain0_n[3]}]
#set_property ODT RTT_48 [get_ports {ch2_datain0_n[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain0_n[3]}]

#set_property ODT RTT_48 [get_ports ch2_clkin1_p]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch2_clkin1_p]
#set_property PACKAGE_PIN L7 [get_ports ch2_clkin1_p]
#set_property PACKAGE_PIN L6 [get_ports ch2_clkin1_n]
#set_property ODT RTT_48 [get_ports ch2_clkin1_n]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports ch2_clkin1_n]

#set_property ODT RTT_48 [get_ports {ch2_datain1_p[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_p[0]}]
#set_property PACKAGE_PIN K8 [get_ports {ch2_datain1_p[0]}]
#set_property PACKAGE_PIN K7 [get_ports {ch2_datain1_n[0]}]
#set_property ODT RTT_48 [get_ports {ch2_datain1_n[0]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_n[0]}]
#set_property ODT RTT_48 [get_ports {ch2_datain1_p[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_p[1]}]
#set_property PACKAGE_PIN J6 [get_ports {ch2_datain1_p[1]}]
#set_property PACKAGE_PIN H6 [get_ports {ch2_datain1_n[1]}]
#set_property ODT RTT_48 [get_ports {ch2_datain1_n[1]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_n[1]}]
#set_property ODT RTT_48 [get_ports {ch2_datain1_p[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_p[2]}]
#set_property PACKAGE_PIN N9 [get_ports {ch2_datain1_p[2]}]
#set_property PACKAGE_PIN N8 [get_ports {ch2_datain1_n[2]}]
#set_property ODT RTT_48 [get_ports {ch2_datain1_n[2]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_n[2]}]
#set_property ODT RTT_48 [get_ports {ch2_datain1_p[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_p[3]}]
#set_property PACKAGE_PIN K2 [get_ports {ch2_datain1_p[3]}]
#set_property PACKAGE_PIN J2 [get_ports {ch2_datain1_n[3]}]
#set_property ODT RTT_48 [get_ports {ch2_datain1_n[3]}]
#set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {ch2_datain1_n[3]}]
#################################################################################################

#set_false_path -from [get_clocks -of_objects [get_pins system_i/lvds4x2_1to7_0/inst/rx_inst/rx_clkgen/rx_plle2_adv_inst/CLKFBOUT]] -to [get_clocks -of_objects [get_pins system_i/lvds4x2_1to7_1/inst/rx_inst/rx_clkgen/rx_plle2_adv_inst/CLKFBOUT]]
#set_false_path -from [get_clocks -of_objects [get_pins system_i/lvds4x2_1to7_1/inst/rx_inst/rx_clkgen/rx_plle2_adv_inst/CLKFBOUT]] -to [get_clocks -of_objects [get_pins system_i/lvds4x2_1to7_0/inst/rx_inst/rx_clkgen/rx_plle2_adv_inst/CLKFBOUT]]


set_property LOC BITSLICE_CONTROL_X0Y16 [get_cells system_i/lvds4x2_1to7_0/inst/icontrol]
#set_property LOC BITSLICE_CONTROL_X0Y19 [get_cells system_i/lvds4x2_1to7_1/inst/icontrol]
#set_property LOC BITSLICE_CONTROL_X0Y12 [get_cells system_i/lvds4x2_1to7_0/inst/icontrol]
#set_property LOC BITSLICE_CONTROL_X0Y19 [get_cells system_i/lvds4x2_1to7_1/inst/icontrol]
#set_property LOC BITSLICE_CONTROL_X0Y20 [get_cells system_i/lvds4x2_1to7_3/inst/icontrol]

set_clock_groups -asynchronous \
  -group [get_clocks clk_pl_0] \
  -group [get_clocks {px_pllmmcm}]

#set_false_path -from [get_clocks clk_pl_0] -to [get_clocks -of_objects [get_pins system_i/lvds4x2_1to7_0/inst/rx_inst/rx_clkgen/rx_plle2_adv_inst/CLKFBOUT]]
#set_false_path -from [get_clocks clk_pl_0] -to [get_clocks -of_objects [get_pins system_i/lvds4x2_1to7_1/inst/rx_inst/rx_clkgen/rx_plle2_adv_inst/CLKFBOUT]]


