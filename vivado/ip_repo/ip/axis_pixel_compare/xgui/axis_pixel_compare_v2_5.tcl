# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PPC" -parent ${Page_0} -widget comboBox
  ipgui::add_param $IPINST -name "TUSER_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "ROI_RST_XS" -parent ${Page_0}
  ipgui::add_param $IPINST -name "ROI_RST_XE" -parent ${Page_0}
  ipgui::add_param $IPINST -name "ROI_RST_YS" -parent ${Page_0}
  ipgui::add_param $IPINST -name "ROI_RST_YE" -parent ${Page_0}


}

proc update_PARAM_VALUE.C_S00_AXI_ADDR_WIDTH { PARAM_VALUE.C_S00_AXI_ADDR_WIDTH } {
	# Procedure called to update C_S00_AXI_ADDR_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S00_AXI_ADDR_WIDTH { PARAM_VALUE.C_S00_AXI_ADDR_WIDTH } {
	# Procedure called to validate C_S00_AXI_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S00_AXI_BASEADDR { PARAM_VALUE.C_S00_AXI_BASEADDR } {
	# Procedure called to update C_S00_AXI_BASEADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S00_AXI_BASEADDR { PARAM_VALUE.C_S00_AXI_BASEADDR } {
	# Procedure called to validate C_S00_AXI_BASEADDR
	return true
}

proc update_PARAM_VALUE.C_S00_AXI_DATA_WIDTH { PARAM_VALUE.C_S00_AXI_DATA_WIDTH } {
	# Procedure called to update C_S00_AXI_DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S00_AXI_DATA_WIDTH { PARAM_VALUE.C_S00_AXI_DATA_WIDTH } {
	# Procedure called to validate C_S00_AXI_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S00_AXI_HIGHADDR { PARAM_VALUE.C_S00_AXI_HIGHADDR } {
	# Procedure called to update C_S00_AXI_HIGHADDR when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S00_AXI_HIGHADDR { PARAM_VALUE.C_S00_AXI_HIGHADDR } {
	# Procedure called to validate C_S00_AXI_HIGHADDR
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

proc update_PARAM_VALUE.ROI_RST_XE { PARAM_VALUE.ROI_RST_XE } {
	# Procedure called to update ROI_RST_XE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ROI_RST_XE { PARAM_VALUE.ROI_RST_XE } {
	# Procedure called to validate ROI_RST_XE
	return true
}

proc update_PARAM_VALUE.ROI_RST_XS { PARAM_VALUE.ROI_RST_XS } {
	# Procedure called to update ROI_RST_XS when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ROI_RST_XS { PARAM_VALUE.ROI_RST_XS } {
	# Procedure called to validate ROI_RST_XS
	return true
}

proc update_PARAM_VALUE.ROI_RST_YE { PARAM_VALUE.ROI_RST_YE } {
	# Procedure called to update ROI_RST_YE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ROI_RST_YE { PARAM_VALUE.ROI_RST_YE } {
	# Procedure called to validate ROI_RST_YE
	return true
}

proc update_PARAM_VALUE.ROI_RST_YS { PARAM_VALUE.ROI_RST_YS } {
	# Procedure called to update ROI_RST_YS when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ROI_RST_YS { PARAM_VALUE.ROI_RST_YS } {
	# Procedure called to validate ROI_RST_YS
	return true
}

proc update_PARAM_VALUE.TUSER_WIDTH { PARAM_VALUE.TUSER_WIDTH } {
	# Procedure called to update TUSER_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.TUSER_WIDTH { PARAM_VALUE.TUSER_WIDTH } {
	# Procedure called to validate TUSER_WIDTH
	return true
}

proc update_PARAM_VALUE.WIDTH { PARAM_VALUE.WIDTH } {
	# Procedure called to update WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.WIDTH { PARAM_VALUE.WIDTH } {
	# Procedure called to validate WIDTH
	return true
}


proc update_MODELPARAM_VALUE.WIDTH { MODELPARAM_VALUE.WIDTH PARAM_VALUE.WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.WIDTH}] ${MODELPARAM_VALUE.WIDTH}
}

proc update_MODELPARAM_VALUE.TUSER_WIDTH { MODELPARAM_VALUE.TUSER_WIDTH PARAM_VALUE.TUSER_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.TUSER_WIDTH}] ${MODELPARAM_VALUE.TUSER_WIDTH}
}

proc update_MODELPARAM_VALUE.PPC { MODELPARAM_VALUE.PPC PARAM_VALUE.PPC } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PPC}] ${MODELPARAM_VALUE.PPC}
}

proc update_MODELPARAM_VALUE.FREQ_HZ { MODELPARAM_VALUE.FREQ_HZ PARAM_VALUE.FREQ_HZ } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.FREQ_HZ}] ${MODELPARAM_VALUE.FREQ_HZ}
}

proc update_MODELPARAM_VALUE.ROI_RST_XS { MODELPARAM_VALUE.ROI_RST_XS PARAM_VALUE.ROI_RST_XS } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ROI_RST_XS}] ${MODELPARAM_VALUE.ROI_RST_XS}
}

proc update_MODELPARAM_VALUE.ROI_RST_XE { MODELPARAM_VALUE.ROI_RST_XE PARAM_VALUE.ROI_RST_XE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ROI_RST_XE}] ${MODELPARAM_VALUE.ROI_RST_XE}
}

proc update_MODELPARAM_VALUE.ROI_RST_YS { MODELPARAM_VALUE.ROI_RST_YS PARAM_VALUE.ROI_RST_YS } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ROI_RST_YS}] ${MODELPARAM_VALUE.ROI_RST_YS}
}

proc update_MODELPARAM_VALUE.ROI_RST_YE { MODELPARAM_VALUE.ROI_RST_YE PARAM_VALUE.ROI_RST_YE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ROI_RST_YE}] ${MODELPARAM_VALUE.ROI_RST_YE}
}

proc update_MODELPARAM_VALUE.C_S00_AXI_DATA_WIDTH { MODELPARAM_VALUE.C_S00_AXI_DATA_WIDTH PARAM_VALUE.C_S00_AXI_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S00_AXI_DATA_WIDTH}] ${MODELPARAM_VALUE.C_S00_AXI_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S00_AXI_ADDR_WIDTH { MODELPARAM_VALUE.C_S00_AXI_ADDR_WIDTH PARAM_VALUE.C_S00_AXI_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S00_AXI_ADDR_WIDTH}] ${MODELPARAM_VALUE.C_S00_AXI_ADDR_WIDTH}
}

