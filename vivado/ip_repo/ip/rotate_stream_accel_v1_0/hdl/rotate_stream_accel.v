// rotate_stream_accel.v
// 实时流旋转：CAPTURE/ROTATE/EMIT 并发 + 双缓冲；支持 NPC=1/2/4（多 PPC）
`timescale 1ns / 1ps

module rotate_stream_accel #(
    parameter integer NPC    = 1,           // pixels per clock: 1, 2, or 4
    parameter integer AXIS_W = NPC * 24,    // must be NPC*24
    parameter integer MAX_H  = 64,
    parameter integer MAX_W  = 128
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
    output reg               m_axis_tlast,

    output reg               dbg_cap_active,
    output reg               dbg_rot_active,
    output reg               dbg_emit_active,
    output reg  [31:0]       dbg_frames_in,
    output reg  [31:0]       dbg_frames_out,
    output reg  [31:0]       dbg_overlap_cycles
);

    localparam integer BPP       = 3;
    localparam integer MAX_PIX   = MAX_H * MAX_W;
    localparam integer STRIDE    = MAX_PIX * BPP;
    localparam integer MEM_BYTES = STRIDE * 4;

    // 综合期检查
    // synthesis translate_off
    initial begin
        if ((NPC != 1) && (NPC != 2) && (NPC != 4))
            $fatal(1, "NPC must be 1, 2, or 4");
        if (AXIS_W != NPC * 24)
            $fatal(1, "AXIS_W must equal NPC*24");
    end
    // synthesis translate_on

    reg [7:0] mem [0:MEM_BYTES-1];

    reg        running;
    reg [15:0] in_h, in_w, out_h, out_w;
    reg [1:0]  dir_r;

    reg in_valid0, in_valid1;
    reg out_valid0, out_valid1;
    reg wr_idx, rd_idx;
    reg rot_in_idx, rot_out_idx;

    localparam C_IDLE = 1'd0, C_RUN = 1'd1;
    reg c_state;
    reg [15:0] c_x, c_y;

    localparam R_IDLE = 1'd0, R_RUN = 1'd1;
    reg r_state;
    reg [31:0] r_idx, r_total, r_in_base, r_out_base;

    localparam E_IDLE = 1'd0, E_RUN = 1'd1;
    reg e_state;
    reg [15:0] e_x, e_y;
    reg [31:0] e_base;

    integer sx, sy, ox, oy, src_a, dst_a, k, pbase;
    reg [31:0] tmp_in_base;
    reg [AXIS_W-1:0] pack_word;

    function automatic [15:0] f_out_h;
        input [15:0] h, w;
        input [1:0] d;
        begin
            f_out_h = (d == 2'd1) ? h : w;
        end
    endfunction

    function automatic [15:0] f_out_w;
        input [15:0] h, w;
        input [1:0] d;
        begin
            f_out_w = (d == 2'd1) ? w : h;
        end
    endfunction

    always @(posedge ap_clk or negedge ap_rst_n) begin
        if (!ap_rst_n) begin
            running <= 0;
            ap_done <= 0; ap_idle <= 1; ap_ready <= 1;
            in_h <= 0; in_w <= 0; out_h <= 0; out_w <= 0; dir_r <= 0;
            in_valid0 <= 0; in_valid1 <= 0; out_valid0 <= 0; out_valid1 <= 0;
            wr_idx <= 0; rd_idx <= 0; rot_in_idx <= 0; rot_out_idx <= 0;
            c_state <= C_IDLE; c_x <= 0; c_y <= 0; s_axis_tready <= 0;
            r_state <= R_IDLE; r_idx <= 0; r_total <= 0; r_in_base <= 0; r_out_base <= 0;
            e_state <= E_IDLE; e_x <= 0; e_y <= 0; e_base <= 0;
            m_axis_tvalid <= 0; m_axis_tdata <= 0; m_axis_tuser <= 0; m_axis_tlast <= 0;
            dbg_cap_active <= 0; dbg_rot_active <= 0; dbg_emit_active <= 0;
            dbg_frames_in <= 0; dbg_frames_out <= 0; dbg_overlap_cycles <= 0;
        end else begin
            ap_done <= 1'b0;

            if (!enable) begin
                running <= 0;
                ap_idle <= 1; ap_ready <= 1;
                c_state <= C_IDLE; r_state <= R_IDLE; e_state <= E_IDLE;
                s_axis_tready <= 0; m_axis_tvalid <= 0;
                dbg_cap_active <= 0; dbg_rot_active <= 0; dbg_emit_active <= 0;
                in_valid0 <= 0; in_valid1 <= 0; out_valid0 <= 0; out_valid1 <= 0;
            end else if (ap_start && ap_ready) begin
                running <= 1;
                ap_idle <= 0;
                ap_ready <= 0;
                in_h <= height; in_w <= width;
                out_h <= f_out_h(height, width, direction);
                out_w <= f_out_w(height, width, direction);
                dir_r <= direction;
                in_valid0 <= 0; in_valid1 <= 0; out_valid0 <= 0; out_valid1 <= 0;
                wr_idx <= 0; rd_idx <= 0;
                c_state <= C_IDLE; r_state <= R_IDLE; e_state <= E_IDLE;
                dbg_frames_in <= 0; dbg_frames_out <= 0; dbg_overlap_cycles <= 0;
            end else if (running) begin
                if (dbg_cap_active && dbg_emit_active)
                    dbg_overlap_cycles <= dbg_overlap_cycles + 1;

                // ========== CAPTURE：每拍 NPC 像素 ==========
                case (c_state)
                    C_IDLE: begin
                        dbg_cap_active <= 0;
                        s_axis_tready <= 0;
                        if (!in_valid0 || !in_valid1) begin
                            c_x <= 0; c_y <= 0;
                            c_state <= C_RUN;
                            s_axis_tready <= 1;
                            dbg_cap_active <= 1;
                        end
                    end
                    C_RUN: begin
                        dbg_cap_active <= 1;
                        s_axis_tready <= 1;
                        if (s_axis_tvalid && s_axis_tready) begin
                            tmp_in_base = wr_idx ? STRIDE : 0;
                            for (k = 0; k < NPC; k = k + 1) begin
                                pbase = tmp_in_base + (c_y * in_w + c_x + k) * BPP;
                                mem[pbase + 0] <= s_axis_tdata[k*24 + 7  -: 8];
                                mem[pbase + 1] <= s_axis_tdata[k*24 + 15 -: 8];
                                mem[pbase + 2] <= s_axis_tdata[k*24 + 23 -: 8];
                            end
                            if ((c_x + NPC) >= in_w) begin
                                c_x <= 0;
                                if (c_y == (in_h - 16'd1)) begin
                                    if (wr_idx == 0) in_valid0 <= 1'b1;
                                    else             in_valid1 <= 1'b1;
                                    wr_idx <= ~wr_idx;
                                    dbg_frames_in <= dbg_frames_in + 1;
                                    c_state <= C_IDLE;
                                    s_axis_tready <= 0;
                                    dbg_cap_active <= 0;
                                end else c_y <= c_y + 1;
                            end else c_x <= c_x + NPC[15:0];
                        end
                    end
                endcase

                // ========== ROTATE：仍按像素（FB 字节序） ==========
                case (r_state)
                    R_IDLE: begin
                        dbg_rot_active <= 0;
                        if ((in_valid0 || in_valid1) && (!out_valid0 || !out_valid1)) begin
                            if (in_valid0) begin
                                rot_in_idx <= 0; r_in_base <= 0;
                            end else begin
                                rot_in_idx <= 1; r_in_base <= STRIDE;
                            end
                            if (!out_valid0) begin
                                rot_out_idx <= 0; r_out_base <= 2 * STRIDE;
                            end else begin
                                rot_out_idx <= 1; r_out_base <= 3 * STRIDE;
                            end
                            r_idx <= 0;
                            r_total <= out_h * out_w;
                            r_state <= R_RUN;
                            dbg_rot_active <= 1;
                        end
                    end
                    R_RUN: begin
                        dbg_rot_active <= 1;
                        ox = r_idx % out_w;
                        oy = r_idx / out_w;
                        if (dir_r == 2'd0) begin
                            sx = oy; sy = in_h - 1 - ox;
                        end else if (dir_r == 2'd1) begin
                            sx = in_w - 1 - ox; sy = in_h - 1 - oy;
                        end else begin
                            sx = in_w - 1 - oy; sy = ox;
                        end
                        src_a = r_in_base + (sy * in_w + sx) * BPP;
                        dst_a = r_out_base + (oy * out_w + ox) * BPP;
                        mem[dst_a + 0] <= mem[src_a + 0];
                        mem[dst_a + 1] <= mem[src_a + 1];
                        mem[dst_a + 2] <= mem[src_a + 2];
                        if (r_idx >= (r_total - 1)) begin
                            if (rot_in_idx == 0) in_valid0 <= 1'b0;
                            else                 in_valid1 <= 1'b0;
                            if (rot_out_idx == 0) out_valid0 <= 1'b1;
                            else                  out_valid1 <= 1'b1;
                            r_state <= R_IDLE;
                            dbg_rot_active <= 0;
                        end else r_idx <= r_idx + 1;
                    end
                endcase

                // ========== EMIT：每拍打包 NPC 像素 ==========
                case (e_state)
                    E_IDLE: begin
                        dbg_emit_active <= 0;
                        m_axis_tvalid <= 0;
                        if (out_valid0 || out_valid1) begin
                            if (out_valid0) begin
                                rd_idx <= 0; e_base <= 2 * STRIDE;
                            end else begin
                                rd_idx <= 1; e_base <= 3 * STRIDE;
                            end
                            e_x <= 0; e_y <= 0;
                            e_state <= E_RUN;
                            dbg_emit_active <= 1;
                        end
                    end
                    E_RUN: begin
                        dbg_emit_active <= 1;
                        if (!m_axis_tvalid) begin
                            pack_word = {AXIS_W{1'b0}};
                            for (k = 0; k < NPC; k = k + 1) begin
                                pbase = e_base + (e_y * out_w + e_x + k) * BPP;
                                pack_word[k*24 + 7  -: 8] = mem[pbase + 0];
                                pack_word[k*24 + 15 -: 8] = mem[pbase + 1];
                                pack_word[k*24 + 23 -: 8] = mem[pbase + 2];
                            end
                            m_axis_tdata  <= pack_word;
                            m_axis_tuser  <= (e_y == 0 && e_x == 0);
                            m_axis_tlast  <= ((e_x + NPC) >= out_w);
                            m_axis_tvalid <= 1;
                        end else if (m_axis_tready) begin
                            if ((e_x + NPC) >= out_w) begin
                                if (e_y == (out_h - 1)) begin
                                    m_axis_tvalid <= 0;
                                    if (rd_idx == 0) out_valid0 <= 1'b0;
                                    else             out_valid1 <= 1'b0;
                                    dbg_frames_out <= dbg_frames_out + 1;
                                    ap_done <= 1;
                                    e_state <= E_IDLE;
                                    dbg_emit_active <= 0;
                                end else begin
                                    e_x <= 0;
                                    e_y <= e_y + 1;
                                    pack_word = {AXIS_W{1'b0}};
                                    for (k = 0; k < NPC; k = k + 1) begin
                                        pbase = e_base + ((e_y + 1) * out_w + k) * BPP;
                                        pack_word[k*24 + 7  -: 8] = mem[pbase + 0];
                                        pack_word[k*24 + 15 -: 8] = mem[pbase + 1];
                                        pack_word[k*24 + 23 -: 8] = mem[pbase + 2];
                                    end
                                    m_axis_tdata  <= pack_word;
                                    m_axis_tuser  <= 0;
                                    m_axis_tlast  <= (NPC >= out_w);
                                    m_axis_tvalid <= 1;
                                end
                            end else begin
                                e_x <= e_x + NPC[15:0];
                                pack_word = {AXIS_W{1'b0}};
                                for (k = 0; k < NPC; k = k + 1) begin
                                    pbase = e_base + (e_y * out_w + (e_x + NPC + k)) * BPP;
                                    pack_word[k*24 + 7  -: 8] = mem[pbase + 0];
                                    pack_word[k*24 + 15 -: 8] = mem[pbase + 1];
                                    pack_word[k*24 + 23 -: 8] = mem[pbase + 2];
                                end
                                m_axis_tdata  <= pack_word;
                                m_axis_tuser  <= 0;
                                m_axis_tlast  <= ((e_x + NPC + NPC) >= out_w);
                                m_axis_tvalid <= 1;
                            end
                        end
                    end
                endcase
            end
        end
    end

endmodule
