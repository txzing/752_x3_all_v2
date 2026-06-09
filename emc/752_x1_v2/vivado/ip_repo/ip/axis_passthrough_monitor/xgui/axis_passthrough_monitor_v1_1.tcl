# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "AXIS_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PPC" -parent ${Page_0} -widget comboBox
  ipgui::add_param $IPINST -name "TUSER_WIDTH" -parent ${Page_0}

  ipgui::add_param $IPINST -name "C_S_AXI_LITE_DATA_WIDTH" -widget comboBox
  ipgui::add_param $IPINST -name "C_S_AXI_LITE_HIGHADDR"
  ipgui::add_param $IPINST -name "C_S_AXI_LITE_BASEADDR"
  ipgui::add_param $IPINST -name "C_S_AXI_LITE_ADDR_WIDTH"

}

proc update_PARAM_VALUE.AXIS_WIDTH { PARAM_VALUE.AXIS_WIDTH } {
	# Procedure called to update AXIS_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.AXIS_WIDTH { PARAM_VALUE.AXIS_WIDTH } {
	# Procedure called to validate AXIS_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH } {
	# Procedure called to update C_S_AXI_LITE_ADDR_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH } {
	# Procedure called to validate C_S_AXI_LITE_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_BASEADDR { PARAM_VALUE.C_S_AXI_LITE_BASEADDR } {
	# Procedure called to update C_S_AXI_LITE_BASEADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_BASEADDR { PARAM_VALUE.C_S_AXI_LITE_BASEADDR } {
	# Procedure called to validate C_S_AXI_LITE_BASEADDR
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH { PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH } {
	# Procedure called to update C_S_AXI_LITE_DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH { PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH } {
	# Procedure called to validate C_S_AXI_LITE_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_LITE_HIGHADDR { PARAM_VALUE.C_S_AXI_LITE_HIGHADDR } {
	# Procedure called to update C_S_AXI_LITE_HIGHADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S_AXI_LITE_HIGHADDR { PARAM_VALUE.C_S_AXI_LITE_HIGHADDR } {
	# Procedure called to validate C_S_AXI_LITE_HIGHADDR
	return true
}

proc update_PARAM_VALUE.FREQ_HZ { PARAM_VALUE.FREQ_HZ } {
	# Procedure called to update FREQ_HZ when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.FREQ_HZ { PARAM_VALUE.FREQ_HZ } {
	# Procedure called to validate FREQ_HZ
	return true
}

proc update_PARAM_VALUE.PPC { PARAM_VALUE.PPC } {
	# Procedure called to update PPC when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PPC { PARAM_VALUE.PPC } {
	# Procedure called to validate PPC
	return true
}

proc update_PARAM_VALUE.TUSER_WIDTH { PARAM_VALUE.TUSER_WIDTH } {
	# Procedure called to update TUSER_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.TUSER_WIDTH { PARAM_VALUE.TUSER_WIDTH } {
	# Procedure called to validate TUSER_WIDTH
	return true
}


proc update_MODELPARAM_VALUE.AXIS_WIDTH { MODELPARAM_VALUE.AXIS_WIDTH PARAM_VALUE.AXIS_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.AXIS_WIDTH}] ${MODELPARAM_VALUE.AXIS_WIDTH}
}

proc update_MODELPARAM_VALUE.TUSER_WIDTH { MODELPARAM_VALUE.TUSER_WIDTH PARAM_VALUE.TUSER_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.TUSER_WIDTH}] ${MODELPARAM_VALUE.TUSER_WIDTH}
}

proc update_MODELPARAM_VALUE.TDEST_WIDTH { MODELPARAM_VALUE.TDEST_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "TDEST_WIDTH". Setting updated value from the model parameter.
set_property value 1 ${MODELPARAM_VALUE.TDEST_WIDTH}
}

proc update_MODELPARAM_VALUE.TKEEP_WIDTH { MODELPARAM_VALUE.TKEEP_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "TKEEP_WIDTH". Setting updated value from the model parameter.
set_property value 1 ${MODELPARAM_VALUE.TKEEP_WIDTH}
}

proc update_MODELPARAM_VALUE.TSTRB_WIDTH { MODELPARAM_VALUE.TSTRB_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "TSTRB_WIDTH". Setting updated value from the model parameter.
set_property value 1 ${MODELPARAM_VALUE.TSTRB_WIDTH}
}

proc update_MODELPARAM_VALUE.TID_WIDTH { MODELPARAM_VALUE.TID_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "TID_WIDTH". Setting updated value from the model parameter.
set_property value 1 ${MODELPARAM_VALUE.TID_WIDTH}
}

proc update_MODELPARAM_VALUE.FREQ_HZ { MODELPARAM_VALUE.FREQ_HZ PARAM_VALUE.FREQ_HZ } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.FREQ_HZ}] ${MODELPARAM_VALUE.FREQ_HZ}
}

proc update_MODELPARAM_VALUE.PPC { MODELPARAM_VALUE.PPC PARAM_VALUE.PPC } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PPC}] ${MODELPARAM_VALUE.PPC}
}

proc update_MODELPARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH { MODELPARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_LITE_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH { MODELPARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_LITE_ADDR_WIDTH}
}

