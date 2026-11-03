# Implementation only. Synthesis does not have clk_pl_0 yet, and an if{} in an XDC
# is rejected (Designutils 20-1307). Pixel clocks are generated from the LVDS inputs.
set_clock_groups -asynchronous \
  -group [get_clocks -include_generated_clocks clk_pl_0] \
  -group [get_clocks -include_generated_clocks -of_objects [get_ports {ch0_clkin0_p ch0_clkin1_p ch1_clkin0_p ch1_clkin1_p ch2_clkin0_p ch2_clkin1_p}]]

# Clock region X1Y2 has one IDELAYCTRL. The three lvds4x2_1to7 instances share
# pl_clk0 and gpio bit0, so they must share this group or place fails (PLIDC-7).
set_property IODELAY_GROUP ioclockGroup_rx [get_cells -quiet -hier -filter {NAME =~ *lvds4x2_1to7_* && (REF_NAME == IDELAYCTRL || REF_NAME == IDELAYE3)}]
