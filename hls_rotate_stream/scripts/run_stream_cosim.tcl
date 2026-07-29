# run_stream_cosim.tcl — csynth + cosim for rotate_stream_accel
set PROJ_ROOT [file normalize [file join [file dirname [info script]] ..]]
set SRC_DIR   [file join $PROJ_ROOT src]
set INC_DIR   [file join $PROJ_ROOT include]
set TB_DIR    [file join $PROJ_ROOT tb_data]
set PROJ_NAME "rotate_stream_cosim"
set SOLN      "sol1"
set XPART     "xczu2cg-sfvc784-1-e"
set CLKP      5.0

open_project -reset $PROJ_NAME
set CFLAGS "-I$SRC_DIR -I$INC_DIR -D__SDSVHLS__ -D__VITIS_HLS__ -std=c++14"
add_files [file join $SRC_DIR xf_rotate_stream_accel.cpp] -cflags $CFLAGS
# Cosim TB must drive AXIS; use dedicated cosim harness if available.
# Fallback: host model C-sim path is not cosim-compatible; keep synthesizable TB later.
add_files -tb [file join $SRC_DIR xf_rotate_stream_axis_tb.cpp] -cflags $CFLAGS
set_top rotate_stream_accel
open_solution -reset $SOLN
set_part $XPART
create_clock -period $CLKP -name default
csynth_design
cosim_design -argv "$TB_DIR 0 3 32 64"
exit
