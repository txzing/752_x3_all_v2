# run_modelsim.do — ModelSim / Questa regression for rotate_stream_accel
#
# Prefer: hls_rotate_stream\scripts\run_modelsim.bat

if {[info exists ::env(ROTATE_STREAM_ROOT)]} {
    set ROOT [file normalize $::env(ROTATE_STREAM_ROOT)]
} else {
    set ROOT [file normalize [file join [file dirname [info script]] ..]]
}

set RTL [file join $ROOT rtl rotate_stream_accel.v]
set TB  [file join $ROOT sim tb_rotate_stream.sv]
set SIMDIR [file join $ROOT sim]

puts "========================================"
puts " rotate_stream_accel ModelSim regression"
puts " ROOT=$ROOT"
puts "========================================"

if {![file exists $RTL]} {
    echo "ERROR: RTL not found: $RTL"
    quit -code 1 -f
}
if {![file exists $TB]} {
    echo "ERROR: TB not found: $TB"
    quit -code 1 -f
}

cd $SIMDIR
transcript file [file join $SIMDIR modelsim_transcript.log]

catch {vdel -all}
vlib work
vmap work work

# Stop on compile error
onerror {echo "ERROR: compile/elab failed"; quit -code 1 -f}

vlog -sv +define+SIMULATION $RTL $TB
vsim -c work.tb_rotate_stream -voptargs=+acc

onerror {resume}
run -all

# If TB finished with TEST PASSED, transcript will contain it.
# Bat wrapper greps the log; still exit 0 here after normal $finish path.
quit -f
