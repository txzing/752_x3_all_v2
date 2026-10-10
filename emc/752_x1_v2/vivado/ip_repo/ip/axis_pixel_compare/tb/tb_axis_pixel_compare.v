// tb_axis_pixel_compare.v
// Smoke TB: ERR_PIXEL_CNT threshold IRQ + ERR_PIXEL_CNT_TOTAL accumulate
`timescale 1ns / 1ps

module tb_axis_pixel_compare;

localparam integer CLK_NS = 10;
localparam integer WIDTH  = 48;

reg aclk;
reg aresetn;
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

integer pass_cnt;
integer fail_cnt;
integer irq_edges;
reg     intr_d;

// Pack RBG halves: {ref[23:0], video[23:0]}
function [47:0] pack_px;
    input [23:0] ref_rgb;
    input [23:0] vid_rgb;
    begin
        pack_px = {ref_rgb, vid_rgb};
    end
endfunction

axis_pixel_compare #(
    .WIDTH(WIDTH),
    .PPC(1),
    .TUSER_WIDTH(1),
    .FREQ_HZ(1000),
    .ROI_RST_XS(0),
    .ROI_RST_XE(7),
    .ROI_RST_YS(0),
    .ROI_RST_YE(3)
) dut (
    .aclk(aclk),
    .aresetn(aresetn),
    .intr(intr),
    .s_axis_tvalid(s_axis_tvalid),
    .s_axis_tready(s_axis_tready),
    .s_axis_tdata(s_axis_tdata),
    .s_axis_tlast(s_axis_tlast),
    .s_axis_tuser(s_axis_tuser),
    .m_axis_tvalid(m_axis_tvalid),
    .m_axis_tready(m_axis_tready),
    .m_axis_tdata(m_axis_tdata),
    .m_axis_tlast(m_axis_tlast),
    .m_axis_tuser(m_axis_tuser),
    .s00_axi_aclk(aclk),
    .s00_axi_aresetn(aresetn),
    .s00_axi_awaddr(s00_axi_awaddr),
    .s00_axi_awprot(s00_axi_awprot),
    .s00_axi_awvalid(s00_axi_awvalid),
    .s00_axi_awready(s00_axi_awready),
    .s00_axi_wdata(s00_axi_wdata),
    .s00_axi_wstrb(s00_axi_wstrb),
    .s00_axi_wvalid(s00_axi_wvalid),
    .s00_axi_wready(s00_axi_wready),
    .s00_axi_bresp(s00_axi_bresp),
    .s00_axi_bvalid(s00_axi_bvalid),
    .s00_axi_bready(s00_axi_bready),
    .s00_axi_araddr(s00_axi_araddr),
    .s00_axi_arprot(s00_axi_arprot),
    .s00_axi_arvalid(s00_axi_arvalid),
    .s00_axi_arready(s00_axi_arready),
    .s00_axi_rdata(s00_axi_rdata),
    .s00_axi_rresp(s00_axi_rresp),
    .s00_axi_rvalid(s00_axi_rvalid),
    .s00_axi_rready(s00_axi_rready)
);

// Clock
initial begin
    aclk = 1'b0;
    forever #(CLK_NS/2) aclk = ~aclk;
end

// Count rising edges of intr
always @(posedge aclk) begin
    if (!aresetn) begin
        intr_d    <= 1'b0;
        irq_edges <= 0;
    end else begin
        if (intr && !intr_d)
            irq_edges <= irq_edges + 1;
        intr_d <= intr;
    end
end

task axi_write;
    input [6:0] addr;
    input [31:0] data;
    begin
        @(posedge aclk);
        s00_axi_awaddr  <= addr;
        s00_axi_awprot  <= 3'b0;
        s00_axi_awvalid <= 1'b1;
        s00_axi_wdata   <= data;
        s00_axi_wstrb   <= 4'hF;
        s00_axi_wvalid  <= 1'b1;
        s00_axi_bready  <= 1'b1;
        wait (s00_axi_awready && s00_axi_wready);
        @(posedge aclk);
        s00_axi_awvalid <= 1'b0;
        s00_axi_wvalid  <= 1'b0;
        wait (s00_axi_bvalid);
        @(posedge aclk);
        s00_axi_bready  <= 1'b0;
        @(posedge aclk);
    end
endtask

task axi_read;
    input  [6:0]  addr;
    output [31:0] data;
    begin
        @(posedge aclk);
        s00_axi_araddr  <= addr;
        s00_axi_arprot  <= 3'b0;
        s00_axi_arvalid <= 1'b1;
        s00_axi_rready  <= 1'b1;
        wait (s00_axi_arready);
        @(posedge aclk);
        s00_axi_arvalid <= 1'b0;
        wait (s00_axi_rvalid);
        data = s00_axi_rdata;
        @(posedge aclk);
        s00_axi_rready  <= 1'b0;
        @(posedge aclk);
    end
endtask

task check_eq;
    input [8*48-1:0] name; // up to 48 chars (avoid Verilog string truncate)
    input [31:0]     got;
    input [31:0]     exp;
    begin
        if (got === exp) begin
            $display("PASS: %0s got=0x%0h (%0d)", name, got, got);
            pass_cnt = pass_cnt + 1;
        end else begin
            $display("FAIL: %0s got=0x%0h exp=0x%0h", name, got, exp);
            fail_cnt = fail_cnt + 1;
        end
    end
endtask

// Drive one AXI-Stream beat (handshake)
task axis_beat;
    input [47:0] data;
    input        sof;
    input        eol;
    begin
        @(posedge aclk);
        s_axis_tdata  <= data;
        s_axis_tuser  <= sof;
        s_axis_tlast  <= eol;
        s_axis_tvalid <= 1'b1;
        wait (s_axis_tready);
        @(posedge aclk);
        s_axis_tvalid <= 1'b0;
        s_axis_tuser  <= 1'b0;
        s_axis_tlast  <= 1'b0;
    end
endtask

// Send one small frame: 8 cols x 2 rows. err_mask[i]=1 inserts error at pixel i (row-major).
task send_frame_with_errors;
    input [15:0] err_mask; // bit0 = first pixel after SOF
    integer c, r, idx;
    reg [47:0] px;
    reg        is_err;
    begin
        for (r = 0; r < 2; r = r + 1) begin
            for (c = 0; c < 8; c = c + 1) begin
                idx = r * 8 + c;
                is_err = err_mask[idx];
                if (is_err)
                    px = pack_px(24'h101010, 24'h808080); // |diff|=0x70 > TH=0
                else
                    px = pack_px(24'h101010, 24'h101010);
                axis_beat(px, (r == 0 && c == 0), (c == 7));
            end
        end
    end
endtask

task idle_cycles;
    input integer n;
    integer i;
    begin
        for (i = 0; i < n; i = i + 1)
            @(posedge aclk);
    end
endtask

reg [31:0] rdata;
integer irq_before;

initial begin
    pass_cnt = 0;
    fail_cnt = 0;
    aresetn = 1'b0;
    s_axis_tvalid = 1'b0;
    s_axis_tdata  = 48'd0;
    s_axis_tlast  = 1'b0;
    s_axis_tuser  = 1'b0;
    m_axis_tready = 1'b1;
    s00_axi_awaddr = 7'd0;
    s00_axi_awprot = 3'd0;
    s00_axi_awvalid = 1'b0;
    s00_axi_wdata = 32'd0;
    s00_axi_wstrb = 4'd0;
    s00_axi_wvalid = 1'b0;
    s00_axi_bready = 1'b0;
    s00_axi_araddr = 7'd0;
    s00_axi_arprot = 3'd0;
    s00_axi_arvalid = 1'b0;
    s00_axi_rready = 1'b0;

    repeat (10) @(posedge aclk);
    aresetn = 1'b1;
    idle_cycles(5);

    // ---- Config: TH=0, NOT_PIXEL far away, enable compare ----
    axi_write(7'h28, 32'd0);          // PIXEL_THRESHOLD = 0
    axi_write(7'h3C, 32'h00FF0000);   // RGB_NOT_PIXEL != video
    axi_write(7'h0C, 32'd1);          // STATUS enable
    idle_cycles(4);                   // wait enable/threshold retiming

    // ---- Test A: thr=1；错误帧结束后下一 SOF 才 IRQ，此时 TOTAL=本帧计数 ----
    axi_read(7'h5C, rdata);
    check_eq("default ERR_PIXEL_CNT", rdata, 32'd1);

    irq_before = irq_edges;
    send_frame_with_errors(16'h0002); // 1 error mid-frame
    idle_cycles(4);
    axi_read(7'h60, rdata);
    check_eq("TOTAL still 0 before next SOF", rdata, 32'd0);
    check_eq("no IRQ before frame_end", irq_edges - irq_before, 32'd0);

    // Next SOF = frame_end of error frame: latch TOTAL=1 and IRQ
    send_frame_with_errors(16'h0000);
    idle_cycles(4);
    axi_read(7'h60, rdata);
    check_eq("TOTAL=1 at IRQ SOF", rdata, 32'd1);
    check_eq("IRQ at frame_end thr=1", irq_edges - irq_before, 32'd1);
    check_eq("intr sticky after frame_end IRQ", {31'd0, intr}, 32'd1);

    // Clean frame SOF: TOTAL=0, no new IRQ
    irq_before = irq_edges;
    send_frame_with_errors(16'h0000);
    idle_cycles(4);
    axi_read(7'h60, rdata);
    check_eq("TOTAL latched 0 after clean frame", rdata, 32'd0);
    check_eq("no IRQ on clean frame_end", irq_edges - irq_before, 32'd0);

    // ---- Test B: thr=3；帧末才判阈 ----
    axi_write(7'h10, 32'd1); // INTR_CLEAR
    idle_cycles(2);
    axi_write(7'h5C, 32'd3);
    idle_cycles(4);
    axi_read(7'h5C, rdata);
    check_eq("ERR_PIXEL_CNT write/read", rdata, 32'd3);

    irq_before = irq_edges;
    send_frame_with_errors(16'h0007); // 3 errors — no IRQ until next SOF
    idle_cycles(4);
    check_eq("no IRQ until SOF after 3-err", irq_edges - irq_before, 32'd0);

    send_frame_with_errors(16'h001F); // 5 errs; SOF IRQs for prior TOTAL=3
    idle_cycles(4);
    axi_read(7'h60, rdata);
    check_eq("TOTAL=3 at IRQ after 3-err frame", rdata, 32'd3);
    check_eq("IRQ once for completed 3-err", irq_edges - irq_before, 32'd1);

    irq_before = irq_edges;
    send_frame_with_errors(16'h0003); // 2 errs; SOF IRQs for prior TOTAL=5
    idle_cycles(4);
    axi_read(7'h60, rdata);
    check_eq("TOTAL=5 at IRQ after 5-err frame", rdata, 32'd5);
    check_eq("IRQ for completed 5-err", irq_edges - irq_before, 32'd1);

    irq_before = irq_edges;
    send_frame_with_errors(16'h0000); // clean; SOF of 2-err frame: 2<3 no IRQ, TOTAL=2
    idle_cycles(4);
    axi_read(7'h60, rdata);
    check_eq("TOTAL=2 after 2-err below thr", rdata, 32'd2);
    check_eq("no IRQ when final<thr", irq_edges - irq_before, 32'd0);

    // ================================================================
    // Regression
    // ================================================================
    $display("---- REGRESSION ----");
    axi_write(7'h10, 32'd1);
    axi_write(7'h5C, 32'd1);
    axi_write(7'h28, 32'd0);
    axi_write(7'h3C, 32'h00FF0000);
    idle_cycles(4);

    // R1: multi error -> one IRQ at next SOF; HOLD/coords kept
    irq_before = irq_edges;
    send_frame_with_errors(16'h00FF);
    idle_cycles(2);
    send_frame_with_errors(16'h0000); // frame_end IRQ
    idle_cycles(4);
    check_eq("legacy multi-err one IRQ", irq_edges - irq_before, 32'd1);
    axi_read(7'h60, rdata);
    check_eq("legacy TOTAL=8 at IRQ", rdata, 32'd8);
    axi_read(7'h2C, rdata);
    check_eq("ERR_COL first err col0", rdata, 32'd0);
    axi_read(7'h30, rdata);
    check_eq("ERR_LINE first err row0", rdata, 32'd0);
    axi_read(7'h24, rdata);
    check_eq("STREAM_HOLD first err vid", rdata, 32'h00808080);

    // R2: INTR_CLEAR clears sticky intr
    check_eq("intr before CLEAR", {31'd0, intr}, 32'd1);
    axi_write(7'h10, 32'd1);
    idle_cycles(4);
    check_eq("intr after CLEAR", {31'd0, intr}, 32'd0);
    axi_read(7'h24, rdata);
    check_eq("HOLD cleared by INTR_CLEAR", rdata, 32'd0);

    // R3: ignore colour
    send_frame_with_errors(16'h0000);
    idle_cycles(2);
    axi_write(7'h3C, 32'h00808080);
    idle_cycles(4);
    irq_before = irq_edges;
    send_frame_with_errors(16'h000F);
    idle_cycles(2);
    send_frame_with_errors(16'h0000);
    idle_cycles(4);
    check_eq("ignore colour no IRQ", irq_edges - irq_before, 32'd0);
    axi_read(7'h60, rdata);
    check_eq("ignore colour TOTAL latched 0", rdata, 32'd0);

    // R4: AXIS passthrough (combinational assign s->m)
    axi_write(7'h3C, 32'h00FF0000);
    idle_cycles(2);
    begin : passthrough
        reg [47:0] px;
        px = pack_px(24'hAABBCC, 24'h112233);
        @(posedge aclk);
        s_axis_tdata  <= px;
        s_axis_tuser  <= 1'b1;
        s_axis_tlast  <= 1'b1;
        s_axis_tvalid <= 1'b1;
        #1;
        check_eq("passthru tdata hi", {8'd0, m_axis_tdata[47:24]}, {8'd0, 24'hAABBCC});
        check_eq("passthru tdata lo", {8'd0, m_axis_tdata[23:0]}, {8'd0, 24'h112233});
        check_eq("passthru tuser", {31'd0, m_axis_tuser}, 32'd1);
        check_eq("passthru tlast", {31'd0, m_axis_tlast}, 32'd1);
        @(posedge aclk);
        s_axis_tvalid <= 1'b0;
        s_axis_tuser  <= 1'b0;
        s_axis_tlast  <= 1'b0;
    end
    idle_cycles(2);

    // R5: point sample at (3,1)
    axi_write(7'h50, 32'd3);          // POINT_X
    axi_write(7'h54, 32'd1);          // POINT_Y
    idle_cycles(4);
    begin : point_samp
        integer c, r;
        reg [47:0] px;
        for (r = 0; r < 2; r = r + 1) begin
            for (c = 0; c < 8; c = c + 1) begin
                if ((r == 1) && (c == 3))
                    px = pack_px(24'h000000, 24'h55AA55);
                else
                    px = pack_px(24'h101010, 24'h101010);
                axis_beat(px, (r == 0 && c == 0), (c == 7));
            end
        end
    end
    idle_cycles(4);
    axi_read(7'h58, rdata);
    check_eq("POINT_PIXEL sample", rdata, 32'h0055AA55);

    // R6: RGB_PIXEL_TOTAL SOF latch (all pixels match target in ROI)
    axi_write(7'h34, 32'h00101010);  // RGB_CNT_PIXEL
    axi_write(7'h28, 32'd0);
    axi_write(7'h0C, 32'd1);
    idle_cycles(4);
    send_frame_with_errors(16'h0000); // 16 matching pixels
    idle_cycles(2);
    send_frame_with_errors(16'h0000); // SOF latches previous RGB count
    idle_cycles(4);
    axi_read(7'h38, rdata);
    check_eq("RGB_PIXEL_TOTAL 8x2", rdata, 32'd16);

    // Summary
    $display("========================================");
    $display("RESULT: PASS=%0d FAIL=%0d", pass_cnt, fail_cnt);
    if (fail_cnt == 0)
        $display("TB_PASS");
    else
        $display("TB_FAIL");
    $finish;
end

// Safety timeout
initial begin
    #500000;
    $display("FAIL: timeout");
    $display("TB_FAIL");
    $finish;
end

endmodule
