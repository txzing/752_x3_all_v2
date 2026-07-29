# run_stream_csim.tcl — Vitis HLS C simulation (when vitis_hls available)
# Usage: vitis_hls -f scripts/run_stream_csim.tcl

set PROJ_ROOT [file normalize [file join [file dirname [info script]] ..]]
set SRC_DIR   [file join $PROJ_ROOT src]
set INC_DIR   [file join $PROJ_ROOT include]
set TB_DIR    [file join $PROJ_ROOT tb_data]
set PROJ_NAME "rotate_stream_csim"
set SOLN      "sol1"
set XPART     "xczu2cg-sfvc784-1-e"
set CLKP      5.0

open_project -reset $PROJ_NAME

set CFLAGS "-I$SRC_DIR -I$INC_DIR -D__SDSVHLS__ -D__VITIS_HLS__ -std=c++14"

add_files [file join $SRC_DIR xf_rotate_stream_accel.cpp] -cflags $CFLAGS
add_files -tb [file join $SRC_DIR xf_rotate_stream_tb.cpp] -cflags "$CFLAGS -DHOST_CSIM_WITH_HLS=0"
# Host model used when not linking HLS stream TB against synthesizable top:
add_files -tb [file join $SRC_DIR rotate_stream_csim_model.cpp] -cflags "-I$INC_DIR -std=c++14"

set_top rotate_stream_accel

open_solution -reset $SOLN
set_part $XPART
create_clock -period $CLKP -name default

csim_design -argv "$TB_DIR 0 3 64 128"

exit
