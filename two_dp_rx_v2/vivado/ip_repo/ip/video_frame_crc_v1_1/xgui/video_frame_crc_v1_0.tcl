# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  set BPC [ipgui::add_param $IPINST -name "BPC" -parent ${Page_0} -widget comboBox]
  set_property tooltip {Component Bits Per Color} ${BPC}
  #set Pixel_Per_Clock [ipgui::add_param $IPINST -name "Pixel_Per_Clock" -parent ${Page_0} -widget comboBox]
  #set_property tooltip {Pixels Per Clock} ${Pixel_Per_Clock}


}

proc update_PARAM_VALUE.BPC { PARAM_VALUE.BPC } {
	# Procedure called to update BPC when any of the dependent parameters in the arguments change
}

proc update_MODELPARAM_VALUE.C_PPC_MODE { MODELPARAM_VALUE.C_PPC_MODE PARAM_VALUE.Pixel_Per_Clock } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value  [get_property value ${PARAM_VALUE.Pixel_Per_Clock}] ${MODELPARAM_VALUE.C_PPC_MODE}
}

proc update_MODELPARAM_VALUE.C_Vid_In_AXIS_TDATA_WIDTH { MODELPARAM_VALUE.C_Vid_In_AXIS_TDATA_WIDTH PARAM_VALUE.Pixel_Per_Clock PARAM_VALUE.BPC } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	#set_property value  [expr {([get_property value ${PARAM_VALUE.BPC}]*[get_property value ${PARAM_VALUE.Pixel_Per_Clock}]) * 3}] ${MODELPARAM_VALUE.C_Vid_In_AXIS_TDATA_WIDTH}
	set_property value  [expr {([get_property value ${PARAM_VALUE.BPC}]*4) * 3}] ${MODELPARAM_VALUE.C_Vid_In_AXIS_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_Vid_Out_AXIS_TDATA_WIDTH { MODELPARAM_VALUE.C_Vid_Out_AXIS_TDATA_WIDTH PARAM_VALUE.Pixel_Per_Clock PARAM_VALUE.BPC } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	#set_property value  [expr {([get_property value ${PARAM_VALUE.BPC}]*[get_property value ${PARAM_VALUE.Pixel_Per_Clock}]) * 3}] ${MODELPARAM_VALUE.C_Vid_Out_AXIS_TDATA_WIDTH}
	set_property value  [expr {([get_property value ${PARAM_VALUE.BPC}]*4) * 3}] ${MODELPARAM_VALUE.C_Vid_Out_AXIS_TDATA_WIDTH}
}


