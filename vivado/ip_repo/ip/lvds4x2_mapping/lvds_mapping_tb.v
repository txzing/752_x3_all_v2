`timescale 1ns/1ns
                
module lvds4x2_mapping_tb();
//激励信号定义 
reg	[55:0]	din	 ;
//输出信号定义	 
wire        VS0  ;
wire        VS1  ;
wire        HS0  ;
wire        HS1  ;
wire        DE0  ;
wire        DE1  ;
wire [23:0] dout0;
wire [23:0] dout1;
                                          
//时钟周期参数定义					        
    parameter		CLOCK_CYCLE = 20;    
    parameter		DATA_FORMAT = "PER_CLOCK";                                         
lvds4x2_mapping
#(
.DATA_FORMAT(DATA_FORMAT) // PER_CLOCK or PER_LINE data formatting  
)
u_lvds_mapping( 
/*input	[55:0]     */.din    (din),
/*output           */.VS0    (VS0),
/*output           */.VS1    (VS1),
/*output           */.HS0    (HS0),
/*output           */.HS1    (HS1),
/*output           */.DE0    (DE0),
/*output           */.DE1    (DE1),
/*output   [23:0]  */.dout0  (dout0),
/*output   [23:0]  */.dout1  (dout1) 
);		
                                                   
//产生激励							       	
always #CLOCK_CYCLE din = $random;

endmodule 									       	
