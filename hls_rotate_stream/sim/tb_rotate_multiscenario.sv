// tb_rotate_multiscenario.sv
// ModelSim 多场景：NPC×方向×分辨率×背压，一次仿真全部跑完
`timescale 1ns / 1ps

module tb_rotate_multiscenario;
    localparam integer CLK_NS = 10;
    localparam integer MAX_H  = 64;
    localparam integer MAX_W  = 128;
    localparam integer MAX_NPC = 4;
    localparam integer MAX_AXIS = MAX_NPC * 24;
    localparam integer BPP = 3;
    localparam integer MAX_FRAMES = 4;
    localparam integer MAX_IN_BYTES  = MAX_FRAMES * MAX_H * MAX_W * BPP;
    localparam integer MAX_OUT_BYTES = MAX_IN_BYTES;

    // DUT ports — use max AXIS width; unused MSBs ignored when NPC smaller
    reg                    ap_clk, ap_rst_n, ap_start, enable;
    wire                   ap_done, ap_idle, ap_ready;
    reg  [15:0]            height, width;
    reg  [1:0]             direction;
    reg  [MAX_AXIS-1:0]    s_tdata;
    reg                    s_tvalid, s_tuser, s_tlast;
    wire                   s_tready;
    wire [MAX_AXIS-1:0]    m_tdata;
    wire                   m_tvalid, m_tuser, m_tlast;
    reg                    m_tready;

    wire                   dbg_cap_active, dbg_rot_active, dbg_emit_active;
    wire [31:0]            dbg_frames_in, dbg_frames_out, dbg_overlap_cycles;

    // Three DUT instances for NPC=1/2/4 (AXIS width must match parameter)
    wire        s_tready_n1, s_tready_n2, s_tready_n4;
    wire [23:0] m_tdata_n1;
    wire [47:0] m_tdata_n2;
    wire [95:0] m_tdata_n4;
    wire        m_tvalid_n1, m_tvalid_n2, m_tvalid_n4;
    wire        m_tuser_n1, m_tuser_n2, m_tuser_n4;
    wire        m_tlast_n1, m_tlast_n2, m_tlast_n4;
    wire        ap_done_n1, ap_done_n2, ap_done_n4;
    wire        ap_idle_n1, ap_idle_n2, ap_idle_n4;
    wire        ap_ready_n1, ap_ready_n2, ap_ready_n4;
    wire        dbg_cap_n1, dbg_cap_n2, dbg_cap_n4;
    wire        dbg_rot_n1, dbg_rot_n2, dbg_rot_n4;
    wire        dbg_emit_n1, dbg_emit_n2, dbg_emit_n4;
    wire [31:0] fin_n1, fin_n2, fin_n4;
    wire [31:0] fout_n1, fout_n2, fout_n4;
    wire [31:0] ov_n1, ov_n2, ov_n4;

    reg [1:0]  active_npc_sel; // 0->1, 1->2, 2->4

    assign s_tready = (active_npc_sel == 2'd0) ? s_tready_n1 :
                      (active_npc_sel == 2'd1) ? s_tready_n2 : s_tready_n4;
    assign m_tdata  = (active_npc_sel == 2'd0) ? {{(MAX_AXIS-24){1'b0}}, m_tdata_n1} :
                      (active_npc_sel == 2'd1) ? {{(MAX_AXIS-48){1'b0}}, m_tdata_n2} :
                                                 m_tdata_n4;
    assign m_tvalid = (active_npc_sel == 2'd0) ? m_tvalid_n1 :
                      (active_npc_sel == 2'd1) ? m_tvalid_n2 : m_tvalid_n4;
    assign m_tuser  = (active_npc_sel == 2'd0) ? m_tuser_n1 :
                      (active_npc_sel == 2'd1) ? m_tuser_n2 : m_tuser_n4;
    assign m_tlast  = (active_npc_sel == 2'd0) ? m_tlast_n1 :
                      (active_npc_sel == 2'd1) ? m_tlast_n2 : m_tlast_n4;
    assign ap_done  = (active_npc_sel == 2'd0) ? ap_done_n1 :
                      (active_npc_sel == 2'd1) ? ap_done_n2 : ap_done_n4;
    assign ap_idle  = (active_npc_sel == 2'd0) ? ap_idle_n1 :
                      (active_npc_sel == 2'd1) ? ap_idle_n2 : ap_idle_n4;
    assign ap_ready = (active_npc_sel == 2'd0) ? ap_ready_n1 :
                      (active_npc_sel == 2'd1) ? ap_ready_n2 : ap_ready_n4;
    assign dbg_frames_in      = (active_npc_sel == 2'd0) ? fin_n1  : (active_npc_sel == 2'd1) ? fin_n2  : fin_n4;
    assign dbg_frames_out     = (active_npc_sel == 2'd0) ? fout_n1 : (active_npc_sel == 2'd1) ? fout_n2 : fout_n4;
    assign dbg_overlap_cycles = (active_npc_sel == 2'd0) ? ov_n1   : (active_npc_sel == 2'd1) ? ov_n2   : ov_n4;

    rotate_stream_accel #(.NPC(1), .AXIS_W(24), .MAX_H(MAX_H), .MAX_W(MAX_W)) u_n1 (
        .ap_clk(ap_clk), .ap_rst_n(ap_rst_n),
        .ap_start(ap_start && (active_npc_sel==2'd0)),
        .ap_done(ap_done_n1), .ap_idle(ap_idle_n1), .ap_ready(ap_ready_n1),
        .height(height), .width(width), .direction(direction),
        .enable(enable && (active_npc_sel==2'd0)),
        .s_axis_tdata(s_tdata[23:0]), .s_axis_tvalid(s_tvalid && (active_npc_sel==2'd0)),
        .s_axis_tready(s_tready_n1), .s_axis_tuser(s_tuser), .s_axis_tlast(s_tlast),
        .m_axis_tdata(m_tdata_n1), .m_axis_tvalid(m_tvalid_n1), .m_axis_tready(m_tready),
        .m_axis_tuser(m_tuser_n1), .m_axis_tlast(m_tlast_n1),
        .dbg_cap_active(dbg_cap_n1), .dbg_rot_active(dbg_rot_n1), .dbg_emit_active(dbg_emit_n1),
        .dbg_frames_in(fin_n1), .dbg_frames_out(fout_n1), .dbg_overlap_cycles(ov_n1)
    );

    rotate_stream_accel #(.NPC(2), .AXIS_W(48), .MAX_H(MAX_H), .MAX_W(MAX_W)) u_n2 (
        .ap_clk(ap_clk), .ap_rst_n(ap_rst_n),
        .ap_start(ap_start && (active_npc_sel==2'd1)),
        .ap_done(ap_done_n2), .ap_idle(ap_idle_n2), .ap_ready(ap_ready_n2),
        .height(height), .width(width), .direction(direction),
        .enable(enable && (active_npc_sel==2'd1)),
        .s_axis_tdata(s_tdata[47:0]), .s_axis_tvalid(s_tvalid && (active_npc_sel==2'd1)),
        .s_axis_tready(s_tready_n2), .s_axis_tuser(s_tuser), .s_axis_tlast(s_tlast),
        .m_axis_tdata(m_tdata_n2), .m_axis_tvalid(m_tvalid_n2), .m_axis_tready(m_tready),
        .m_axis_tuser(m_tuser_n2), .m_axis_tlast(m_tlast_n2),
        .dbg_cap_active(dbg_cap_n2), .dbg_rot_active(dbg_rot_n2), .dbg_emit_active(dbg_emit_n2),
        .dbg_frames_in(fin_n2), .dbg_frames_out(fout_n2), .dbg_overlap_cycles(ov_n2)
    );

    rotate_stream_accel #(.NPC(4), .AXIS_W(96), .MAX_H(MAX_H), .MAX_W(MAX_W)) u_n4 (
        .ap_clk(ap_clk), .ap_rst_n(ap_rst_n),
        .ap_start(ap_start && (active_npc_sel==2'd2)),
        .ap_done(ap_done_n4), .ap_idle(ap_idle_n4), .ap_ready(ap_ready_n4),
        .height(height), .width(width), .direction(direction),
        .enable(enable && (active_npc_sel==2'd2)),
        .s_axis_tdata(s_tdata[95:0]), .s_axis_tvalid(s_tvalid && (active_npc_sel==2'd2)),
        .s_axis_tready(s_tready_n4), .s_axis_tuser(s_tuser), .s_axis_tlast(s_tlast),
        .m_axis_tdata(m_tdata_n4), .m_axis_tvalid(m_tvalid_n4), .m_axis_tready(m_tready),
        .m_axis_tuser(m_tuser_n4), .m_axis_tlast(m_tlast_n4),
        .dbg_cap_active(dbg_cap_n4), .dbg_rot_active(dbg_rot_n4), .dbg_emit_active(dbg_emit_n4),
        .dbg_frames_in(fin_n4), .dbg_frames_out(fout_n4), .dbg_overlap_cycles(ov_n4)
    );

    reg [7:0] din  [0:MAX_IN_BYTES-1];
    reg [7:0] dout [0:MAX_OUT_BYTES-1];
    reg [7:0] gold [0:MAX_OUT_BYTES-1];

    integer npc, dir, hin, win, nframes, out_h, out_w;
    integer in_bytes, out_bytes, out_pix;
    integer f, xx, yy, kk, ox, oy, isx, isy, in_off, out_off;
    integer b, g, r, got, mism, first, timeout, i;
    integer scen_pass, scen_fail, backpressure;
    reg [MAX_AXIS-1:0] beat;
    integer bp_cnt;

    initial ap_clk = 0;
    always #(CLK_NS/2) ap_clk = ~ap_clk;

    task automatic map_src;
        input integer ox_i, oy_i, ih, iw, d;
        output integer sx_o, sy_o;
        begin
            if (d == 0) begin sx_o = oy_i; sy_o = ih - 1 - ox_i; end
            else if (d == 1) begin sx_o = iw - 1 - ox_i; sy_o = ih - 1 - oy_i; end
            else begin sx_o = iw - 1 - oy_i; sy_o = ox_i; end
        end
    endtask

    task automatic build_vectors;
        input integer ih, iw, d, nf;
        integer oh, ow;
        begin
            if (d == 1) begin oh = ih; ow = iw; end
            else begin oh = iw; ow = ih; end
            out_h = oh; out_w = ow;
            in_bytes  = nf * ih * iw * BPP;
            out_bytes = nf * oh * ow * BPP;
            out_pix   = oh * ow;
            for (f = 0; f < nf; f = f + 1) begin
                for (yy = 0; yy < ih; yy = yy + 1)
                    for (xx = 0; xx < iw; xx = xx + 1) begin
                        b = ((xx * 3 + yy * 5) ^ (f * 17)) & 8'hFF;
                        g = ((xx * 7 + yy * 11) ^ (f * 17)) & 8'hFF;
                        r = ((xx * 13 + yy * 17) ^ (f * 17)) & 8'hFF;
                        in_off = f * ih * iw * BPP + (yy * iw + xx) * BPP;
                        din[in_off+0] = b[7:0];
                        din[in_off+1] = g[7:0];
                        din[in_off+2] = r[7:0];
                    end
                for (oy = 0; oy < oh; oy = oy + 1)
                    for (ox = 0; ox < ow; ox = ox + 1) begin
                        map_src(ox, oy, ih, iw, d, isx, isy);
                        in_off  = f * ih * iw * BPP + (isy * iw + isx) * BPP;
                        out_off = f * oh * ow * BPP + (oy * ow + ox) * BPP;
                        gold[out_off+0] = din[in_off+0];
                        gold[out_off+1] = din[in_off+1];
                        gold[out_off+2] = din[in_off+2];
                    end
            end
        end
    endtask

    task automatic reset_dut;
        begin
            ap_rst_n = 0; ap_start = 0; enable = 0;
            s_tvalid = 0; s_tuser = 0; s_tlast = 0; s_tdata = 0;
            m_tready = 1;
            repeat (4) @(posedge ap_clk);
            ap_rst_n = 1;
            repeat (2) @(posedge ap_clk);
        end
    endtask

    task automatic run_scenario;
        input integer npc_i, dir_i, ih, iw, nf, bp_en;
        integer axis_w, sel;
        begin
            npc = npc_i; dir = dir_i; hin = ih; win = iw; nframes = nf;
            backpressure = bp_en;
            if (npc_i == 1) sel = 0;
            else if (npc_i == 2) sel = 1;
            else sel = 2;
            active_npc_sel = sel[1:0];
            axis_w = npc_i * 24;

            if ((iw % npc_i) != 0) begin
                $display("SKIP scen npc=%0d %0dx%0d (width not divisible)", npc_i, iw, ih);
                disable run_scenario;
            end

            build_vectors(ih, iw, dir_i, nf);
            height = ih; width = iw; direction = dir_i[1:0];

            reset_dut();
            enable = 1;
            @(posedge ap_clk);
            while (!ap_ready) @(posedge ap_clk);
            ap_start <= 1;
            @(posedge ap_clk);
            ap_start <= 0;

            $display("SCENARIO start: NPC=%0d DIR=%0d %0dx%0d frames=%0d bp=%0d",
                     npc_i, dir_i, iw, ih, nf, bp_en);

            fork
                begin : drive
                    bp_cnt = 0;
                    for (f = 0; f < nf; f = f + 1) begin
                        for (yy = 0; yy < ih; yy = yy + 1) begin
                            for (xx = 0; xx < iw; xx = xx + npc_i) begin
                                beat = {MAX_AXIS{1'b0}};
                                for (kk = 0; kk < npc_i; kk = kk + 1) begin
                                    in_off = f * ih * iw * BPP + (yy * iw + xx + kk) * BPP;
                                    beat[kk*24 + 7  -: 8] = din[in_off+0];
                                    beat[kk*24 + 15 -: 8] = din[in_off+1];
                                    beat[kk*24 + 23 -: 8] = din[in_off+2];
                                end
                                s_tdata  <= beat;
                                s_tuser  <= (yy == 0 && xx == 0);
                                s_tlast  <= ((xx + npc_i) >= iw);
                                s_tvalid <= 1'b1;
                                @(posedge ap_clk);
                                while (!s_tready) @(posedge ap_clk);
                            end
                        end
                    end
                    s_tvalid <= 0; s_tuser <= 0; s_tlast <= 0;
                end
                begin : monitor
                    got = 0;
                    while (got < nf * out_pix) begin
                        @(posedge ap_clk);
                        if (bp_en) begin
                            bp_cnt = bp_cnt + 1;
                            m_tready <= ((bp_cnt % 3) != 0); // 2/3 duty
                        end else m_tready <= 1'b1;

                        if (m_tvalid && m_tready) begin
                            for (kk = 0; kk < npc_i; kk = kk + 1) begin
                                out_off = (got + kk) * BPP;
                                dout[out_off+0] = m_tdata[kk*24 + 7  -: 8];
                                dout[out_off+1] = m_tdata[kk*24 + 15 -: 8];
                                dout[out_off+2] = m_tdata[kk*24 + 23 -: 8];
                            end
                            got = got + npc_i;
                        end
                    end
                end
            join

            timeout = 0;
            while ((dbg_frames_out < nf) && (timeout < 500000)) begin
                @(posedge ap_clk);
                timeout = timeout + 1;
            end

            mism = 0; first = -1;
            for (i = 0; i < out_bytes; i = i + 1) begin
                if (dout[i] !== gold[i]) begin
                    if (first < 0) first = i;
                    mism = mism + 1;
                end
            end

            if ((mism != 0) || (dbg_frames_out < nf) ||
                ((nf >= 2) && (dbg_overlap_cycles == 0) && (bp_en == 0))) begin
                $display("SCENARIO FAIL: NPC=%0d DIR=%0d %0dx%0d mism=%0d fout=%0d ov=%0d first=%0d",
                         npc_i, dir_i, iw, ih, mism, dbg_frames_out, dbg_overlap_cycles, first);
                scen_fail = scen_fail + 1;
            end else begin
                $display("SCENARIO PASS: NPC=%0d DIR=%0d %0dx%0d frames=%0d ov=%0d bp=%0d",
                         npc_i, dir_i, iw, ih, nf, dbg_overlap_cycles, bp_en);
                scen_pass = scen_pass + 1;
            end

            // isolate between scenarios
            enable = 0;
            repeat (5) @(posedge ap_clk);
        end
    endtask

    initial begin
        scen_pass = 0; scen_fail = 0;
        active_npc_sel = 0;
        ap_rst_n = 0; ap_start = 0; enable = 0;
        height = 0; width = 0; direction = 0;
        s_tdata = 0; s_tvalid = 0; s_tuser = 0; s_tlast = 0; m_tready = 1;

        $display("======== ModelSim multi-scenario rotate_stream_accel ========");

        // NPC=1, dirs, basic size
        run_scenario(1, 0, 32, 64, 4, 0);
        run_scenario(1, 1, 32, 64, 3, 0);
        run_scenario(1, 2, 32, 64, 3, 0);

        // NPC=2
        run_scenario(2, 0, 32, 64, 4, 0);
        run_scenario(2, 1, 32, 64, 3, 0);
        run_scenario(2, 2, 32, 64, 3, 0);

        // NPC=4
        run_scenario(4, 0, 32, 64, 4, 0);
        run_scenario(4, 1, 32, 64, 2, 0);
        run_scenario(4, 2, 32, 64, 2, 0);

        // alternate geometry
        run_scenario(1, 0, 48, 96, 2, 0);
        run_scenario(2, 0, 48, 96, 2, 0);
        run_scenario(4, 0, 48, 96, 2, 0);

        // backpressure on output
        run_scenario(1, 0, 32, 64, 3, 1);
        run_scenario(2, 0, 32, 64, 3, 1);

        $display("======== SUMMARY pass=%0d fail=%0d ========", scen_pass, scen_fail);
        if (scen_fail != 0) begin
            $display("TEST FAILED");
            $finish(1);
        end
        $display("TEST PASSED");
        $finish(0);
    end

    initial begin
        #500_000_000;
        $display("ERROR: global timeout");
        $display("TEST FAILED");
        $finish(1);
    end
endmodule
