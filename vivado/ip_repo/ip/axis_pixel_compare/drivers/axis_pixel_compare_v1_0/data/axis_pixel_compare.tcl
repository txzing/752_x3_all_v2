###############################################################################
# axis_pixel_compare.tcl - BSP/HSI driver generation
#
# Version: 2.7 (must match spirit:version in component.xml and OPTION VERSION in
#               drivers/axis_pixel_compare_v1_0/data/axis_pixel_compare.mdd)
# Revision date: 2026-05-15
#
# - xdefine_include_file -> xparameters.h (NUM_INSTANCES, DEVICE_ID, S00 base/high)
# - axis_pixel_compare_g.c written here: DeviceId, S00_axi_BaseAddr, IntrId (no HighAddr).
# - IntrId: 解析 xparameters.h；PMU 域为 0U。BSP 生成时本驱动常早于 INTC 驱动，中断宏可能
#   尚未写入 xparameters.h，故 MicroBlaze 回退为 XPAR_INTC_<n>_AXIS_PIXEL_COMPARE_<i>_VEC_ID
#   （编译期展开），MPSoC 回退为 XPAR_FABRIC_<UP>_INTR_INTR。勿用 get_interrupt_id 数值作
#   GIC SPI（常为 4U/5U/6U 等错误值）。
#
# IntrId 选择策略（按顺序命中即返回；不写死 LVDS 等板级实例名）：
#   A) xparameters.h 已有：AXI_INTC_<UP>_INTR_INTR 行、VEC_ID、FABRIC_* 等
#   B) 逐行扫描：符号名包含 <UP> 且后缀为 *_INTR_* / *_VEC_ID
#   C) 编译期回退（宏可尚未出现在生成时的 xparameters.h 中）：
#        MicroBlaze -> XPAR_INTC_<n>_AXIS_PIXEL_COMPARE_<inst_idx>_VEC_ID
#        ARM PS/GIC -> XPAR_FABRIC_<UP>_INTR_INTR
#   D) PMU 或无中断连接：0U
#
# xparameters.h 定位：从 [info script] 与 [pwd] 双向向上查找，且同时尝试
#   include/xparameters.h 与 bspinclude/include/xparameters.h（Vitis 导出布局）。
# regexp 使用 Tcl8.5 兼容写法（禁止 regexp -> var）。
###############################################################################

# Try one candidate path; return normalized path if readable file.
proc axis_pixel_compare__try_xph {p} {
	if {$p eq ""} {
		return ""
	}
	set n [file normalize $p]
	if {[file isfile $n]} {
		return $n
	}
	return ""
}

# Collect unique candidate paths from a root directory walking upward.
proc axis_pixel_compare__collect_xph_candidates {root_dir max_up} {
	set out [list]
	set d [file normalize $root_dir]
	for {set i 0} {$i < $max_up} {incr i} {
		lappend out [file join $d include xparameters.h]
		lappend out [file join $d bspinclude include xparameters.h]
		set parent [file dirname $d]
		if {$parent eq $d} {
			break
		}
		set d $parent
	}
	return $out
}

proc axis_pixel_compare_find_xparameters_h {} {
	# 1) From this script (BSP libsrc/.../data or export/hw/drivers/.../data)
	foreach c [axis_pixel_compare__collect_xph_candidates [file dirname [file normalize [info script]]] 32] {
		set n [axis_pixel_compare__try_xph $c]
		if {$n ne ""} {
			return $n
		}
	}
	# 2) From process cwd (HSI often chdir to platform / domain / misc)
	foreach c [axis_pixel_compare__collect_xph_candidates [pwd] 32] {
		set n [axis_pixel_compare__try_xph $c]
		if {$n ne ""} {
			return $n
		}
	}
	# 3) Legacy relative to cwd (old SDK layout)
	set legacy [file normalize [file join [pwd] .. .. .. include xparameters.h]]
	set n [axis_pixel_compare__try_xph $legacy]
	if {$n ne ""} {
		return $n
	}
	set legacy2 [file normalize [file join [pwd] .. .. .. bspinclude include xparameters.h]]
	return [axis_pixel_compare__try_xph $legacy2]
}

