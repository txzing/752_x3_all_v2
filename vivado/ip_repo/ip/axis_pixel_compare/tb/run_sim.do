# ModelSim: smoke + stress TBs for axis_pixel_compare
# Bounded run only (no run -all). Invoke from tb/.

if {![info exists RUN_US] || $RUN_US <= 0} { set RUN_US 500 }
if {![info exists RUN_STRESS_US] || $RUN_STRESS_US <= 0} { set RUN_STRESS_US 2000 }

set TB_DIR [pwd]
if {[file exists [file join $TB_DIR tb_axis_pixel_compare.v]] == 0} {
    if {[info script] != ""} {
        set TB_DIR [file normalize [file dirname [info script]]]
    }
}
set HDL_DIR [file normalize [file join $TB_DIR .. hdl]]
set LOG_DIR [file join $TB_DIR logs]
file mkdir $LOG_DIR
file mkdir [file join $TB_DIR work]

echo "INFO: TB_DIR=$TB_DIR"
cd $TB_DIR
onerror { echo "ERROR: Tcl error"; catch {quit -sim}; quit -code 1 -f }

vlib work
vmap work work

echo "INFO: compiling RTL + TBs"
vlog -work work [file join $HDL_DIR AXI_LITE_REG_v1_0_S00_AXI.v]
vlog -work work [file join $HDL_DIR axis_pixel_compare.v]
vlog -work work [file join $TB_DIR tb_axis_pixel_compare.v]
vlog -work work [file join $TB_DIR tb_axis_pixel_compare_stress.v]

proc run_one {top run_us logfile} {
    global LOG_DIR
    set logpath [file join $LOG_DIR $logfile]
    transcript file $logpath
    echo "INFO: elaborating $top"
    vsim -c -voptargs=+acc work.$top
    onfinish stop
    echo "INFO: run ${run_us}us -> $logfile"
    run ${run_us}us
    catch {quit -sim}

    set fh [open $logpath r]
    set content [read $fh]
    close $fh
    if {[string match "*TB_PASS*" $content] && ![string match "*TB_FAIL*" $content]} {
        echo "INFO: $top PASS"
        return 1
    }
    echo "ERROR: $top FAIL (see logs/$logfile)"
    return 0
}

set smoke_ok  [run_one tb_axis_pixel_compare        $RUN_US        smoke_transcript.log]
set stress_ok [run_one tb_axis_pixel_compare_stress $RUN_STRESS_US stress_transcript.log]

# also keep combined pointer
transcript file [file join $LOG_DIR sim_transcript.log]
echo "smoke_ok=$smoke_ok stress_ok=$stress_ok"

if {$smoke_ok && $stress_ok} {
    echo "INFO: ALL simulation PASS"
    quit -f
} else {
    echo "ERROR: simulation suite FAIL"
    quit -code 1 -f
}
