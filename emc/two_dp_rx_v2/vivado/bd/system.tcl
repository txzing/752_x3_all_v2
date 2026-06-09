
################################################################
# This is a generated script based on design: system
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2020.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source system_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xcku040-ffva1156-2-i
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name system

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_gid_msg -ssname BD::TCL -id 2005 -severity "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_gid_msg -ssname BD::TCL -id 2006 -severity "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:xlconstant:1.1\
xilinx.com:ip:axi_gpio:2.0\
xilinx.com:ip:axi_vdma:6.3\
xilinx.com:user:axis_passthrough_monitor:1.1\
xilinx.com:ip:axis_switch:1.1\
xilinx.com:ip:vid_phy_controller:2.2\
xilinx.com:ip:xlslice:1.0\
xilinx.com:ip:util_reduced_logic:2.0\
xilinx.com:ip:v_dp_rxss1:2.1\
xilinx.com:ip:vid_edid:1.1\
xilinx.com:ip:xlconcat:2.1\
xilinx.com:ip:axi_dma:7.1\
xilinx.com:ip:axi_ethernet:7.2\
xilinx.com:hls:AXI4_Stream_Terminator:1.0\
xilinx.com:ip:axis_broadcaster:1.1\
xilinx.com:user:axis_combiner:1.1\
xilinx.com:user:axis_pixel_compare:2.19\
xilinx.com:ip:axis_subset_converter:1.1\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:util_vector_logic:2.0\
xilinx.com:user:AXI_LITE_REG:1.0\
xilinx.com:ip:axi_quad_spi:3.2\
xilinx.com:ip:smartconnect:1.0\
xilinx.com:ip:axi_timer:2.0\
xilinx.com:ip:axi_uartlite:2.0\
xilinx.com:ip:clk_wiz:6.0\
xilinx.com:ip:ddr4:2.2\
xilinx.com:ip:mdm:3.2\
xilinx.com:ip:microblaze:11.0\
xilinx.com:ip:axi_intc:4.1\
alinx.com.cn:user:xgpio_to_i2c:1.0\
xilinx.com:ip:lmb_bram_if_cntlr:4.0\
xilinx.com:ip:lmb_v10:3.0\
xilinx.com:ip:blk_mem_gen:8.4\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################


