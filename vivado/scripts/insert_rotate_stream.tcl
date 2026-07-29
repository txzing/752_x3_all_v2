# insert_rotate_stream.tcl
# Insert rotate_stream_accel into an open BD on the 1ppc RGB24 path.
#
# Typical (792_x1 / similar):
#   capture AXIS (24b) -> rotate_stream_accel -> display VDMA S2MM / video_out
#
# Usage (Vivado TCL console, BD open):
#   source <this_file>
#   insert_rotate_stream_accel

proc insert_rotate_stream_accel {{cell_name rotate_stream_accel_0}} {
  if {[get_bd_cells -quiet $cell_name] ne ""} {
    puts "INFO: $cell_name already exists"
    return
  }

  set ip [create_bd_cell -type ip -vlnv xilinx.com:vision:rotate_stream_accel:1.0 $cell_name]
  puts "INFO: created $cell_name ($ip)"
  puts "INFO: Manually connect:"
  puts "  - ap_clk / ap_rst_n"
  puts "  - s_axis_video  <- 1ppc RGB24 source"
  puts "  - m_axis_video  -> display path"
  puts "  - m_axi / s_axi_control via smartconnect / PS"
  puts "  - Assign address; rebuild XSA; SW calls rotate_stream_init_once() once"
}
