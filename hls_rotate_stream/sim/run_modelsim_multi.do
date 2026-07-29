# run_modelsim_multi.do — multi-scenario ModelSim regression
onerror {echo "ERROR: compile/elab/sim failed"; quit -code 1 -f}

if {[info exists ::env(ROTATE_STREAM_ROOT)]} {
    set ROOT [file normalize $::env(ROTATE_STREAM_ROOT)]
} else {
    set ROOT [file normalize [file join [file dirname [info script]] ..]]
}

set RTL  [file join $ROOT rtl rotate_stream_accel.v]
set TB   [file join $ROOT sim  tb_rotate_multiscenario.sv]
set SIMDIR [file join $ROOT sim]

puts "========================================"
puts " ModelSim MULTI-SCENARIO regression"
puts " ROOT=$ROOT"
puts "========================================"

if {![file exists $RTL]} { echo "ERROR: missing $RTL"; quit -code 1 -f }
if {![file exists $TB]}  { echo "ERROR: missing $TB";  quit -code 1 -f }

cd $SIMDIR
file mkdir [file join $SIMDIR results]
transcript file [file join $SIMDIR results modelsim_multi_transcript.log]

# Keep library mapping local to sim/ (do not touch global modelsim.ini)
set ini [file join $SIMDIR modelsim.ini]
if {![file exists $ini]} {
    set fh [open $ini w]
    puts $fh "\[Library\]"
    puts $fh "others = \$MODEL_TECH/../modelsim.ini"
    close $fh
}
catch {vdel -lib work -all}
vlib work
vmap -modelsimini $ini work work

vlog -modelsimini $ini -sv +define+SIMULATION $RTL $TB
vsim -modelsimini $ini -c work.tb_rotate_multiscenario -voptargs=+acc
run -all
quit -f
