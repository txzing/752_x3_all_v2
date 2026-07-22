// tb_axis_pixel_compare_stress.v
// Stress / edge-case TB: real-world + extreme streams to expose bugs
`timescale 1ns / 1ps

module tb_axis_pixel_compare_stress;

localparam integer CLK_NS = 10;
localparam integer WIDTH  = 48;
localparam integer COLS   = 8;
localparam integer ROWS   = 4;

reg aclk, aresetn;
wire intr;

reg                  s_axis_tvalid;
wire                 s_axis_tready;
reg  [WIDTH-1:0]     s_axis_tdata;
reg                  s_axis_tlast;
reg                  s_axis_tuser;
wire                 m_axis_tvalid;
reg                  m_axis_tready;
wire [WIDTH-1:0]     m_axis_tdata;
wire                 m_axis_tlast;
wire                 m_axis_tuser;

reg  [6:0]  s00_axi_awaddr;
reg  [2:0]  s00_axi_awprot;
reg         s00_axi_awvalid;
wire        s00_axi_awready;
reg  [31:0] s00_axi_wdata;
reg  [3:0]  s00_axi_wstrb;
reg         s00_axi_wvalid;
wire        s00_axi_wready;
wire [1:0]  s00_axi_bresp;
wire        s00_axi_bvalid;
reg         s00_axi_bready;
reg  [6:0]  s00_axi_araddr;
reg  [2:0]  s00_axi_arprot;
reg         s00_axi_arvalid;
wire        s00_axi_arready;
wire [31:0] s00_axi_rdata;
wire [1:0]  s00_axi_rresp;
wire        s00_axi_rvalid;
reg         s00_axi_rready;

integer pass_cnt, fail_cnt, irq_edges;
reg     intr_d;
reg [31:0] rdata;
integer irq_before;

function [47:0] pack_px;
    input [23:0] ref_rgb;
    input [23:0] vid_rgb;
    begin
        pack_px = {ref_rgb, vid_rgb};
    end
endfunction

axis_pixel_compare #(
    .WIDTH(WIDTH), .PPC(1), .TUSER_WIDTH(1), .FREQ_HZ(1000),
    .ROI_RST_XS(0), .ROI_RST_XE(7), .ROI_RST_YS(0), .ROI_RST_YE(3)
) dut (
    .aclk(aclk), .aresetn(aresetn), .intr(intr),
    .s_axis_tvalid(s_axis_tvalid), .s_axis_tready(s_axis_tready),
    .s_axis_tdata(s_axis_tdata), .s_axis_tlast(s_axis_tlast), .s_axis_tuser(s_axis_tuser),
    .m_axis_tvalid(m_axis_tvalid), .m_axis_tready(m_axis_tready),
    .m_axis_tdata(m_axis_tdata), .m_axis_tlast(m_axis_tlast), .m_axis_tuser(m_axis_tuser),
    .s00_axi_aclk(aclk), .s00_axi_aresetn(aresetn),
    .s00_axi_awaddr(s00_axi_awaddr), .s00_axi_awprot(s00_axi_awprot),
    .s00_axi_awvalid(s00_axi_awvalid), .s00_axi_awready(s00_axi_awready),
    .s00_axi_wdata(s00_axi_wdata), .s00_axi_wstrb(s00_axi_wstrb),
    .s00_axi_wvalid(s00_axi_wvalid), .s00_axi_wready(s00_axi_wready),
    .s00_axi_bresp(s00_axi_bresp), .s00_axi_bvalid(s00_axi_bvalid), .s00_axi_bready(s00_axi_bready),
    .s00_axi_araddr(s00_axi_araddr), .s00_axi_arprot(s00_axi_arprot),
    .s00_axi_arvalid(s00_axi_arvalid), .s00_axi_arready(s00_axi_arready),
    .s00_axi_rdata(s00_axi_rdata), .s00_axi_rresp(s00_axi_rresp),
    .s00_axi_rvalid(s00_axi_rvalid), .s00_axi_rready(s00_axi_rready)
);

initial begin aclk = 0; forever #(CLK_NS/2) aclk = ~aclk; end

always @(posedge aclk) begin
    if (!aresetn) begin
        intr_d <= 0; irq_edges <= 0;
    end else begin
        if (intr && !intr_d) irq_edges <= irq_edges + 1;
        intr_d <= intr;
    end
end

task axi_write;
    input [6:0] addr; input [31:0] data;
    begin
        @(posedge aclk);
        s00_axi_awaddr <= addr; s00_axi_awprot <= 0; s00_axi_awvalid <= 1;
        s00_axi_wdata <= data; s00_axi_wstrb <= 4'hF; s00_axi_wvalid <= 1; s00_axi_bready <= 1;
        wait (s00_axi_awready && s00_axi_wready);
        @(posedge aclk);
        s00_axi_awvalid <= 0; s00_axi_wvalid <= 0;
        wait (s00_axi_bvalid);
        @(posedge aclk); s00_axi_bready <= 0; @(posedge aclk);
    end
endtask

task axi_read;
    input [6:0] addr; output [31:0] data;
    begin
        @(posedge aclk);
        s00_axi_araddr <= addr; s00_axi_arprot <= 0; s00_axi_arvalid <= 1; s00_axi_rready <= 1;
        wait (s00_axi_arready);
        @(posedge aclk); s00_axi_arvalid <= 0;
        wait (s00_axi_rvalid);
        data = s00_axi_rdata;
        @(posedge aclk); s00_axi_rready <= 0; @(posedge aclk);
    end
endtask

task check_eq;
    input [8*56-1:0] name; input [31:0] got; input [31:0] exp;
    begin
        if (got === exp) begin
            $display("PASS: %0s got=%0d", name, got);
            pass_cnt = pass_cnt + 1;
        end else begin
            $display("FAIL: %0s got=%0d exp=%0d", name, got, exp);
            fail_cnt = fail_cnt + 1;
        end
    end
endtask

task idle;
    input integer n; integer i;
    begin for (i = 0; i < n; i = i + 1) @(posedge aclk); end
endtask

// Handshake beat; optional stall_before ready-low cycles before accepting
task axis_beat_stall;
    input [47:0] data; input sof; input eol; input integer stall;
    integer k;
    begin
        @(posedge aclk);
        s_axis_tdata <= data; s_axis_tuser <= sof; s_axis_tlast <= eol; s_axis_tvalid <= 1;
        m_axis_tready <= 0;
        for (k = 0; k < stall; k = k + 1) @(posedge aclk);
        m_axis_tready <= 1;
        wait (s_axis_tready);
        @(posedge aclk);
        s_axis_tvalid <= 0; s_axis_tuser <= 0; s_axis_tlast <= 0;
    end
endtask

task axis_beat;
    input [47:0] data; input sof; input eol;
    begin axis_beat_stall(data, sof, eol, 0); end
endtask

// err_mask bit i => error at pixel i (row-major, COLS*ROWS)
task send_frame_mask;
    input [31:0] err_mask; input integer stall_mod;
    integer c, r, idx; reg [47:0] px; integer st;
    begin
        for (r = 0; r < ROWS; r = r + 1) begin
            for (c = 0; c < COLS; c = c + 1) begin
                idx = r * COLS + c;
                if (err_mask[idx])
                    px = pack_px(24'h101010, 24'h808080);
                else
                    px = pack_px(24'h101010, 24'h101010);
                st = (stall_mod != 0 && ((idx % stall_mod) == 0)) ? 2 : 0;
                axis_beat_stall(px, (r == 0 && c == 0), (c == COLS - 1), st);
            end
        end
    end
endtask

task cfg_basic;
    begin
        axi_write(7'h28, 32'd0);          // TH=0
        axi_write(7'h3C, 32'h00FF0000);   // ignore far
        axi_write(7'h5C, 32'd1);          // thr=1
        axi_write(7'h0C, 32'd1);          // enable
        idle(4);
    end
endtask

initial begin
    pass_cnt = 0; fail_cnt = 0;
    aresetn = 0;
    s_axis_tvalid = 0; s_axis_tdata = 0; s_axis_tlast = 0; s_axis_tuser = 0;
    m_axis_tready = 1;
    s00_axi_awaddr = 0; s00_axi_awprot = 0; s00_axi_awvalid = 0;
    s00_axi_wdata = 0; s00_axi_wstrb = 0; s00_axi_wvalid = 0; s00_axi_bready = 0;
    s00_axi_araddr = 0; s00_axi_arprot = 0; s00_axi_arvalid = 0; s00_axi_rready = 0;

    repeat (8) @(posedge aclk);
    aresetn = 1; idle(4);
    cfg_basic();

    // ============================================================
    // 约定 (v2.22): IRQ 仅在下一帧 SOF(frame_end) 产生；其后 TOTAL=刚结束帧的错误数
    // ============================================================
    $display("==== S1: SOF pixel is error (thr=1) ====");
    irq_before = irq_edges;
    send_frame_mask(32'h0000_0001, 0);
    idle(4);
    check_eq("S1 no IRQ before next SOF", irq_edges - irq_before, 32'd0);
    send_frame_mask(32'h0, 0); // frame_end IRQ
    idle(4);
    check_eq("S1 IRQ at frame_end", irq_edges - irq_before, 32'd1);
    axi_read(7'h60, rdata);
    check_eq("S1 TOTAL=1 at IRQ", rdata, 32'd1);
    axi_read(7'h2C, rdata);
    check_eq("S1 ERR_COL sof", rdata, 32'd0);
    axi_read(7'h30, rdata);
    check_eq("S1 ERR_LINE sof", rdata, 32'd0);

    // ============================================================
    $display("==== S2: last pixel of frame is only error ====");
    axi_write(7'h10, 32'd1); idle(2);
    irq_before = irq_edges;
    send_frame_mask(32'h8000_0000, 0);
    idle(4);
    check_eq("S2 no IRQ before next SOF", irq_edges - irq_before, 32'd0);
    // ERR_COL/LINE 为 frame_end 快照，须在 IRQ SOF 后读
    send_frame_mask(32'h0, 0);
    idle(4);
    check_eq("S2 IRQ at frame_end", irq_edges - irq_before, 32'd1);
    axi_read(7'h2C, rdata);
    check_eq("S2 ERR_COL last", rdata, 32'd7);
    axi_read(7'h30, rdata);
    check_eq("S2 ERR_LINE last", rdata, 32'd3);
    axi_read(7'h60, rdata);
    check_eq("S2 TOTAL=1", rdata, 32'd1);

    // ============================================================
    $display("==== S3: backpressure stalls mid-frame ====");
    axi_write(7'h10, 32'd1); idle(2);
    irq_before = irq_edges;
    send_frame_mask(32'h0000_0008, 3);
    idle(2);
    send_frame_mask(32'h0, 0);
    idle(4);
    check_eq("S3 IRQ with stalls", irq_edges - irq_before, 32'd1);
    axi_read(7'h60, rdata);
    check_eq("S3 TOTAL still 1", rdata, 32'd1);

    // ============================================================
    $display("==== S4: single-pixel frame (SOF+EOL same beat) ====");
    axi_write(7'h10, 32'd1); idle(2);
    irq_before = irq_edges;
    axis_beat(pack_px(24'h101010, 24'h808080), 1, 1);
    idle(4);
    check_eq("S4 no IRQ until next SOF", irq_edges - irq_before, 32'd0);
    axis_beat(pack_px(24'h101010, 24'h101010), 1, 1); // clean SOF -> IRQ
    idle(4);
    check_eq("S4 IRQ 1px frame", irq_edges - irq_before, 32'd1);
    axi_read(7'h60, rdata);
    check_eq("S4 TOTAL after 1px", rdata, 32'd1);
    send_frame_mask(32'h0, 0);
    idle(4);

    // ============================================================
    $display("==== S5: ERR_PIXEL_CNT=0 (edge) ====");
    axi_write(7'h10, 32'd1); idle(2);
    axi_write(7'h5C, 32'd0); idle(4);
    irq_before = irq_edges;
    send_frame_mask(32'h0000_0002, 0);
    idle(2);
    send_frame_mask(32'h0, 0);
    idle(4);
    check_eq("S5 thr0 still IRQ", irq_edges - irq_before, 32'd1);
    axi_write(7'h5C, 32'd1); idle(4);

    // ============================================================
    $display("==== S6: thr larger than frame errors -> no IRQ ====");
    axi_write(7'h10, 32'd1); idle(2);
    axi_write(7'h5C, 32'd100); idle(4);
    irq_before = irq_edges;
    send_frame_mask(32'h0000_000F, 0); // 4 errors
    idle(2);
    send_frame_mask(32'h0, 0); // frame_end: 4 < 100
    idle(4);
    check_eq("S6 no IRQ thr100", irq_edges - irq_before, 32'd0);
    axi_read(7'h60, rdata);
    check_eq("S6 TOTAL latched 4", rdata, 32'd4);

    // ============================================================
    $display("==== S7: drop thr mid-frame; IRQ uses thr at frame_end ====");
    // 3 errors, thr 10->2 mid-frame -> at frame_end final=3 >= 2 -> IRQ
    axi_write(7'h5C, 32'd10); idle(4);
    irq_before = irq_edges;
    begin : mid_drop
        integer c, r, idx; reg [47:0] px;
        for (r = 0; r < ROWS; r = r + 1) begin
            for (c = 0; c < COLS; c = c + 1) begin
                idx = r * COLS + c;
                if (idx < 3)
                    px = pack_px(24'h101010, 24'h808080);
                else
                    px = pack_px(24'h101010, 24'h101010);
                axis_beat(px, (r == 0 && c == 0), (c == COLS - 1));
                if (idx == 2) begin
                    axi_write(7'h5C, 32'd2);
                    idle(4);
                end
            end
        end
    end
    idle(2);
    send_frame_mask(32'h0, 0);
    idle(4);
    check_eq("S7 IRQ at end after thr drop", irq_edges - irq_before, 32'd1);
    axi_write(7'h5C, 32'd1); idle(4);

    // ============================================================
    $display("==== S8: disable compare mid-frame ====");
    axi_write(7'h10, 32'd1); idle(2);
    irq_before = irq_edges;
    begin : mid_dis
        integer c; reg [47:0] px;
        for (c = 0; c < COLS; c = c + 1) begin
            px = pack_px(24'h101010, 24'h808080);
            axis_beat(px, (c == 0), (c == COLS - 1));
            if (c == 3) begin
                axi_write(7'h0C, 32'd0);
                idle(4);
            end
        end
        for (c = 0; c < COLS; c = c + 1) begin
            px = pack_px(24'h101010, 24'h808080);
            axis_beat(px, 0, (c == COLS - 1));
        end
        for (c = 0; c < COLS * 2; c = c + 1)
            axis_beat(pack_px(24'h101010, 24'h808080), 0, ((c % COLS) == COLS - 1));
    end
    idle(2);
    axi_write(7'h0C, 32'd1); idle(4);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S8 IRQ for pre-disable errs", irq_edges - irq_before, 32'd1);
    axi_read(7'h60, rdata);
    check_eq("S8 TOTAL only pre-disable errs", rdata, 32'd4);

    // ============================================================
    $display("==== S9: INTR_CLEAR mid-frame; still one IRQ at frame_end ====");
    axi_write(7'h5C, 32'd1); idle(4);
    irq_before = irq_edges;
    begin : clear_mid
        integer c; reg [47:0] px;
        px = pack_px(24'h101010, 24'h808080);
        axis_beat(px, 1, 0);
        idle(2);
        // mid-frame: no IRQ yet, intr should be 0
        check_eq("S9 no intr mid-frame", {31'd0, intr}, 32'd0);
        axi_write(7'h10, 32'd1);
        idle(4);
        for (c = 1; c < COLS; c = c + 1) begin
            px = pack_px(24'h101010, 24'h808080);
            axis_beat(px, 0, (c == COLS - 1));
        end
        for (c = 0; c < COLS * (ROWS - 1); c = c + 1)
            axis_beat(pack_px(24'h101010, 24'h808080), 0, ((c % COLS) == COLS - 1));
    end
    idle(2);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S9 one IRQ at frame_end", irq_edges - irq_before, 32'd1);

    // ============================================================
    $display("==== S10: consecutive error frames (N frames 1 IRQ each) ====");
    axi_write(7'h10, 32'd1); idle(2);
    irq_before = irq_edges;
    // 每帧首错列不同，验证 IRQ 快照 ERR_COL 随帧更新（非卡在第一帧）
    begin : burst
        integer f;
        send_frame_mask(32'h0000_0002, 0); // col1；尚无 IRQ
        for (f = 1; f < 5; f = f + 1) begin
            send_frame_mask(32'h1 << (f + 1), 0); // 触发上一帧 IRQ
            idle(2);
            axi_read(7'h2C, rdata);
            check_eq("S10 ERR_COL per-frame", rdata, f);
        end
        send_frame_mask(32'h0, 0); // last frame_end for 5th error frame
        idle(2);
        axi_read(7'h2C, rdata);
        check_eq("S10 ERR_COL frame5", rdata, 32'd5);
    end
    idle(4);
    check_eq("S10 5 frames 5 IRQs", irq_edges - irq_before, 32'd5);

    // ============================================================
    $display("==== S11: all pixels error, thr=1, one IRQ, TOTAL=full ====");
    axi_write(7'h10, 32'd1); idle(2);
    irq_before = irq_edges;
    send_frame_mask(32'hFFFF_FFFF, 0);
    idle(2);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S11 one IRQ all-err", irq_edges - irq_before, 32'd1);
    axi_read(7'h60, rdata);
    check_eq("S11 TOTAL full frame", rdata, 32'd32);

    // ============================================================
    $display("==== S12: ignore colour exact match TH=0 ====");
    axi_write(7'h10, 32'd1); idle(2);
    axi_write(7'h3C, 32'h00808080); idle(4);
    irq_before = irq_edges;
    send_frame_mask(32'h0000_00FF, 0);
    idle(2);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S12 ignore no IRQ", irq_edges - irq_before, 32'd0);
    axi_read(7'h60, rdata);
    check_eq("S12 ignore TOTAL 0", rdata, 32'd0);
    axi_write(7'h3C, 32'h00FF0000); idle(4);

    // ============================================================
    $display("==== S13: TH boundary |diff|==TH not error; |diff|>TH is ====");
    axi_write(7'h28, 32'd16); idle(4);
    irq_before = irq_edges;
    begin : th_bound
        integer c;
        axis_beat(pack_px(24'h101010, 24'h202020), 1, 0);
        for (c = 1; c < COLS; c = c + 1)
            axis_beat(pack_px(24'h101010, 24'h101010), 0, (c == COLS - 1));
        for (c = 0; c < COLS * (ROWS - 1); c = c + 1)
            axis_beat(pack_px(24'h101010, 24'h101010), 0, ((c % COLS) == COLS - 1));
    end
    idle(2);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S13a eq TH no IRQ", irq_edges - irq_before, 32'd0);
    irq_before = irq_edges;
    begin : th_over
        integer c;
        axis_beat(pack_px(24'h101010, 24'h212121), 1, 0);
        for (c = 1; c < COLS; c = c + 1)
            axis_beat(pack_px(24'h101010, 24'h101010), 0, (c == COLS - 1));
        for (c = 0; c < COLS * (ROWS - 1); c = c + 1)
            axis_beat(pack_px(24'h101010, 24'h101010), 0, ((c % COLS) == COLS - 1));
    end
    idle(2);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S13b gt TH IRQ", irq_edges - irq_before, 32'd1);
    axi_write(7'h28, 32'd0); idle(4);

    // ============================================================
    $display("==== S14: tuser held high without xfer then late SOF xfer ====");
    axi_write(7'h10, 32'd1); idle(2);
    axi_write(7'h5C, 32'd1); idle(2);
    send_frame_mask(32'h0000_007F, 0); idle(2); // 7 errors
    @(posedge aclk);
    s_axis_tuser <= 1; s_axis_tvalid <= 0; m_axis_tready <= 1;
    idle(5);
    s_axis_tuser <= 0;
    idle(2);
    begin : late_sof
        integer c, r; reg [47:0] px;
        for (r = 0; r < ROWS; r = r + 1)
            for (c = 0; c < COLS; c = c + 1) begin
                px = pack_px(24'h101010, 24'h101010);
                axis_beat(px, (r == 0 && c == 0), (c == COLS - 1));
            end
    end
    idle(4);
    axi_read(7'h60, rdata);
    if (rdata !== 32'd7) begin
        $display("BUG_FOUND: S14 idle-tuser ate frame_end; TOTAL got=%0d exp=7", rdata);
        $display("  root: frame_end not gated by axis_xfer (tuser sideband while !tvalid)");
    end
    check_eq("S14 TOTAL after idle-tuser glitch", rdata, 32'd7);

    // ============================================================
    $display("==== S15: STATUS bit1 sticky vs INTR_CLEAR ====");
    axi_write(7'h10, 32'd1); idle(2);
    axi_write(7'h5C, 32'd1); idle(4);
    send_frame_mask(32'h0000_0004, 0); idle(4);
    axi_read(7'h0C, rdata);
    check_eq("S15 STATUS err bit set", rdata[1], 1'b1);
    axi_write(7'h10, 32'd1); idle(4);
    axi_read(7'h0C, rdata);
    check_eq("S15 STATUS bit1 after CLEAR", rdata[1], 1'b1);
    send_frame_mask(32'h0, 0); idle(4); // IRQ SOF; then need another clean to clear flag?
    // After IRQ SOF with thr_met, flag stays 1; next clean SOF clears
    send_frame_mask(32'h0, 0); idle(4);
    axi_read(7'h0C, rdata);
    check_eq("S15 STATUS bit1 cleared after clean SOF", rdata[1], 1'b0);

    // ============================================================
    $display("==== S16: SOF beat then tvalid gap then rest of frame ====");
    axi_write(7'h10, 32'd1); idle(2);
    axi_write(7'h5C, 32'd1); idle(4);

    irq_before = irq_edges;
    begin : sof_gap_mid_err
        integer c, r, idx; reg [47:0] px;
        axis_beat(pack_px(24'h101010, 24'h101010), 1, 0);
        s_axis_tvalid <= 0;
        s_axis_tuser  <= 0;
        s_axis_tlast  <= 0;
        idle(8);
        for (r = 0; r < ROWS; r = r + 1) begin
            for (c = 0; c < COLS; c = c + 1) begin
                idx = r * COLS + c;
                if (idx != 0) begin
                    if ((idx == 2) || (idx == 5) || (idx == 10))
                        px = pack_px(24'h101010, 24'h808080);
                    else
                        px = pack_px(24'h101010, 24'h101010);
                    axis_beat(px, 0, (c == COLS - 1));
                end
            end
        end
    end
    idle(2);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S16a IRQ after gap mid-err", irq_edges - irq_before, 32'd1);
    axi_read(7'h2C, rdata);
    check_eq("S16a ERR_COL first mid err", rdata, 32'd2);
    axi_read(7'h30, rdata);
    check_eq("S16a ERR_LINE first mid err", rdata, 32'd0);
    axi_read(7'h60, rdata);
    check_eq("S16a TOTAL latched 3", rdata, 32'd3);

    axi_write(7'h10, 32'd1); idle(2);
    irq_before = irq_edges;
    begin : sof_gap_sof_err
        integer c, r, idx; reg [47:0] px;
        axis_beat(pack_px(24'h101010, 24'h808080), 1, 0);
        s_axis_tvalid <= 0;
        s_axis_tuser  <= 0;
        s_axis_tlast  <= 0;
        idle(6);
        for (r = 0; r < ROWS; r = r + 1) begin
            for (c = 0; c < COLS; c = c + 1) begin
                idx = r * COLS + c;
                if (idx != 0) begin
                    if (idx == 7)
                        px = pack_px(24'h101010, 24'h808080);
                    else
                        px = pack_px(24'h101010, 24'h101010);
                    axis_beat(px, 0, (c == COLS - 1));
                end
            end
        end
    end
    idle(2);
    send_frame_mask(32'h0, 0); idle(4);
    check_eq("S16b IRQ on SOF err", irq_edges - irq_before, 32'd1);
    axi_read(7'h2C, rdata);
    check_eq("S16b ERR_COL sof", rdata, 32'd0);
    axi_read(7'h30, rdata);
    check_eq("S16b ERR_LINE sof", rdata, 32'd0);
    axi_read(7'h60, rdata);
    check_eq("S16b TOTAL latched 2", rdata, 32'd2);

    $display("========================================");
    $display("STRESS RESULT: PASS=%0d FAIL=%0d", pass_cnt, fail_cnt);
    if (fail_cnt == 0) $display("TB_PASS");
    else $display("TB_FAIL");
    $finish;
end

initial begin
    #2_000_000;
    $display("FAIL: stress timeout");
    $display("TB_FAIL");
    $finish;
end

endmodule
