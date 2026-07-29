// tb_rotate_stream.sv
// Multi-frame AXIS rotate TB: configure once, stream N frames, compare golden.
`timescale 1ns / 1ps

module tb_rotate_stream;
    localparam integer CLK_NS   = 10;
    localparam integer HEIGHT   = 32;
    localparam integer WIDTH    = 64;
    localparam integer DIR      = 0;   // 90 CW
    localparam integer NFRAMES  = 3;
    localparam integer BPP      = 3;
    localparam integer OUT_H    = WIDTH;
    localparam integer OUT_W    = HEIGHT;
    localparam integer IN_PIX   = HEIGHT * WIDTH;
    localparam integer OUT_PIX  = OUT_H * OUT_W;
    localparam integer IN_BYTES = IN_PIX * BPP;
    localparam integer OUT_BYTES = OUT_PIX * BPP;

    reg         ap_clk;
    reg         ap_rst_n;
    reg         ap_start;
    wire        ap_done;
    wire        ap_idle;
    wire        ap_ready;
    reg  [15:0] height;
    reg  [15:0] width;
    reg  [1:0]  direction;
    reg         enable;

    reg  [23:0] s_tdata;
    reg         s_tvalid;
    wire        s_tready;
    reg         s_tuser;
    reg         s_tlast;

    wire [23:0] m_tdata;
    wire        m_tvalid;
    reg         m_tready;
    wire        m_tuser;
    wire        m_tlast;

    // 激励与期望缓冲
    reg [7:0] din  [0:NFRAMES*IN_BYTES-1];
    reg [7:0] dout [0:NFRAMES*OUT_BYTES-1];
    reg [7:0] gold [0:NFRAMES*OUT_BYTES-1];

    integer f, x, y, i, sx, sy, mism, first;
    integer out_idx;
    integer timeout;
    integer frame_done_cnt;

    rotate_stream_accel #(
        .AXIS_W(24),
        .MAX_H(HEIGHT),
        .MAX_W(WIDTH)
    ) dut (
        .ap_clk(ap_clk),
        .ap_rst_n(ap_rst_n),
        .ap_start(ap_start),
        .ap_done(ap_done),
        .ap_idle(ap_idle),
        .ap_ready(ap_ready),
        .height(height),
        .width(width),
        .direction(direction),
        .enable(enable),
        .s_axis_tdata(s_tdata),
        .s_axis_tvalid(s_tvalid),
        .s_axis_tready(s_tready),
        .s_axis_tuser(s_tuser),
        .s_axis_tlast(s_tlast),
        .m_axis_tdata(m_tdata),
        .m_axis_tvalid(m_tvalid),
        .m_axis_tready(m_tready),
        .m_axis_tuser(m_tuser),
        .m_axis_tlast(m_tlast)
    );

    // 时钟
    initial ap_clk = 1'b0;
    always #(CLK_NS/2) ap_clk = ~ap_clk;

    // 生成输入与 golden（与 C rotate_kernel 一致）
    task automatic build_vectors;
        integer ff, xx, yy, b, g, r, ox, oy, isx, isy;
        integer in_off, out_off;
        begin
            for (ff = 0; ff < NFRAMES; ff = ff + 1) begin
                for (yy = 0; yy < HEIGHT; yy = yy + 1) begin
                    for (xx = 0; xx < WIDTH; xx = xx + 1) begin
                        b = ((xx * 3 + yy * 5) ^ (ff * 17)) & 8'hFF;
                        g = ((xx * 7 + yy * 11) ^ (ff * 17)) & 8'hFF;
                        r = ((xx * 13 + yy * 17) ^ (ff * 17)) & 8'hFF;
                        in_off = ff * IN_BYTES + (yy * WIDTH + xx) * BPP;
                        din[in_off + 0] = b[7:0];
                        din[in_off + 1] = g[7:0];
                        din[in_off + 2] = r[7:0];
                    end
                end
                for (oy = 0; oy < OUT_H; oy = oy + 1) begin
                    for (ox = 0; ox < OUT_W; ox = ox + 1) begin
                        // dir=0 90CW
                        isx = oy;
                        isy = HEIGHT - 1 - ox;
                        in_off = ff * IN_BYTES + (isy * WIDTH + isx) * BPP;
                        out_off = ff * OUT_BYTES + (oy * OUT_W + ox) * BPP;
                        gold[out_off + 0] = din[in_off + 0];
                        gold[out_off + 1] = din[in_off + 1];
                        gold[out_off + 2] = din[in_off + 2];
                    end
                end
            end
        end
    endtask

    // 送一帧 AXIS
    task automatic send_frame;
        input integer fi;
        integer xx, yy, off;
        begin
            @(posedge ap_clk);
            ap_start <= 1'b1;
            @(posedge ap_clk);
            ap_start <= 1'b0;

            for (yy = 0; yy < HEIGHT; yy = yy + 1) begin
                for (xx = 0; xx < WIDTH; xx = xx + 1) begin
                    off = fi * IN_BYTES + (yy * WIDTH + xx) * BPP;
                    s_tdata  <= {din[off+2], din[off+1], din[off+0]};
                    s_tuser  <= (yy == 0 && xx == 0);
                    s_tlast  <= (xx == WIDTH - 1);
                    s_tvalid <= 1'b1;
                    @(posedge ap_clk);
                    while (!s_tready) @(posedge ap_clk);
                end
            end
            s_tvalid <= 1'b0;
            s_tuser  <= 1'b0;
            s_tlast  <= 1'b0;
        end
    endtask

    // 收一帧 AXIS
    task automatic recv_frame;
        input integer fi;
        integer got;
        integer off;
        begin
            got = 0;
            while (got < OUT_PIX) begin
                @(posedge ap_clk);
                if (m_tvalid && m_tready) begin
                    off = fi * OUT_BYTES + got * BPP;
                    dout[off + 0] = m_tdata[7:0];
                    dout[off + 1] = m_tdata[15:8];
                    dout[off + 2] = m_tdata[23:16];
                    got = got + 1;
                end
            end
            // wait ap_done
            timeout = 0;
            while (!ap_done && timeout < 100000) begin
                @(posedge ap_clk);
                timeout = timeout + 1;
            end
            if (!ap_done) begin
                $display("ERROR: timeout waiting ap_done frame %0d", fi);
                $finish(1);
            end
        end
    endtask

    initial begin
        ap_rst_n   = 1'b0;
        ap_start   = 1'b0;
        height     = HEIGHT;
        width      = WIDTH;
        direction  = DIR;
        enable     = 1'b1;
        s_tdata    = 24'd0;
        s_tvalid   = 1'b0;
        s_tuser    = 1'b0;
        s_tlast    = 1'b0;
        m_tready   = 1'b1;
        out_idx    = 0;
        frame_done_cnt = 0;

        build_vectors();

        repeat (5) @(posedge ap_clk);
        ap_rst_n = 1'b1;
        repeat (5) @(posedge ap_clk);

        $display("ModelSim/iverilog TB: %0d frames %0dx%0d dir=%0d -> %0dx%0d (config once)",
                 NFRAMES, WIDTH, HEIGHT, DIR, OUT_W, OUT_H);

        // 配置一次后连续多帧
        for (f = 0; f < NFRAMES; f = f + 1) begin
            fork
                send_frame(f);
                recv_frame(f);
            join
            frame_done_cnt = frame_done_cnt + 1;
            $display("Frame %0d done", f);
        end

        mism = 0;
        first = -1;
        for (i = 0; i < NFRAMES * OUT_BYTES; i = i + 1) begin
            if (dout[i] !== gold[i]) begin
                if (first < 0) first = i;
                mism = mism + 1;
            end
        end

        if (mism != 0) begin
            $display("Mismatch bytes: %0d / %0d first@%0d got=%02h gold=%02h",
                     mism, NFRAMES * OUT_BYTES, first, dout[first], gold[first]);
            $display("TEST FAILED");
            $finish(1);
        end

        $display("Mismatch bytes: 0 / %0d", NFRAMES * OUT_BYTES);
        $display("TEST PASSED");
        $finish(0);
    end

    // 全局超时
    initial begin
        #50_000_000;
        $display("ERROR: global timeout");
        $display("TEST FAILED");
        $finish(1);
    end

endmodule