# Leaf BD instance name in UPPER (XPAR_* token), without hierarchy slashes.
proc axis_pixel_compare_periph_up {periph} {
	set n [common::get_property NAME $periph]
	if {$n eq ""} {
		return ""
	}
	if {[string first "/" $n] >= 0} {
		set n [lindex [split $n "/"] end]
	}
	return [string toupper $n]
}

# True if the active software domain targets the PMU (no PL fabric IRQ macros in xparameters).
proc axis_pixel_compare_is_pmu_sw_domain {} {
	if {[catch {set swp [hsi::get_sw_processor]}]} {
		return 0
	}
	set nm [string tolower [common::get_property NAME $swp]]
	if {[string match *pmu* $nm]} {
		return 1
	}
	if {[catch {
		set hw [hsi::get_cells -hier [common::get_property HW_INSTANCE $swp]]
		set ipn [string tolower [common::get_property IP_NAME $hw]]
	}]} {
		return 0
	}
	if {[string match *pmu* $ipn]} {
		return 1
	}
	return 0
}

# True if target is MicroBlaze (INTC VEC_ID compile-time fallback, not FABRIC_*).
proc axis_pixel_compare_is_microblaze_platform {txt} {
	if {$txt ne ""} {
		if {[regexp {\#define[ \t]+XPAR_MICROBLAZE} $txt]} {
			return 1
		}
		if {[regexp {\#define[ \t]+XPAR_[A-Za-z0-9_]*MICROBLAZE_[A-Za-z0-9_]+[ \t]+} $txt]} {
			return 1
		}
		if {[regexp {\#define[ \t]+XPAR_INTC_[0-9]+_DEVICE_ID} $txt]} {
			return 1
		}
	}
	if {[catch {set swp [hsi::get_sw_processor]}]} {
		return 0
	}
	if {[catch {
		set hw [hsi::get_cells -hier [common::get_property HW_INSTANCE $swp]]
		set ipn [string tolower [common::get_property IP_NAME $hw]]
		if {[string match *microblaze* $ipn]} {
			return 1
		}
	}]} {
		return 0
	}
	return 0
}

# XPAR_INTC_0 etc. from partial xparameters.h (present before IRQ vector lines).
proc axis_pixel_compare_get_intc_canonical_prefix {txt} {
	if {$txt ne ""} {
		foreach rawline [split $txt \n] {
			set line [string trimright $rawline "\r"]
			if {[regexp {\#define[ \t]+(XPAR_INTC_[0-9]+)_DEVICE_ID} $line -> prefix]} {
				return $prefix
			}
		}
	}
	return "XPAR_INTC_0"
}

# MicroBlaze canonical vector macro (INTC driver adds #define at end of BSP gen).
proc axis_pixel_compare_mb_vec_id_expr {txt inst_idx} {
	set prefix [axis_pixel_compare_get_intc_canonical_prefix $txt]
	return [format "%s_AXIS_PIXEL_COMPARE_%s_VEC_ID" $prefix $inst_idx]
}

# Platform-specific IntrId when xparameters.h has no IRQ lines yet at HSI time.
proc axis_pixel_compare_intr_fallback_expr {txt up inst_idx} {
	if {[axis_pixel_compare_is_microblaze_platform $txt]} {
		return [axis_pixel_compare_mb_vec_id_expr $txt $inst_idx]
	}
	return "XPAR_FABRIC_${up}_INTR_INTR"
}

# True if xparameters.h contains "#define <sym> <value>" (per-line; CRLF-safe; no whole-file regexp).
proc axis_pixel_compare_xparam_has_define {txt sym} {
	if {$txt eq "" || $sym eq ""} {
		return 0
	}
	foreach rawline [split $txt \n] {
		set line [string trimright $rawline "\r"]
		if {![regexp {\#define[ \t]+([A-Za-z0-9_]+)[ \t]+} $line -> name]} {
			continue
		}
		if {$name eq $sym} {
			return 1
		}
	}
	return 0
}

proc axis_pixel_compare_get_vec_macro_base_from_txt {txt} {
	if {[regexp {\#define[ \t]+(XPAR_INTC[0-9A-Za-z_]*_AXIS_PIXEL_COMPARE)_0_VEC_ID} $txt _m base]} {
		return $base
	}
	if {[regexp {\#define[ \t]+(XPAR_AXI_INTC[0-9A-Za-z_]*_AXIS_PIXEL_COMPARE)_0_VEC_ID} $txt _m base]} {
		return $base
	}
	if {[regexp {\#define[ \t]+(XPAR_FABRIC_AXIS_PIXEL_COMPARE)_0_VEC_ID} $txt _m base]} {
		return $base
	}
	set pairs [regexp -all -inline {\#define[ \t]+(XPAR_[A-Za-z0-9_]+_AXIS_PIXEL_COMPARE)_0_VEC_ID} $txt]
	for {set i 1} {$i < [llength $pairs]} {incr i 2} {
		set base [lindex $pairs $i]
		if {[string match *PROCESSOR_SUBSYSTEM* $base] || [string match *MICROBLAZE* $base]} {
			continue
		}
		return $base
	}
	if {[axis_pixel_compare_is_microblaze_platform $txt]} {
		return [format "%s_AXIS_PIXEL_COMPARE" [axis_pixel_compare_get_intc_canonical_prefix $txt]]
	}
	return "XPAR_FABRIC_AXIS_PIXEL_COMPARE"
}

# MicroBlaze (etc.): BSP emits XPAR_<...>_AXI_INTC_<UP>_INTR_INTR — no FABRIC_*.
# Use per-line #define + glob on symbol (HSI Tcl regexp with long anchored patterns can miss).
proc axis_pixel_compare_axi_intc_intr_sym {txt up} {
	if {$txt eq "" || $up eq ""} {
		return ""
	}
	set needle "AXI_INTC_${up}_INTR_INTR"
	foreach rawline [split $txt \n] {
		set line [string trimright $rawline "\r"]
		if {![regexp {\#define[ \t]+(XPAR_[A-Za-z0-9_]+)[ \t]+} $line -> sym]} {
			continue
		}
		if {[string match *$needle $sym]} {
			return $sym
		}
	}
	return ""
}

# Tcl 8.5-safe line scan (no "regexp -> var")
proc axis_pixel_compare_intr_sym_by_up_scan {txt up} {
	if {$txt eq "" || $up eq ""} {
		return ""
	}
	set best ""
	set bestpri 99
	foreach rawline [split $txt \n] {
		set line [string trimright $rawline "\r"]
		if {![regexp {\#define[ \t]+(XPAR_[A-Za-z0-9_]+)[ \t]+} $line full sym]} {
			continue
		}
		if {[string first $up $sym] < 0} {
			continue
		}
		set pri 99
		if {[string match *_INTR_INTR $sym]} {
			if {[string match *AXI_INTC* $sym]} {
				set pri 0
			} else {
				set pri 1
			}
		} elseif {[string match *_INTERRUPT_INTR $sym]} {
			set pri 2
		} elseif {[string match *_IP2INTC_IRPT_INTR $sym]} {
			set pri 3
		} elseif {[string match *_INTERRUPT_ID $sym]} {
			set pri 4
		} elseif {[string match *_VEC_ID $sym]} {
			set pri 5
		} else {
			continue
		}
		if {$pri < $bestpri} {
			set bestpri $pri
			set best $sym
		}
	}
	return $best
}

proc axis_pixel_compare_intr_c_expr {txt periph inst_idx} {
	set up [axis_pixel_compare_periph_up $periph]
	if {[axis_pixel_compare_is_pmu_sw_domain]} {
		return "0U"
	}
	set s_axi_intc [axis_pixel_compare_axi_intc_intr_sym $txt $up]
	if {$s_axi_intc ne ""} {
		return $s_axi_intc
	}
	# VEC_ID (e.g. XPAR_INTC_0_AXIS_PIXEL_COMPARE_n_VEC_ID) before FABRIC: MicroBlaze BSP
	# always has these when INTC is connected; avoids FABRIC fallback if AXI_INTC line scan fails.
	set s_alias [format "XPAR_FABRIC_AXIS_PIXEL_COMPARE_%s_VEC_ID" $inst_idx]
	if {[axis_pixel_compare_xparam_has_define $txt $s_alias]} {
		return $s_alias
	}
	set vec_base [axis_pixel_compare_get_vec_macro_base_from_txt $txt]
	set s_vec [format "%s_%s_VEC_ID" $vec_base $inst_idx]
	if {[axis_pixel_compare_xparam_has_define $txt $s_vec]} {
		return $s_vec
	}
	if {[axis_pixel_compare_is_microblaze_platform $txt]} {
		return $s_vec
	}
	set s_fabric_intr "XPAR_FABRIC_${up}_INTR_INTR"
	if {[axis_pixel_compare_xparam_has_define $txt $s_fabric_intr]} {
		return $s_fabric_intr
	}
	set s_fabric_alt "XPAR_FABRIC_${up}_INTERRUPT_INTR"
	if {[axis_pixel_compare_xparam_has_define $txt $s_fabric_alt]} {
		return $s_fabric_alt
	}
	set s_ip2 "XPAR_FABRIC_${up}_IP2INTC_IRPT_INTR"
	if {[axis_pixel_compare_xparam_has_define $txt $s_ip2]} {
		return $s_ip2
	}
	set scanned [axis_pixel_compare_intr_sym_by_up_scan $txt $up]
	if {$scanned ne ""} {
		return $scanned
	}
	return [axis_pixel_compare_intr_fallback_expr $txt $up $inst_idx]
}

proc axis_pixel_compare_write_g_c {drv_handle} {
	if {[catch {set periphs [hsi::utils::get_common_driver_ips $drv_handle]}]} {
		set periphs [xget_sw_iplist_for_driver $drv_handle]
	}
	set xph [axis_pixel_compare_find_xparameters_h]
	set txt ""
	if {$xph ne "" && [file isfile $xph]} {
		set fd [open $xph r]
		set txt [read $fd]
		close $fd
	} else {
		puts "WARNING: axis_pixel_compare.tcl: xparameters.h not found (IntrId -> 0U). Tried include/ and bspinclude/include/ from \[info script\] and \[pwd\]."
	}

	set filename [file join "src" "axis_pixel_compare_g.c"]
	set f [open $filename w]
	puts $f "/*******************************************************************"
	puts $f "* CAUTION: This file is automatically generated by HSI (axis_pixel_compare.tcl)."
	puts $f "* DO NOT EDIT."
	puts $f "*"
	puts $f "* Description: axis_pixel_compare driver configuration (DeviceId, BaseAddr, IntrId)."
	puts $f "* IntrId: MB->XPAR_INTC_*_AXIS_PIXEL_COMPARE_*_VEC_ID; MPSoC->XPAR_FABRIC_* (see axis_pixel_compare.tcl)."
	if {$xph ne ""} {
		puts $f "* xparameters.h: $xph"
	}
	puts $f "*******************************************************************/"
	puts $f ""
	puts $f "\#include \"xparameters.h\""
	puts $f "\#include \"axis_pixel_compare.h\""
	puts $f ""
	puts $f "/* Configuration table: one entry per axis_pixel_compare instance in the design. */"
	puts $f "XAxisPixelCompare_Config XAxisPixelCompare_ConfigTable\[XPAR_AXI_PIXEL_COMPARE_NUM_INSTANCES\] ="
	puts $f "{"

	set comma ""
	set inst_idx 0
	foreach periph $periphs {
		set up [axis_pixel_compare_periph_up $periph]
		set intr_expr [axis_pixel_compare_intr_c_expr $txt $periph $inst_idx]
		puts $f "${comma}\t{"
		puts $f "\t\tXPAR_${up}_DEVICE_ID,"
		puts $f "\t\tXPAR_${up}_S00_AXI_BASEADDR,"
		puts $f "\t\t${intr_expr}"
		puts $f "\t}"
		set comma ","
		incr inst_idx
	}

	puts $f "};"
	puts $f ""
	close $f
}


proc generate {drv_handle} {
	xdefine_include_file $drv_handle "xparameters.h" "AXI_PIXEL_COMPARE" \
		"NUM_INSTANCES" "DEVICE_ID" "C_S00_AXI_BASEADDR" "C_S00_AXI_HIGHADDR"
	axis_pixel_compare_write_g_c $drv_handle
}
