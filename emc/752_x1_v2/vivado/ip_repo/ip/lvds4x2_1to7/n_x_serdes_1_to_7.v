`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/25/2022 05:58:21 PM
// Design Name: 
// Module Name: n_x_serdes_1_to_7
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


module n_x_serdes_1_to_7 #
(
    parameter integer LINES         = 4,            // Number of data lines
    parameter integer CHANNELS      = 2,            // Number of data channels
    parameter real 	  CLKIN_PERIOD  = 6.000,        // clock period (ns) of input clock on clkin_p
    parameter real    REF_FREQ      = 300.0,        // Reference clock frequency for idelay control
    parameter         DIFF_TERM     = "FALSE",      // Parameter to enable internal differential termination
    parameter         USE_PLL       = "FALSE",      // Parameter to enable PLL use rather than MMCM use
    parameter         SIM_DEVICE    = "ULTRASCALE_PLUS",    // ULTRASCALE_PLUS or ULTRASCALE
    parameter         CLK_PATTERN   = 7'b1100011, // Clock pattern for alignment
    parameter         DATA_FORMAT   = "PER_CLOCK"   // PER_CLOCK or PER_LINE data formatting
)
(
    input   [CHANNELS-1:0]          clkin_p,        // Clock input LVDS P-side
    input   [CHANNELS-1:0]          clkin_n,        // Clock input LVDS N-side
    input 	[CHANNELS*LINES-1:0]    datain_p,		// Input from LVDS clock data pins
    input 	[CHANNELS*LINES-1:0]    datain_n,		// Input from LVDS clock data pins
    input 			                reset,          // Reset line
    input                           idelay_rdy,     // Asyncrhonous IDELAYCRL ready
    
    output                          cmt_locked,           // PLL/MMCM locked output
    output                          px_clk,         // Pixel clock output
    output  [CHANNELS*LINES*7-1:0]  px_data,        // Pixel data bus output
    output  [CHANNELS-1:0]          px_ready        // Pixel data ready
);

wire                            rx_reset;
wire         [4:0]              rx_wr_addr;
wire         [8:0]              rx_cntval;
wire                            rx_dlyload;
wire                            rx_ready;
wire         [4:0]              px_rd_addr;
wire         [2:0]              px_rd_seq;
//wire    [CHANNELS*LINES*7-1:0]  rx_px_data;  
wire                            rx_clkdiv2;
wire                            rx_clkdiv8;

wire      [CHANNELS-1:0]          clkin_p_i;        // Clock input LVDS P-side
wire      [CHANNELS-1:0]          clkin_n_i;

rx_clkgen_1to7 # (
      .CLKIN_PERIOD (CLKIN_PERIOD),       // Clock period (ns) of input clock on clkin_p
      .REF_FREQ     (REF_FREQ),        // Reference frequency used by idelay controller
      .DIFF_TERM    (DIFF_TERM),       // Enable internal differential termination
      .USE_PLL      (USE_PLL),      // Enable PLL use rather than MMCM
      .CLK_PATTERN  (CLK_PATTERN)   // Clock bit pattern
   )
   rx_clkgen (
	  .clkin_p      (clkin_p_i[0]),
	  .clkin_n   	(clkin_n_i[0]),
      .reset        (reset),  // Reset line
      .cmt_locked   (cmt_locked),// PLL/MMCM locked
      .rx_clkdiv2   (rx_clkdiv2),
      .rx_clkdiv8   (rx_clkdiv8),
      .px_clk       (px_clk)      // Pixel clock output
   );

genvar i ;
genvar j ;

IBUFGDS_DIFF_OUT # (
  .DIFF_TERM        (DIFF_TERM)
)
iob_clk_in (
  .I                (clkin_p[0]),
  .IB               (clkin_n[0]),
  .O                (clkin_p_i[0]),
  .OB               (clkin_n_i[0])
);

rx_clkdelay_1to7 # (
      .CLKIN_PERIOD (CLKIN_PERIOD), // Clock period (ns) of input clock on clkin_p
      .REF_FREQ     (REF_FREQ),     // Reference clock frequency for idelay control
      .DIFF_TERM    (DIFF_TERM),    // Enable internal differential termination
      .CLK_PATTERN  (CLK_PATTERN),  // Expected clock pattern
      .SIM_DEVICE   (SIM_DEVICE),   // ULTRASCALE_PLUS or ULTRASCALE
      .USE_PLL      (USE_PLL)       // Enable PLL use rather than MMCM
   )
   rxc_delay
   (
      .clkin_p   (clkin_p_i[0]),      // Input from LVDS clock receiver pin
      .clkin_n   (clkin_n_i[0]),      // Input from LVDS clock receiver pin
      .reset     (reset),        // Reset 
      .idelay_rdy(idelay_rdy),   // Idelay control ready
      .cmt_locked(cmt_locked),   // PLL/MMCM locked
      //
      .rx_clkdiv2(rx_clkdiv2),   // RX clock div2 output
      .rx_clkdiv8(rx_clkdiv8),   // RX clock div8 output
      .rx_reset  (rx_reset),     // RX reset
      .rx_wr_addr(rx_wr_addr),   // RX write address
      .rx_cntval (rx_cntval),    // RX input delay count value
      .rx_dlyload(rx_dlyload),   // RX input delay load
      .rx_ready  (rx_ready),     // RX ready
      //
      .px_clk    (px_clk),       // Pixel clock output
      .px_rd_addr(px_rd_addr),   // Pixel read address
      .px_rd_seq (px_rd_seq),    // Pixel read sequence
      .px_ready  (px_ready)      // Pixel data ready data
   );

generate
if (CHANNELS > 0) 
begin
    for (i = 0 ; i <= (CHANNELS-1) ; i = i+1)
    begin : loop0

    
        rx_channel_1to7 # (
            .LINES        (LINES),            // Number of data lines
            .REF_FREQ     (REF_FREQ),     // Reference clock frequency for idelay control
            .DIFF_TERM    (DIFF_TERM),       // Enable internal differential termination
            .DATA_FORMAT  (DATA_FORMAT),   // PER_CLOCK or PER_LINE data formatting
            .SIM_DEVICE   (SIM_DEVICE),   // ULTRASCALE_PLUS or ULTRASCALE
            .RX_SWAP_MASK (16'b0)         // Allows P/N inputs to be invered to ease PCB routing
        )
        rxn (
	        .datain_p     (datain_p[LINES*i+:LINES]),
	        .datain_n     (datain_n[LINES*i+:LINES]),
            .reset        (reset),  // Reset line
            .rx_clkdiv2   (rx_clkdiv2),
            .rx_clkdiv8   (rx_clkdiv8),
            .rx_reset     (rx_reset),
            .rx_wr_addr   (rx_wr_addr),
            .rx_cntval    (rx_cntval),
            .rx_dlyload   (rx_dlyload),
            .rx_ready     (rx_ready),
            .px_rd_addr   (px_rd_addr),
            .px_rd_seq    (px_rd_seq),
            
            .px_clk       (px_clk),    // Pixel clock output
            .px_data      (px_data[i*28+:28])//,   // Pixel data
        );
    
    end
end
endgenerate


endmodule
