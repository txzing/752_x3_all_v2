transcript on
#compile
if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vlog -vlog01compat -work work {./vid_4to1_multiplexer.v}
vlog -vlog01compat -work work {./vid_4to1_multiplexer_tb.v}

#simulate
vsim -novopt vid_4to1_multiplexer_tb
#vsim -voptargs="+acc" vid_4to1_multiplexer_tb

add wave -radix hexadecimal /vid_4to1_multiplexer_tb/*
view structure
view signals

run -all
