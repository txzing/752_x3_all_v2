
`timescale 1 ns / 1 ps
/*
思路1:1路进fifo，一路根据握手信号进行读出对齐
思路2:2路数据流都进fifo,当满时阻塞上级，当2个都满时开始读拼接
(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)
*/
module axis_combiner
#
(
	parameter CHANNEL_IN_W  = 24,
	parameter CHANNEL_OUT_W = 48,
	parameter FIFO_D = 4096
)
(
	input 								clk				,
	input 								rst_n			,

	//AXI_ST Slave 接口0，来自摄像头 
	input [CHANNEL_IN_W -1 : 0] 		s0_axis_tdata   ,
	input        						s0_axis_tvalid  ,
	output       						s0_axis_tready  ,
	input        						s0_axis_tuser   ,
	input        						s0_axis_tlast   ,

	//AXI_ST Slave 接口1，来自VDMA 
	input [CHANNEL_IN_W -1 : 0] 		s1_axis_tdata   ,
	input        						s1_axis_tvalid  ,
	output       						s1_axis_tready  ,
	input        						s1_axis_tuser   ,
	input        						s1_axis_tlast   ,

	//AXI_ST Master 接口
	output  reg  [CHANNEL_OUT_W -1 : 0] 	m_axis_tdata	,
	output  reg        					m_axis_tvalid	,
	input             					    m_axis_tready	,
	output  reg        					m_axis_tuser	,
	output  reg        					m_axis_tlast 
);



//函数声明
function integer clog2b(input integer data);begin 
	data = data - 1;
	for(clog2b = 0;data > 0;clog2b = clog2b + 1)begin
		data = data >> 1;
	end 
end 
endfunction 

//参数定义
localparam	FIFO_W      = CHANNEL_IN_W + 2  ,
			FIFO_NUM_W  = clog2b(FIFO_D) + 1 ; 



////*****************************************************
//
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg   [FIFO_W - 1 : 0]		    fifo_0_data;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg           					fifo_0_wr	 ;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg 							fifo_0_wr_en;	
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire          					fifo_0_alfull;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg          					fifo_0_rd   ;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg          	 				fifo_0_rd_en;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire [FIFO_W - 1 : 0]   	    fifo_0_q    ;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire          					fifo_0_empty;
//
//
////fifo_0例化
//xilinx_sync_fifo#(.FIFO_DEPTH(FIFO_D),.FIFO_WIDTH(FIFO_W),.FIFO_MODE("FWFT")) u_input_fifo_0(
//    .clk         (clk           ),
//    .rst_n       (rst_n         ),
//    
//    //write
//    .wr_en       (fifo_0_wr_en && fifo_0_wr),
//    .wr_data     (fifo_0_data     ),
//    .wr_count    (              ),
//    .al_full     (fifo_0_alfull   ),
//    .full        (              ),
//    .wr_ack      (              ),
//
//    //read
//    .rd_en       (fifo_0_rd && fifo_0_rd_en),
//    .rd_data     (fifo_0_q     	),
//    .rd_data_vld (              ),
//    .rd_count    (              ),
//    .empty       (fifo_0_empty    ),
//    .al_empty    (              )    
//);
//
//
//
////*****************************************************
//
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg   [FIFO_W - 1 : 0]		    fifo_1_data;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg           					fifo_1_wr	 ;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg 							fifo_1_wr_en;	
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire          					fifo_1_alfull;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg          					fifo_1_rd   ;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg          	 				fifo_1_rd_en;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire [FIFO_W - 1 : 0]   	    fifo_1_q    ;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire          					fifo_1_empty;
//
//
////fifo_1例化
//xilinx_sync_fifo#(.FIFO_DEPTH(FIFO_D),.FIFO_WIDTH(FIFO_W),.FIFO_MODE("FWFT")) u_input_fifo_1(
//    .clk         (clk           ),
//    .rst_n       (rst_n         ),
//    
//    //write
//    .wr_en       (fifo_1_wr_en && fifo_1_wr),
//    .wr_data     (fifo_1_data     ),
//    .wr_count    (              ),
//    .al_full     (fifo_1_alfull   ),
//    .full        (              ),
//    .wr_ack      (              ),
//
//    //read
//    .rd_en       (fifo_1_rd && fifo_1_rd_en),
//    .rd_data     (fifo_1_q     	),
//    .rd_data_vld (              ),
//    .rd_count    (              ),
//    .empty       (fifo_1_empty    ),
//    .al_empty    (              )    
//);
//
//
////fifo满后阻塞上级模块写入
//assign s0_axis_tready = ~fifo_0_alfull;
//
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		fifo_0_wr_en	<= 1'b0;
//        fifo_0_wr 	<= 1'b0;
//		fifo_0_data	<= 0;
//	end
//    else begin
//		//检测到SOF后才打开写fifo_0使能，保证fifo_0中写入的第一个数据为SOF
//		if(s0_axis_tvalid && s0_axis_tready & s0_axis_tuser) 
//			fifo_0_wr_en	<= 1'b1;
//	
//		if(s0_axis_tvalid && s0_axis_tready) begin
//			fifo_0_wr 	<= 1'b1;
//			fifo_0_data	<= {s0_axis_tuser,s0_axis_tlast,s0_axis_tdata};; 
//		end  
//		else  begin
//			fifo_0_wr 	<= 1'b0;
//			fifo_0_data	<= fifo_0_data;
//		end
//	end
//end
//
////fifo满后阻塞上级模块写入
//assign s1_axis_tready = ~fifo_1_alfull;
//
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		fifo_1_wr_en	<= 1'b0;
//        fifo_1_wr 	<= 1'b0;
//		fifo_1_data	<= 0;
//	end
//    else begin
//		//检测到SOF后才打开写fifo_1使能，保证fifo_1中写入的第一个数据为SOF
//		if(s1_axis_tvalid && s1_axis_tready & s1_axis_tuser) 
//			fifo_1_wr_en	<= 1'b1;
//	
//		if(s1_axis_tvalid && s1_axis_tready) begin
//			fifo_1_wr 	<= 1'b1;
//			fifo_1_data	<= {s1_axis_tuser,s1_axis_tlast,s1_axis_tdata};; 
//		end  
//		else  begin
//			fifo_1_wr 	<= 1'b0;
//			fifo_1_data	<= fifo_1_data;
//		end
//	end
//end
//
////*****************************************************
////当2个fifo都满时开始读数据拼接，进行同步
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		fifo_0_rd_en	<= 1'b0;
//        fifo_1_rd_en 	<= 1'b0;
//	end
//    else begin
//		if(fifo_0_alfull && fifo_1_alfull)
//		begin
//			fifo_0_rd_en	<= 1'b1;
//			fifo_1_rd_en 	<= 1'b1;
//		end
//		else begin
//			fifo_0_rd_en	<= fifo_0_rd_en;
//			fifo_1_rd_en 	<= fifo_1_rd_en;
//		end
//	end
//end
//
//
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		fifo_0_rd	<= 1'b0;
//	end
//    else begin
//		if(~fifo_0_empty && ~fifo_1_empty && fifo_0_rd_en && video_in_rdy)
//		begin
//			fifo_0_rd	<= 1'b1;
//		end
//		else begin
//			fifo_0_rd	<= 1'b0;
//		end
//	end
//end
//
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		fifo_1_rd	<= 1'b0;
//	end
//    else begin
//		if(~fifo_0_empty && ~fifo_1_empty && fifo_1_rd_en && video_in_rdy)
//		begin
//			fifo_1_rd	<= 1'b1;
//		end
//		else begin
//			fifo_1_rd	<= 1'b0;
//		end
//	end
//end
//
//
//
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg  [CHANNEL_OUT_W - 1 : 0] 	com_data;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg								pixel_in_vld;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg								pixel_in_sof;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)reg								pixel_in_eol;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire 							video_in_rdy;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire [23:0] s1;
//(* DONT_TOUCH = "yes", s="true",keep="true" *)(* mark_debug="true" *)wire [23:0] s2;
////
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		pixel_in_vld	<= 1'b0;
//	end
//    else if(~fifo_0_empty && ~fifo_1_empty && fifo_1_rd_en && video_in_rdy) begin
//		pixel_in_vld 	<= 1;
//	end
//	else begin
//		pixel_in_vld 	<= 0;
//	end
//end
//
//// pixel_in_vld与pixel_in_sof、pixel_in_eol是否相差一个时钟周期?
//
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		pixel_in_sof	<= 1'b0;
//	end
//    else if(~fifo_0_empty && ~fifo_1_empty && fifo_1_rd_en && video_in_rdy) begin
//		pixel_in_sof 	<= fifo_0_q[FIFO_W - 1] & fifo_1_q[FIFO_W - 1];
//	end
//	else begin
//		pixel_in_sof 	<= 0;
//	end
//end
//
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//		pixel_in_eol	<= 1'b0;
//	end
//    else if(~fifo_0_empty && ~fifo_1_empty && fifo_1_rd_en && video_in_rdy) begin
//		pixel_in_eol 	<= fifo_0_q[FIFO_W - 2] & fifo_1_q[FIFO_W - 2];
//	end
//	else begin
//		pixel_in_eol 	<= 0;
//	end
//end
//
////*****************************************************消耗一个时钟周期
//always @ (posedge clk or negedge rst_n) begin
//    if(!rst_n) begin
//        com_data <= 'd0;
//    end
//    else if(~fifo_0_empty && ~fifo_1_empty && fifo_1_rd_en && video_in_rdy) begin
//        com_data <= {s1,s2};
//    end
//end
//
//assign s1 = fifo_0_q[CHANNEL_IN_W - 1 : 0];
//assign s2 = fifo_1_q[CHANNEL_IN_W - 1 : 0];
//
//
//
//stream_output 
//#(
//	.CHANNEL_OUT_W(CHANNEL_OUT_W),
//	.FIFO_D(FIFO_D)
//) 
//u_stream_output(  //视频流输出
///*input                               */.clk             (clk           ),
///*input                               */.rst_n           (rst_n         ),
//
////视频流输入
///*input                               */.pixel_in        (com_data      ),
///*input                               */.pixel_in_vld    (pixel_in_vld  ),
///*input                               */.pixel_in_sof    (pixel_in_sof  ),
///*input                               */.pixel_in_eol    (pixel_in_eol  ),
///*output                              */.pixel_in_rdy    (video_in_rdy  ),
//
////视频流输出
///*output                              */.pixel_out       (m_axis_tdata  ),
///*output                              */.pixel_out_vld   (m_axis_tvalid ),
///*output                              */.pixel_out_sof   (m_axis_tuser ),
///*output                              */.pixel_out_eol   (m_axis_tlast ),
///*input                               */.pixel_out_rdy   (m_axis_tready ),
////Debug
///*output                              */.over_flow       (over_flow     ),
///*output                              */.under_flow      (under_flow    ) 
//);


//*****************************************************
//将VDMA的图像缓存到FIFO中

reg   [FIFO_W - 1 : 0]		fifo_data;
reg           				fifo_wr	 ;
reg 						fifo_wr_en;	
wire          				fifo_alfull;
reg          				fifo_rd   ;
reg          	 			fifo_rd_en;
wire [FIFO_W - 1 : 0]   	fifo_q    ;
reg          				fifo_valid;
wire          				fifo_empty;


//FIFO例化
xilinx_sync_fifo #(.FIFO_DEPTH(FIFO_D),.FIFO_WIDTH(FIFO_W),.FIFO_MODE("FWFT")) u_input_fifo(
    .clk         (clk           ),
    .rst_n       (rst_n         ),
    
    //write
    .wr_en       (fifo_wr_en && fifo_wr),
    .wr_data     (fifo_data     ),
    .wr_count    (              ),
    .al_full     (fifo_alfull   ),
    .full        (              ),
    .wr_ack      (              ),

    //read
    .rd_en       (fifo_rd && fifo_rd_en),
    .rd_data     (fifo_q     	),
    .rd_data_vld (              ),
    .rd_count    (              ),
    .empty       (fifo_empty    ),
    .al_empty    (              )    
);



//FIFO不满时，持续从VDMA读取数据
assign s1_axis_tready = ~fifo_alfull;

always @ (posedge clk or negedge rst_n) begin
    if(!rst_n) begin
		fifo_wr_en	<= 1'b0;
        fifo_wr 	<= 1'b0;
		fifo_data	<= 0;
	end
    else begin
		//检测到SOF后才打开写FIFO使能，保证FIFO中写入的第一个数据为SOF
		if(s1_axis_tvalid & s1_axis_tready & s1_axis_tuser) 
			fifo_wr_en	<= 1'b1;
	
		if(s1_axis_tvalid & s1_axis_tready) begin
			fifo_wr 	<= 1'b1;
			fifo_data	<= {s1_axis_tuser,s1_axis_tlast,s1_axis_tdata};; 
		end  
		else  begin
			fifo_wr 	<= 1'b0;
			fifo_data	<= fifo_data;
		end
	end
end

//*****************************************************
//摄像头输入像素的同时，从FIFO中读出VDMA像素，以进行帧差运算

always @ (posedge clk or negedge rst_n) begin
    if(!rst_n) begin
		fifo_rd_en	<= 1'b0;
        fifo_rd 	<= 1'b0;
	end
    else begin
//等待FIFO中缓存一定量的数据之后，检测摄像头输入SOF后才打开读FIFO使能，使得与FIFO中读取的VMDA视频同步
//当fifo满时，s1的tready拉高，s1被堵塞，s0数据流正常传输
//s0_axis_tuser & fifo_alfull进行同步
		if(s0_axis_tvalid & s0_axis_tready & s0_axis_tuser & fifo_alfull) 
			fifo_rd_en	<= 1'b1;
		
		//此处FIFO读信号相对于摄像头输入视频延迟了一个时钟周期
		if(s0_axis_tvalid & s0_axis_tready) 
			fifo_rd 	<= 1'b1;
 		else 
			fifo_rd 	<= 1'b0;
	end
end

//FIFO输出的数据有效标志，相对于摄像头输入视频延迟了两个时钟周期
always @ (posedge clk or negedge rst_n) begin
    if(!rst_n) begin
		fifo_valid	<= 1'b0;
	end
    else begin
		fifo_valid 	<= fifo_rd & fifo_rd_en;		//unused
	end
end

//为了与FIFO中读出的数据同步，将摄像头输入视频也延迟两个时钟周期 
reg [CHANNEL_IN_W -1 : 0]	s0_axis_tdata_dly1;
	
always @ (posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        s0_axis_tdata_dly1 <= 0;
	end
    else begin
        s0_axis_tdata_dly1 <=  s0_axis_tdata;
	end
end	

//前面fifo延迟2个时钟周期，合并数据一个时钟周期
//*****************************************************延迟3个时钟周期
//将摄像头输入的同步信号延迟3个时钟周期，与数据同步
reg  [1:0] 	s0_axis_tvalid_reg;

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        s0_axis_tvalid_reg <= 3'd0;
    end
    else begin
        s0_axis_tvalid_reg <= {s0_axis_tvalid_reg[1:0],s0_axis_tvalid & s0_axis_tready }; 
    end
end



reg  [CHANNEL_OUT_W - 1 : 0] 	com_data;
//*****************************************************消耗一个时钟周期
always @ (posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        com_data <= 'd0;
    end
    else if(s0_axis_tvalid_reg[0]) begin
        com_data <= {s2,s1};
    end  
end

wire [CHANNEL_IN_W - 1:0] s1;
wire [CHANNEL_IN_W - 1:0] s2;

assign s1 = s0_axis_tdata_dly1;
assign s2 = fifo_q[CHANNEL_IN_W - 1 : 0];


assign s0_axis_tready = m_axis_tready;
	
//给AXI ST Master接口赋值
always @ (posedge clk or negedge rst_n ) begin
	if(!rst_n) begin
		m_axis_tvalid   <= 1'd0;
        m_axis_tuser    <= 1'd0;
        m_axis_tlast    <= 1'd0;
        m_axis_tdata    <= 'd0;
	end
    else begin
         m_axis_tvalid  <= s0_axis_tvalid;
         m_axis_tuser   <= s0_axis_tuser ;
         m_axis_tlast   <= s0_axis_tlast ;
         m_axis_tdata	<= com_data;
    end
end

endmodule