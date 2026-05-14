`timescale 1ns / 1ns
`define UD #1

module vid_4to1_multiplexer
(
    input   [1:0]        port_sel,
    
    input                pclk0   ,
    input 	        	 VS0     , 
    input                HS0     ,
    input                DE0     ,	 
    input	[23:0]       dout0_0 ,
    input	[23:0]	     dout0_1 ,

    input                pclk1   , 
    input 	        	 VS1     , 
    input                HS1     ,
    input                DE1     ,	 
    input	[23:0]       dout1_0 ,
    input	[23:0]	     dout1_1 ,
 
    input                pclk2   , 
    input 	        	 VS2     , 
    input                HS2     ,
    input                DE2     ,	 
    input	[23:0]       dout2_0 ,
    input	[23:0]	     dout2_1 ,
  
    input                pclk3   , 
    input 	        	 VS3     , 
    input                HS3     ,
    input                DE3     ,	 
    input	[23:0]       dout3_0 ,
    input	[23:0]	     dout3_1 ,
 
    output              pclk     ,	 
    output              VS       ,
    output              HS       ,
    output              DE       ,
    output  [23:0]      dout0    ,
    output  [23:0]      dout1 
);

wire    pclk_i;
assign  pclk_i = port_sel[0] ?  (port_sel[1] ? pclk3   :pclk1  ) : (port_sel[1] ? pclk2   : pclk0  );
assign  VS     = port_sel[0] ?  (port_sel[1] ? VS3     :VS1    ) : (port_sel[1] ? VS2     : VS0    );
assign  HS     = port_sel[0] ?  (port_sel[1] ? HS3     :HS1    ) : (port_sel[1] ? HS2     : HS0    );
assign  DE     = port_sel[0] ?  (port_sel[1] ? DE3     :DE1    ) : (port_sel[1] ? DE2     : DE0    );
assign  dout0  = port_sel[0] ?  (port_sel[1] ? dout3_0 :dout1_0) : (port_sel[1] ? dout2_0 : dout0_0);
assign  dout1  = port_sel[0] ?  (port_sel[1] ? dout3_1 :dout1_1) : (port_sel[1] ? dout2_1 : dout0_1);

BUFG bg_pclk
(
    .I          (pclk_i), 
    .O          (pclk)
) ;

// generate
// begin 
//     case (port_sel)
//         2'b00:
//         begin
//             assign pclk  = pclk0   ;
//             assign VS    = VS0     ;
//             assign HS    = HS0     ;
//             assign DE    = DE0     ;
//             assign dout0 = dout0_0 ;
//             assign dout1 = dout0_1 ;
//         end
//         2'b01:
//         begin
//             assign pclk  = pclk1   ;
//             assign VS    = VS1     ;
//             assign HS    = HS1     ;
//             assign DE    = DE1     ;
//             assign dout0 = dout1_0 ;
//             assign dout1 = dout1_1 ;
//         end
//         2'b10:
//         begin
//             assign pclk  = pclk2   ;
//             assign VS    = VS2     ;
//             assign HS    = HS2     ;
//             assign DE    = DE2     ;
//             assign dout0 = dout2_0 ;
//             assign dout1 = dout2_1 ;
//         end
//         2'b11:
//         begin
//             assign pclk  = pclk3   ;
//             assign VS    = VS3     ;
//             assign HS    = HS3     ;
//             assign DE    = DE3     ;
//             assign dout0 = dout3_0 ;
//             assign dout1 = dout3_1 ;
//         end
//         default:begin       //若N数值不匹配，则默认输出第一个通道
//             assign pclk  = pclk0   ;
//             assign VS    = VS0     ;
//             assign HS    = HS0     ;
//             assign DE    = DE0     ;
//             assign dout0 = dout0_0 ;
//             assign dout1 = dout0_1 ;
//         end
//     endcase
// end
// endgenerate

// always @(*)
// begin 
//     case (port_sel)
//         2'b00:
//         begin
//             pclk  = pclk0   ;
//             VS    = VS0     ;
//             HS    = HS0     ;
//             DE    = DE0     ;
//             dout0 = dout0_0 ;
//             dout1 = dout0_1 ;
//         end
//         2'b01:
//         begin
//             pclk  = pclk1   ;
//             VS    = VS1     ;
//             HS    = HS1     ;
//             DE    = DE1     ;
//             dout0 = dout1_0 ;
//             dout1 = dout1_1 ;
//         end
//         2'b10:
//         begin
//             pclk  = pclk2   ;
//             VS    = VS2     ;
//             HS    = HS2     ;
//             DE    = DE2     ;
//             dout0 = dout2_0 ;
//             dout1 = dout2_1 ;
//         end
//         2'b11:
//         begin
//             pclk  = pclk3   ;
//             VS    = VS3     ;
//             HS    = HS3     ;
//             DE    = DE3     ;
//             dout0 = dout3_0 ;
//             dout1 = dout3_1 ;
//         end
//         default:begin       //若N数值不匹配，则默认输出第一个通道
//             pclk  = pclk0   ;
//             VS    = VS0     ;
//             HS    = HS0     ;
//             DE    = DE0     ;
//             dout0 = dout0_0 ;
//             dout1 = dout0_1 ;
//         end
//     endcase
// end

endmodule
