`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/26/2022 10:15:29 AM
// Design Name: 
// Module Name: top4x2_7to1_rx
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module lvds4x2_1to7 #
(
    parameter integer LINES         = 4,            // Number of data lines
    parameter integer CHANNELS      = 2,            // Number of data channels
    parameter real 	  CLKIN_PERIOD  = 6.000,        // clock period (ns) of input clock on clkin_p
    parameter real    REF_FREQ      = 300.0,        // Reference clock frequency for idelay control
    parameter         DIFF_TERM     = "FALSE",      // Parameter to enable internal differential termination
    parameter         USE_PLL       = "TRUE",      // Parameter to enable PLL use rather than MMCM use
    parameter         DATA_FORMAT   = "PER_CLOCK"   // PER_CLOCK or PER_LINE data formatting
)
(
   input            refclk,
   input            reset,                  // reset (active high)
   // Receivers
   input            clkin1_p,  clkin1_n,    // lvds channel 1 clock input
   input  [3:0]     datain1_p, datain1_n,   // lvds channel 1 data inputs
   input            clkin2_p,  clkin2_n,    // lvds channel 2 clock input
   input  [3:0]     datain2_p, datain2_n,   // lvds channel 2 data inputs
   
   output           px_clk,                 // Pixel clock output
   //output [55:0]    px_data,                // Pixel data bus output
   output             vs0    ,
   output             vs1    ,
   output             hs0    ,
   output             hs1    ,
   output             de0    ,
   output             de1    ,
   output   [23:0]    dout0  ,
   output   [23:0]    dout1  ,
   output [1:0]     px_ready                // Pixel data ready
);

wire            refclk_i;         
wire            clk300_g;
wire            idly_reset_int;
wire            cmt_locked;
wire            rx_idelay_rdy;
wire  [55:0]    px_data;
wire            px_clk_i;

IBUF ib_refclk(
	.I    			(refclk),
	.O         		(refclk_i));

BUFG bg_ref(
	.I 			(refclk_i), 
	.O 			(clk300_g)) ;


assign idly_reset_int = reset | !cmt_locked;

//
//  Idelay control block
//
IDELAYCTRL #( // Instantiate input delay control block
      .SIM_DEVICE ("ULTRASCALE"))
   icontrol (
      .REFCLK (clk300_g),
      .RST    (idly_reset_int),
      .RDY    (rx_idelay_rdy)
   );

n_x_serdes_1_to_7 #
(
    .LINES              (LINES          ), // Number of data lines
    .CHANNELS           (CHANNELS       ), // Number of data channels
    .CLKIN_PERIOD       (CLKIN_PERIOD   ), // clock period (ns) of input clock on clkin_p
    .REF_FREQ           (REF_FREQ       ), // Reference clock frequency for idelay control
    .DIFF_TERM          (DIFF_TERM      ), // Parameter to enable internal differential termination
    .USE_PLL            (USE_PLL        ), // Parameter to enable PLL use rather than MMCM use
    .CLK_PATTERN        (7'b1100011     ),
    .DATA_FORMAT        (DATA_FORMAT    )  // PER_CLOCK or PER_LINE data formatting
)
rx_inst
(
    .clkin_p            ({clkin2_p, clkin1_p}       ), // Clock input LVDS P-side
    .clkin_n            ({clkin2_n, clkin1_n}       ), // Clock input LVDS N-side
    .datain_p		    ({datain2_p, datain1_p}     ), // Input from LVDS clock data pins
    .datain_n		    ({datain2_n, datain1_n}     ), // Input from LVDS clock data pins
    .reset              (reset                      ), // Reset line
    .idelay_rdy         (rx_idelay_rdy              ), // Asyncrhonous IDELAYCRL ready
    .cmt_locked         (cmt_locked                 ), // PLL/MMCM locked output
    .px_clk             (px_clk_i                   ), // Pixel clock output
    .px_data            (px_data                    ), // Pixel data bus output
    .px_ready           (px_ready                   )  // Pixel data ready
);

assign px_clk = ~px_clk_i;

lvds4x2_mapping #
(
    .DATA_FORMAT        (DATA_FORMAT    )  // PER_CLOCK or PER_LINE data formatting
)
mapping_inst
(
    .din                (px_data),
    .VS0                (vs0),
    .VS1                (vs1),
    .HS0                (hs0),
    .HS1                (hs1),
    .DE0                (de0),
    .DE1                (de1),
    .dout0              (dout0),
    .dout1              (dout1) 
);


endmodule
