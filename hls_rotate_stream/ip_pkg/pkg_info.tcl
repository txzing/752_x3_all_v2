# rotate_stream_accel IP package for Vivado IP Catalog
# VLNV: xilinx.com:vision:rotate_stream_accel:1.0
#
# After HLS export_design, replace rtl/ with generated HDL and refresh.
# This checked-in package uses the verified simulation RTL as a placeholder
# so BD scripting and SW bring-up can proceed.

set ip_name rotate_stream_accel
set ip_dir [file normalize [file join [file dirname [info script]]]]

# Helper used by scripts/install_ip_to_repo.tcl
puts "rotate_stream_accel package root: $ip_dir"
