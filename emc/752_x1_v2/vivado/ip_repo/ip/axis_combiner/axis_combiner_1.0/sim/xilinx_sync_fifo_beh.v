// xilinx_sync_fifo_beh.v
// 仿真用行为级 FIFO，接口与综合版 xilinx_sync_fifo 一致，避免依赖 xpm_fifo_sync
`timescale 1ns / 1ps

module xilinx_sync_fifo #(
    parameter FIFO_DEPTH = 512,
    parameter FIFO_WIDTH = 8,
    parameter FIFO_MODE  = "FWFT"
)(
    input                         clk,
    input                         rst_n,
    input                         wr_en,
    input      [FIFO_WIDTH-1:0]   wr_data,
    output     [15:0]             wr_count,
    output                        al_full,
    output                        full,
    output                        wr_ack,
    input                         rd_en,
    output     [FIFO_WIDTH-1:0]   rd_data,
    output                        rd_data_vld,
    output     [15:0]             rd_count,
    output                        empty,
    output                        al_empty
);

localparam integer ADDR_W = (FIFO_DEPTH <= 2) ? 1 :
                            (FIFO_DEPTH <= 4) ? 2 :
                            (FIFO_DEPTH <= 8) ? 3 :
                            (FIFO_DEPTH <= 16) ? 4 :
                            (FIFO_DEPTH <= 32) ? 5 :
                            (FIFO_DEPTH <= 64) ? 6 :
                            (FIFO_DEPTH <= 128) ? 7 :
                            (FIFO_DEPTH <= 256) ? 8 :
                            (FIFO_DEPTH <= 512) ? 9 :
                            (FIFO_DEPTH <= 1024) ? 10 :
                            (FIFO_DEPTH <= 2048) ? 11 :
                            (FIFO_DEPTH <= 4096) ? 12 : 16;

reg [FIFO_WIDTH-1:0] mem [0:FIFO_DEPTH-1];
reg [15:0]           count;
reg [ADDR_W-1:0]     wr_ptr;
reg [ADDR_W-1:0]     rd_ptr;

wire wr_fire = wr_en && !full;
wire rd_fire = rd_en && !empty;

assign full     = (count == FIFO_DEPTH[15:0]);
assign empty    = (count == 16'd0);
assign wr_count = count;
assign rd_count = count;
assign al_full  = (count >= (FIFO_DEPTH - 8));
assign al_empty = (count <= 16'd8);
assign wr_ack   = wr_fire;

// FWFT: 非空时 dout 立即有效
assign rd_data     = mem[rd_ptr];
assign rd_data_vld = !empty;

// 写/读指针与占用计数
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        count  <= 16'd0;
        wr_ptr <= {ADDR_W{1'b0}};
        rd_ptr <= {ADDR_W{1'b0}};
    end else begin
        case ({wr_fire, rd_fire})
            2'b10: begin
                mem[wr_ptr] <= wr_data;
                wr_ptr      <= wr_ptr + 1'b1;
                count       <= count + 16'd1;
            end
            2'b01: begin
                rd_ptr <= rd_ptr + 1'b1;
                count  <= count - 16'd1;
            end
            2'b11: begin
                mem[wr_ptr] <= wr_data;
                wr_ptr      <= wr_ptr + 1'b1;
                rd_ptr      <= rd_ptr + 1'b1;
                count       <= count;
            end
            default: begin
                count <= count;
            end
        endcase
    end
end

endmodule
