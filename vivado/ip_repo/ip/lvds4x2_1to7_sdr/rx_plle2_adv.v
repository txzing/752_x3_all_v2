`timescale 1ps/1ps

module  rx_plle2_adv #(
    parameter integer      	MMCM_MODE = 1 ,
    parameter real 	  	    CLKIN_PERIOD = 6.000,
    parameter         	    PIXEL_CLOCK = "BUF_R",
    parameter         	    SAMPL_CLOCK = "BUFIO"
)
(
    input                   RST,
    input                   CLKIN1,
    output 	[6:0]		    status,
//    output                  CLKFBOUT,
//    output                  CLKOUT0,
    output                  LOCKED,
    output                  rxclk_div,
    output                  rxclk
);

wire			rx_mmcmout_x1 ;
wire			rx_mmcmout_xs ;

assign status[6] = 1'b0 ;

PLLE2_ADV #(
      	.BANDWIDTH		("OPTIMIZED"),
      	.CLKFBOUT_MULT		(7*MMCM_MODE),
      	.CLKFBOUT_PHASE		(0.0),
      	.CLKIN1_PERIOD		(CLKIN_PERIOD),
      	.CLKIN2_PERIOD		(CLKIN_PERIOD),
      	.CLKOUT0_DIVIDE		(1*MMCM_MODE),
      	.CLKOUT0_DUTY_CYCLE	(0.5),
      	.CLKOUT0_PHASE		(0.0),
      	.CLKOUT1_DIVIDE		(4*MMCM_MODE),
      	.CLKOUT1_DUTY_CYCLE	(0.5),
      	.CLKOUT1_PHASE		(22.5),
      	.CLKOUT2_DIVIDE		(7*MMCM_MODE),
      	.CLKOUT2_DUTY_CYCLE	(0.5),
      	.CLKOUT2_PHASE		(0.0),
      	.CLKOUT3_DIVIDE		(7),
      	.CLKOUT3_DUTY_CYCLE	(0.5),
      	.CLKOUT3_PHASE		(0.0),
      	.CLKOUT4_DIVIDE		(7),
      	.CLKOUT4_DUTY_CYCLE	(0.5),
      	.CLKOUT4_PHASE		(0.0),
      	.CLKOUT5_DIVIDE		(7),
      	.CLKOUT5_DUTY_CYCLE	(0.5),
      	.CLKOUT5_PHASE		(0.0),
      	.COMPENSATION		("ZHOLD"),
      	.DIVCLK_DIVIDE		(1),
      	.REF_JITTER1		(0.100))
rx_plle2_adv_inst (
      	.CLKFBOUT		(rx_mmcmout_x1),
      	.CLKOUT0		(rx_mmcmout_xs),
      	.CLKOUT1		(),
      	.CLKOUT2		(),
      	.CLKOUT3		(),
      	.CLKOUT4		(),
      	.CLKOUT5		(),
      	.DO			    (),
      	.DRDY			(),
      	.PWRDWN			(1'b0),
      	.LOCKED			(LOCKED),
      	.CLKFBIN		(rxclk_div),
      	.CLKIN1			(CLKIN1),
      	.CLKIN2			(1'b0),
      	.CLKINSEL		(1'b1),
      	.DADDR			(7'h00),
      	.DCLK			(1'b0),
      	.DEN			(1'b0),
      	.DI			    (16'h0000),
      	.DWE			(1'b0),
      	.RST			(RST)) ;

   assign status[3:2] = 2'b00 ;

   if (PIXEL_CLOCK == "BUF_G") begin 						// Final clock selection
      BUFG	bufg_pll_x1 (.I(rx_mmcmout_x1), .O(rxclk_div)) ;
      assign status[1:0] = 2'b00 ;
   end
   else if (PIXEL_CLOCK == "BUF_R") begin
      BUFR #(.BUFR_DIVIDE("1"),.SIM_DEVICE("7SERIES"))bufr_pll_x1 (.I(rx_mmcmout_x1),.CE(1'b1),.O(rxclk_div),.CLR(1'b0)) ;
      assign status[1:0] = 2'b01 ;
   end
   else begin
      BUFH	bufh_pll_x1 (.I(rx_mmcmout_x1), .O(rxclk_div)) ;
      assign status[1:0] = 2'b10 ;
   end

   if (SAMPL_CLOCK == "BUF_G") begin						// Sample clock selection
      BUFG	bufg_pll_xn (.I(rx_mmcmout_xs), .O(rxclk)) ;
      assign status[5:4] = 2'b00 ;
   end
   else if (SAMPL_CLOCK == "BUFIO") begin
      BUFIO  	bufio_pll_xn (.I (rx_mmcmout_xs), .O(rxclk)) ;
      assign status[5:4] = 2'b11 ;
   end
   else begin
      BUFH	bufh_pll_xn (.I(rx_mmcmout_xs), .O(rxclk)) ;
      assign status[5:4] = 2'b10 ;
   end

endmodule
