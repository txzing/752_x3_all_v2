`timescale 1ps/1ps

module  rx_mmcm_adv #(
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

assign status[6] = 1'b1 ;

MMCME2_ADV #(
      	.BANDWIDTH		("OPTIMIZED"),  		
      	.CLKFBOUT_MULT_F	(7*MMCM_MODE),
      	.CLKFBOUT_PHASE		(0.0),
      	.CLKIN1_PERIOD		(CLKIN_PERIOD),
      	.CLKIN2_PERIOD		(CLKIN_PERIOD),
      	.CLKOUT0_DIVIDE_F	(1*MMCM_MODE),
      	.CLKOUT0_DUTY_CYCLE	(0.5),
      	.CLKOUT0_PHASE		(0.0),
	.CLKOUT0_USE_FINE_PS	("FALSE"),
      	.CLKOUT1_DIVIDE		(6*MMCM_MODE),
      	.CLKOUT1_DUTY_CYCLE	(0.5),
      	.CLKOUT1_PHASE		(22.5),
	.CLKOUT1_USE_FINE_PS	("FALSE"),
      	.CLKOUT2_DIVIDE		(7*MMCM_MODE),
      	.CLKOUT2_DUTY_CYCLE	(0.5),
      	.CLKOUT2_PHASE		(0.0),
	.CLKOUT2_USE_FINE_PS	("FALSE"),
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
rx_mmcm_adv_inst (
      	.CLKFBOUT		(rx_mmcmout_x1),
      	.CLKFBOUTB		(),
      	.CLKFBSTOPPED	(),
      	.CLKINSTOPPED	(),
      	.CLKOUT0		(rx_mmcmout_xs),
      	.CLKOUT0B		(),
      	.CLKOUT1		(),
      	.CLKOUT1B		(),
      	.CLKOUT2		(),
      	.CLKOUT2B		(),
      	.CLKOUT3		(),
      	.CLKOUT3B		(),
      	.CLKOUT4		(),
      	.CLKOUT5		(),
      	.CLKOUT6		(),
      	.DO			    (),
      	.DRDY			(),
      	.PSDONE			(),
      	.PSCLK			(1'b0),
      	.PSEN			(1'b0),
      	.PSINCDEC		(1'b0),
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
      BUFG	bufg_mmcm_x1 (.I(rx_mmcmout_x1), .O(rxclk_div)) ;
      assign status[1:0] = 2'b00 ;
   end
   else if (PIXEL_CLOCK == "BUF_R") begin
      BUFR #(.BUFR_DIVIDE("1"),.SIM_DEVICE("7SERIES"))bufr_mmcm_x1 (.I(rx_mmcmout_x1),.CE(1'b1),.O(rxclk_div),.CLR(1'b0)) ;
      assign status[1:0] = 2'b01 ;
   end
   else begin
      BUFH	bufh_mmcm_x1 (.I(rx_mmcmout_x1), .O(rxclk_div)) ;
      assign status[1:0] = 2'b10 ;
   end

   if (SAMPL_CLOCK == "BUF_G") begin						// Sample clock selection
      BUFG	bufg_mmcm_xn (.I(rx_mmcmout_xs), .O(rxclk)) ;
      assign status[5:4] = 2'b00 ;
   end
   else if (SAMPL_CLOCK == "BUFIO") begin
      BUFIO  	bufio_mmcm_xn (.I (rx_mmcmout_xs), .O(rxclk)) ;
      assign status[5:4] = 2'b11 ;
   end
   else begin
      BUFH	bufh_mmcm_xn (.I(rx_mmcmout_xs), .O(rxclk)) ;
      assign status[5:4] = 2'b10 ;
   end

endmodule
