`timescale 1ps/1ps

module icontrol (
    input           RST,
    input           REFCLK,
    output          RDY
);

IDELAYCTRL icontrol_inst (              			// Instantiate input delay control block
	.REFCLK			(REFCLK),
	.RST			(RST),
	.RDY			(RDY));

endmodule
