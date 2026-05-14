# =============================================================================
# export_timing_for_review.tcl
# -----------------------------------------------------------------------------
# 用途：在 Vivado 中一键导出时序/时钟/约束相关报告，便于离线分析或发给他人。
#
# 使用前提（重要）：
#   1) 已打开工程，并已 Open Implemented Design（或 open_run impl_*）。
#   2) 若在综合后网表上运行，报告为预布线时序，与实现后不一致。
#
# 调用方式（Tcl Console，本脚本位于 vivado 目录下）：
#   source -notrace {D:/workdir/cariad/ccu/752_x3_all_v2/vivado/export_timing_for_review.tcl}
# 或使用相对当前工程的路径自行替换。
#
# 可选：在 source 之前设置输出根目录（默认：工程目录下带时间戳的子目录）
#   set timing_export_root {D:/workdir/.../my_timing_dump}
#   source -notrace {.../vivado/export_timing_for_review.tcl}
#
# 可选：关闭较慢的报告（默认 1 = 开启）
#   set timing_export_include_methodology 0
# =============================================================================

proc __timing_export_stamp {} {
    return [clock format [clock seconds] -format {%Y%m%d_%H%M%S}]
}

if {![info exists timing_export_root] || $timing_export_root eq ""} {
    if {![catch {current_project -quiet} proj] && $proj ne ""} {
        set timing_export_root [file join [get_property DIRECTORY $proj] \
            "timing_export_for_review_[__timing_export_stamp]"]
    } else {
        set timing_export_root [file join [pwd] "timing_export_for_review_[__timing_export_stamp]"]
    }
}

if {![info exists timing_export_include_methodology]} {
    set timing_export_include_methodology 1
}

set rpt_dir $timing_export_root
file mkdir $rpt_dir

puts "\n=========================================================="
puts "Timing export -> $rpt_dir"
puts "==========================================================\n"

proc __safe_report {args} {
    set cmd [lindex $args 0]
    if {[catch {eval $args} err]} {
        puts "WARNING: $cmd failed: $err"
        return 1
    }
    return 0
}

# 0) 工程与实现信息
set fp [open [file join $rpt_dir 00_project_info.txt] w]
puts $fp "Vivado: [version -short]"
if {![catch {current_project -quiet} p] && $p ne ""} {
    puts $fp "Project: $p"
    puts $fp "Part:    [get_property PART $p]"
}
if {![catch {current_design -quiet} d] && $d ne ""} {
    puts $fp "Design:  $d"
    if {![catch {get_property TOP $d} top]} {
        puts $fp "Top:     $top"
    }
}
if {![catch {current_run -quiet} r] && $r ne ""} {
    puts $fp "Run:     $r"
    catch { puts $fp "Strategy: [get_property STRATEGY $r]" }
}
close $fp

# 1) 时钟与交互
__safe_report report_clock_networks -file [file join $rpt_dir 01_clock_networks.rpt]
__safe_report report_clocks           -file [file join $rpt_dir 02_clocks.rpt]
__safe_report report_clock_interaction -file [file join $rpt_dir 03_clock_interaction.rpt]

# 2) check_timing（部分版本 -file 不可用，用 redirect）
if {[catch {
    check_timing -verbose -file [file join $rpt_dir 04_check_timing_verbose.rpt]
}]} {
    redirect -file [file join $rpt_dir 04_check_timing_verbose.rpt] { check_timing -verbose }
}

# 3) 摘要
__safe_report report_timing_summary -file [file join $rpt_dir 05_timing_summary.rpt] \
    -max_paths 50 -nworst 10

# 4) 失败 Setup（slack < 0）；若无违例可能生成空或极少路径
__safe_report report_timing -delay_type min_max -max_paths 50 -nworst 10 \
    -slack_less_than 0 -sort_by slack \
    -file [file join $rpt_dir 06_timing_setup_violations.rpt]

# 5) 全局最差路径（不筛 slack，便于对照）
__safe_report report_timing -delay_type min_max -max_paths 80 -nworst 1 \
    -sort_by slack -file [file join $rpt_dir 07_timing_worst_paths.rpt]

# 6) Hold / Pulse width / Bus skew
__safe_report report_timing -delay_type min -max_paths 30 -nworst 5 \
    -slack_less_than 0 -sort_by slack \
    -file [file join $rpt_dir 08_timing_hold_violations.rpt]

__safe_report report_pulse_width -file [file join $rpt_dir 09_pulse_width.rpt]

catch { report_bus_skew -file [file join $rpt_dir 10_bus_skew.rpt] }

# 7) 合并约束（write_xdc 选项随版本变化，失败时仅告警）
if {[catch {
    write_xdc -force -exclude_physical [file join $rpt_dir 11_merged_timing.xdc]
}]} {
    if {[catch {
        write_xdc -force [file join $rpt_dir 11_merged_constraints.xdc]
    } err]} {
        puts "WARNING: write_xdc failed: $err"
    }
}

# 8) 方法论（较慢，可关；不同版本选项略有差异）
if {$timing_export_include_methodology} {
    if {[catch { report_methodology -file [file join $rpt_dir 12_methodology.rpt] } err]} {
        puts "WARNING: report_methodology: $err"
    }
}

# 9) 时钟名列表（便于复制到 set_clock_groups / report_timing）
set fp [open [file join $rpt_dir 13_clock_names.txt] w]
if {![catch {get_clocks} clks]} {
    foreach c [lsort $clks] {
        puts $fp $c
    }
}
close $fp

puts "\nDone. Pack this folder for review:\n  $rpt_dir\n"
puts "Tip: 在资源管理器中压缩为 zip 即可发送。\n"
