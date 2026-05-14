# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "CHANNEL_IN_W" -parent ${Page_0}
  ipgui::add_param $IPINST -name "CHANNEL_OUT_W" -parent ${Page_0}
  ipgui::add_param $IPINST -name "FIFO_D" -parent ${Page_0}


}

proc update_PARAM_VALUE.CHANNEL_IN_W { PARAM_VALUE.CHANNEL_IN_W } {
	# Procedure called to update CHANNEL_IN_W when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.CHANNEL_IN_W { PARAM_VALUE.CHANNEL_IN_W } {
	# Procedure called to validate CHANNEL_IN_W
	return true
}

proc update_PARAM_VALUE.CHANNEL_OUT_W { PARAM_VALUE.CHANNEL_OUT_W } {
	# Procedure called to update CHANNEL_OUT_W when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.CHANNEL_OUT_W { PARAM_VALUE.CHANNEL_OUT_W } {
	# Procedure called to validate CHANNEL_OUT_W
	return true
}

proc update_PARAM_VALUE.FIFO_D { PARAM_VALUE.FIFO_D } {
	# Procedure called to update FIFO_D when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.FIFO_D { PARAM_VALUE.FIFO_D } {
	# Procedure called to validate FIFO_D
	return true
}


proc update_MODELPARAM_VALUE.CHANNEL_IN_W { MODELPARAM_VALUE.CHANNEL_IN_W PARAM_VALUE.CHANNEL_IN_W } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.CHANNEL_IN_W}] ${MODELPARAM_VALUE.CHANNEL_IN_W}
}

proc update_MODELPARAM_VALUE.CHANNEL_OUT_W { MODELPARAM_VALUE.CHANNEL_OUT_W PARAM_VALUE.CHANNEL_OUT_W } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.CHANNEL_OUT_W}] ${MODELPARAM_VALUE.CHANNEL_OUT_W}
}

proc update_MODELPARAM_VALUE.FIFO_D { MODELPARAM_VALUE.FIFO_D PARAM_VALUE.FIFO_D } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.FIFO_D}] ${MODELPARAM_VALUE.FIFO_D}
}

