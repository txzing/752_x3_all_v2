//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Xilinx, Inc.
// This design is confidential and proprietary of Xilinx, All Rights Reserved.
//////////////////////////////////////////////////////////////////////////////
//   ____  ____
//  /   /\/   /
// /___/  \  /   Vendor: Xilinx
// \   \   \/    Version: 1.0
//  \   \        Filename: rx_clkgen_1to7.v
//  /   /        Date Last Modified:  04/03/2017
// /___/   /\    Date Created: 02/27/2017
// \   \  /  \
//  \___\/\___\
//
// Device    :  Ultrascale
//
// Purpose   :  Receiver clock generation, training and alignment
//
// Parameters:  CLKIN_PERIOD - Real - Default = 6.600
//                 - Period in nanoseconds of the input clock clkin_p
//                 - Range = 6.364 to 17.500
//              REF_FREQ - Real - Default = 300.00
//                 - Frequency of the reference clock provided to IDELAYCTRL
//                 - Range = 200.0 to 800.0
//              DIFF_TERM - String - Default = "FALSE"
//                 - Enable internal LVDS termination
//                 - Range = "FALSE" or "TRUE"
//              USE_PLL - String - Default = "FALSE"
//                 - Selects either PLL or MMCM for clocking
//                 - Range = "FALSE" or "TRUE"
//              CLK_PATTERN - Binary - Default = 7'b1100011
//                 - Sets the clock pattern that is expected to be received
//                   and is used for aligning the data lines
//                 - Range = 0 to 2^7-1 typically 7'b1100011 or 7'b1100001
//
// Reference:	XAPPxxx
//
// Revision History:
//    Rev 1.0 - Initial Release (knagara)
//    Rev 0.9 - Early Access Release (mcgett)
//////////////////////////////////////////////////////////////////////////////
//
//  Disclaimer:
//
// This disclaimer is not a license and does not grant any rights to the
// materials distributed herewith. Except as otherwise provided in a valid
// license issued to you by Xilinx, and to the maximum extent permitted by
// applicable law:
//
// (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND
// XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR
// STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
// NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx
// shall not be liable (whether in contract or tort, including negligence, or
// under any other theory of liability) for any loss or damage of any kind or
// nature related to, arising under or in connection with these materials,
// including for any direct, or any indirect, special, incidental, or
// consequential loss or damage (including loss of data, profits, goodwill, or
// any type of loss or damage suffered as a result of any action brought by a
// third party) even if such damage or loss was reasonably foreseeable or
// Xilinx had been advised of the possibility of the same.
//
// Critical Applications:
//
// Xilinx products are not designed or intended to be fail-safe, or for use in
// any application requiring fail-safe performance, such as life-support or
// safety devices or systems, class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any other applications
// that could lead to death, personal injury, or severe property or
// environmental damage (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and liability of any use of
// Xilinx products in Critical Applications, subject only to applicable laws
// and regulations governing limitations on product liability.
//
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
// AT ALL TIMES.
//
//////////////////////////////////////////////////////////////////////////////

