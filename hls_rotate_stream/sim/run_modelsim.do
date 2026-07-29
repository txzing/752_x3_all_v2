# run_modelsim.do — ModelSim / Questa regression for rotate_stream_accel
# Usage: vsim -c -do sim/run_modelsim.do

if {[info exists ::env(ROTATE_STREAM_ROOT)]} {
    set ROOT $::env(ROTATE_STREAM_ROOT)
} else {
    set ROOT [file normalize [file join [file dirname [info script]] ..]]
}

set RTL [file join $ROOT rtl rotate_stream_accel.v]
set TB  [file join $ROOT sim tb_rotate_stream.sv]

file mkdir [file join $ROOT sim work]
cd [file join $ROOT sim]

vlib work
vlog -sv +define+SIMULATION $RTL $TB
vsim -c work.tb_rotate_stream -voptargs=+acc
run -all
quit -f
