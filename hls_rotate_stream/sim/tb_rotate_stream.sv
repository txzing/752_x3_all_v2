// tb_rotate_stream.sv — 实时流：一次 ap_start，连续多帧，检查 CAPTURE∥EMIT 重叠
`timescale 1ns / 1ps

module tb_rotate_stream;
    localparam integer CLK_NS  = 10;
    localparam integer HEIGHT  = 32;
    localparam integer WIDTH   = 64;
    localparam integer DIR     = 0;
    localparam integer NFRAMES = 4;
    localparam integer BPP     = 3;
    localparam integer OUT_H   = WIDTH;
    localparam integer OUT_W   = HEIGHT;
    localparam integer IN_PIX  = HEIGHT * WIDTH;
    localparam integer OUT_PIX = OUT_H * OUT_W;
    localparam integer IN_BYTES  = IN_PIX * BPP;
    localparam integer OUT_BYTES = OUT_PIX * BPP;

    reg ap_clk, ap_rst_n, ap_start, enable;
    wire ap_done, ap_idle, ap_ready;
    reg [15:0] height, width;
    reg [1:0] direction;

    reg [23:0] s_tdata;
    reg s_tvalid, s_tuser, s_tlast;
    wire s_tready;

    wire [23:0] m_tdata;
    wire m_tvalid, m_tuser, m_tlast;
    reg m_tready;

    wire dbg_cap_active, dbg_rot_active, dbg_emit_active;
    wire [31:0] dbg_frames_in, dbg_frames_out, dbg_overlap_cycles;

    reg [7:0] din  [0:NFRAMES*IN_BYTES-1];
    reg [7:0] dout [0:NFRAMES*OUT_BYTES-1];
    reg [7:0] gold [0:NFRAMES*OUT_BYTES-1];

    integer f, i, mism, first, timeout, got;
    integer ox, oy, isx, isy, in_off, out_off;
    integer xx, yy, b, g, r;

    rotate_stream_accel #(.AXIS_W(24), .MAX_H(HEIGHT), .MAX_W(WIDTH)) dut (
        .ap_clk(ap_clk), .ap_rst_n(ap_rst_n),
        .ap_start(ap_start), .ap_done(ap_done), .ap_idle(ap_idle), .ap_ready(ap_ready),
        .height(height), .width(width), .direction(direction), .enable(enable),
        .s_axis_tdata(s_tdata), .s_axis_tvalid(s_tvalid), .s_axis_tready(s_tready),
        .s_axis_tuser(s_tuser), .s_axis_tlast(s_tlast),
        .m_axis_tdata(m_tdata), .m_axis_tvalid(m_tvalid), .m_axis_tready(m_tready),
        .m_axis_tuser(m_tuser), .m_axis_tlast(m_tlast),
        .dbg_cap_active(dbg_cap_active), .dbg_rot_active(dbg_rot_active),
        .dbg_emit_active(dbg_emit_active),
        .dbg_frames_in(dbg_frames_in), .dbg_frames_out(dbg_frames_out),
        .dbg_overlap_cycles(dbg_overlap_cycles)
    );

    initial ap_clk = 0;
    always #(CLK_NS/2) ap_clk = ~ap_clk;

    task automatic build_vectors;
        begin
            for (f = 0; f < NFRAMES; f = f + 1) begin
                for (yy = 0; yy < HEIGHT; yy = yy + 1)
                    for (xx = 0; xx < WIDTH; xx = xx + 1) begin
                        b = ((xx * 3 + yy * 5) ^ (f * 17)) & 8'hFF;
                        g = ((xx * 7 + yy * 11) ^ (f * 17)) & 8'hFF;
                        r = ((xx * 13 + yy * 17) ^ (f * 17)) & 8'hFF;
                        in_off = f * IN_BYTES + (yy * WIDTH + xx) * BPP;
                        din[in_off+0] = b[7:0];
                        din[in_off+1] = g[7:0];
                        din[in_off+2] = r[7:0];
                    end
                for (oy = 0; oy < OUT_H; oy = oy + 1)
                    for (ox = 0; ox < OUT_W; ox = ox + 1) begin
                        isx = oy;
                        isy = HEIGHT - 1 - ox;
                        in_off = f * IN_BYTES + (isy * WIDTH + isx) * BPP;
                        out_off = f * OUT_BYTES + (oy * OUT_W + ox) * BPP;
                        gold[out_off+0] = din[in_off+0];
                        gold[out_off+1] = din[in_off+1];
                        gold[out_off+2] = din[in_off+2];
                    end
            end
        end
    endtask

    // 连续送多帧（跟随 tready，模拟实时上游）
    task automatic send_all_frames;
        begin
            for (f = 0; f < NFRAMES; f = f + 1) begin
                for (yy = 0; yy < HEIGHT; yy = yy + 1) begin
                    for (xx = 0; xx < WIDTH; xx = xx + 1) begin
                        in_off = f * IN_BYTES + (yy * WIDTH + xx) * BPP;
                        s_tdata <= {din[in_off+2], din[in_off+1], din[in_off+0]};
                        s_tuser <= (yy == 0 && xx == 0);
                        s_tlast <= (xx == WIDTH - 1);
                        s_tvalid <= 1'b1;
                        @(posedge ap_clk);
                        while (!s_tready) @(posedge ap_clk);
                    end
                end
                $display("Sent frame %0d", f);
            end
            s_tvalid <= 1'b0;
            s_tuser <= 1'b0;
            s_tlast <= 1'b0;
        end
    endtask

    // 连续收多帧
    task automatic recv_all_frames;
        begin
            got = 0;
            while (got < NFRAMES * OUT_PIX) begin
                @(posedge ap_clk);
                if (m_tvalid && m_tready) begin
                    out_off = got * BPP;
                    dout[out_off+0] = m_tdata[7:0];
                    dout[out_off+1] = m_tdata[15:8];
                    dout[out_off+2] = m_tdata[23:16];
                    got = got + 1;
                    if ((got % OUT_PIX) == 0)
                        $display("Recv frame %0d", got / OUT_PIX - 1);
                end
            end
        end
    endtask

    initial begin
        ap_rst_n = 0; ap_start = 0; enable = 0;
        height = HEIGHT; width = WIDTH; direction = DIR;
        s_tdata = 0; s_tvalid = 0; s_tuser = 0; s_tlast = 0;
        m_tready = 1;
        build_vectors();

        repeat (5) @(posedge ap_clk);
        ap_rst_n = 1;
        enable = 1;
        repeat (2) @(posedge ap_clk);

        // 只启动一次
        @(posedge ap_clk);
        ap_start <= 1;
        @(posedge ap_clk);
        ap_start <= 0;

        $display("Realtime TB: ONE ap_start, %0d frames %0dx%0d dir=%0d", NFRAMES, WIDTH, HEIGHT, DIR);

        fork
            send_all_frames();
            recv_all_frames();
        join

        timeout = 0;
        while ((dbg_frames_out < NFRAMES) && (timeout < 100000)) begin
            @(posedge ap_clk);
            timeout = timeout + 1;
        end

        mism = 0; first = -1;
        for (i = 0; i < NFRAMES * OUT_BYTES; i = i + 1) begin
            if (dout[i] !== gold[i]) begin
                if (first < 0) first = i;
                mism = mism + 1;
            end
        end

        $display("frames_in=%0d frames_out=%0d overlap_cycles=%0d",
                 dbg_frames_in, dbg_frames_out, dbg_overlap_cycles);
        $display("Mismatch bytes: %0d / %0d", mism, NFRAMES * OUT_BYTES);

        if (mism != 0) begin
            $display("First mismatch @%0d got=%02h gold=%02h", first, dout[first], gold[first]);
            $display("TEST FAILED");
            $finish(1);
        end
        if (dbg_frames_out < NFRAMES) begin
            $display("ERROR: frames_out=%0d expected %0d", dbg_frames_out, NFRAMES);
            $display("TEST FAILED");
            $finish(1);
        end
        // 实时性：多帧时 CAPTURE 与 EMIT 应出现重叠
        if (NFRAMES >= 2 && dbg_overlap_cycles == 0) begin
            $display("ERROR: no CAPTURE||EMIT overlap — pipeline not concurrent");
            $display("TEST FAILED");
            $finish(1);
        end

        $display("TEST PASSED");
        $finish(0);
    end

    initial begin
        #100_000_000;
        $display("ERROR: global timeout");
        $display("TEST FAILED");
        $finish(1);
    end
endmodule
