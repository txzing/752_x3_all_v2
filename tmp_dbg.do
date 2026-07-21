vlib work
vlog -work work ../hdl/AXI_LITE_REG_v1_0_S00_AXI.v ../hdl/axis_pixel_compare.v tb_axis_pixel_compare_stress.v
vsim -c -voptargs=+acc work.tb_axis_pixel_compare_stress -do "run 25us; echo COL_R=[examine -hex /tb_axis_pixel_compare_stress/dut/err_col_r]; echo COL=[examine -hex /tb_axis_pixel_compare_stress/dut/err_col]; echo LATCH=[examine /tb_axis_pixel_compare_stress/dut/err_latched]; echo MIDQ=[examine /tb_axis_pixel_compare_stress/dut/mid_err_beat_q]; echo CNT=[examine -hex /tb_axis_pixel_compare_stress/dut/err_pixel_cnt_frame]; quit -f"
