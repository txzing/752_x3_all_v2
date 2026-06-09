module stream_output 
#(
	parameter CHANNEL_OUT_W = 48,
	parameter FIFO_D = 4096
)
(
    input                               clk             ,
    input                               rst_n           ,
    
    //视频流输入
    input   [CHANNEL_OUT_W - 1 : 0]     pixel_in        ,
    input                               pixel_in_vld    ,
    input                               pixel_in_sof    ,
    input                               pixel_in_eol    ,
    output                              pixel_in_rdy    ,
    
    //视频流输出
    output  [CHANNEL_OUT_W - 1 : 0]     pixel_out       ,
    output                              pixel_out_vld   ,
    output                              pixel_out_sof   ,
    output                              pixel_out_eol   ,
    input                               pixel_out_rdy   ,
    
    //Debug
    output                              over_flow       ,
    output                              under_flow      
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
//参数定义
localparam	FIFO_W      = CHANNEL_OUT_W + 2  ,
			FIFO_NUM_W  = clog2b(FIFO_D) + 1 ;    

//信号定义
    reg     [FIFO_NUM_W - 1 : 0]        cnt             ;
    wire                                add_cnt         ;
    wire                                end_cnt         ;
    reg                                 add_flag        ;

    reg                                 data_in_rdy     ;

    wire                                fifo_write      ;
    wire    [FIFO_W - 1 : 0]            fifo_data       ;
    wire                                fifo_alfull     ;
    wire                                fifo_full       ;
    wire                                fifo_read       ;
    wire    [FIFO_W - 1 : 0]            fifo_qout       ;
    wire                                fifo_qout_vld   ; 
    wire                                fifo_empty      ;
    wire    [FIFO_NUM_W - 1 : 0]        fifo_num        ;

//计数器
    always @(posedge clk or negedge rst_n)begin
        if(!rst_n)begin
            cnt <= 0;
        end
        else if(add_cnt)begin
            if(end_cnt)
                cnt <= 0;
            else
                cnt <= cnt + 1;
        end
    end
    assign add_cnt = add_flag && pixel_out_rdy;       
    assign end_cnt = add_cnt && cnt == (FIFO_D >> 1) - 1;   

    always  @(posedge clk or negedge rst_n)begin
        if(rst_n==1'b0)begin
            add_flag <= 1'b0;
        end
        else if(~add_flag && fifo_num >= (FIFO_D >> 1))begin
            add_flag <= 1'b1;
        end
        else if(add_flag && end_cnt)begin
            add_flag <= 1'b0;
        end
    end


//输入数据流握手信号
    always  @(posedge clk or negedge rst_n)begin
        if(rst_n==1'b0)begin
            data_in_rdy <= 1'b0;
        end
        else begin
            data_in_rdy <= ~fifo_alfull;
        end
    end

   xilinx_sync_fifo #(.FIFO_DEPTH(FIFO_D),.FIFO_WIDTH(FIFO_W),.FIFO_MODE("FWFT")) u_output_fifo(
    .clk         (clk           ),
    .rst_n       (rst_n         ),
    
    //write
    .wr_en       (fifo_write    ),
    .wr_data     (fifo_data     ),
    .wr_count    (              ),
    .al_full     (fifo_alfull   ),
    .full        (fifo_full     ),
    .wr_ack      (              ),

    //read
    .rd_en       (fifo_read     ),
    .rd_data     (fifo_qout     ),
    .rd_data_vld (fifo_qout_vld ),
    .rd_count    (fifo_num      ),
    .empty       (fifo_empty    ),
    .al_empty    (              )    
    );

    assign fifo_data  = {pixel_in_sof,pixel_in_eol,pixel_in};
    assign fifo_write = ~fifo_full && pixel_in_vld;
    assign fifo_read  = add_cnt && ~fifo_empty;

//输出
    assign pixel_out     = fifo_qout[CHANNEL_OUT_W - 1 : 0];// R G B
    assign pixel_out_vld = add_flag;
    assign pixel_out_sof = add_flag && fifo_qout[FIFO_W - 1];
    assign pixel_out_eol = add_flag && fifo_qout[FIFO_W - 2];
    assign pixel_in_rdy  = data_in_rdy ;

    assign over_flow = fifo_full;
    assign under_flow = fifo_empty;

endmodule 