# Hierarchical cell: microblaze_0_local_memory
proc create_hier_cell_microblaze_0_local_memory { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_microblaze_0_local_memory() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode MirroredMaster -vlnv xilinx.com:interface:lmb_rtl:1.0 DLMB

  create_bd_intf_pin -mode MirroredMaster -vlnv xilinx.com:interface:lmb_rtl:1.0 ILMB


  # Create pins
  create_bd_pin -dir I -type clk LMB_Clk
  create_bd_pin -dir I -type rst SYS_Rst

  # Create instance: dlmb_bram_if_cntlr, and set properties
  set dlmb_bram_if_cntlr [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_bram_if_cntlr:4.0 dlmb_bram_if_cntlr ]
  set_property -dict [ list \
   CONFIG.C_ECC {0} \
 ] $dlmb_bram_if_cntlr

  # Create instance: dlmb_v10, and set properties
  set dlmb_v10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_v10:3.0 dlmb_v10 ]

  # Create instance: ilmb_bram_if_cntlr, and set properties
  set ilmb_bram_if_cntlr [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_bram_if_cntlr:4.0 ilmb_bram_if_cntlr ]
  set_property -dict [ list \
   CONFIG.C_ECC {0} \
 ] $ilmb_bram_if_cntlr

  # Create instance: ilmb_v10, and set properties
  set ilmb_v10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_v10:3.0 ilmb_v10 ]

  # Create instance: lmb_bram, and set properties
  set lmb_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 lmb_bram ]
  set_property -dict [ list \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Use_RSTB_Pin {true} \
   CONFIG.use_bram_block {BRAM_Controller} \
 ] $lmb_bram

  # Create interface connections
  connect_bd_intf_net -intf_net microblaze_0_DLMB [get_bd_intf_pins DLMB] [get_bd_intf_pins dlmb_v10/LMB_M]
  connect_bd_intf_net -intf_net microblaze_0_ILMB [get_bd_intf_pins ILMB] [get_bd_intf_pins ilmb_v10/LMB_M]
  connect_bd_intf_net -intf_net microblaze_0_dlmb_bus [get_bd_intf_pins dlmb_bram_if_cntlr/SLMB] [get_bd_intf_pins dlmb_v10/LMB_Sl_0]
  connect_bd_intf_net -intf_net microblaze_0_dlmb_cntlr [get_bd_intf_pins dlmb_bram_if_cntlr/BRAM_PORT] [get_bd_intf_pins lmb_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net microblaze_0_ilmb_bus [get_bd_intf_pins ilmb_bram_if_cntlr/SLMB] [get_bd_intf_pins ilmb_v10/LMB_Sl_0]
  connect_bd_intf_net -intf_net microblaze_0_ilmb_cntlr [get_bd_intf_pins ilmb_bram_if_cntlr/BRAM_PORT] [get_bd_intf_pins lmb_bram/BRAM_PORTB]

  # Create port connections
  connect_bd_net -net SYS_Rst_1 [get_bd_pins SYS_Rst] [get_bd_pins dlmb_bram_if_cntlr/LMB_Rst] [get_bd_pins dlmb_v10/SYS_Rst] [get_bd_pins ilmb_bram_if_cntlr/LMB_Rst] [get_bd_pins ilmb_v10/SYS_Rst]
  connect_bd_net -net microblaze_0_Clk [get_bd_pins LMB_Clk] [get_bd_pins dlmb_bram_if_cntlr/LMB_Clk] [get_bd_pins dlmb_v10/LMB_Clk] [get_bd_pins ilmb_bram_if_cntlr/LMB_Clk] [get_bd_pins ilmb_v10/LMB_Clk]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: xgpio_i2c_0
proc create_hier_cell_xgpio_i2c_0 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_xgpio_i2c_0() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_0

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_1

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_2

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_3

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_4


  # Create pins
  create_bd_pin -dir I -type clk s_axi_aclk
  create_bd_pin -dir I -type rst s_axi_aresetn

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]
  set_property -dict [ list \
   CONFIG.C_ALL_OUTPUTS_2 {0} \
   CONFIG.C_GPIO2_WIDTH {32} \
   CONFIG.C_GPIO_WIDTH {10} \
   CONFIG.C_IS_DUAL {0} \
 ] $axi_gpio_0

  # Create instance: xgpio_to_i2c_0, and set properties
  set xgpio_to_i2c_0 [ create_bd_cell -type ip -vlnv alinx.com.cn:user:xgpio_to_i2c:1.0 xgpio_to_i2c_0 ]

  # Create instance: xgpio_to_i2c_1, and set properties
  set xgpio_to_i2c_1 [ create_bd_cell -type ip -vlnv alinx.com.cn:user:xgpio_to_i2c:1.0 xgpio_to_i2c_1 ]

  # Create instance: xgpio_to_i2c_2, and set properties
  set xgpio_to_i2c_2 [ create_bd_cell -type ip -vlnv alinx.com.cn:user:xgpio_to_i2c:1.0 xgpio_to_i2c_2 ]

  # Create instance: xgpio_to_i2c_3, and set properties
  set xgpio_to_i2c_3 [ create_bd_cell -type ip -vlnv alinx.com.cn:user:xgpio_to_i2c:1.0 xgpio_to_i2c_3 ]

  # Create instance: xgpio_to_i2c_4, and set properties
  set xgpio_to_i2c_4 [ create_bd_cell -type ip -vlnv alinx.com.cn:user:xgpio_to_i2c:1.0 xgpio_to_i2c_4 ]

  # Create instance: xlconcat_1, and set properties
  set xlconcat_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_1 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {10} \
 ] $xlconcat_1

  # Create instance: xlslice_O0, and set properties
  set xlslice_O0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {0} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O0

  # Create instance: xlslice_O1, and set properties
  set xlslice_O1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {1} \
   CONFIG.DIN_TO {1} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O1

  # Create instance: xlslice_O2, and set properties
  set xlslice_O2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O2 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {2} \
   CONFIG.DIN_TO {2} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O2

  # Create instance: xlslice_O3, and set properties
  set xlslice_O3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O3 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {3} \
   CONFIG.DIN_TO {3} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O3

  # Create instance: xlslice_O4, and set properties
  set xlslice_O4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O4 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {4} \
   CONFIG.DIN_TO {4} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O4

  # Create instance: xlslice_O5, and set properties
  set xlslice_O5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O5 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {5} \
   CONFIG.DIN_TO {5} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O5

  # Create instance: xlslice_O6, and set properties
  set xlslice_O6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O6 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {6} \
   CONFIG.DIN_TO {6} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O6

  # Create instance: xlslice_O7, and set properties
  set xlslice_O7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O7 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DIN_TO {7} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O7

  # Create instance: xlslice_O8, and set properties
  set xlslice_O8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O8 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {8} \
   CONFIG.DIN_TO {8} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O8

  # Create instance: xlslice_O9, and set properties
  set xlslice_O9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_O9 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {9} \
   CONFIG.DIN_TO {9} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_O9

  # Create instance: xlslice_T0, and set properties
  set xlslice_T0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {0} \
   CONFIG.DIN_TO {0} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T0

  # Create instance: xlslice_T1, and set properties
  set xlslice_T1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {1} \
   CONFIG.DIN_TO {1} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T1

  # Create instance: xlslice_T2, and set properties
  set xlslice_T2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T2 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {2} \
   CONFIG.DIN_TO {2} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T2

  # Create instance: xlslice_T3, and set properties
  set xlslice_T3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T3 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {3} \
   CONFIG.DIN_TO {3} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T3

  # Create instance: xlslice_T4, and set properties
  set xlslice_T4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T4 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {4} \
   CONFIG.DIN_TO {4} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T4

  # Create instance: xlslice_T5, and set properties
  set xlslice_T5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T5 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {5} \
   CONFIG.DIN_TO {5} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T5

  # Create instance: xlslice_T6, and set properties
  set xlslice_T6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T6 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {6} \
   CONFIG.DIN_TO {6} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T6

  # Create instance: xlslice_T7, and set properties
  set xlslice_T7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T7 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {7} \
   CONFIG.DIN_TO {7} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T7

  # Create instance: xlslice_T8, and set properties
  set xlslice_T8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T8 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {8} \
   CONFIG.DIN_TO {8} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T8

  # Create instance: xlslice_T9, and set properties
  set xlslice_T9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_T9 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {9} \
   CONFIG.DIN_TO {9} \
   CONFIG.DIN_WIDTH {10} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_T9

  # Create interface connections
  connect_bd_intf_net -intf_net S_AXI [get_bd_intf_pins S_AXI] [get_bd_intf_pins axi_gpio_0/S_AXI]
  connect_bd_intf_net -intf_net xgpio_to_i2c_0_i2c_down [get_bd_intf_pins i2c_0] [get_bd_intf_pins xgpio_to_i2c_0/i2c_down]
  connect_bd_intf_net -intf_net xgpio_to_i2c_1_i2c_down [get_bd_intf_pins i2c_1] [get_bd_intf_pins xgpio_to_i2c_1/i2c_down]
  connect_bd_intf_net -intf_net xgpio_to_i2c_2_i2c_down [get_bd_intf_pins i2c_2] [get_bd_intf_pins xgpio_to_i2c_2/i2c_down]
  connect_bd_intf_net -intf_net xgpio_to_i2c_3_i2c_down [get_bd_intf_pins i2c_3] [get_bd_intf_pins xgpio_to_i2c_3/i2c_down]
  connect_bd_intf_net -intf_net xgpio_to_i2c_4_i2c_down [get_bd_intf_pins i2c_4] [get_bd_intf_pins xgpio_to_i2c_4/i2c_down]

  # Create port connections
  connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins axi_gpio_0/gpio_io_o] [get_bd_pins xlslice_O0/Din] [get_bd_pins xlslice_O1/Din] [get_bd_pins xlslice_O2/Din] [get_bd_pins xlslice_O3/Din] [get_bd_pins xlslice_O4/Din] [get_bd_pins xlslice_O5/Din] [get_bd_pins xlslice_O6/Din] [get_bd_pins xlslice_O7/Din] [get_bd_pins xlslice_O8/Din] [get_bd_pins xlslice_O9/Din]
  connect_bd_net -net axi_gpio_0_gpio_io_t [get_bd_pins axi_gpio_0/gpio_io_t] [get_bd_pins xlslice_T0/Din] [get_bd_pins xlslice_T1/Din] [get_bd_pins xlslice_T2/Din] [get_bd_pins xlslice_T3/Din] [get_bd_pins xlslice_T4/Din] [get_bd_pins xlslice_T5/Din] [get_bd_pins xlslice_T6/Din] [get_bd_pins xlslice_T7/Din] [get_bd_pins xlslice_T8/Din] [get_bd_pins xlslice_T9/Din]
  connect_bd_net -net s_axi_aclk_1 [get_bd_pins s_axi_aclk] [get_bd_pins axi_gpio_0/s_axi_aclk]
  connect_bd_net -net s_axi_aresetn_1 [get_bd_pins s_axi_aresetn] [get_bd_pins axi_gpio_0/s_axi_aresetn]
  connect_bd_net -net xgpio_to_i2c_0_upstream_scl_0 [get_bd_pins xgpio_to_i2c_0/upstream_scl_O] [get_bd_pins xlconcat_1/In1]
  connect_bd_net -net xgpio_to_i2c_0_upstream_scl_1 [get_bd_pins xgpio_to_i2c_1/upstream_scl_O] [get_bd_pins xlconcat_1/In3]
  connect_bd_net -net xgpio_to_i2c_0_upstream_scl_2 [get_bd_pins xgpio_to_i2c_2/upstream_scl_O] [get_bd_pins xlconcat_1/In5]
  connect_bd_net -net xgpio_to_i2c_0_upstream_scl_3 [get_bd_pins xgpio_to_i2c_3/upstream_scl_O] [get_bd_pins xlconcat_1/In7]
  connect_bd_net -net xgpio_to_i2c_0_upstream_sda_0 [get_bd_pins xgpio_to_i2c_0/upstream_sda_O] [get_bd_pins xlconcat_1/In0]
  connect_bd_net -net xgpio_to_i2c_0_upstream_sda_1 [get_bd_pins xgpio_to_i2c_1/upstream_sda_O] [get_bd_pins xlconcat_1/In2]
  connect_bd_net -net xgpio_to_i2c_0_upstream_sda_2 [get_bd_pins xgpio_to_i2c_2/upstream_sda_O] [get_bd_pins xlconcat_1/In4]
  connect_bd_net -net xgpio_to_i2c_0_upstream_sda_3 [get_bd_pins xgpio_to_i2c_3/upstream_sda_O] [get_bd_pins xlconcat_1/In6]
  connect_bd_net -net xgpio_to_i2c_4_upstream_scl_O [get_bd_pins xgpio_to_i2c_4/upstream_scl_O] [get_bd_pins xlconcat_1/In9]
  connect_bd_net -net xgpio_to_i2c_4_upstream_sda_O [get_bd_pins xgpio_to_i2c_4/upstream_sda_O] [get_bd_pins xlconcat_1/In8]
  connect_bd_net -net xlconcat_1_dout [get_bd_pins axi_gpio_0/gpio_io_i] [get_bd_pins xlconcat_1/dout]
  connect_bd_net -net xlslice_O0_Dout [get_bd_pins xgpio_to_i2c_0/upstream_sda_I] [get_bd_pins xlslice_O0/Dout]
  connect_bd_net -net xlslice_O1_Dout [get_bd_pins xgpio_to_i2c_0/upstream_scl_I] [get_bd_pins xlslice_O1/Dout]
  connect_bd_net -net xlslice_O2_Dout [get_bd_pins xgpio_to_i2c_1/upstream_sda_I] [get_bd_pins xlslice_O2/Dout]
  connect_bd_net -net xlslice_O3_Dout [get_bd_pins xgpio_to_i2c_1/upstream_scl_I] [get_bd_pins xlslice_O3/Dout]
  connect_bd_net -net xlslice_O4_Dout [get_bd_pins xgpio_to_i2c_2/upstream_sda_I] [get_bd_pins xlslice_O4/Dout]
  connect_bd_net -net xlslice_O5_Dout [get_bd_pins xgpio_to_i2c_2/upstream_scl_I] [get_bd_pins xlslice_O5/Dout]
  connect_bd_net -net xlslice_O6_Dout [get_bd_pins xgpio_to_i2c_3/upstream_sda_I] [get_bd_pins xlslice_O6/Dout]
  connect_bd_net -net xlslice_O7_Dout [get_bd_pins xgpio_to_i2c_3/upstream_scl_I] [get_bd_pins xlslice_O7/Dout]
  connect_bd_net -net xlslice_O8_Dout [get_bd_pins xgpio_to_i2c_4/upstream_sda_I] [get_bd_pins xlslice_O8/Dout]
  connect_bd_net -net xlslice_O9_Dout [get_bd_pins xgpio_to_i2c_4/upstream_scl_I] [get_bd_pins xlslice_O9/Dout]
  connect_bd_net -net xlslice_T0_Dout [get_bd_pins xgpio_to_i2c_0/upstream_sda_T] [get_bd_pins xlslice_T0/Dout]
  connect_bd_net -net xlslice_T1_Dout [get_bd_pins xgpio_to_i2c_0/upstream_scl_T] [get_bd_pins xlslice_T1/Dout]
  connect_bd_net -net xlslice_T2_Dout [get_bd_pins xgpio_to_i2c_1/upstream_sda_T] [get_bd_pins xlslice_T2/Dout]
  connect_bd_net -net xlslice_T3_Dout [get_bd_pins xgpio_to_i2c_1/upstream_scl_T] [get_bd_pins xlslice_T3/Dout]
  connect_bd_net -net xlslice_T4_Dout [get_bd_pins xgpio_to_i2c_2/upstream_sda_T] [get_bd_pins xlslice_T4/Dout]
  connect_bd_net -net xlslice_T5_Dout [get_bd_pins xgpio_to_i2c_2/upstream_scl_T] [get_bd_pins xlslice_T5/Dout]
  connect_bd_net -net xlslice_T6_Dout [get_bd_pins xgpio_to_i2c_3/upstream_sda_T] [get_bd_pins xlslice_T6/Dout]
  connect_bd_net -net xlslice_T7_Dout [get_bd_pins xgpio_to_i2c_3/upstream_scl_T] [get_bd_pins xlslice_T7/Dout]
  connect_bd_net -net xlslice_T8_Dout [get_bd_pins xgpio_to_i2c_4/upstream_sda_T] [get_bd_pins xlslice_T8/Dout]
  connect_bd_net -net xlslice_T9_Dout [get_bd_pins xgpio_to_i2c_4/upstream_scl_T] [get_bd_pins xlslice_T9/Dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: processor_subsystem
proc create_hier_cell_processor_subsystem { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_processor_subsystem() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:ddr4_rtl:1.0 C0_DDR4

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M08_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M09_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M10_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M11_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M12_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M13_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M14_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M15_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M16_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M17_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M18_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M19_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M20_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M21_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M22_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M23_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M24_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M25_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M26_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M27_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M28_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M29_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S02_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S03_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S04_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S05_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S06_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S07_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S08_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S09_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S10_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S11_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S12_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S13_AXI

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 gpio

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 sys

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:uart_rtl:1.0 uart_0


  # Create pins
  create_bd_pin -dir I -from 31 -to 0 VERSION
  create_bd_pin -dir O -type clk addn_ui_clkout1
  create_bd_pin -dir O -from 0 -to 0 -type rst aresetn_100m
  create_bd_pin -dir O -from 0 -to 0 -type rst aresetn_200m
  create_bd_pin -dir O -type rst c0_ddr4_ui_clk_sync_rst
  create_bd_pin -dir O c0_init_calib_complete
  create_bd_pin -dir O -type clk clk_100m
  create_bd_pin -dir O -type clk clk_200m
  create_bd_pin -dir O -type clk clk_39m
  create_bd_pin -dir O -type clk clk_50m
  create_bd_pin -dir I -type rst ddr_rst
  create_bd_pin -dir I -from 0 -to 0 irq_in_0
  create_bd_pin -dir I -from 0 -to 0 irq_in_1
  create_bd_pin -dir I -from 0 -to 0 irq_in_2
  create_bd_pin -dir I -from 0 -to 0 irq_in_3
  create_bd_pin -dir I -from 0 -to 0 irq_in_4
  create_bd_pin -dir I -from 0 -to 0 irq_in_5
  create_bd_pin -dir I -from 0 -to 0 irq_in_6
  create_bd_pin -dir I -from 0 -to 0 irq_in_7
  create_bd_pin -dir I -from 0 -to 0 irq_in_8
  create_bd_pin -dir I -from 0 -to 0 irq_in_9
  create_bd_pin -dir I -from 0 -to 0 irq_in_10
  create_bd_pin -dir I -from 0 -to 0 irq_in_11
  create_bd_pin -dir I -from 0 -to 0 irq_in_12
  create_bd_pin -dir O -from 0 -to 0 -type rst reset_100m
  create_bd_pin -dir O -from 0 -to 0 -type rst reset_200m
  create_bd_pin -dir I vcc

  # Create instance: AXI_LITE_REG_0, and set properties
  set AXI_LITE_REG_0 [ create_bd_cell -type ip -vlnv xilinx.com:user:AXI_LITE_REG:1.0 AXI_LITE_REG_0 ]

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]
  set_property -dict [ list \
   CONFIG.C_GPIO_WIDTH {12} \
 ] $axi_gpio_0

  # Create instance: axi_interconnect_0, and set properties
  set axi_interconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {30} \
 ] $axi_interconnect_0

  # Create instance: axi_quad_spi_0, and set properties
  set axi_quad_spi_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_quad_spi:3.2 axi_quad_spi_0 ]
  set_property -dict [ list \
   CONFIG.C_FIFO_DEPTH {256} \
   CONFIG.C_SCK_RATIO {2} \
   CONFIG.C_SPI_MEMORY {2} \
   CONFIG.C_SPI_MODE {2} \
   CONFIG.C_USE_STARTUP {1} \
   CONFIG.C_USE_STARTUP_INT {1} \
 ] $axi_quad_spi_0

  # Create instance: axi_smc, and set properties
  set axi_smc [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 axi_smc ]
  set_property -dict [ list \
   CONFIG.NUM_CLKS {2} \
   CONFIG.NUM_SI {14} \
 ] $axi_smc

  # Create instance: axi_timer_0, and set properties
  set axi_timer_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_0 ]

  # Create instance: axi_timer_1, and set properties
  set axi_timer_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_1 ]

  # Create instance: axi_timer_2, and set properties
  set axi_timer_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_2 ]

  # Create instance: axi_uartlite_0, and set properties
  set axi_uartlite_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0 ]
  set_property -dict [ list \
   CONFIG.C_BAUDRATE {115200} \
   CONFIG.C_S_AXI_ACLK_FREQ_HZ {100000000} \
 ] $axi_uartlite_0

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.AXI_DRP {false} \
   CONFIG.CLKOUT1_DRIVES {Buffer} \
   CONFIG.CLKOUT1_JITTER {98.146} \
   CONFIG.CLKOUT1_PHASE_ERROR {89.971} \
   CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200.000} \
   CONFIG.CLKOUT1_USED {true} \
   CONFIG.CLKOUT2_DRIVES {Buffer} \
   CONFIG.CLKOUT2_JITTER {112.316} \
   CONFIG.CLKOUT2_PHASE_ERROR {89.971} \
   CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {100.000} \
   CONFIG.CLKOUT2_USED {true} \
   CONFIG.CLKOUT3_DRIVES {Buffer} \
   CONFIG.CLKOUT3_JITTER {112.316} \
   CONFIG.CLKOUT3_PHASE_ERROR {89.971} \
   CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {100.000} \
   CONFIG.CLKOUT3_USED {false} \
   CONFIG.FEEDBACK_SOURCE {FDBK_AUTO} \
   CONFIG.JITTER_SEL {No_Jitter} \
   CONFIG.MMCM_BANDWIDTH {OPTIMIZED} \
   CONFIG.MMCM_CLKFBOUT_MULT_F {5.000} \
   CONFIG.MMCM_CLKIN2_PERIOD {10.0} \
   CONFIG.MMCM_CLKOUT0_DIVIDE_F {5.000} \
   CONFIG.MMCM_CLKOUT1_DIVIDE {10} \
   CONFIG.MMCM_CLKOUT2_DIVIDE {1} \
   CONFIG.MMCM_DIVCLK_DIVIDE {1} \
   CONFIG.NUM_OUT_CLKS {2} \
   CONFIG.PHASE_DUTY_CONFIG {false} \
   CONFIG.USE_DYN_RECONFIG {false} \
 ] $clk_wiz_0

  # Create instance: ddr4_0, and set properties
  set ddr4_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ddr4:2.2 ddr4_0 ]
  set_property -dict [ list \
   CONFIG.ADDN_UI_CLKOUT1_FREQ_HZ {200} \
   CONFIG.ADDN_UI_CLKOUT2_FREQ_HZ {39} \
   CONFIG.ADDN_UI_CLKOUT3_FREQ_HZ {50} \
   CONFIG.C0.BANK_GROUP_WIDTH {1} \
   CONFIG.C0.DDR4_AxiAddressWidth {32} \
   CONFIG.C0.DDR4_AxiDataWidth {512} \
   CONFIG.C0.DDR4_CasLatency {18} \
   CONFIG.C0.DDR4_DataWidth {64} \
   CONFIG.C0.DDR4_InputClockPeriod {4998} \
   CONFIG.C0.DDR4_MemoryPart {MT40A512M16HA-083E} \
   CONFIG.C0.DDR4_Specify_MandD {false} \
   CONFIG.C0.DDR4_isCustom {false} \
   CONFIG.System_Clock {Differential} \
 ] $ddr4_0

  # Create instance: mdm_1, and set properties
  set mdm_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:mdm:3.2 mdm_1 ]
  set_property -dict [ list \
   CONFIG.C_ADDR_SIZE {32} \
   CONFIG.C_M_AXI_ADDR_WIDTH {32} \
   CONFIG.C_USE_UART {0} \
 ] $mdm_1

  # Create instance: microblaze_0, and set properties
  set microblaze_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:11.0 microblaze_0 ]
  set_property -dict [ list \
   CONFIG.C_ADDR_TAG_BITS {16} \
   CONFIG.C_AREA_OPTIMIZED {0} \
   CONFIG.C_CACHE_BYTE_SIZE {32768} \
   CONFIG.C_DCACHE_ADDR_TAG {16} \
   CONFIG.C_DCACHE_BYTE_SIZE {32768} \
   CONFIG.C_DCACHE_LINE_LEN {16} \
   CONFIG.C_DCACHE_USE_WRITEBACK {1} \
   CONFIG.C_DCACHE_VICTIMS {8} \
   CONFIG.C_DEBUG_ENABLED {1} \
   CONFIG.C_D_AXI {1} \
   CONFIG.C_D_LMB {1} \
   CONFIG.C_ICACHE_LINE_LEN {16} \
   CONFIG.C_ICACHE_STREAMS {1} \
   CONFIG.C_ICACHE_VICTIMS {8} \
   CONFIG.C_I_AXI {0} \
   CONFIG.C_I_LMB {1} \
   CONFIG.C_MMU_ZONES {2} \
   CONFIG.C_USE_BARREL {1} \
   CONFIG.C_USE_BRANCH_TARGET_CACHE {1} \
   CONFIG.C_USE_DCACHE {1} \
   CONFIG.C_USE_DIV {1} \
   CONFIG.C_USE_FPU {2} \
   CONFIG.C_USE_HW_MUL {2} \
   CONFIG.C_USE_ICACHE {1} \
   CONFIG.C_USE_MSR_INSTR {1} \
   CONFIG.C_USE_PCMP_INSTR {1} \
   CONFIG.G_TEMPLATE_LIST {2} \
 ] $microblaze_0

  # Create instance: microblaze_0_axi_intc, and set properties
  set microblaze_0_axi_intc [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_intc:4.1 microblaze_0_axi_intc ]
  set_property -dict [ list \
   CONFIG.C_HAS_FAST {1} \
 ] $microblaze_0_axi_intc

  # Create instance: microblaze_0_local_memory
  create_hier_cell_microblaze_0_local_memory $hier_obj microblaze_0_local_memory

  # Create instance: microblaze_0_xlconcat, and set properties
  set microblaze_0_xlconcat [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 microblaze_0_xlconcat ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {18} \
 ] $microblaze_0_xlconcat

  # Create instance: rst_100m, and set properties
  set rst_100m [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_100m ]
  set_property -dict [ list \
   CONFIG.C_AUX_RST_WIDTH {1} \
   CONFIG.C_EXT_RST_WIDTH {1} \
 ] $rst_100m

  # Create instance: rst_200m, and set properties
  set rst_200m [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_200m ]
  set_property -dict [ list \
   CONFIG.C_AUX_RST_WIDTH {1} \
   CONFIG.C_EXT_RST_WIDTH {1} \
 ] $rst_200m

  # Create instance: rst_mig_ui_300M, and set properties
  set rst_mig_ui_300M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_mig_ui_300M ]
  set_property -dict [ list \
   CONFIG.C_AUX_RST_WIDTH {1} \
   CONFIG.C_EXT_RST_WIDTH {1} \
 ] $rst_mig_ui_300M

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins sys] [get_bd_intf_pins ddr4_0/C0_SYS_CLK]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins C0_DDR4] [get_bd_intf_pins ddr4_0/C0_DDR4]
  connect_bd_intf_net -intf_net Conn3 [get_bd_intf_pins M08_AXI] [get_bd_intf_pins axi_interconnect_0/M08_AXI]
  connect_bd_intf_net -intf_net Conn4 [get_bd_intf_pins M09_AXI] [get_bd_intf_pins axi_interconnect_0/M09_AXI]
  connect_bd_intf_net -intf_net Conn5 [get_bd_intf_pins M10_AXI] [get_bd_intf_pins axi_interconnect_0/M10_AXI]
  connect_bd_intf_net -intf_net Conn6 [get_bd_intf_pins M11_AXI] [get_bd_intf_pins axi_interconnect_0/M11_AXI]
  connect_bd_intf_net -intf_net Conn7 [get_bd_intf_pins M12_AXI] [get_bd_intf_pins axi_interconnect_0/M12_AXI]
  connect_bd_intf_net -intf_net Conn8 [get_bd_intf_pins M13_AXI] [get_bd_intf_pins axi_interconnect_0/M13_AXI]
  connect_bd_intf_net -intf_net Conn9 [get_bd_intf_pins M14_AXI] [get_bd_intf_pins axi_interconnect_0/M14_AXI]
  connect_bd_intf_net -intf_net Conn10 [get_bd_intf_pins M15_AXI] [get_bd_intf_pins axi_interconnect_0/M15_AXI]
  connect_bd_intf_net -intf_net Conn11 [get_bd_intf_pins M16_AXI] [get_bd_intf_pins axi_interconnect_0/M16_AXI]
  connect_bd_intf_net -intf_net Conn12 [get_bd_intf_pins M17_AXI] [get_bd_intf_pins axi_interconnect_0/M17_AXI]
  connect_bd_intf_net -intf_net Conn13 [get_bd_intf_pins M18_AXI] [get_bd_intf_pins axi_interconnect_0/M18_AXI]
  connect_bd_intf_net -intf_net Conn14 [get_bd_intf_pins M19_AXI] [get_bd_intf_pins axi_interconnect_0/M19_AXI]
  connect_bd_intf_net -intf_net Conn15 [get_bd_intf_pins M20_AXI] [get_bd_intf_pins axi_interconnect_0/M20_AXI]
  connect_bd_intf_net -intf_net Conn16 [get_bd_intf_pins M21_AXI] [get_bd_intf_pins axi_interconnect_0/M21_AXI]
  connect_bd_intf_net -intf_net Conn17 [get_bd_intf_pins M22_AXI] [get_bd_intf_pins axi_interconnect_0/M22_AXI]
  connect_bd_intf_net -intf_net Conn18 [get_bd_intf_pins M23_AXI] [get_bd_intf_pins axi_interconnect_0/M23_AXI]
  connect_bd_intf_net -intf_net Conn19 [get_bd_intf_pins M24_AXI] [get_bd_intf_pins axi_interconnect_0/M24_AXI]
  connect_bd_intf_net -intf_net Conn20 [get_bd_intf_pins M25_AXI] [get_bd_intf_pins axi_interconnect_0/M25_AXI]
  connect_bd_intf_net -intf_net Conn21 [get_bd_intf_pins M26_AXI] [get_bd_intf_pins axi_interconnect_0/M26_AXI]
  connect_bd_intf_net -intf_net Conn22 [get_bd_intf_pins M27_AXI] [get_bd_intf_pins axi_interconnect_0/M27_AXI]
  connect_bd_intf_net -intf_net Conn23 [get_bd_intf_pins M28_AXI] [get_bd_intf_pins axi_interconnect_0/M28_AXI]
  connect_bd_intf_net -intf_net Conn24 [get_bd_intf_pins M29_AXI] [get_bd_intf_pins axi_interconnect_0/M29_AXI]
  connect_bd_intf_net -intf_net S02_AXI_1 [get_bd_intf_pins S02_AXI] [get_bd_intf_pins axi_smc/S03_AXI]
  connect_bd_intf_net -intf_net S03_AXI_1 [get_bd_intf_pins S03_AXI] [get_bd_intf_pins axi_smc/S04_AXI]
  connect_bd_intf_net -intf_net S04_AXI_1 [get_bd_intf_pins S04_AXI] [get_bd_intf_pins axi_smc/S05_AXI]
  connect_bd_intf_net -intf_net S05_AXI_1 [get_bd_intf_pins S05_AXI] [get_bd_intf_pins axi_smc/S06_AXI]
  connect_bd_intf_net -intf_net S06_AXI_1 [get_bd_intf_pins S06_AXI] [get_bd_intf_pins axi_smc/S07_AXI]
  connect_bd_intf_net -intf_net S07_AXI_1 [get_bd_intf_pins S07_AXI] [get_bd_intf_pins axi_smc/S02_AXI]
  connect_bd_intf_net -intf_net S08_AXI_1 [get_bd_intf_pins S08_AXI] [get_bd_intf_pins axi_smc/S08_AXI]
  connect_bd_intf_net -intf_net S09_AXI_1 [get_bd_intf_pins S09_AXI] [get_bd_intf_pins axi_smc/S09_AXI]
  connect_bd_intf_net -intf_net S10_AXI_1 [get_bd_intf_pins S10_AXI] [get_bd_intf_pins axi_smc/S10_AXI]
  connect_bd_intf_net -intf_net S11_AXI_1 [get_bd_intf_pins S11_AXI] [get_bd_intf_pins axi_smc/S11_AXI]
  connect_bd_intf_net -intf_net S12_AXI_1 [get_bd_intf_pins S12_AXI] [get_bd_intf_pins axi_smc/S12_AXI]
  connect_bd_intf_net -intf_net S13_AXI_1 [get_bd_intf_pins S13_AXI] [get_bd_intf_pins axi_smc/S13_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins axi_interconnect_0/M00_AXI] [get_bd_intf_pins microblaze_0_axi_intc/s_axi]
  connect_bd_intf_net -intf_net axi_interconnect_0_M01_AXI [get_bd_intf_pins AXI_LITE_REG_0/S00_AXI] [get_bd_intf_pins axi_interconnect_0/M01_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M02_AXI [get_bd_intf_pins axi_gpio_0/S_AXI] [get_bd_intf_pins axi_interconnect_0/M02_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M03_AXI [get_bd_intf_pins axi_interconnect_0/M03_AXI] [get_bd_intf_pins axi_quad_spi_0/AXI_LITE]
  connect_bd_intf_net -intf_net axi_interconnect_0_M04_AXI [get_bd_intf_pins axi_interconnect_0/M04_AXI] [get_bd_intf_pins axi_timer_0/S_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M05_AXI [get_bd_intf_pins axi_interconnect_0/M05_AXI] [get_bd_intf_pins axi_timer_1/S_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M06_AXI [get_bd_intf_pins axi_interconnect_0/M06_AXI] [get_bd_intf_pins axi_timer_2/S_AXI]
  connect_bd_intf_net -intf_net axi_interconnect_0_M07_AXI [get_bd_intf_pins axi_interconnect_0/M07_AXI] [get_bd_intf_pins axi_uartlite_0/S_AXI]
  connect_bd_intf_net -intf_net axi_smc_M00_AXI [get_bd_intf_pins axi_smc/M00_AXI] [get_bd_intf_pins ddr4_0/C0_DDR4_S_AXI]
  connect_bd_intf_net -intf_net interconnect_MBDEBUG_0 [get_bd_intf_pins mdm_1/MBDEBUG_0] [get_bd_intf_pins microblaze_0/DEBUG]
  connect_bd_intf_net -intf_net microblaze_0_DLMB [get_bd_intf_pins microblaze_0/DLMB] [get_bd_intf_pins microblaze_0_local_memory/DLMB]
  connect_bd_intf_net -intf_net microblaze_0_ILMB [get_bd_intf_pins microblaze_0/ILMB] [get_bd_intf_pins microblaze_0_local_memory/ILMB]
  connect_bd_intf_net -intf_net microblaze_0_M_AXI_DC [get_bd_intf_pins axi_smc/S00_AXI] [get_bd_intf_pins microblaze_0/M_AXI_DC]
  connect_bd_intf_net -intf_net microblaze_0_M_AXI_DP [get_bd_intf_pins axi_interconnect_0/S00_AXI] [get_bd_intf_pins microblaze_0/M_AXI_DP]
  connect_bd_intf_net -intf_net microblaze_0_M_AXI_IC [get_bd_intf_pins axi_smc/S01_AXI] [get_bd_intf_pins microblaze_0/M_AXI_IC]
  connect_bd_intf_net -intf_net microblaze_0_axi_intc_interrupt [get_bd_intf_pins microblaze_0/INTERRUPT] [get_bd_intf_pins microblaze_0_axi_intc/interrupt]
  connect_bd_intf_net -intf_net processor_subsystem_gpio_rtl_0 [get_bd_intf_pins gpio] [get_bd_intf_pins axi_gpio_0/GPIO]
  connect_bd_intf_net -intf_net processor_subsystem_uart_0 [get_bd_intf_pins uart_0] [get_bd_intf_pins axi_uartlite_0/UART]

  # Create port connections
  connect_bd_net -net HW_VER_dout [get_bd_pins VERSION] [get_bd_pins AXI_LITE_REG_0/VERSION]
  connect_bd_net -net Reset_1 [get_bd_pins microblaze_0/Reset] [get_bd_pins microblaze_0_axi_intc/processor_rst] [get_bd_pins rst_200m/mb_reset]
  connect_bd_net -net SYS_Rst_1 [get_bd_pins microblaze_0_local_memory/SYS_Rst] [get_bd_pins rst_200m/bus_struct_reset]
  connect_bd_net -net VCC_dout [get_bd_pins vcc] [get_bd_pins rst_100m/ext_reset_in] [get_bd_pins rst_200m/ext_reset_in]
  connect_bd_net -net axi_quad_spi_0_ip2intc_irpt [get_bd_pins axi_quad_spi_0/ip2intc_irpt] [get_bd_pins microblaze_0_xlconcat/In0]
  connect_bd_net -net axi_timer_0_interrupt [get_bd_pins axi_timer_0/interrupt] [get_bd_pins microblaze_0_xlconcat/In2]
  connect_bd_net -net axi_timer_1_interrupt [get_bd_pins axi_timer_1/interrupt] [get_bd_pins microblaze_0_xlconcat/In3]
  connect_bd_net -net axi_timer_2_interrupt [get_bd_pins axi_timer_2/interrupt] [get_bd_pins microblaze_0_xlconcat/In4]
  connect_bd_net -net axi_uartlite_0_interrupt [get_bd_pins axi_uartlite_0/interrupt] [get_bd_pins microblaze_0_xlconcat/In1]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins clk_200m] [get_bd_pins axi_interconnect_0/ACLK] [get_bd_pins axi_interconnect_0/M00_ACLK] [get_bd_pins axi_interconnect_0/S00_ACLK] [get_bd_pins axi_smc/aclk1] [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins microblaze_0/Clk] [get_bd_pins microblaze_0_axi_intc/processor_clk] [get_bd_pins microblaze_0_axi_intc/s_axi_aclk] [get_bd_pins microblaze_0_local_memory/LMB_Clk] [get_bd_pins rst_200m/slowest_sync_clk]
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_pins clk_100m] [get_bd_pins AXI_LITE_REG_0/s00_axi_aclk] [get_bd_pins axi_gpio_0/s_axi_aclk] [get_bd_pins axi_interconnect_0/M01_ACLK] [get_bd_pins axi_interconnect_0/M02_ACLK] [get_bd_pins axi_interconnect_0/M03_ACLK] [get_bd_pins axi_interconnect_0/M04_ACLK] [get_bd_pins axi_interconnect_0/M05_ACLK] [get_bd_pins axi_interconnect_0/M06_ACLK] [get_bd_pins axi_interconnect_0/M07_ACLK] [get_bd_pins axi_interconnect_0/M08_ACLK] [get_bd_pins axi_interconnect_0/M09_ACLK] [get_bd_pins axi_interconnect_0/M10_ACLK] [get_bd_pins axi_interconnect_0/M11_ACLK] [get_bd_pins axi_interconnect_0/M12_ACLK] [get_bd_pins axi_interconnect_0/M13_ACLK] [get_bd_pins axi_interconnect_0/M14_ACLK] [get_bd_pins axi_interconnect_0/M15_ACLK] [get_bd_pins axi_interconnect_0/M16_ACLK] [get_bd_pins axi_interconnect_0/M17_ACLK] [get_bd_pins axi_interconnect_0/M18_ACLK] [get_bd_pins axi_interconnect_0/M19_ACLK] [get_bd_pins axi_interconnect_0/M20_ACLK] [get_bd_pins axi_interconnect_0/M21_ACLK] [get_bd_pins axi_interconnect_0/M22_ACLK] [get_bd_pins axi_interconnect_0/M23_ACLK] [get_bd_pins axi_interconnect_0/M24_ACLK] [get_bd_pins axi_interconnect_0/M25_ACLK] [get_bd_pins axi_interconnect_0/M26_ACLK] [get_bd_pins axi_interconnect_0/M27_ACLK] [get_bd_pins axi_interconnect_0/M28_ACLK] [get_bd_pins axi_interconnect_0/M29_ACLK] [get_bd_pins axi_quad_spi_0/ext_spi_clk] [get_bd_pins axi_quad_spi_0/s_axi_aclk] [get_bd_pins axi_timer_0/s_axi_aclk] [get_bd_pins axi_timer_1/capturetrig0] [get_bd_pins axi_timer_1/s_axi_aclk] [get_bd_pins axi_timer_2/s_axi_aclk] [get_bd_pins axi_uartlite_0/s_axi_aclk] [get_bd_pins clk_wiz_0/clk_out2] [get_bd_pins rst_100m/slowest_sync_clk]
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins rst_100m/dcm_locked] [get_bd_pins rst_200m/dcm_locked]
  connect_bd_net -net ddr4_0_addn_ui_clkout1 [get_bd_pins addn_ui_clkout1] [get_bd_pins clk_wiz_0/clk_in1] [get_bd_pins ddr4_0/addn_ui_clkout1]
  connect_bd_net -net ddr4_0_addn_ui_clkout2 [get_bd_pins clk_39m] [get_bd_pins ddr4_0/addn_ui_clkout2]
  connect_bd_net -net ddr4_0_addn_ui_clkout3 [get_bd_pins clk_50m] [get_bd_pins ddr4_0/addn_ui_clkout3]
  connect_bd_net -net ddr4_0_c0_ddr4_ui_clk1 [get_bd_pins axi_smc/aclk] [get_bd_pins ddr4_0/c0_ddr4_ui_clk] [get_bd_pins rst_mig_ui_300M/slowest_sync_clk]
  connect_bd_net -net ddr4_0_c0_ddr4_ui_clk_sync_rst [get_bd_pins c0_ddr4_ui_clk_sync_rst] [get_bd_pins clk_wiz_0/reset] [get_bd_pins ddr4_0/c0_ddr4_ui_clk_sync_rst] [get_bd_pins rst_100m/aux_reset_in] [get_bd_pins rst_200m/aux_reset_in] [get_bd_pins rst_mig_ui_300M/ext_reset_in]
  connect_bd_net -net ddr4_0_c0_init_calib_complete [get_bd_pins c0_init_calib_complete] [get_bd_pins ddr4_0/c0_init_calib_complete]
  connect_bd_net -net irq_in_0_1 [get_bd_pins irq_in_0] [get_bd_pins microblaze_0_xlconcat/In5]
  connect_bd_net -net irq_in_10_1 [get_bd_pins irq_in_10] [get_bd_pins microblaze_0_xlconcat/In15]
  connect_bd_net -net irq_in_11_1 [get_bd_pins irq_in_11] [get_bd_pins microblaze_0_xlconcat/In16]
  connect_bd_net -net irq_in_12_1 [get_bd_pins irq_in_12] [get_bd_pins microblaze_0_xlconcat/In17]
  connect_bd_net -net irq_in_1_1 [get_bd_pins irq_in_1] [get_bd_pins microblaze_0_xlconcat/In6]
  connect_bd_net -net irq_in_2_1 [get_bd_pins irq_in_2] [get_bd_pins microblaze_0_xlconcat/In7]
  connect_bd_net -net irq_in_3_1 [get_bd_pins irq_in_3] [get_bd_pins microblaze_0_xlconcat/In8]
  connect_bd_net -net irq_in_4_1 [get_bd_pins irq_in_4] [get_bd_pins microblaze_0_xlconcat/In9]
  connect_bd_net -net irq_in_5_1 [get_bd_pins irq_in_5] [get_bd_pins microblaze_0_xlconcat/In10]
  connect_bd_net -net irq_in_6_1 [get_bd_pins irq_in_6] [get_bd_pins microblaze_0_xlconcat/In11]
  connect_bd_net -net irq_in_7_1 [get_bd_pins irq_in_7] [get_bd_pins microblaze_0_xlconcat/In12]
  connect_bd_net -net irq_in_8_1 [get_bd_pins irq_in_8] [get_bd_pins microblaze_0_xlconcat/In13]
  connect_bd_net -net irq_in_9_1 [get_bd_pins irq_in_9] [get_bd_pins microblaze_0_xlconcat/In14]
  connect_bd_net -net mdm_1_debug_sys_rst [get_bd_pins mdm_1/Debug_SYS_Rst] [get_bd_pins rst_200m/mb_debug_sys_rst]
  connect_bd_net -net microblaze_0_xlconcat_dout [get_bd_pins microblaze_0_axi_intc/intr] [get_bd_pins microblaze_0_xlconcat/dout]
  connect_bd_net -net processor_subsystem_peripheral_reset [get_bd_pins reset_200m] [get_bd_pins rst_200m/peripheral_reset]
  connect_bd_net -net rst_lite_peripheral_aresetn [get_bd_pins aresetn_100m] [get_bd_pins AXI_LITE_REG_0/s00_axi_aresetn] [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_interconnect_0/M01_ARESETN] [get_bd_pins axi_interconnect_0/M02_ARESETN] [get_bd_pins axi_interconnect_0/M03_ARESETN] [get_bd_pins axi_interconnect_0/M04_ARESETN] [get_bd_pins axi_interconnect_0/M05_ARESETN] [get_bd_pins axi_interconnect_0/M06_ARESETN] [get_bd_pins axi_interconnect_0/M07_ARESETN] [get_bd_pins axi_interconnect_0/M08_ARESETN] [get_bd_pins axi_interconnect_0/M09_ARESETN] [get_bd_pins axi_interconnect_0/M10_ARESETN] [get_bd_pins axi_interconnect_0/M11_ARESETN] [get_bd_pins axi_interconnect_0/M12_ARESETN] [get_bd_pins axi_interconnect_0/M13_ARESETN] [get_bd_pins axi_interconnect_0/M14_ARESETN] [get_bd_pins axi_interconnect_0/M15_ARESETN] [get_bd_pins axi_interconnect_0/M16_ARESETN] [get_bd_pins axi_interconnect_0/M17_ARESETN] [get_bd_pins axi_interconnect_0/M18_ARESETN] [get_bd_pins axi_interconnect_0/M19_ARESETN] [get_bd_pins axi_interconnect_0/M20_ARESETN] [get_bd_pins axi_interconnect_0/M21_ARESETN] [get_bd_pins axi_interconnect_0/M22_ARESETN] [get_bd_pins axi_interconnect_0/M23_ARESETN] [get_bd_pins axi_interconnect_0/M24_ARESETN] [get_bd_pins axi_interconnect_0/M25_ARESETN] [get_bd_pins axi_interconnect_0/M26_ARESETN] [get_bd_pins axi_interconnect_0/M27_ARESETN] [get_bd_pins axi_interconnect_0/M28_ARESETN] [get_bd_pins axi_interconnect_0/M29_ARESETN] [get_bd_pins axi_quad_spi_0/s_axi_aresetn] [get_bd_pins axi_timer_0/s_axi_aresetn] [get_bd_pins axi_timer_1/s_axi_aresetn] [get_bd_pins axi_timer_2/s_axi_aresetn] [get_bd_pins axi_uartlite_0/s_axi_aresetn] [get_bd_pins rst_100m/peripheral_aresetn]
  connect_bd_net -net rst_lite_peripheral_reset [get_bd_pins reset_100m] [get_bd_pins rst_100m/peripheral_reset]
  connect_bd_net -net rst_mig_ui_300M_peripheral_aresetn [get_bd_pins ddr4_0/c0_ddr4_aresetn] [get_bd_pins rst_mig_ui_300M/peripheral_aresetn]
  connect_bd_net -net rst_system_peripheral_aresetn [get_bd_pins aresetn_200m] [get_bd_pins axi_interconnect_0/ARESETN] [get_bd_pins axi_interconnect_0/M00_ARESETN] [get_bd_pins axi_interconnect_0/S00_ARESETN] [get_bd_pins axi_smc/aresetn] [get_bd_pins microblaze_0_axi_intc/s_axi_aresetn] [get_bd_pins rst_200m/peripheral_aresetn]
  connect_bd_net -net sys_rst_2 [get_bd_pins ddr_rst] [get_bd_pins ddr4_0/sys_rst]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: lvds_s1
proc create_hier_cell_lvds_s1 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_lvds_s1() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_MM2S

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM1

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM2

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S00_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S_AXIS

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_CTRL

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_LITE

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_LITE1


  # Create pins
  create_bd_pin -dir I -type clk aclk_100m
  create_bd_pin -dir I -type clk aclk_200m
  create_bd_pin -dir I -type rst aresetn_100m
  create_bd_pin -dir I -type rst aresetn_200m
  create_bd_pin -dir O -type intr intr
  create_bd_pin -dir I -type rst rst

  # Create instance: AXI4_Stream_Terminat_0, and set properties
  set AXI4_Stream_Terminat_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:AXI4_Stream_Terminator:1.0 AXI4_Stream_Terminat_0 ]
  set_property -dict [ list \
   CONFIG.SAMPLES_PER_CLOCK {1} \
 ] $AXI4_Stream_Terminat_0

  # Create instance: axi_vdma_0, and set properties
  set axi_vdma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_vdma:6.3 axi_vdma_0 ]
  set_property -dict [ list \
   CONFIG.c_m_axi_mm2s_data_width {128} \
   CONFIG.c_m_axis_mm2s_tdata_width {24} \
   CONFIG.c_mm2s_max_burst_length {128} \
   CONFIG.c_num_fstores {3} \
   CONFIG.c_s2mm_max_burst_length {128} \
 ] $axi_vdma_0

  # Create instance: axi_vdma_1, and set properties
  set axi_vdma_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_vdma:6.3 axi_vdma_1 ]
  set_property -dict [ list \
   CONFIG.c_m_axi_mm2s_data_width {128} \
   CONFIG.c_m_axi_s2mm_data_width {128} \
   CONFIG.c_m_axis_mm2s_tdata_width {24} \
   CONFIG.c_mm2s_max_burst_length {128} \
   CONFIG.c_num_fstores {1} \
   CONFIG.c_s2mm_max_burst_length {128} \
 ] $axi_vdma_1

  # Create instance: axis_broadcaster_0, and set properties
  set axis_broadcaster_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_broadcaster:1.1 axis_broadcaster_0 ]
  set_property -dict [ list \
   CONFIG.M_TUSER_WIDTH {1} \
 ] $axis_broadcaster_0

  # Create instance: axis_combiner_0, and set properties
  set axis_combiner_0 [ create_bd_cell -type ip -vlnv xilinx.com:user:axis_combiner:1.1 axis_combiner_0 ]

  # Create instance: axis_pixel_compare_0, and set properties
  set axis_pixel_compare_0 [ create_bd_cell -type ip -vlnv xilinx.com:user:axis_pixel_compare:2.19 axis_pixel_compare_0 ]
  set_property -dict [ list \
   CONFIG.TUSER_WIDTH {1} \
 ] $axis_pixel_compare_0

  # Create instance: axis_subset_converter_0, and set properties
  set axis_subset_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter:1.1 axis_subset_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {3} \
   CONFIG.S_TDATA_NUM_BYTES {6} \
   CONFIG.TDATA_REMAP {tdata[23:0]} \
 ] $axis_subset_converter_0

  # Create instance: axis_switch_0, and set properties
  set axis_switch_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_switch:1.1 axis_switch_0 ]
  set_property -dict [ list \
   CONFIG.DECODER_REG {1} \
   CONFIG.NUM_MI {2} \
   CONFIG.NUM_SI {1} \
   CONFIG.ROUTING_MODE {1} \
 ] $axis_switch_0

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]
  set_property -dict [ list \
   CONFIG.C_AUX_RST_WIDTH {1} \
   CONFIG.C_EXT_RST_WIDTH {1} \
 ] $proc_sys_reset_0

  # Create instance: util_vector_logic_0, and set properties
  set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_0 ]
  set_property -dict [ list \
   CONFIG.C_OPERATION {not} \
   CONFIG.C_SIZE {1} \
   CONFIG.LOGO_FILE {data/sym_notgate.png} \
 ] $util_vector_logic_0

  # Create interface connections
  connect_bd_intf_net -intf_net S_AXIS_1 [get_bd_intf_pins S_AXIS] [get_bd_intf_pins axi_vdma_0/S_AXIS_S2MM]
  connect_bd_intf_net -intf_net S_AXI_CTRL_1 [get_bd_intf_pins S_AXI_CTRL] [get_bd_intf_pins axis_switch_0/S_AXI_CTRL]
  connect_bd_intf_net -intf_net S_AXI_LITE1_1 [get_bd_intf_pins S_AXI_LITE1] [get_bd_intf_pins axi_vdma_0/S_AXI_LITE]
  connect_bd_intf_net -intf_net axi_vdma_0_M_AXIS_MM2S [get_bd_intf_pins axi_vdma_0/M_AXIS_MM2S] [get_bd_intf_pins axis_broadcaster_0/S_AXIS]
  connect_bd_intf_net -intf_net axi_vdma_0_M_AXI_MM2S [get_bd_intf_pins M_AXI_S2MM] [get_bd_intf_pins axi_vdma_0/M_AXI_MM2S]
  connect_bd_intf_net -intf_net axi_vdma_0_M_AXI_S2MM [get_bd_intf_pins M_AXI_S2MM1] [get_bd_intf_pins axi_vdma_0/M_AXI_S2MM]
  connect_bd_intf_net -intf_net axi_vdma_1_M_AXIS_MM2S [get_bd_intf_pins axi_vdma_1/M_AXIS_MM2S] [get_bd_intf_pins axis_combiner_0/axis_s1]
  connect_bd_intf_net -intf_net axi_vdma_1_M_AXI_MM2S [get_bd_intf_pins M_AXI_MM2S] [get_bd_intf_pins axi_vdma_1/M_AXI_MM2S]
  connect_bd_intf_net -intf_net axi_vdma_1_M_AXI_S2MM [get_bd_intf_pins M_AXI_S2MM2] [get_bd_intf_pins axi_vdma_1/M_AXI_S2MM]
  connect_bd_intf_net -intf_net axis_broadcaster_0_M00_AXIS [get_bd_intf_pins axis_broadcaster_0/M00_AXIS] [get_bd_intf_pins axis_combiner_0/axis_s0]
  connect_bd_intf_net -intf_net axis_broadcaster_0_M01_AXIS [get_bd_intf_pins axi_vdma_1/S_AXIS_S2MM] [get_bd_intf_pins axis_broadcaster_0/M01_AXIS]
  connect_bd_intf_net -intf_net axis_combiner_0_axis_m [get_bd_intf_pins axis_combiner_0/axis_m] [get_bd_intf_pins axis_pixel_compare_0/s_axis]
  connect_bd_intf_net -intf_net axis_pixel_compare_0_m_axis [get_bd_intf_pins axis_pixel_compare_0/m_axis] [get_bd_intf_pins axis_subset_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net axis_subset_converter_0_M_AXIS [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins axis_switch_0/S00_AXIS]
  connect_bd_intf_net -intf_net axis_switch_0_M00_AXIS [get_bd_intf_pins M_AXIS] [get_bd_intf_pins axis_switch_0/M00_AXIS]
  connect_bd_intf_net -intf_net axis_switch_0_M01_AXIS [get_bd_intf_pins AXI4_Stream_Terminat_0/s_axis_video] [get_bd_intf_pins axis_switch_0/M01_AXIS]
  connect_bd_intf_net -intf_net processor_subsystem_M12_AXI [get_bd_intf_pins S00_AXI] [get_bd_intf_pins axis_pixel_compare_0/S00_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M04_AXI [get_bd_intf_pins S_AXI_LITE] [get_bd_intf_pins axi_vdma_1/S_AXI_LITE]

  # Create port connections
  connect_bd_net -net Net [get_bd_pins aclk_100m] [get_bd_pins axi_vdma_0/s_axi_lite_aclk] [get_bd_pins axi_vdma_1/s_axi_lite_aclk] [get_bd_pins axis_pixel_compare_0/s00_axi_aclk] [get_bd_pins axis_switch_0/s_axi_ctrl_aclk]
  connect_bd_net -net Net1 [get_bd_pins aresetn_100m] [get_bd_pins axi_vdma_0/axi_resetn] [get_bd_pins axi_vdma_1/axi_resetn] [get_bd_pins axis_pixel_compare_0/s00_axi_aresetn] [get_bd_pins axis_switch_0/s_axi_ctrl_aresetn]
  connect_bd_net -net axis_pixel_compare_0_intr [get_bd_pins intr] [get_bd_pins axis_pixel_compare_0/intr]
  connect_bd_net -net rst_1 [get_bd_pins rst] [get_bd_pins util_vector_logic_0/Op1]
  connect_bd_net -net rst_200m_peripheral_aresetn [get_bd_pins axis_broadcaster_0/aresetn] [get_bd_pins axis_combiner_0/rst_n] [get_bd_pins axis_pixel_compare_0/aresetn] [get_bd_pins proc_sys_reset_0/peripheral_aresetn]
  connect_bd_net -net s_axi_lite_aresetn_1 [get_bd_pins aresetn_200m] [get_bd_pins AXI4_Stream_Terminat_0/ap_rst_n] [get_bd_pins axis_subset_converter_0/aresetn] [get_bd_pins axis_switch_0/aresetn]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins proc_sys_reset_0/ext_reset_in] [get_bd_pins util_vector_logic_0/Res]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins aclk_200m] [get_bd_pins AXI4_Stream_Terminat_0/ap_clk] [get_bd_pins axi_vdma_0/m_axi_mm2s_aclk] [get_bd_pins axi_vdma_0/m_axi_s2mm_aclk] [get_bd_pins axi_vdma_0/m_axis_mm2s_aclk] [get_bd_pins axi_vdma_0/s_axis_s2mm_aclk] [get_bd_pins axi_vdma_1/m_axi_mm2s_aclk] [get_bd_pins axi_vdma_1/m_axi_s2mm_aclk] [get_bd_pins axi_vdma_1/m_axis_mm2s_aclk] [get_bd_pins axi_vdma_1/s_axis_s2mm_aclk] [get_bd_pins axis_broadcaster_0/aclk] [get_bd_pins axis_combiner_0/clk] [get_bd_pins axis_pixel_compare_0/aclk] [get_bd_pins axis_subset_converter_0/aclk] [get_bd_pins axis_switch_0/aclk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: lvds_s0
proc create_hier_cell_lvds_s0 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_lvds_s0() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_MM2S

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM1

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM2

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S00_AXI

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S_AXIS

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_CTRL

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_LITE

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_LITE1


  # Create pins
  create_bd_pin -dir I -type clk aclk_100m
  create_bd_pin -dir I -type clk aclk_200m
  create_bd_pin -dir I -type rst aresetn_100m
  create_bd_pin -dir I -type rst aresetn_200m
  create_bd_pin -dir O -type intr intr
  create_bd_pin -dir I -type rst rst

  # Create instance: AXI4_Stream_Terminat_0, and set properties
  set AXI4_Stream_Terminat_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:AXI4_Stream_Terminator:1.0 AXI4_Stream_Terminat_0 ]

  # Create instance: axi_vdma_0, and set properties
  set axi_vdma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_vdma:6.3 axi_vdma_0 ]
  set_property -dict [ list \
   CONFIG.c_m_axi_mm2s_data_width {128} \
   CONFIG.c_m_axis_mm2s_tdata_width {24} \
   CONFIG.c_mm2s_max_burst_length {128} \
   CONFIG.c_num_fstores {3} \
   CONFIG.c_s2mm_max_burst_length {128} \
 ] $axi_vdma_0

  # Create instance: axi_vdma_1, and set properties
  set axi_vdma_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_vdma:6.3 axi_vdma_1 ]
  set_property -dict [ list \
   CONFIG.c_m_axi_mm2s_data_width {128} \
   CONFIG.c_m_axi_s2mm_data_width {128} \
   CONFIG.c_m_axis_mm2s_tdata_width {24} \
   CONFIG.c_mm2s_max_burst_length {128} \
   CONFIG.c_num_fstores {1} \
   CONFIG.c_s2mm_max_burst_length {128} \
 ] $axi_vdma_1

  # Create instance: axis_broadcaster_0, and set properties
  set axis_broadcaster_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_broadcaster:1.1 axis_broadcaster_0 ]
  set_property -dict [ list \
   CONFIG.M_TUSER_WIDTH {1} \
 ] $axis_broadcaster_0

  # Create instance: axis_combiner_0, and set properties
  set axis_combiner_0 [ create_bd_cell -type ip -vlnv xilinx.com:user:axis_combiner:1.1 axis_combiner_0 ]

  # Create instance: axis_pixel_compare_0, and set properties
  set axis_pixel_compare_0 [ create_bd_cell -type ip -vlnv xilinx.com:user:axis_pixel_compare:2.19 axis_pixel_compare_0 ]
  set_property -dict [ list \
   CONFIG.TUSER_WIDTH {1} \
 ] $axis_pixel_compare_0

  # Create instance: axis_subset_converter_0, and set properties
  set axis_subset_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter:1.1 axis_subset_converter_0 ]
  set_property -dict [ list \
   CONFIG.M_TDATA_NUM_BYTES {3} \
   CONFIG.S_TDATA_NUM_BYTES {6} \
   CONFIG.TDATA_REMAP {tdata[23:0]} \
 ] $axis_subset_converter_0

  # Create instance: axis_switch_0, and set properties
  set axis_switch_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_switch:1.1 axis_switch_0 ]
  set_property -dict [ list \
   CONFIG.DECODER_REG {1} \
   CONFIG.NUM_MI {2} \
   CONFIG.NUM_SI {1} \
   CONFIG.ROUTING_MODE {1} \
 ] $axis_switch_0

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]
  set_property -dict [ list \
   CONFIG.C_AUX_RST_WIDTH {1} \
   CONFIG.C_EXT_RST_WIDTH {1} \
 ] $proc_sys_reset_0

  # Create instance: util_vector_logic_0, and set properties
  set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_0 ]
  set_property -dict [ list \
   CONFIG.C_OPERATION {not} \
   CONFIG.C_SIZE {1} \
   CONFIG.LOGO_FILE {data/sym_notgate.png} \
 ] $util_vector_logic_0

  # Create interface connections
  connect_bd_intf_net -intf_net S_AXIS_1 [get_bd_intf_pins S_AXIS] [get_bd_intf_pins axi_vdma_0/S_AXIS_S2MM]
  connect_bd_intf_net -intf_net S_AXI_CTRL_1 [get_bd_intf_pins S_AXI_CTRL] [get_bd_intf_pins axis_switch_0/S_AXI_CTRL]
  connect_bd_intf_net -intf_net S_AXI_LITE1_1 [get_bd_intf_pins S_AXI_LITE1] [get_bd_intf_pins axi_vdma_0/S_AXI_LITE]
  connect_bd_intf_net -intf_net axi_vdma_0_M_AXIS_MM2S [get_bd_intf_pins axi_vdma_0/M_AXIS_MM2S] [get_bd_intf_pins axis_broadcaster_0/S_AXIS]
  connect_bd_intf_net -intf_net axi_vdma_0_M_AXI_MM2S [get_bd_intf_pins M_AXI_S2MM] [get_bd_intf_pins axi_vdma_0/M_AXI_MM2S]
  connect_bd_intf_net -intf_net axi_vdma_0_M_AXI_S2MM [get_bd_intf_pins M_AXI_S2MM1] [get_bd_intf_pins axi_vdma_0/M_AXI_S2MM]
  connect_bd_intf_net -intf_net axi_vdma_1_M_AXIS_MM2S [get_bd_intf_pins axi_vdma_1/M_AXIS_MM2S] [get_bd_intf_pins axis_combiner_0/axis_s1]
  connect_bd_intf_net -intf_net axi_vdma_1_M_AXI_MM2S [get_bd_intf_pins M_AXI_MM2S] [get_bd_intf_pins axi_vdma_1/M_AXI_MM2S]
  connect_bd_intf_net -intf_net axi_vdma_1_M_AXI_S2MM [get_bd_intf_pins M_AXI_S2MM2] [get_bd_intf_pins axi_vdma_1/M_AXI_S2MM]
  connect_bd_intf_net -intf_net axis_broadcaster_0_M00_AXIS [get_bd_intf_pins axis_broadcaster_0/M00_AXIS] [get_bd_intf_pins axis_combiner_0/axis_s0]
  connect_bd_intf_net -intf_net axis_broadcaster_0_M01_AXIS [get_bd_intf_pins axi_vdma_1/S_AXIS_S2MM] [get_bd_intf_pins axis_broadcaster_0/M01_AXIS]
  connect_bd_intf_net -intf_net axis_combiner_0_axis_m [get_bd_intf_pins axis_combiner_0/axis_m] [get_bd_intf_pins axis_pixel_compare_0/s_axis]
  connect_bd_intf_net -intf_net axis_pixel_compare_0_m_axis [get_bd_intf_pins axis_pixel_compare_0/m_axis] [get_bd_intf_pins axis_subset_converter_0/S_AXIS]
  connect_bd_intf_net -intf_net axis_subset_converter_0_M_AXIS [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins axis_switch_0/S00_AXIS]
  connect_bd_intf_net -intf_net axis_switch_0_M00_AXIS [get_bd_intf_pins M_AXIS] [get_bd_intf_pins axis_switch_0/M00_AXIS]
  connect_bd_intf_net -intf_net axis_switch_0_M01_AXIS [get_bd_intf_pins AXI4_Stream_Terminat_0/s_axis_video] [get_bd_intf_pins axis_switch_0/M01_AXIS]
  connect_bd_intf_net -intf_net processor_subsystem_M12_AXI [get_bd_intf_pins S00_AXI] [get_bd_intf_pins axis_pixel_compare_0/S00_AXI]
  connect_bd_intf_net -intf_net ps8_0_axi_periph_M04_AXI [get_bd_intf_pins S_AXI_LITE] [get_bd_intf_pins axi_vdma_1/S_AXI_LITE]

  # Create port connections
  connect_bd_net -net Net [get_bd_pins aclk_100m] [get_bd_pins axi_vdma_0/s_axi_lite_aclk] [get_bd_pins axi_vdma_1/s_axi_lite_aclk] [get_bd_pins axis_pixel_compare_0/s00_axi_aclk] [get_bd_pins axis_switch_0/s_axi_ctrl_aclk]
  connect_bd_net -net Net1 [get_bd_pins aresetn_100m] [get_bd_pins axi_vdma_0/axi_resetn] [get_bd_pins axi_vdma_1/axi_resetn] [get_bd_pins axis_pixel_compare_0/s00_axi_aresetn] [get_bd_pins axis_switch_0/s_axi_ctrl_aresetn]
  connect_bd_net -net axis_pixel_compare_0_intr [get_bd_pins intr] [get_bd_pins axis_pixel_compare_0/intr]
  connect_bd_net -net rst_1 [get_bd_pins rst] [get_bd_pins util_vector_logic_0/Op1]
  connect_bd_net -net rst_200m_peripheral_aresetn [get_bd_pins axis_broadcaster_0/aresetn] [get_bd_pins axis_combiner_0/rst_n] [get_bd_pins axis_pixel_compare_0/aresetn] [get_bd_pins proc_sys_reset_0/peripheral_aresetn]
  connect_bd_net -net s_axi_lite_aresetn_1 [get_bd_pins aresetn_200m] [get_bd_pins AXI4_Stream_Terminat_0/ap_rst_n] [get_bd_pins axis_subset_converter_0/aresetn] [get_bd_pins axis_switch_0/aresetn]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins proc_sys_reset_0/ext_reset_in] [get_bd_pins util_vector_logic_0/Res]
  connect_bd_net -net zynq_ultra_ps_e_0_pl_clk0 [get_bd_pins aclk_200m] [get_bd_pins AXI4_Stream_Terminat_0/ap_clk] [get_bd_pins axi_vdma_0/m_axi_mm2s_aclk] [get_bd_pins axi_vdma_0/m_axi_s2mm_aclk] [get_bd_pins axi_vdma_0/m_axis_mm2s_aclk] [get_bd_pins axi_vdma_0/s_axis_s2mm_aclk] [get_bd_pins axi_vdma_1/m_axi_mm2s_aclk] [get_bd_pins axi_vdma_1/m_axi_s2mm_aclk] [get_bd_pins axi_vdma_1/m_axis_mm2s_aclk] [get_bd_pins axi_vdma_1/s_axis_s2mm_aclk] [get_bd_pins axis_broadcaster_0/aclk] [get_bd_pins axis_combiner_0/clk] [get_bd_pins axis_pixel_compare_0/aclk] [get_bd_pins axis_subset_converter_0/aclk] [get_bd_pins axis_switch_0/aclk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: ethernet_subsystem
proc create_hier_cell_ethernet_subsystem { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_ethernet_subsystem() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_MM2S

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_SG

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXI_LITE

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 mgt_clk_0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:sfp_rtl:1.0 sfp_0


  # Create pins
  create_bd_pin -dir I -type clk axis_clk
  create_bd_pin -dir O -type intr interrupt
  create_bd_pin -dir I -type clk lite_aclk
  create_bd_pin -dir I -type rst lite_resetn
  create_bd_pin -dir O -type intr mac_irq
  create_bd_pin -dir O -type intr mm2s_introut
  create_bd_pin -dir I -type clk ref_clk
  create_bd_pin -dir O -type intr s2mm_introut
  create_bd_pin -dir I signal_detect

  # Create instance: axi_dma_0, and set properties
  set axi_dma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma_0 ]
  set_property -dict [ list \
   CONFIG.c_include_mm2s_dre {1} \
   CONFIG.c_include_s2mm_dre {1} \
   CONFIG.c_mm2s_burst_size {64} \
   CONFIG.c_s2mm_burst_size {64} \
   CONFIG.c_sg_length_width {16} \
   CONFIG.c_sg_use_stsapp_length {1} \
 ] $axi_dma_0

  # Create instance: axi_ethernet_0, and set properties
  set axi_ethernet_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_ethernet:7.2 axi_ethernet_0 ]
  set_property -dict [ list \
   CONFIG.MCAST_EXTEND {false} \
   CONFIG.PHYADDR {7} \
   CONFIG.PHY_TYPE {1000BaseX} \
   CONFIG.RXCSUM {Full} \
   CONFIG.RXMEM {32k} \
   CONFIG.RXVLAN_STRP {false} \
   CONFIG.RXVLAN_TAG {false} \
   CONFIG.RXVLAN_TRAN {false} \
   CONFIG.Statistics_Reset {true} \
   CONFIG.TXCSUM {Full} \
   CONFIG.TXMEM {32k} \
   CONFIG.TXVLAN_STRP {false} \
   CONFIG.TXVLAN_TAG {false} \
   CONFIG.TXVLAN_TRAN {false} \
   CONFIG.axiliteclkrate {100.0} \
   CONFIG.axisclkrate {200.0} \
   CONFIG.gtlocation {X0Y12} \
   CONFIG.gtrefclkrate {156.25} \
 ] $axi_ethernet_0

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins mgt_clk_0] [get_bd_intf_pins axi_ethernet_0/mgt_clk]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins sfp_0] [get_bd_intf_pins axi_ethernet_0/sfp]
  connect_bd_intf_net -intf_net S_AXI_LITE_1 [get_bd_intf_pins S_AXI_LITE] [get_bd_intf_pins axi_dma_0/S_AXI_LITE]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXIS_CNTRL [get_bd_intf_pins axi_dma_0/M_AXIS_CNTRL] [get_bd_intf_pins axi_ethernet_0/s_axis_txc]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXIS_MM2S [get_bd_intf_pins axi_dma_0/M_AXIS_MM2S] [get_bd_intf_pins axi_ethernet_0/s_axis_txd]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXI_MM2S [get_bd_intf_pins M_AXI_MM2S] [get_bd_intf_pins axi_dma_0/M_AXI_MM2S]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXI_S2MM [get_bd_intf_pins M_AXI_S2MM] [get_bd_intf_pins axi_dma_0/M_AXI_S2MM]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXI_SG [get_bd_intf_pins M_AXI_SG] [get_bd_intf_pins axi_dma_0/M_AXI_SG]
  connect_bd_intf_net -intf_net axi_ethernet_0_m_axis_rxd [get_bd_intf_pins axi_dma_0/S_AXIS_S2MM] [get_bd_intf_pins axi_ethernet_0/m_axis_rxd]
  connect_bd_intf_net -intf_net axi_ethernet_0_m_axis_rxs [get_bd_intf_pins axi_dma_0/S_AXIS_STS] [get_bd_intf_pins axi_ethernet_0/m_axis_rxs]
  connect_bd_intf_net -intf_net s_axi_1 [get_bd_intf_pins s_axi] [get_bd_intf_pins axi_ethernet_0/s_axi]

  # Create port connections
  connect_bd_net -net axi_dma_0_mm2s_cntrl_reset_out_n [get_bd_pins axi_dma_0/mm2s_cntrl_reset_out_n] [get_bd_pins axi_ethernet_0/axi_txc_arstn]
  connect_bd_net -net axi_dma_0_mm2s_introut [get_bd_pins mm2s_introut] [get_bd_pins axi_dma_0/mm2s_introut]
  connect_bd_net -net axi_dma_0_mm2s_prmry_reset_out_n [get_bd_pins axi_dma_0/mm2s_prmry_reset_out_n] [get_bd_pins axi_ethernet_0/axi_txd_arstn]
  connect_bd_net -net axi_dma_0_s2mm_introut [get_bd_pins s2mm_introut] [get_bd_pins axi_dma_0/s2mm_introut]
  connect_bd_net -net axi_dma_0_s2mm_prmry_reset_out_n [get_bd_pins axi_dma_0/s2mm_prmry_reset_out_n] [get_bd_pins axi_ethernet_0/axi_rxd_arstn]
  connect_bd_net -net axi_dma_0_s2mm_sts_reset_out_n [get_bd_pins axi_dma_0/s2mm_sts_reset_out_n] [get_bd_pins axi_ethernet_0/axi_rxs_arstn]
  connect_bd_net -net axi_ethernet_0_interrupt [get_bd_pins interrupt] [get_bd_pins axi_ethernet_0/interrupt]
  connect_bd_net -net axi_ethernet_0_mac_irq [get_bd_pins mac_irq] [get_bd_pins axi_ethernet_0/mac_irq]
  connect_bd_net -net clk_in1_1 [get_bd_pins axis_clk] [get_bd_pins axi_dma_0/m_axi_mm2s_aclk] [get_bd_pins axi_dma_0/m_axi_s2mm_aclk] [get_bd_pins axi_dma_0/m_axi_sg_aclk] [get_bd_pins axi_ethernet_0/axis_clk]
  connect_bd_net -net processor_subsystem_c0_ddr4_ui_clk [get_bd_pins lite_aclk] [get_bd_pins axi_dma_0/s_axi_lite_aclk] [get_bd_pins axi_ethernet_0/s_axi_lite_clk]
  connect_bd_net -net ref_clk_1 [get_bd_pins ref_clk] [get_bd_pins axi_ethernet_0/ref_clk]
  connect_bd_net -net rst_mig_7series_0_100M_peripheral_aresetn [get_bd_pins lite_resetn] [get_bd_pins axi_dma_0/axi_resetn] [get_bd_pins axi_ethernet_0/s_axi_lite_resetn]
  connect_bd_net -net signal_detect_1 [get_bd_pins signal_detect] [get_bd_pins axi_ethernet_0/signal_detect]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: DP_RX_hier_1
