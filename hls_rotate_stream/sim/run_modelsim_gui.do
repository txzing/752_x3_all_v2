# run_modelsim_gui.do — open waves for rotate_stream_accel
onerror {resume}

if {[info exists ::env(ROTATE_STREAM_ROOT)]} {
    set ROOT [file normalize $::env(ROTATE_STREAM_ROOT)]
} else {
    set ROOT [file normalize [file join [file dirname [info script]] ..]]
}

set RTL [file join $ROOT rtl rotate_stream_accel.v]
set TB  [file join $ROOT sim  tb_rotate_stream.sv]
set SIMDIR [file join $ROOT sim]
cd $SIMDIR

vlib work
vmap work work
vlog -sv +define+SIMULATION $RTL $TB
vsim work.tb_rotate_stream -voptargs=+acc

add wave -radix hex /tb_rotate_stream/dut/*
add wave /tb_rotate_stream/ap_start
add wave /tb_rotate_stream/ap_done
add wave /tb_rotate_stream/s_tvalid
add wave /tb_rotate_stream/s_tready
add wave /tb_rotate_stream/m_tvalid
add wave /tb_rotate_stream/m_tready
run -all
