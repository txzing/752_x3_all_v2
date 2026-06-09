
set_false_path -to [get_pins icontrol/RST]

# Clock group constraint to ensure correct clock skew for ISERDES
set_property CLOCK_DELAY_GROUP ioclockGroup_rx [get_nets rx_inst/rx_clkdiv*]

set_false_path -to [get_pins {rx_inst/rxc_delay/iserdes_m/D}]
set_false_path -to [get_pins {rx_inst/rxc_delay/iserdes_s/D}]
set_false_path -to [get_pins {rx_inst/rxc_delay/px_reset_sync_reg[*]/PRE}]
set_false_path -to [get_pins {rx_inst/rxc_delay/px_rx_ready_sync_reg[*]/CLR}]
set_false_path -to [get_pins {rx_inst/rxc_delay/px_data_reg[*]/D}]
set_false_path -to [get_pins {rx_inst/rxc_delay/px_rd_last_reg[*]/D}]
set_false_path -to [get_pins {rx_inst/loop0[*].rxn/rxd[*].sipo/px_data_reg[*]/D}]
set_false_path -to [get_pins {rx_inst/loop0[*].rxn/rxd[*].sipo/px_rd_last_reg[*]/D}]

