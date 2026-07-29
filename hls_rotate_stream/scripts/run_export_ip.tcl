# run_export_ip.tcl — export rotate_stream_accel Vivado IP
set PROJ_ROOT [file normalize [file join [file dirname [info script]] ..]]
set SRC_DIR   [file join $PROJ_ROOT src]
set INC_DIR   [file join $PROJ_ROOT include]
set OUT_DIR   [file join $PROJ_ROOT ip_repo]
set PROJ_NAME "rotate_stream_accel"
set SOLN      "sol1"
set XPART     "xczu2cg-sfvc784-1-e"
set CLKP      5.0

file mkdir $OUT_DIR
open_project -reset $PROJ_NAME
set CFLAGS "-I$SRC_DIR -I$INC_DIR -D__SDSVHLS__ -D__VITIS_HLS__ -std=c++14"
add_files [file join $SRC_DIR xf_rotate_stream_accel.cpp] -cflags $CFLAGS
set_top rotate_stream_accel
open_solution -reset $SOLN
set_part $XPART
create_clock -period $CLKP -name default
csynth_design
export_design -format ip_catalog -output [file join $OUT_DIR rotate_stream_accel.zip] \
  -description "AXIS RGB888 stream rotate with DDR multi-buffer (90/180/270)" \
  -vendor "xilinx.com" -library "vision" -version "1.0" -display_name "rotate_stream_accel"
exit
