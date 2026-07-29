// rotate_stream_accel.v
// Stream-in/Stream-out RGB888 rotate with on-chip multi-buffer (sim RTL; HLS exports m_axi)
`timescale 1ns / 1ps

module rotate_stream_accel #(
    parameter integer AXIS_W     = 24,
    parameter integer MAX_H      = 64,
    parameter integer MAX_W      = 128,
    parameter integer NUM_IN_BUF = 2,
    parameter integer NUM_OUT_BUF = 2
) (
    input  wire              ap_clk,
    input  wire              ap_rst_n,

    input  wire              ap_start,
    output reg               ap_done,
    output reg               ap_idle,
    output reg               ap_ready,

    input  wire [15:0]       height,
    input  wire [15:0]       width,
    input  wire [1:0]        direction,
    input  wire              enable,

    input  wire [AXIS_W-1:0] s_axis_tdata,
    input  wire              s_axis_tvalid,
    output reg               s_axis_tready,
    input  wire              s_axis_tuser,
    input  wire              s_axis_tlast,

    output reg  [AXIS_W-1:0] m_axis_tdata,
    output reg               m_axis_tvalid,
    input  wire              m_axis_tready,
    output reg               m_axis_tuser,
    output reg               m_axis_tlast
);

    localparam integer BPP = 3;
    localparam integer MAX_PIX = MAX_H * MAX_W;
    localparam integer STRIDE = MAX_PIX * BPP;
    localparam integer MEM_BYTES = STRIDE * (NUM_IN_BUF + NUM_OUT_BUF);

    // 片上多帧缓冲：IN0/IN1/OUT0/OUT1（仿真模型；板级由 m_axi DDR 承担）
    reg [7:0] mem [0:MEM_BYTES-1];

    localparam S_IDLE    = 3'd0;
    localparam S_CAPTURE = 3'd1;
    localparam S_ROTATE  = 3'd2;
    localparam S_EMIT    = 3'd3;
    localparam S_DONE    = 3'd4;

    reg [2:0]  state;
    reg        wr_sel;
    reg        rd_sel;
    reg [15:0] in_h, in_w, out_h, out_w;
    reg [31:0] in_base, out_base;
    reg [15:0] x_cnt, y_cnt;
    reg [31:0] rot_idx;
    reg [31:0] rot_total;

    integer sx, sy;
    integer src_a, dst_a;
    integer ox, oy;

    // 计算输出高
    function automatic [15:0] f_out_h;
        input [15:0] h, w;
        input [1:0] d;
        begin
            f_out_h = (d == 2'd1) ? h : w;
        end
    endfunction

    // 计算输出宽
    function automatic [15:0] f_out_w;
        input [15:0] h, w;
        input [1:0] d;
        begin
            f_out_w = (d == 2'd1) ? w : h;
        end
    endfunction

    // 复位与帧处理状态机
    always @(posedge ap_clk or negedge ap_rst_n) begin
        if (!ap_rst_n) begin
            state         <= S_IDLE;
            ap_done       <= 1'b0;
            ap_idle       <= 1'b1;
            ap_ready      <= 1'b1;
            s_axis_tready <= 1'b0;
            m_axis_tvalid <= 1'b0;
            m_axis_tdata  <= {AXIS_W{1'b0}};
            m_axis_tuser  <= 1'b0;
            m_axis_tlast  <= 1'b0;
            wr_sel        <= 1'b0;
            rd_sel        <= 1'b0;
            x_cnt         <= 16'd0;
            y_cnt         <= 16'd0;
            rot_idx       <= 32'd0;
            in_h          <= 16'd0;
            in_w          <= 16'd0;
            out_h         <= 16'd0;
            out_w         <= 16'd0;
            in_base       <= 32'd0;
            out_base      <= 32'd0;
            rot_total     <= 32'd0;
        end else begin
            ap_done <= 1'b0;

            case (state)
                S_IDLE: begin
                    ap_idle       <= 1'b1;
                    ap_ready      <= 1'b1;
                    s_axis_tready <= 1'b0;
                    m_axis_tvalid <= 1'b0;
                    if (ap_start && enable) begin
                        ap_idle  <= 1'b0;
                        ap_ready <= 1'b0;
                        in_h     <= height;
                        in_w     <= width;
                        out_h    <= f_out_h(height, width, direction);
                        out_w    <= f_out_w(height, width, direction);
                        in_base  <= wr_sel ? STRIDE : 0;
                        out_base <= (2 * STRIDE) + (rd_sel ? STRIDE : 0);
                        x_cnt    <= 16'd0;
                        y_cnt    <= 16'd0;
                        s_axis_tready <= 1'b1;
                        state    <= S_CAPTURE;
                    end
                end

                S_CAPTURE: begin
                    if (s_axis_tvalid && s_axis_tready) begin
                        mem[in_base + (y_cnt * in_w + x_cnt) * BPP + 0] <= s_axis_tdata[7:0];
                        mem[in_base + (y_cnt * in_w + x_cnt) * BPP + 1] <= s_axis_tdata[15:8];
                        mem[in_base + (y_cnt * in_w + x_cnt) * BPP + 2] <= s_axis_tdata[23:16];
                        if (x_cnt == (in_w - 16'd1)) begin
                            x_cnt <= 16'd0;
                            if (y_cnt == (in_h - 16'd1)) begin
                                s_axis_tready <= 1'b0;
                                y_cnt         <= 16'd0;
                                rot_idx       <= 32'd0;
                                rot_total     <= out_h * out_w;
                                state         <= S_ROTATE;
                            end else begin
                                y_cnt <= y_cnt + 16'd1;
                            end
                        end else begin
                            x_cnt <= x_cnt + 16'd1;
                        end
                    end
                end

                S_ROTATE: begin
                    ox = rot_idx % out_w;
                    oy = rot_idx / out_w;
                    if (direction == 2'd0) begin
                        sx = oy;
                        sy = in_h - 1 - ox;
                    end else if (direction == 2'd1) begin
                        sx = in_w - 1 - ox;
                        sy = in_h - 1 - oy;
                    end else begin
                        sx = in_w - 1 - oy;
                        sy = ox;
                    end
                    src_a = in_base + (sy * in_w + sx) * BPP;
                    dst_a = out_base + (oy * out_w + ox) * BPP;
                    mem[dst_a + 0] <= mem[src_a + 0];
                    mem[dst_a + 1] <= mem[src_a + 1];
                    mem[dst_a + 2] <= mem[src_a + 2];
                    if (rot_idx >= (rot_total - 32'd1)) begin
                        x_cnt         <= 16'd0;
                        y_cnt         <= 16'd0;
                        m_axis_tvalid <= 1'b0;
                        state         <= S_EMIT;
                    end else begin
                        rot_idx <= rot_idx + 32'd1;
                    end
                end

                S_EMIT: begin
                    // 握手：无效或对方 ready 时装载下一拍
                    if (!m_axis_tvalid) begin
                        m_axis_tdata <= {
                            mem[out_base + (y_cnt * out_w + x_cnt) * BPP + 2],
                            mem[out_base + (y_cnt * out_w + x_cnt) * BPP + 1],
                            mem[out_base + (y_cnt * out_w + x_cnt) * BPP + 0]
                        };
                        m_axis_tuser  <= ((y_cnt == 16'd0) && (x_cnt == 16'd0));
                        m_axis_tlast  <= (x_cnt == (out_w - 16'd1));
                        m_axis_tvalid <= 1'b1;
                    end else if (m_axis_tready) begin
                        if (x_cnt == (out_w - 16'd1)) begin
                            if (y_cnt == (out_h - 16'd1)) begin
                                m_axis_tvalid <= 1'b0;
                                state         <= S_DONE;
                            end else begin
                                x_cnt <= 16'd0;
                                y_cnt <= y_cnt + 16'd1;
                                m_axis_tdata <= {
                                    mem[out_base + ((y_cnt + 16'd1) * out_w + 0) * BPP + 2],
                                    mem[out_base + ((y_cnt + 16'd1) * out_w + 0) * BPP + 1],
                                    mem[out_base + ((y_cnt + 16'd1) * out_w + 0) * BPP + 0]
                                };
                                m_axis_tuser  <= 1'b0;
                                m_axis_tlast  <= (out_w == 16'd1);
                                m_axis_tvalid <= 1'b1;
                            end
                        end else begin
                            x_cnt <= x_cnt + 16'd1;
                            m_axis_tdata <= {
                                mem[out_base + (y_cnt * out_w + (x_cnt + 16'd1)) * BPP + 2],
                                mem[out_base + (y_cnt * out_w + (x_cnt + 16'd1)) * BPP + 1],
                                mem[out_base + (y_cnt * out_w + (x_cnt + 16'd1)) * BPP + 0]
                            };
                            m_axis_tuser  <= 1'b0;
                            m_axis_tlast  <= ((x_cnt + 16'd1) == (out_w - 16'd1));
                            m_axis_tvalid <= 1'b1;
                        end
                    end
                end

                S_DONE: begin
                    ap_done <= 1'b1;
                    wr_sel  <= ~wr_sel;
                    rd_sel  <= ~rd_sel;
                    state   <= S_IDLE;
                end

                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