`timescale 1ps/1ps

module rx_clkgen_1to7 # (
      parameter real    CLKIN_PERIOD = 6.600,      // Clock period (ns) of input clock on clkin_p
      parameter real    REF_FREQ     = 300.0,      // Reference clock frequency for idelay control
      parameter         DIFF_TERM    = "FALSE",    // Enable internal LVDS termination
      parameter         USE_PLL      = "FALSE",    // Selects either PLL or MMCM for clocking
      parameter         CLK_PATTERN  = 7'b1100011  // Clock pattern for alignment
   )
   (
      input             clkin_p,              // Clock input LVDS P-side
      input             clkin_n,              // Clock input LVDS N-side
      input             reset,                // Asynchronous interface reset
      //
      (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output            rx_clkdiv2,           // RX clock div2 output
      (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output            rx_clkdiv8,           // RX clock div8 output
      (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output            cmt_locked,           // PLL/MMCM locked output
      
      //
      (* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)output            px_clk               // Pixel clock output
   );

//
// Set VCO multiplier for PLL/MMCM 
//  2  - if clock_period is greater than 750 MHz/7 
//  1  - if clock period is <= 750 MHz/7  
//

localparam VCO_MULTIPLIER = (USE_PLL == "FALSE") ? ((CLKIN_PERIOD > 17.5) ? 4 : (CLKIN_PERIOD >8.75) ? 2 : 1) : ((CLKIN_PERIOD >11.666) ? 2 : 1);


wire       clkin_p_i;
wire       clkin_n_i;
wire       clkin_p_d;
wire       clkin_n_d;
wire       px_pllmmcm;
wire       rx_pllmmcm_div2;

wire       locked_and_idlyrdy;
wire       px_reset;

assign clkin_p_i = clkin_p;
assign clkin_n_i = clkin_n;

//
// Instantitate a PLL or a MMCM
//
generate
if (USE_PLL == "FALSE") begin    // use an MMCM
   MMCME3_BASE # (
         .CLKIN1_PERIOD      (CLKIN_PERIOD),
         .BANDWIDTH          ("OPTIMIZED"),
         .CLKFBOUT_MULT_F    (7*VCO_MULTIPLIER),
         .CLKFBOUT_PHASE     (0.0),
         .CLKOUT0_DIVIDE_F   (2*VCO_MULTIPLIER),
         .CLKOUT0_DUTY_CYCLE (0.5),
         .CLKOUT0_PHASE      (0.0),
         .DIVCLK_DIVIDE      (1),
         .REF_JITTER1        (0.100)
      )
      rx_mmcm_adv_inst (
         .CLKFBOUT       (px_pllmmcm),
         .CLKFBOUTB      (),
         .CLKOUT0        (rx_pllmmcm_div2),
         .CLKOUT0B       (),
         .CLKOUT1        (),
         .CLKOUT1B       (),
         .CLKOUT2        (),
         .CLKOUT2B       (),
         .CLKOUT3        (),
         .CLKOUT3B       (),
         .CLKOUT4        (),
         .CLKOUT5        (),
         .CLKOUT6        (),
         .LOCKED         (cmt_locked),
         .CLKFBIN        (px_clk),
         .CLKIN1         (clkin_p_i),
         .PWRDWN         (1'b0),
         .RST            (reset)
     );
   end
   else begin           // Use a PLL
   PLLE3_BASE # (
         .CLKIN_PERIOD       ((CLKIN_PERIOD>12.5)?12.5:CLKIN_PERIOD),
         .CLKFBOUT_MULT      (7*VCO_MULTIPLIER),
         .CLKFBOUT_PHASE     (0.0),
         .CLKOUT0_DIVIDE     (2*VCO_MULTIPLIER),
         .CLKOUT0_DUTY_CYCLE (0.5),
         .REF_JITTER         (0.100),
         .DIVCLK_DIVIDE      (1)
      )
      rx_plle2_adv_inst (
          .CLKFBOUT       (px_pllmmcm),
          .CLKOUT0        (rx_pllmmcm_div2),
          .CLKOUT0B       (),
          .CLKOUT1        (),
          .CLKOUT1B       (),
          .CLKOUTPHY      (),
          .LOCKED         (cmt_locked),
          .CLKFBIN        (px_clk),
          .CLKIN          (clkin_p_i),
          .CLKOUTPHYEN    (1'b0),
          .PWRDWN         (1'b0),
          .RST            (reset)
      );
   end
endgenerate

//
// Global Clock Buffers
//
BUFG  bg_px     (.I(px_pllmmcm),      .O(px_clk)) ;
BUFG  bg_rxdiv2 (.I(rx_pllmmcm_div2), .O(rx_clkdiv2)) ;
BUFGCE_DIV  # (
       .BUFGCE_DIVIDE(4)
     )
     bg_rxdiv8 (
       .I(rx_pllmmcm_div2),
       .CLR(!cmt_locked),
       .CE(1'b1),
       .O(rx_clkdiv8)
      );

endmodule
