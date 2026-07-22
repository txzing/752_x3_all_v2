# run_tb.do — axis_combiner SOF 复现仿真（禁止 run -all）
# 在 sim 目录下执行: vsim -c -do "cd <sim>; do run_tb.do"

onerror { echo "ERROR: macro aborted"; catch {quit -sim}; quit -code 1 -f }

set SIMDIR [pwd]
set HDLDIR [file normalize [file join $SIMDIR .. hdl]]

echo "INFO: SIMDIR=$SIMDIR"
echo "INFO: HDLDIR=$HDLDIR"

if {![file exists [file join $SIMDIR xilinx_sync_fifo_beh.v]]} {
    echo "ERROR: xilinx_sync_fifo_beh.v not found in $SIMDIR"
    quit -code 2 -f
}

if {[file exists work]} {
    file delete -force work
}
vlib work
vmap work work

vlog -sv +acc [file join $SIMDIR xilinx_sync_fifo_beh.v]
vlog -sv +acc [file join $HDLDIR axis_combiner.v]
vlog -sv +acc [file join $SIMDIR tb_axis_combiner.v]

vsim -c -voptargs=+acc -onfinish stop work.tb_axis_combiner

set RUN_US 5000
echo "INFO: bounded run ${RUN_US}us (no run -all)"
run ${RUN_US}us

echo "INFO: simulation finished or timed out"
catch {quit -sim}
quit -f