proc create_hier_cell_DP_RX_hier_1 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_DP_RX_hier_1() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 Vid_Out_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 ext_iic_1

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_phy_rx_sb_control

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi1

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_dprxss

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane1

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane2

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane3

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_phy_rx_sb_status


  # Create pins
  create_bd_pin -dir I -type rst areset_200m
  create_bd_pin -dir I -type rst aresetn_100m
  create_bd_pin -dir IO aux_rx_io_n_0
  create_bd_pin -dir IO aux_rx_io_p_0
  create_bd_pin -dir I -type clk clk_100m
  create_bd_pin -dir I -type clk clk_200m
  create_bd_pin -dir O -type intr dprxss_dp_irq
  create_bd_pin -dir O -type intr dprxss_iic_irq
  create_bd_pin -dir I -type rst reset_100m
  create_bd_pin -dir O -type data rx_hpd
  create_bd_pin -dir I -type clk rx_lnk_clk

  # Create instance: util_reduced_logic_0, and set properties
  set util_reduced_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_reduced_logic:2.0 util_reduced_logic_0 ]
  set_property -dict [ list \
   CONFIG.C_SIZE {2} \
 ] $util_reduced_logic_0

  # Create instance: v_dp_rxss1_0, and set properties
  set v_dp_rxss1_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_dp_rxss1:2.1 v_dp_rxss1_0 ]
  set_property -dict [ list \
   CONFIG.AUDIO_ENABLE {0} \
   CONFIG.AUX_IO_LOC {0} \
   CONFIG.C_LINK_RATE {30} \
   CONFIG.INCLUDE_AXI_IIC {1} \
   CONFIG.LINK_RATE {8.1} \
 ] $v_dp_rxss1_0

  # Create instance: vid_edid_0, and set properties
  set vid_edid_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:vid_edid:1.1 vid_edid_0 ]

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins s_axis_lnk_rx_lane0] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane0]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins s_axi1] [get_bd_intf_pins vid_edid_0/s_axi]
  connect_bd_intf_net -intf_net Conn3 [get_bd_intf_pins ext_iic_1] [get_bd_intf_pins v_dp_rxss1_0/ext_iic]
  connect_bd_intf_net -intf_net Conn4 [get_bd_intf_pins s_axis_lnk_rx_lane2] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane2]
  connect_bd_intf_net -intf_net Conn5 [get_bd_intf_pins s_axis_phy_rx_sb_status] [get_bd_intf_pins v_dp_rxss1_0/s_axis_phy_rx_sb_status]
  connect_bd_intf_net -intf_net Conn6 [get_bd_intf_pins m_axis_phy_rx_sb_control] [get_bd_intf_pins v_dp_rxss1_0/m_axis_phy_rx_sb_control]
  connect_bd_intf_net -intf_net Conn7 [get_bd_intf_pins s_axis_lnk_rx_lane1] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane1]
  connect_bd_intf_net -intf_net Conn8 [get_bd_intf_pins s_axis_lnk_rx_lane3] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane3]
  connect_bd_intf_net -intf_net s_axi_dprxss_1 [get_bd_intf_pins s_axi_dprxss] [get_bd_intf_pins v_dp_rxss1_0/s_axi]
  connect_bd_intf_net -intf_net v_dp_rxss1_0_m_axis_video_stream1 [get_bd_intf_pins Vid_Out_AXIS] [get_bd_intf_pins v_dp_rxss1_0/m_axis_video_stream1]

  # Create port connections
  connect_bd_net -net Net [get_bd_pins v_dp_rxss1_0/edid_iic_scl_i] [get_bd_pins v_dp_rxss1_0/edid_iic_scl_t] [get_bd_pins vid_edid_0/iic_scl_in]
  connect_bd_net -net Net1 [get_bd_pins aux_rx_io_p_0] [get_bd_pins v_dp_rxss1_0/aux_rx_io_p]
  connect_bd_net -net Net2 [get_bd_pins aux_rx_io_n_0] [get_bd_pins v_dp_rxss1_0/aux_rx_io_n]
  connect_bd_net -net Net3 [get_bd_pins clk_200m] [get_bd_pins v_dp_rxss1_0/m_axis_aclk_stream1] [get_bd_pins v_dp_rxss1_0/rx_vid_clk]
  connect_bd_net -net ctl_reset_1 [get_bd_pins areset_200m] [get_bd_pins v_dp_rxss1_0/rx_vid_rst]
  connect_bd_net -net ctl_reset_2 [get_bd_pins reset_100m] [get_bd_pins vid_edid_0/ctl_reset]
  connect_bd_net -net dp_rx_subsystem_0_edid_iic_sda_t [get_bd_pins v_dp_rxss1_0/edid_iic_sda_t] [get_bd_pins vid_edid_0/iic_sda_in] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net rx_lnk_clk_1 [get_bd_pins rx_lnk_clk] [get_bd_pins v_dp_rxss1_0/rx_lnk_clk]
  connect_bd_net -net util_reduced_logic_0_Res [get_bd_pins util_reduced_logic_0/Res] [get_bd_pins v_dp_rxss1_0/edid_iic_sda_i]
  connect_bd_net -net v_dp_rxss1_0_dprxss_dp_irq [get_bd_pins dprxss_dp_irq] [get_bd_pins v_dp_rxss1_0/dprxss_dp_irq]
  connect_bd_net -net v_dp_rxss1_0_dprxss_iic_irq [get_bd_pins dprxss_iic_irq] [get_bd_pins v_dp_rxss1_0/dprxss_iic_irq]
  connect_bd_net -net v_dp_rxss1_0_rx_hpd [get_bd_pins rx_hpd] [get_bd_pins v_dp_rxss1_0/rx_hpd]
  connect_bd_net -net vid_edid_0_iic_sda_out [get_bd_pins vid_edid_0/iic_sda_out] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net vid_phy_axi4lite_aclk_1 [get_bd_pins clk_100m] [get_bd_pins v_dp_rxss1_0/s_axi_aclk] [get_bd_pins vid_edid_0/ctl_clk] [get_bd_pins vid_edid_0/s_axi_aclk]
  connect_bd_net -net vid_phy_axi4lite_aresetn_1 [get_bd_pins aresetn_100m] [get_bd_pins v_dp_rxss1_0/s_axi_aresetn] [get_bd_pins vid_edid_0/s_axi_aresetn]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins util_reduced_logic_0/Op1] [get_bd_pins xlconcat_0/dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: DP_RX_hier_0
proc create_hier_cell_DP_RX_hier_0 { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_DP_RX_hier_0() - Empty argument(s)!"}
     return
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 Vid_Out_AXIS

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 ext_iic_0

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_phy_rx_sb_control

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi1

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_dprxss

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane0

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane1

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane2

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_lnk_rx_lane3

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_phy_rx_sb_status


  # Create pins
  create_bd_pin -dir I -type rst areset_200m
  create_bd_pin -dir I -type rst aresetn_100m
  create_bd_pin -dir IO aux_rx_io_n_0
  create_bd_pin -dir IO aux_rx_io_p_0
  create_bd_pin -dir I -type clk clk_100m
  create_bd_pin -dir I -type clk clk_200m
  create_bd_pin -dir O -type intr dprxss_dp_irq
  create_bd_pin -dir O -type intr dprxss_iic_irq
  create_bd_pin -dir I -type rst reset_100m
  create_bd_pin -dir O -type data rx_hpd
  create_bd_pin -dir I -type clk rx_lnk_clk

  # Create instance: util_reduced_logic_0, and set properties
  set util_reduced_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_reduced_logic:2.0 util_reduced_logic_0 ]
  set_property -dict [ list \
   CONFIG.C_SIZE {2} \
 ] $util_reduced_logic_0

  # Create instance: v_dp_rxss1_0, and set properties
  set v_dp_rxss1_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:v_dp_rxss1:2.1 v_dp_rxss1_0 ]
  set_property -dict [ list \
   CONFIG.AUDIO_ENABLE {0} \
   CONFIG.AUX_IO_LOC {0} \
   CONFIG.C_LINK_RATE {30} \
   CONFIG.INCLUDE_AXI_IIC {1} \
   CONFIG.LINK_RATE {8.1} \
 ] $v_dp_rxss1_0

  # Create instance: vid_edid_0, and set properties
  set vid_edid_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:vid_edid:1.1 vid_edid_0 ]

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins s_axis_lnk_rx_lane3] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane3]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins s_axi1] [get_bd_intf_pins vid_edid_0/s_axi]
  connect_bd_intf_net -intf_net Conn3 [get_bd_intf_pins ext_iic_0] [get_bd_intf_pins v_dp_rxss1_0/ext_iic]
  connect_bd_intf_net -intf_net Conn4 [get_bd_intf_pins s_axis_lnk_rx_lane0] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane0]
  connect_bd_intf_net -intf_net Conn5 [get_bd_intf_pins m_axis_phy_rx_sb_control] [get_bd_intf_pins v_dp_rxss1_0/m_axis_phy_rx_sb_control]
  connect_bd_intf_net -intf_net Conn6 [get_bd_intf_pins s_axis_lnk_rx_lane1] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane1]
  connect_bd_intf_net -intf_net Conn7 [get_bd_intf_pins s_axis_lnk_rx_lane2] [get_bd_intf_pins v_dp_rxss1_0/s_axis_lnk_rx_lane2]
  connect_bd_intf_net -intf_net Conn8 [get_bd_intf_pins s_axis_phy_rx_sb_status] [get_bd_intf_pins v_dp_rxss1_0/s_axis_phy_rx_sb_status]
  connect_bd_intf_net -intf_net s_axi_dprxss_1 [get_bd_intf_pins s_axi_dprxss] [get_bd_intf_pins v_dp_rxss1_0/s_axi]
  connect_bd_intf_net -intf_net v_dp_rxss1_0_m_axis_video_stream1 [get_bd_intf_pins Vid_Out_AXIS] [get_bd_intf_pins v_dp_rxss1_0/m_axis_video_stream1]

  # Create port connections
  connect_bd_net -net Net [get_bd_pins v_dp_rxss1_0/edid_iic_scl_i] [get_bd_pins v_dp_rxss1_0/edid_iic_scl_t] [get_bd_pins vid_edid_0/iic_scl_in]
  connect_bd_net -net Net1 [get_bd_pins aux_rx_io_p_0] [get_bd_pins v_dp_rxss1_0/aux_rx_io_p]
  connect_bd_net -net Net2 [get_bd_pins aux_rx_io_n_0] [get_bd_pins v_dp_rxss1_0/aux_rx_io_n]
  connect_bd_net -net Net3 [get_bd_pins clk_200m] [get_bd_pins v_dp_rxss1_0/m_axis_aclk_stream1] [get_bd_pins v_dp_rxss1_0/rx_vid_clk]
  connect_bd_net -net ctl_reset_1 [get_bd_pins areset_200m] [get_bd_pins v_dp_rxss1_0/rx_vid_rst]
  connect_bd_net -net ctl_reset_2 [get_bd_pins reset_100m] [get_bd_pins vid_edid_0/ctl_reset]
  connect_bd_net -net dp_rx_subsystem_0_edid_iic_sda_t [get_bd_pins v_dp_rxss1_0/edid_iic_sda_t] [get_bd_pins vid_edid_0/iic_sda_in] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net rx_lnk_clk_1 [get_bd_pins rx_lnk_clk] [get_bd_pins v_dp_rxss1_0/rx_lnk_clk]
  connect_bd_net -net util_reduced_logic_0_Res [get_bd_pins util_reduced_logic_0/Res] [get_bd_pins v_dp_rxss1_0/edid_iic_sda_i]
  connect_bd_net -net v_dp_rxss1_0_dprxss_dp_irq [get_bd_pins dprxss_dp_irq] [get_bd_pins v_dp_rxss1_0/dprxss_dp_irq]
  connect_bd_net -net v_dp_rxss1_0_dprxss_iic_irq [get_bd_pins dprxss_iic_irq] [get_bd_pins v_dp_rxss1_0/dprxss_iic_irq]
  connect_bd_net -net v_dp_rxss1_0_rx_hpd [get_bd_pins rx_hpd] [get_bd_pins v_dp_rxss1_0/rx_hpd]
  connect_bd_net -net vid_edid_0_iic_sda_out [get_bd_pins vid_edid_0/iic_sda_out] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net vid_phy_axi4lite_aclk_1 [get_bd_pins clk_100m] [get_bd_pins v_dp_rxss1_0/s_axi_aclk] [get_bd_pins vid_edid_0/ctl_clk] [get_bd_pins vid_edid_0/s_axi_aclk]
  connect_bd_net -net vid_phy_axi4lite_aresetn_1 [get_bd_pins aresetn_100m] [get_bd_pins v_dp_rxss1_0/s_axi_aresetn] [get_bd_pins vid_edid_0/s_axi_aresetn]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins util_reduced_logic_0/Op1] [get_bd_pins xlconcat_0/dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}


# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set C0_DDR4 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:ddr4_rtl:1.0 C0_DDR4 ]

  set MCDP6000_IIC_0 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 MCDP6000_IIC_0 ]

  set MCDP6000_IIC_1 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 MCDP6000_IIC_1 ]

  set gpio [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 gpio ]

  set i2c_0 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_0 ]

  set i2c_1 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_1 ]

  set i2c_2 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_2 ]

  set i2c_3 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_3 ]

  set i2c_4 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 i2c_4 ]

  set mgt_clk_0 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 mgt_clk_0 ]

  set sfp_0 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:sfp_rtl:1.0 sfp_0 ]

  set sys [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 sys ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {200000000} \
   ] $sys

  set uart_0 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:uart_rtl:1.0 uart_0 ]


  # Create ports
  set aux_rx_io_n_0 [ create_bd_port -dir IO aux_rx_io_n_0 ]
  set aux_rx_io_n_1 [ create_bd_port -dir IO aux_rx_io_n_1 ]
  set aux_rx_io_p_0 [ create_bd_port -dir IO aux_rx_io_p_0 ]
  set aux_rx_io_p_1 [ create_bd_port -dir IO aux_rx_io_p_1 ]
  set c0_init_calib_complete [ create_bd_port -dir O c0_init_calib_complete ]
  set lnk_rx_lane_n_0 [ create_bd_port -dir I -from 3 -to 0 lnk_rx_lane_n_0 ]
  set lnk_rx_lane_n_1 [ create_bd_port -dir I -from 3 -to 0 lnk_rx_lane_n_1 ]
  set lnk_rx_lane_p_0 [ create_bd_port -dir I -from 3 -to 0 lnk_rx_lane_p_0 ]
  set lnk_rx_lane_p_1 [ create_bd_port -dir I -from 3 -to 0 lnk_rx_lane_p_1 ]
  set mgtrefclk0_pad_n_in_0 [ create_bd_port -dir I -type clk mgtrefclk0_pad_n_in_0 ]
  set mgtrefclk0_pad_n_in_1 [ create_bd_port -dir I -type clk mgtrefclk0_pad_n_in_1 ]
  set mgtrefclk0_pad_p_in_0 [ create_bd_port -dir I -type clk mgtrefclk0_pad_p_in_0 ]
  set mgtrefclk0_pad_p_in_1 [ create_bd_port -dir I -type clk mgtrefclk0_pad_p_in_1 ]
  set mgtrefclk1_pad_n_in_0 [ create_bd_port -dir I -type clk mgtrefclk1_pad_n_in_0 ]
  set mgtrefclk1_pad_n_in_1 [ create_bd_port -dir I -type clk mgtrefclk1_pad_n_in_1 ]
  set mgtrefclk1_pad_p_in_0 [ create_bd_port -dir I -type clk mgtrefclk1_pad_p_in_0 ]
  set mgtrefclk1_pad_p_in_1 [ create_bd_port -dir I -type clk mgtrefclk1_pad_p_in_1 ]
  set rx_hpd_0 [ create_bd_port -dir O -type data rx_hpd_0 ]
  set rx_hpd_1 [ create_bd_port -dir O -type data rx_hpd_1 ]

  # Create instance: DP_RX_hier_0
  create_hier_cell_DP_RX_hier_0 [current_bd_instance .] DP_RX_hier_0

  # Create instance: DP_RX_hier_1
  create_hier_cell_DP_RX_hier_1 [current_bd_instance .] DP_RX_hier_1

  # Create instance: GND, and set properties
  set GND [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 GND ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
 ] $GND

  # Create instance: HW_VER, and set properties
  set HW_VER [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 HW_VER ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0x20260607} \
   CONFIG.CONST_WIDTH {32} \
 ] $HW_VER

  # Create instance: VCC, and set properties
  set VCC [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 VCC ]

  # Create instance: axi_gpio_lvds_reset, and set properties
  set axi_gpio_lvds_reset [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_lvds_reset ]
  set_property -dict [ list \
   CONFIG.C_ALL_OUTPUTS {1} \
   CONFIG.C_GPIO_WIDTH {2} \
 ] $axi_gpio_lvds_reset

  # Create instance: axi_vdma_lwip, and set properties
  set axi_vdma_lwip [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_vdma:6.3 axi_vdma_lwip ]
  set_property -dict [ list \
   CONFIG.c_include_mm2s {0} \
   CONFIG.c_m_axi_s2mm_data_width {128} \
   CONFIG.c_mm2s_genlock_mode {0} \
   CONFIG.c_s2mm_max_burst_length {128} \
 ] $axi_vdma_lwip

  # Create instance: axis_passthrough_mon_0, and set properties
  set axis_passthrough_mon_0 [ create_bd_cell -type ip -vlnv xilinx.com:user:axis_passthrough_monitor:1.1 axis_passthrough_mon_0 ]
  set_property -dict [ list \
   CONFIG.AXIS_WIDTH {96} \
   CONFIG.PPC {4} \
 ] $axis_passthrough_mon_0

  # Create instance: axis_passthrough_mon_1, and set properties
  set axis_passthrough_mon_1 [ create_bd_cell -type ip -vlnv xilinx.com:user:axis_passthrough_monitor:1.1 axis_passthrough_mon_1 ]
  set_property -dict [ list \
   CONFIG.AXIS_WIDTH {96} \
   CONFIG.PPC {4} \
 ] $axis_passthrough_mon_1

  # Create instance: axis_switch_0, and set properties
  set axis_switch_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_switch:1.1 axis_switch_0 ]
  set_property -dict [ list \
   CONFIG.NUM_SI {2} \
   CONFIG.ROUTING_MODE {1} \
 ] $axis_switch_0

  # Create instance: ethernet_subsystem
  create_hier_cell_ethernet_subsystem [current_bd_instance .] ethernet_subsystem

  # Create instance: lvds_s0
  create_hier_cell_lvds_s0 [current_bd_instance .] lvds_s0

  # Create instance: lvds_s1
  create_hier_cell_lvds_s1 [current_bd_instance .] lvds_s1

  # Create instance: processor_subsystem
  create_hier_cell_processor_subsystem [current_bd_instance .] processor_subsystem

  # Create instance: vid_phy_controller_0, and set properties
  set vid_phy_controller_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:vid_phy_controller:2.2 vid_phy_controller_0 ]
  set_property -dict [ list \
   CONFIG.Adv_Clk_Mode {false} \
   CONFIG.C_Tx_Protocol {None} \
   CONFIG.Rx_Max_GT_Line_Rate {8.1} \
 ] $vid_phy_controller_0

  # Create instance: vid_phy_controller_1, and set properties
  set vid_phy_controller_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:vid_phy_controller:2.2 vid_phy_controller_1 ]
  set_property -dict [ list \
   CONFIG.Adv_Clk_Mode {false} \
   CONFIG.CHANNEL_ENABLE {X0Y16 X0Y17 X0Y18 X0Y19} \
   CONFIG.CHANNEL_SITE {X0Y16} \
   CONFIG.C_Tx_Protocol {None} \
   CONFIG.Rx_Max_GT_Line_Rate {8.1} \
 ] $vid_phy_controller_1

  # Create instance: xgpio_i2c_0
  create_hier_cell_xgpio_i2c_0 [current_bd_instance .] xgpio_i2c_0

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_WIDTH {2} \
 ] $xlslice_0

  # Create instance: xlslice_1, and set properties
  set xlslice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {1} \
   CONFIG.DIN_TO {1} \
   CONFIG.DIN_WIDTH {2} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_1

  # Create interface connections
  connect_bd_intf_net -intf_net DP_RX_hier_0_Vid_Out_AXIS [get_bd_intf_pins DP_RX_hier_0/Vid_Out_AXIS] [get_bd_intf_pins axis_passthrough_mon_0/s_axis]
  connect_bd_intf_net -intf_net DP_RX_hier_0_ext_iic_0 [get_bd_intf_ports MCDP6000_IIC_0] [get_bd_intf_pins DP_RX_hier_0/ext_iic_0]
  connect_bd_intf_net -intf_net DP_RX_hier_0_m_axis_phy_rx_sb_control [get_bd_intf_pins DP_RX_hier_0/m_axis_phy_rx_sb_control] [get_bd_intf_pins vid_phy_controller_0/vid_phy_control_sb_rx]
  connect_bd_intf_net -intf_net DP_RX_hier_1_Vid_Out_AXIS [get_bd_intf_pins DP_RX_hier_1/Vid_Out_AXIS] [get_bd_intf_pins axis_passthrough_mon_1/s_axis]
  connect_bd_intf_net -intf_net DP_RX_hier_1_ext_iic_1 [get_bd_intf_ports MCDP6000_IIC_1] [get_bd_intf_pins DP_RX_hier_1/ext_iic_1]
  connect_bd_intf_net -intf_net DP_RX_hier_1_m_axis_phy_rx_sb_control [get_bd_intf_pins DP_RX_hier_1/m_axis_phy_rx_sb_control] [get_bd_intf_pins vid_phy_controller_1/vid_phy_control_sb_rx]
  connect_bd_intf_net -intf_net S00_AXI_1 [get_bd_intf_pins lvds_s1/S00_AXI] [get_bd_intf_pins processor_subsystem/M25_AXI]
  connect_bd_intf_net -intf_net S_AXIS_1 [get_bd_intf_pins axis_passthrough_mon_0/m_axis] [get_bd_intf_pins lvds_s0/S_AXIS]
  connect_bd_intf_net -intf_net S_AXIS_2 [get_bd_intf_pins axis_passthrough_mon_1/m_axis] [get_bd_intf_pins lvds_s1/S_AXIS]
  connect_bd_intf_net -intf_net S_AXI_2 [get_bd_intf_pins processor_subsystem/M29_AXI] [get_bd_intf_pins xgpio_i2c_0/S_AXI]
  connect_bd_intf_net -intf_net S_AXI_CTRL_1 [get_bd_intf_pins lvds_s1/S_AXI_CTRL] [get_bd_intf_pins processor_subsystem/M26_AXI]
  connect_bd_intf_net -intf_net S_AXI_LITE1_1 [get_bd_intf_pins lvds_s1/S_AXI_LITE1] [get_bd_intf_pins processor_subsystem/M23_AXI]
  connect_bd_intf_net -intf_net S_AXI_LITE_1 [get_bd_intf_pins ethernet_subsystem/S_AXI_LITE] [get_bd_intf_pins processor_subsystem/M17_AXI]
  connect_bd_intf_net -intf_net S_AXI_LITE_4 [get_bd_intf_pins lvds_s1/S_AXI_LITE] [get_bd_intf_pins processor_subsystem/M24_AXI]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXI_MM2S [get_bd_intf_pins ethernet_subsystem/M_AXI_MM2S] [get_bd_intf_pins processor_subsystem/S02_AXI]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXI_S2MM [get_bd_intf_pins ethernet_subsystem/M_AXI_S2MM] [get_bd_intf_pins processor_subsystem/S03_AXI]
  connect_bd_intf_net -intf_net axi_dma_0_M_AXI_SG [get_bd_intf_pins ethernet_subsystem/M_AXI_SG] [get_bd_intf_pins processor_subsystem/S04_AXI]
  connect_bd_intf_net -intf_net axi_vdma_1_M_AXI_S2MM [get_bd_intf_pins axi_vdma_lwip/M_AXI_S2MM] [get_bd_intf_pins processor_subsystem/S07_AXI]
  connect_bd_intf_net -intf_net axis_switch_0_M00_AXIS [get_bd_intf_pins axi_vdma_lwip/S_AXIS_S2MM] [get_bd_intf_pins axis_switch_0/M00_AXIS]
  connect_bd_intf_net -intf_net ethernet_subsystem_sfp_0 [get_bd_intf_ports sfp_0] [get_bd_intf_pins ethernet_subsystem/sfp_0]
  connect_bd_intf_net -intf_net lvds_s1_M_AXIS [get_bd_intf_pins axis_switch_0/S00_AXIS] [get_bd_intf_pins lvds_s0/M_AXIS]
  connect_bd_intf_net -intf_net lvds_s1_M_AXI_MM2S [get_bd_intf_pins lvds_s0/M_AXI_MM2S] [get_bd_intf_pins processor_subsystem/S08_AXI]
  connect_bd_intf_net -intf_net lvds_s1_M_AXI_S2MM [get_bd_intf_pins lvds_s0/M_AXI_S2MM] [get_bd_intf_pins processor_subsystem/S05_AXI]
  connect_bd_intf_net -intf_net lvds_s1_M_AXI_S2MM1 [get_bd_intf_pins lvds_s0/M_AXI_S2MM1] [get_bd_intf_pins processor_subsystem/S06_AXI]
  connect_bd_intf_net -intf_net lvds_s1_M_AXI_S2MM2 [get_bd_intf_pins lvds_s0/M_AXI_S2MM2] [get_bd_intf_pins processor_subsystem/S09_AXI]
  connect_bd_intf_net -intf_net lvds_s2_M_AXIS [get_bd_intf_pins axis_switch_0/S01_AXIS] [get_bd_intf_pins lvds_s1/M_AXIS]
  connect_bd_intf_net -intf_net lvds_s2_M_AXI_MM2S [get_bd_intf_pins lvds_s1/M_AXI_MM2S] [get_bd_intf_pins processor_subsystem/S12_AXI]
  connect_bd_intf_net -intf_net lvds_s2_M_AXI_S2MM [get_bd_intf_pins lvds_s1/M_AXI_S2MM] [get_bd_intf_pins processor_subsystem/S10_AXI]
  connect_bd_intf_net -intf_net lvds_s2_M_AXI_S2MM1 [get_bd_intf_pins lvds_s1/M_AXI_S2MM1] [get_bd_intf_pins processor_subsystem/S11_AXI]
  connect_bd_intf_net -intf_net lvds_s2_M_AXI_S2MM2 [get_bd_intf_pins lvds_s1/M_AXI_S2MM2] [get_bd_intf_pins processor_subsystem/S13_AXI]
  connect_bd_intf_net -intf_net mgt_clk_0_1 [get_bd_intf_ports mgt_clk_0] [get_bd_intf_pins ethernet_subsystem/mgt_clk_0]
  connect_bd_intf_net -intf_net processor_subsystem_C0_DDR4 [get_bd_intf_ports C0_DDR4] [get_bd_intf_pins processor_subsystem/C0_DDR4]
  connect_bd_intf_net -intf_net processor_subsystem_M09_AXI [get_bd_intf_pins axi_vdma_lwip/S_AXI_LITE] [get_bd_intf_pins processor_subsystem/M09_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M11_AXI [get_bd_intf_pins axis_switch_0/S_AXI_CTRL] [get_bd_intf_pins processor_subsystem/M12_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M11_AXI1 [get_bd_intf_pins lvds_s0/S_AXI_LITE1] [get_bd_intf_pins processor_subsystem/M19_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M13_AXI [get_bd_intf_pins lvds_s0/S_AXI_LITE] [get_bd_intf_pins processor_subsystem/M20_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M19_AXI [get_bd_intf_pins axis_passthrough_mon_0/s_axi_lite] [get_bd_intf_pins processor_subsystem/M10_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M20_AXI [get_bd_intf_pins lvds_s0/S00_AXI] [get_bd_intf_pins processor_subsystem/M21_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M23_AXI1 [get_bd_intf_pins processor_subsystem/M27_AXI] [get_bd_intf_pins vid_phy_controller_0/vid_phy_axi4lite]
  connect_bd_intf_net -intf_net processor_subsystem_M25_AXI [get_bd_intf_pins axis_passthrough_mon_1/s_axi_lite] [get_bd_intf_pins processor_subsystem/M11_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M26_AXI [get_bd_intf_pins lvds_s0/S_AXI_CTRL] [get_bd_intf_pins processor_subsystem/M22_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_M29_AXI [get_bd_intf_pins processor_subsystem/M28_AXI] [get_bd_intf_pins vid_phy_controller_1/vid_phy_axi4lite]
  connect_bd_intf_net -intf_net processor_subsystem_M35_AXI [get_bd_intf_pins axi_gpio_lvds_reset/S_AXI] [get_bd_intf_pins processor_subsystem/M08_AXI]
  connect_bd_intf_net -intf_net processor_subsystem_gpio_rtl_0 [get_bd_intf_ports gpio] [get_bd_intf_pins processor_subsystem/gpio]
  connect_bd_intf_net -intf_net processor_subsystem_uart_0 [get_bd_intf_ports uart_0] [get_bd_intf_pins processor_subsystem/uart_0]
  connect_bd_intf_net -intf_net s_axi1_1 [get_bd_intf_pins DP_RX_hier_0/s_axi1] [get_bd_intf_pins processor_subsystem/M14_AXI]
  connect_bd_intf_net -intf_net s_axi1_2 [get_bd_intf_pins DP_RX_hier_1/s_axi1] [get_bd_intf_pins processor_subsystem/M16_AXI]
  connect_bd_intf_net -intf_net s_axi_1 [get_bd_intf_pins ethernet_subsystem/s_axi] [get_bd_intf_pins processor_subsystem/M18_AXI]
  connect_bd_intf_net -intf_net s_axi_dprxss_1 [get_bd_intf_pins DP_RX_hier_0/s_axi_dprxss] [get_bd_intf_pins processor_subsystem/M13_AXI]
  connect_bd_intf_net -intf_net s_axi_dprxss_2 [get_bd_intf_pins DP_RX_hier_1/s_axi_dprxss] [get_bd_intf_pins processor_subsystem/M15_AXI]
  connect_bd_intf_net -intf_net sys_1 [get_bd_intf_ports sys] [get_bd_intf_pins processor_subsystem/sys]
  connect_bd_intf_net -intf_net vid_phy_controller_0_vid_phy_rx_axi4s_ch0 [get_bd_intf_pins DP_RX_hier_0/s_axis_lnk_rx_lane0] [get_bd_intf_pins vid_phy_controller_0/vid_phy_rx_axi4s_ch0]
  connect_bd_intf_net -intf_net vid_phy_controller_0_vid_phy_rx_axi4s_ch1 [get_bd_intf_pins DP_RX_hier_0/s_axis_lnk_rx_lane1] [get_bd_intf_pins vid_phy_controller_0/vid_phy_rx_axi4s_ch1]
  connect_bd_intf_net -intf_net vid_phy_controller_0_vid_phy_rx_axi4s_ch2 [get_bd_intf_pins DP_RX_hier_0/s_axis_lnk_rx_lane2] [get_bd_intf_pins vid_phy_controller_0/vid_phy_rx_axi4s_ch2]
  connect_bd_intf_net -intf_net vid_phy_controller_0_vid_phy_rx_axi4s_ch3 [get_bd_intf_pins DP_RX_hier_0/s_axis_lnk_rx_lane3] [get_bd_intf_pins vid_phy_controller_0/vid_phy_rx_axi4s_ch3]
  connect_bd_intf_net -intf_net vid_phy_controller_0_vid_phy_status_sb_rx [get_bd_intf_pins DP_RX_hier_0/s_axis_phy_rx_sb_status] [get_bd_intf_pins vid_phy_controller_0/vid_phy_status_sb_rx]
  connect_bd_intf_net -intf_net vid_phy_controller_1_vid_phy_rx_axi4s_ch0 [get_bd_intf_pins DP_RX_hier_1/s_axis_lnk_rx_lane0] [get_bd_intf_pins vid_phy_controller_1/vid_phy_rx_axi4s_ch0]
  connect_bd_intf_net -intf_net vid_phy_controller_1_vid_phy_rx_axi4s_ch1 [get_bd_intf_pins DP_RX_hier_1/s_axis_lnk_rx_lane1] [get_bd_intf_pins vid_phy_controller_1/vid_phy_rx_axi4s_ch1]
  connect_bd_intf_net -intf_net vid_phy_controller_1_vid_phy_rx_axi4s_ch2 [get_bd_intf_pins DP_RX_hier_1/s_axis_lnk_rx_lane2] [get_bd_intf_pins vid_phy_controller_1/vid_phy_rx_axi4s_ch2]
  connect_bd_intf_net -intf_net vid_phy_controller_1_vid_phy_rx_axi4s_ch3 [get_bd_intf_pins DP_RX_hier_1/s_axis_lnk_rx_lane3] [get_bd_intf_pins vid_phy_controller_1/vid_phy_rx_axi4s_ch3]
  connect_bd_intf_net -intf_net vid_phy_controller_1_vid_phy_status_sb_rx [get_bd_intf_pins DP_RX_hier_1/s_axis_phy_rx_sb_status] [get_bd_intf_pins vid_phy_controller_1/vid_phy_status_sb_rx]
  connect_bd_intf_net -intf_net xgpio_i2c_0_i2c_0 [get_bd_intf_ports i2c_0] [get_bd_intf_pins xgpio_i2c_0/i2c_0]
  connect_bd_intf_net -intf_net xgpio_i2c_0_i2c_1 [get_bd_intf_ports i2c_1] [get_bd_intf_pins xgpio_i2c_0/i2c_1]
  connect_bd_intf_net -intf_net xgpio_i2c_0_i2c_2 [get_bd_intf_ports i2c_2] [get_bd_intf_pins xgpio_i2c_0/i2c_2]
  connect_bd_intf_net -intf_net xgpio_i2c_0_i2c_3 [get_bd_intf_ports i2c_3] [get_bd_intf_pins xgpio_i2c_0/i2c_3]
  connect_bd_intf_net -intf_net xgpio_i2c_0_i2c_4 [get_bd_intf_ports i2c_4] [get_bd_intf_pins xgpio_i2c_0/i2c_4]

  # Create port connections
  connect_bd_net -net DP_RX_hier_0_dprxss_dp_irq [get_bd_pins DP_RX_hier_0/dprxss_dp_irq] [get_bd_pins processor_subsystem/irq_in_6]
  connect_bd_net -net DP_RX_hier_0_rx_hpd [get_bd_ports rx_hpd_0] [get_bd_pins DP_RX_hier_0/rx_hpd]
  connect_bd_net -net DP_RX_hier_1_dprxss_dp_irq [get_bd_pins DP_RX_hier_1/dprxss_dp_irq] [get_bd_pins processor_subsystem/irq_in_8]
  connect_bd_net -net DP_RX_hier_1_rx_hpd [get_bd_ports rx_hpd_1] [get_bd_pins DP_RX_hier_1/rx_hpd]
  connect_bd_net -net GND_dout [get_bd_pins GND/dout] [get_bd_pins processor_subsystem/ddr_rst]
  connect_bd_net -net HW_VER_dout [get_bd_pins HW_VER/dout] [get_bd_pins processor_subsystem/VERSION]
  connect_bd_net -net Net [get_bd_ports aux_rx_io_n_0] [get_bd_pins DP_RX_hier_0/aux_rx_io_n_0]
  connect_bd_net -net Net1 [get_bd_ports aux_rx_io_p_0] [get_bd_pins DP_RX_hier_0/aux_rx_io_p_0]
  connect_bd_net -net Net2 [get_bd_ports aux_rx_io_n_1] [get_bd_pins DP_RX_hier_1/aux_rx_io_n_0]
  connect_bd_net -net Net3 [get_bd_ports aux_rx_io_p_1] [get_bd_pins DP_RX_hier_1/aux_rx_io_p_0]
  connect_bd_net -net VCC_dout [get_bd_pins VCC/dout] [get_bd_pins ethernet_subsystem/signal_detect] [get_bd_pins processor_subsystem/vcc] [get_bd_pins vid_phy_controller_0/vid_phy_rx_axi4s_aresetn] [get_bd_pins vid_phy_controller_1/vid_phy_rx_axi4s_aresetn]
  connect_bd_net -net axi_gpio_lvds_reset_gpio_io_o [get_bd_pins axi_gpio_lvds_reset/gpio_io_o] [get_bd_pins xlslice_0/Din] [get_bd_pins xlslice_1/Din]
  connect_bd_net -net axi_vdma_1_s2mm_introut [get_bd_pins axi_vdma_lwip/s2mm_introut] [get_bd_pins processor_subsystem/irq_in_4]
  connect_bd_net -net ethernet_subsystem_interrupt [get_bd_pins ethernet_subsystem/interrupt] [get_bd_pins processor_subsystem/irq_in_3]
  connect_bd_net -net ethernet_subsystem_mac_irq [get_bd_pins ethernet_subsystem/mac_irq] [get_bd_pins processor_subsystem/irq_in_2]
  connect_bd_net -net ethernet_subsystem_mm2s_introut [get_bd_pins ethernet_subsystem/mm2s_introut] [get_bd_pins processor_subsystem/irq_in_0]
  connect_bd_net -net ethernet_subsystem_s2mm_introut [get_bd_pins ethernet_subsystem/s2mm_introut] [get_bd_pins processor_subsystem/irq_in_1]
  connect_bd_net -net irq_in_11_1 [get_bd_pins DP_RX_hier_0/dprxss_iic_irq] [get_bd_pins processor_subsystem/irq_in_11]
  connect_bd_net -net irq_in_12_1 [get_bd_pins DP_RX_hier_1/dprxss_iic_irq] [get_bd_pins processor_subsystem/irq_in_12]
  connect_bd_net -net irq_in_5_1 [get_bd_pins processor_subsystem/irq_in_5] [get_bd_pins vid_phy_controller_0/irq]
  connect_bd_net -net irq_in_7_1 [get_bd_pins processor_subsystem/irq_in_7] [get_bd_pins vid_phy_controller_1/irq]
  connect_bd_net -net lnk_rx_lane_n_0_1 [get_bd_ports lnk_rx_lane_n_0] [get_bd_pins vid_phy_controller_0/phy_rxn_in]
  connect_bd_net -net lnk_rx_lane_n_1_1 [get_bd_ports lnk_rx_lane_n_1] [get_bd_pins vid_phy_controller_1/phy_rxn_in]
  connect_bd_net -net lnk_rx_lane_p_0_1 [get_bd_ports lnk_rx_lane_p_0] [get_bd_pins vid_phy_controller_0/phy_rxp_in]
  connect_bd_net -net lnk_rx_lane_p_1_1 [get_bd_ports lnk_rx_lane_p_1] [get_bd_pins vid_phy_controller_1/phy_rxp_in]
  connect_bd_net -net lvds_s1_intr [get_bd_pins lvds_s0/intr] [get_bd_pins processor_subsystem/irq_in_9]
  connect_bd_net -net lvds_s2_intr [get_bd_pins lvds_s1/intr] [get_bd_pins processor_subsystem/irq_in_10]
  connect_bd_net -net mgtrefclk0_pad_n_in_0_1 [get_bd_ports mgtrefclk0_pad_n_in_0] [get_bd_pins vid_phy_controller_0/mgtrefclk0_pad_n_in]
  connect_bd_net -net mgtrefclk0_pad_n_in_1_1 [get_bd_ports mgtrefclk0_pad_n_in_1] [get_bd_pins vid_phy_controller_1/mgtrefclk0_pad_n_in]
  connect_bd_net -net mgtrefclk0_pad_p_in_0_1 [get_bd_ports mgtrefclk0_pad_p_in_0] [get_bd_pins vid_phy_controller_0/mgtrefclk0_pad_p_in]
  connect_bd_net -net mgtrefclk0_pad_p_in_1_1 [get_bd_ports mgtrefclk0_pad_p_in_1] [get_bd_pins vid_phy_controller_1/mgtrefclk0_pad_p_in]
  connect_bd_net -net mgtrefclk1_pad_n_in_0_1 [get_bd_ports mgtrefclk1_pad_n_in_0] [get_bd_pins vid_phy_controller_0/mgtrefclk1_pad_n_in]
  connect_bd_net -net mgtrefclk1_pad_n_in_1_1 [get_bd_ports mgtrefclk1_pad_n_in_1] [get_bd_pins vid_phy_controller_1/mgtrefclk1_pad_n_in]
  connect_bd_net -net mgtrefclk1_pad_p_in_0_1 [get_bd_ports mgtrefclk1_pad_p_in_0] [get_bd_pins vid_phy_controller_0/mgtrefclk1_pad_p_in]
  connect_bd_net -net mgtrefclk1_pad_p_in_1_1 [get_bd_ports mgtrefclk1_pad_p_in_1] [get_bd_pins vid_phy_controller_1/mgtrefclk1_pad_p_in]
  connect_bd_net -net processor_subsystem_aresetn_200m [get_bd_pins axis_passthrough_mon_0/aresetn] [get_bd_pins axis_passthrough_mon_1/aresetn] [get_bd_pins axis_switch_0/aresetn] [get_bd_pins lvds_s0/aresetn_200m] [get_bd_pins lvds_s1/aresetn_200m] [get_bd_pins processor_subsystem/aresetn_200m]
  connect_bd_net -net processor_subsystem_c0_ddr4_ui_clk [get_bd_pins DP_RX_hier_0/clk_100m] [get_bd_pins DP_RX_hier_1/clk_100m] [get_bd_pins axi_gpio_lvds_reset/s_axi_aclk] [get_bd_pins axi_vdma_lwip/s_axi_lite_aclk] [get_bd_pins axis_passthrough_mon_0/s_axi_lite_aclk] [get_bd_pins axis_passthrough_mon_1/s_axi_lite_aclk] [get_bd_pins axis_switch_0/s_axi_ctrl_aclk] [get_bd_pins ethernet_subsystem/lite_aclk] [get_bd_pins lvds_s0/aclk_100m] [get_bd_pins lvds_s1/aclk_100m] [get_bd_pins processor_subsystem/clk_100m] [get_bd_pins vid_phy_controller_0/vid_phy_axi4lite_aclk] [get_bd_pins vid_phy_controller_0/vid_phy_sb_aclk] [get_bd_pins vid_phy_controller_1/vid_phy_axi4lite_aclk] [get_bd_pins vid_phy_controller_1/vid_phy_sb_aclk] [get_bd_pins xgpio_i2c_0/s_axi_aclk]
  connect_bd_net -net processor_subsystem_c0_init_calib_complete [get_bd_ports c0_init_calib_complete] [get_bd_pins processor_subsystem/c0_init_calib_complete]
  connect_bd_net -net processor_subsystem_clk_200m [get_bd_pins DP_RX_hier_0/clk_200m] [get_bd_pins DP_RX_hier_1/clk_200m] [get_bd_pins axi_vdma_lwip/m_axi_s2mm_aclk] [get_bd_pins axi_vdma_lwip/s_axis_s2mm_aclk] [get_bd_pins axis_passthrough_mon_0/aclk] [get_bd_pins axis_passthrough_mon_1/aclk] [get_bd_pins axis_switch_0/aclk] [get_bd_pins ethernet_subsystem/axis_clk] [get_bd_pins lvds_s0/aclk_200m] [get_bd_pins lvds_s1/aclk_200m] [get_bd_pins processor_subsystem/clk_200m]
  connect_bd_net -net processor_subsystem_clk_39m [get_bd_pins processor_subsystem/clk_39m] [get_bd_pins vid_phy_controller_0/drpclk] [get_bd_pins vid_phy_controller_1/drpclk]
  connect_bd_net -net processor_subsystem_clk_50m [get_bd_pins ethernet_subsystem/ref_clk] [get_bd_pins processor_subsystem/clk_50m]
  connect_bd_net -net processor_subsystem_peripheral_reset [get_bd_pins DP_RX_hier_0/areset_200m] [get_bd_pins DP_RX_hier_1/areset_200m] [get_bd_pins processor_subsystem/reset_200m]
  connect_bd_net -net processor_subsystem_reset_100m [get_bd_pins DP_RX_hier_0/reset_100m] [get_bd_pins DP_RX_hier_1/reset_100m] [get_bd_pins processor_subsystem/reset_100m]
  connect_bd_net -net rst_1 [get_bd_pins lvds_s0/rst] [get_bd_pins xlslice_0/Dout]
  connect_bd_net -net rst_mig_7series_0_100M_peripheral_aresetn [get_bd_pins DP_RX_hier_0/aresetn_100m] [get_bd_pins DP_RX_hier_1/aresetn_100m] [get_bd_pins axi_gpio_lvds_reset/s_axi_aresetn] [get_bd_pins axi_vdma_lwip/axi_resetn] [get_bd_pins axis_passthrough_mon_0/s_axi_lite_aresetn] [get_bd_pins axis_passthrough_mon_1/s_axi_lite_aresetn] [get_bd_pins axis_switch_0/s_axi_ctrl_aresetn] [get_bd_pins ethernet_subsystem/lite_resetn] [get_bd_pins lvds_s0/aresetn_100m] [get_bd_pins lvds_s1/aresetn_100m] [get_bd_pins processor_subsystem/aresetn_100m] [get_bd_pins vid_phy_controller_0/vid_phy_axi4lite_aresetn] [get_bd_pins vid_phy_controller_0/vid_phy_sb_aresetn] [get_bd_pins vid_phy_controller_1/vid_phy_axi4lite_aresetn] [get_bd_pins vid_phy_controller_1/vid_phy_sb_aresetn] [get_bd_pins xgpio_i2c_0/s_axi_aresetn]
  connect_bd_net -net rx_lnk_clk_1 [get_bd_pins DP_RX_hier_0/rx_lnk_clk] [get_bd_pins vid_phy_controller_0/rxoutclk] [get_bd_pins vid_phy_controller_0/vid_phy_rx_axi4s_aclk]
  connect_bd_net -net rx_lnk_clk_2 [get_bd_pins DP_RX_hier_1/rx_lnk_clk] [get_bd_pins vid_phy_controller_1/rxoutclk] [get_bd_pins vid_phy_controller_1/vid_phy_rx_axi4s_aclk]
  connect_bd_net -net xlslice_1_Dout [get_bd_pins lvds_s1/rst] [get_bd_pins xlslice_1/Dout]

  # Create address segments
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces axi_vdma_lwip/Data_S2MM] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_0/Data_SG] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_0/Data_MM2S] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces ethernet_subsystem/axi_dma_0/Data_S2MM] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s0/axi_vdma_0/Data_MM2S] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s0/axi_vdma_0/Data_S2MM] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s0/axi_vdma_1/Data_MM2S] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s0/axi_vdma_1/Data_S2MM] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s1/axi_vdma_0/Data_MM2S] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s1/axi_vdma_0/Data_S2MM] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s1/axi_vdma_1/Data_MM2S] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces lvds_s1/axi_vdma_1/Data_S2MM] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x44AF0000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/AXI_LITE_REG_0/S00_AXI/S00_AXI_reg] -force
  assign_bd_address -offset 0x41E00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs ethernet_subsystem/axi_dma_0/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x40C00000 -range 0x00040000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs ethernet_subsystem/axi_ethernet_0/s_axi/Reg0] -force
  assign_bd_address -offset 0x40010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x40020000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs xgpio_i2c_0/axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x40000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs axi_gpio_lvds_reset/S_AXI/Reg] -force
  assign_bd_address -offset 0x44B00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/axi_quad_spi_0/AXI_LITE/Reg] -force
  assign_bd_address -offset 0x41C00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/axi_timer_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x41C10000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/axi_timer_1/S_AXI/Reg] -force
  assign_bd_address -offset 0x41C20000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/axi_timer_2/S_AXI/Reg] -force
  assign_bd_address -offset 0x40600000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/axi_uartlite_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x44A70000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s0/axi_vdma_0/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x44AB0000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s1/axi_vdma_0/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x44A80000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s0/axi_vdma_1/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x44AC0000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s1/axi_vdma_1/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x44A30000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs axi_vdma_lwip/S_AXI_LITE/Reg] -force
  assign_bd_address -offset 0x44A40000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs axis_passthrough_mon_0/s_axi_lite/s_axi_lite_reg] -force
  assign_bd_address -offset 0x44A50000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs axis_passthrough_mon_1/s_axi_lite/s_axi_lite_reg] -force
  assign_bd_address -offset 0x44A90000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s0/axis_pixel_compare_0/S00_AXI/S00_AXI_reg] -force
  assign_bd_address -offset 0x44AD0000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s1/axis_pixel_compare_0/S00_AXI/S00_AXI_reg] -force
  assign_bd_address -offset 0x44A60000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs axis_switch_0/S_AXI_CTRL/Reg] -force
  assign_bd_address -offset 0x44AA0000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s0/axis_switch_0/S_AXI_CTRL/Reg] -force
  assign_bd_address -offset 0x44AE0000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs lvds_s1/axis_switch_0/S_AXI_CTRL/Reg] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x80000000 -range 0x80000000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Instruction] [get_bd_addr_segs processor_subsystem/ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] -force
  assign_bd_address -offset 0x00000000 -range 0x00080000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/microblaze_0_local_memory/dlmb_bram_if_cntlr/SLMB/Mem] -force
  assign_bd_address -offset 0x00000000 -range 0x00080000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Instruction] [get_bd_addr_segs processor_subsystem/microblaze_0_local_memory/ilmb_bram_if_cntlr/SLMB/Mem] -force
  assign_bd_address -offset 0x41200000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs processor_subsystem/microblaze_0_axi_intc/S_AXI/Reg] -force
  assign_bd_address -offset 0x44A00000 -range 0x00002000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs DP_RX_hier_0/v_dp_rxss1_0/s_axi/Reg] -force
  assign_bd_address -offset 0x44A02000 -range 0x00002000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs DP_RX_hier_1/v_dp_rxss1_0/s_axi/Reg] -force
  assign_bd_address -offset 0x44A10000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs DP_RX_hier_0/vid_edid_0/s_axi/Reg] -force
  assign_bd_address -offset 0x44A20000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs DP_RX_hier_1/vid_edid_0/s_axi/Reg] -force
  assign_bd_address -offset 0x44B10000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs vid_phy_controller_0/vid_phy_axi4lite/Reg] -force
  assign_bd_address -offset 0x44B20000 -range 0x00010000 -target_address_space [get_bd_addr_spaces processor_subsystem/microblaze_0/Data] [get_bd_addr_segs vid_phy_controller_1/vid_phy_axi4lite/Reg] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


