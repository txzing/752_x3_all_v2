
proc init { cellpath otherInfo } {                                                                   

	put "lvds4x2_1to7:init"
	set cell_handle [get_bd_cells $cellpath]                                                                 
	set refpin [get_bd_pins $cellpath/refclk]		                                                     
			                                                                                                 
	set_property CONFIG.FREQ_HZ.VALUE_SRC PROPAGATED $refpin
    #get_property config.FREQ_HZ [find_bd_objs –relation connected_to $refpin]   
    #set_property config.FREQ_HZ [get_property config.FREQ_HZ [find_bd_objs –relation connected_to $refpin]] $refpin
    #set_property config.REF_FREQ [expr [get_property config.FREQ_HZ $refpin]/1000000] $cell_handle
    put [format %s%s "config.REF_FREQ=" [get_property config.REF_FREQ $cell_handle]]
    
}


proc pre_propagate {cellpath otherInfo } {                                                           
                                                                                                             
	put "lvds4x2_1to7:pre_propagate"
	set cell_handle [get_bd_cells $cellpath]                                                                 
	set refpin [get_bd_pins $cellpath/refclk]		                                                     
			                                                                                                 
	set_property CONFIG.FREQ_HZ.VALUE_SRC PROPAGATED $refpin
    #get_property config.FREQ_HZ [find_bd_objs –relation connected_to $refpin]   
    #set_property config.FREQ_HZ [get_property config.FREQ_HZ [find_bd_objs –relation connected_to $refpin]] $refpin
    #put [format %s%s "config.FREQ_HZ=" [get_property config.FREQ_HZ $refpin]]
    if { [string equal [get_property config.FREQ_HZ $refpin] ""] != 1 } {
    	set_property config.REF_FREQ [expr [get_property config.FREQ_HZ $refpin]/1000000] $cell_handle
	put [format %s%s "config.REF_FREQ=" [get_property config.REF_FREQ $cell_handle]]
    }
}


proc propagate {cellpath otherInfo } {                                                               
                                                                                                             
	put "lvds4x2_1to7:propagate"
	set cell_handle [get_bd_cells $cellpath]                                                                 
	set refpin [get_bd_pins $cellpath/refclk]		                                                     
			                                                                                                 
	set_property CONFIG.FREQ_HZ.VALUE_SRC PROPAGATED $refpin
    #get_property config.FREQ_HZ [find_bd_objs –relation connected_to $refpin]   
    #set_property config.FREQ_HZ [get_property config.FREQ_HZ [find_bd_objs –relation connected_to $refpin]] $refpin
    set_property config.REF_FREQ [expr [get_property config.FREQ_HZ $refpin]/1000000] $cell_handle
    put [format %s%s "config.REF_FREQ=" [get_property config.REF_FREQ $cell_handle]]
}

