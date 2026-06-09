`timescale 1ns/1ps
//==============================================================================
// Module: axis_pixel_compare (frame-diff / per-pixel compare)
// Version: 2.19 (must match spirit:version in component.xml and OPTION VERSION in
//           drivers/axis_pixel_compare_v1_0/data/axis_pixel_compare.mdd)
// Last updated: 2026-06-02
//
// Version history (summary):
//   2026-04-21 2.x   RGB hit-count and mask colour via AXI-Lite
//   2026-04-28 2.x   Stream-valid / fps gate updates
//   2026-05-13 2.5   PPC; stat_* lite regs for COL/LINE/FPS readback; intr_hold legacy 752_x1
//   2026-05-19 2.8   Frame-rate monitor reg; DEBUG attributes on key signals
//   2026-05-22 2.9   Fix multiple IRQ asserts within one frame (KU040)
//   2026-05-22 2.9   error_detected pulse; intr_clear clears error holds
//   2026-06-01 2.9   ROI slv_reg16..19 reset via C_RST_ROI_*; ROI_RST_* IP params
//   2026-06-01 2.9   RGB stats: ROI + |video-RGB_CNT_PIXEL|<=TH (no mask skip)
//   2026-06-01 2.9   Ignore band: |video-RGB_NOT_PIXEL|<=TH per ch skips frame-diff
//   2026-06-01 2.9   Point sample slv_reg20/21/22; RTL comments English; GB2312 sources
//   2026-06-02 2.19  Current RTL (see Behaviour summary below). Versions 2.10-2.18
//                    documented a pipelined RGB-stat path that is not in this netlist.
//
// Behaviour summary (matches current netlist):
// - Frame compare: |ref-video|>TH on any channel -> first error + IRQ;
//   |video-RGB_NOT_PIXEL|<=TH per channel -> skip diff (is_not_pixel).
// - RGB stats: rgb_stat_pixel combinational; SOF (axis_xfer && tuser) latches rgb_pixle_total,
//   seeds rgb_pixle_total_cnt to 1 or 0; SOF beat roi_pix_y forced to 0 (L406).
// - ROI: live roi_*_r from AXI each aclk; illegal box (xe<xs or ye<ys) -> in_rgb_roi full frame.
// - Point sample: point_pixel_latched on axis_xfer when roi_pix_x/y match POINT_X/Y.
// - Lite enable/threshold/RGB targets re-registered on aclk (1 cycle vs slv_reg write).
// - line_cnt: SOF (m_axis_tuser) before EOL (tlast); same-beat SOF+tlast resets line only.
// - frame_end = ~prev_tuser & m_axis_tuser[0] (SOF edge): fps tick, irq/error frame clear.
// - ERR_COL/ERR_LINE: latched on first-error beat (same cycle as frame_error_flag);
//   0-based col_cnt; err_line=0 on SOF beat else line_cnt; cleared on SOF or INTR_CLEAR coords.
// - frame_error_flag until SOF on intr_level; INTR_CLEAR clears holds/intr, not STATUS bit1.
// - stat_*_lite: 1-cycle registered COL/LINE/FPS/ERR_* for AXI read timing.
//==============================================================================
module axis_pixel_compare
#
(
    parameter integer WIDTH = 48,
    parameter integer PPC = 1,            // pixels per beat: 1,2,4,8
    parameter integer TUSER_WIDTH = 1,
    parameter integer FREQ_HZ = 100000000,
    // ROI AXI slv_reg16..19 power-on reset (0-based inclusive); use 16'hFFFF for xe/ye = full span.
    parameter integer  ROI_RST_XS = 0,
    parameter integer  ROI_RST_XE = 99,
    parameter integer  ROI_RST_YS = 0,
    parameter integer  ROI_RST_YE = 99,
    // Parameters of Axi Slave Bus Interface S00_AXI
    parameter integer C_S00_AXI_DATA_WIDTH = 32,
    parameter integer C_S00_AXI_ADDR_WIDTH = 7
)
 (
    input aclk,
    input aresetn,
    
    output intr,
    
    //
    (* DONT_TOUCH = "yes", s="true",keep="true" *) (*MARK_DEBUG="TRUE"*)input s_axis_tvalid,
    (* DONT_TOUCH = "yes", s="true",keep="true" *) (*MARK_DEBUG="TRUE"*)output s_axis_tready,
    (* DONT_TOUCH = "yes", s="true",keep="true" *) (*MARK_DEBUG="TRUE"*)input [WIDTH-1:0] s_axis_tdata,
    (* DONT_TOUCH = "yes", s="true",keep="true" *) (*MARK_DEBUG="TRUE"*)input s_axis_tlast,
    (* DONT_TOUCH = "yes", s="true",keep="true" *) (*MARK_DEBUG="TRUE"*)input [TUSER_WIDTH-1:0]s_axis_tuser,
    //
    output m_axis_tvalid,
    input m_axis_tready,
    output [WIDTH-1:0] m_axis_tdata,
    output m_axis_tlast,
    output [TUSER_WIDTH-1:0]m_axis_tuser,
    
    // Ports of Axi Slave Bus Interface S00_AXI
    input s00_axi_aclk,
    input s00_axi_aresetn,
    input [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
    input [2 : 0] s00_axi_awprot,
    input s00_axi_awvalid,
    output s00_axi_awready,
    input [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
    input [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
    input s00_axi_wvalid,
    output s00_axi_wready,
    output [1 : 0] s00_axi_bresp,
    output s00_axi_bvalid,
    input s00_axi_bready,
    input [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
    input [2 : 0] s00_axi_arprot,
    input s00_axi_arvalid,
    output s00_axi_arready,
    output [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
    output [1 : 0] s00_axi_rresp,
    output s00_axi_rvalid,
    input s00_axi_rready
);

//
assign m_axis_tvalid = s_axis_tvalid;
assign s_axis_tready = m_axis_tready;
assign m_axis_tdata = s_axis_tdata;
assign m_axis_tuser = s_axis_tuser;
assign m_axis_tlast = s_axis_tlast;

wire axis_xfer;
assign axis_xfer = s_axis_tvalid & m_axis_tready;

// Forward declarations (intr/err blocks below; ModelSim requires declare-before-use).
reg        prev_tuser;
wire       frame_end;
reg [15:0] col;
reg [15:0] col_cnt;
reg [15:0] line;
reg [15:0] line_cnt;

// Same convention as lite column width: pixel column index advances by PPC per AXI beat.
localparam integer COL_PIX_SHIFT = (PPC == 8) ? 3 : (PPC == 4) ? 2 : (PPC == 2) ? 1 : 0;

reg [WIDTH-1:0] error_data_hold;
reg [WIDTH-1:0] stream_in_data_hold;
reg [31:0] fps_total_cnt;
reg error_detected;

reg intr_hold;
wire axis_compare_enable;
wire intr_clear;
assign intr = intr_hold;

// S00 AXI outputs sampled on aclk (shorten comb paths; assume s00_axi_aclk related to aclk).
reg        axis_compare_enable_q;
reg        intr_clear_q;
reg [7:0]  pixel_threshold_q;
reg [23:0] rgb_target_q;
reg [23:0] rgb_mask_q;

/* Legacy (752_x1): intr_hold cleared by raw intr_clear for one S_AXI clock; set by
 * error_detected && axis_compare_enable (not _q). Matches PS IRQ + software INTR_CLEAR. */
always@(posedge aclk)
begin
    if (!aresetn)
        intr_hold <= 1'b0;
    else if (intr_clear_q)
        intr_hold <= 1'b0;
    else if (error_detected && axis_compare_enable_q)
    begin
        intr_hold <= 1'b1;
    end
    else if (frame_end)
        intr_hold <= 1'b0; // clear at next-frame SOF (frame_end = tuser rising edge)
end

wire [23:0] w_axi_rgb_target;
wire [23:0] w_axi_rgb_mask;

wire pixel_compare_valid;

wire [7:0] pixel_threshold;
// difference threshold per channel

// reference pixels (upper half-word)
wire [7:0] pixel_ref_R = s_axis_tdata[47:40];
wire [7:0] pixel_ref_B = s_axis_tdata[39:32];
wire [7:0] pixel_ref_G = s_axis_tdata[31:24];

// incoming pixels (RBG packing on lower half-word)
wire [7:0] pixel_cmp_R = s_axis_tdata[23:16];
wire [7:0] pixel_cmp_B = s_axis_tdata[15:8];
wire [7:0] pixel_cmp_G = s_axis_tdata[7:0];

// ignore band: |video-RGB_NOT_PIXEL|<=TH per channel -> skip frame-diff (no error)
wire [7:0] mask_R = rgb_mask_q[23:16];
wire [7:0] mask_B = rgb_mask_q[15:8];
wire [7:0] mask_G = rgb_mask_q[7:0];
wire [7:0] mask_dR = (pixel_cmp_R > mask_R) ? (pixel_cmp_R - mask_R) : (mask_R - pixel_cmp_R);
wire [7:0] mask_dB = (pixel_cmp_B > mask_B) ? (pixel_cmp_B - mask_B) : (mask_B - pixel_cmp_B);
wire [7:0] mask_dG = (pixel_cmp_G > mask_G) ? (pixel_cmp_G - mask_G) : (mask_G - pixel_cmp_G);
wire is_not_pixel = (mask_dR <= pixel_threshold_q) &&
                    (mask_dB <= pixel_threshold_q) &&
                    (mask_dG <= pixel_threshold_q);

assign pixel_compare_valid =
        axis_compare_enable_q &&
        (!is_not_pixel) &&
        axis_xfer;

wire [7:0] diff_R = (pixel_ref_R > pixel_cmp_R) ? (pixel_ref_R - pixel_cmp_R) : (pixel_cmp_R - pixel_ref_R);
wire [7:0] diff_G = (pixel_ref_G > pixel_cmp_G) ? (pixel_ref_G - pixel_cmp_G) : (pixel_cmp_G - pixel_ref_G);
wire [7:0] diff_B = (pixel_ref_B > pixel_cmp_B) ? (pixel_ref_B - pixel_cmp_B) : (pixel_cmp_B - pixel_ref_B);

// error if any channel exceeds threshold (ref vs incoming)
wire pixel_error_detected = (diff_R > pixel_threshold_q) ||
                            (diff_G > pixel_threshold_q) ||
                            (diff_B > pixel_threshold_q);

reg frame_error_flag;
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        error_detected <= 1'b0;
        frame_error_flag <= 1'b0;
        stream_in_data_hold <= 'd0;
        error_data_hold <= 'd0;
    end
    else
    begin

        if (pixel_compare_valid && pixel_error_detected && !frame_error_flag) // first error in this frame only
        begin
            error_detected <= 1'b1;
            frame_error_flag <= 1'b1;   // latch: frame already saw an error
            stream_in_data_hold <= s_axis_tdata[23:0];
            error_data_hold <= s_axis_tdata[47:24];
        end
        else if(frame_end)
        begin
            error_detected <= 1'b0;
            frame_error_flag <= 1'b0;   // clear at next-frame SOF (frame_end)
        end
        else if (intr_clear_q)
        begin
            stream_in_data_hold <= 'd0;
            error_data_hold <= 'd0;
            error_detected <= 1'b0;
        end
        else
        begin
            error_detected <= 1'b0;
        end
    end
end

reg [15:0] err_col;
reg [15:0] err_line;
reg err_latched;

always@(posedge aclk)
begin
    if(!aresetn)
    begin
        err_col <= 'b0;
        err_line <= 'b0;
        err_latched <= 1'b0;
    end
    else if (pixel_compare_valid && pixel_error_detected && !err_latched)
    begin
        err_col <= col_cnt;
        err_line <= m_axis_tuser[0] ? 16'd0 : line_cnt;
        err_latched <= 1'b1;
    end
    else if (frame_end)
    begin
        err_latched <= 1'b0;
        err_col <= 'b0;
        err_line <= 'b0;
    end
    else if (intr_clear_q)
    begin
        err_col <= 'b0;
        err_line <= 'b0;
    end
end

reg [31:0] freq_sec_cnt;
reg freq_sec_flag;
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        freq_sec_cnt <= 32'd0;
        freq_sec_flag <= 1'b0;
    end
    else
    begin
        if(freq_sec_cnt < FREQ_HZ)
        begin
            freq_sec_cnt <= freq_sec_cnt + 1'b1;
            freq_sec_flag <= 1'b0;
        end
        else
        begin
            freq_sec_cnt <= 32'd0;
            freq_sec_flag <= 1'b1;
        end
    end
end

reg [31:0] fps;
(* DONT_TOUCH = "yes", s="true",keep="true" *) (*MARK_DEBUG="TRUE"*)reg [31:0] fps_cnt;
reg stream_invalid;

// frame_end: rising edge of m_axis_tuser[0] (start of next frame), not EOL/tlast
assign frame_end = ~prev_tuser & m_axis_tuser[0];

always@(posedge aclk)
begin
    if(!aresetn)
    begin
        prev_tuser <= 1'b0;
        fps <= 32'd0;
        fps_cnt <= 32'd0;
        fps_total_cnt <= 32'd0;
        stream_invalid <= 1'b0;
    end
    else
    begin
        prev_tuser <= m_axis_tuser[0];
        if(freq_sec_flag)
        begin
            if(fps_cnt<32'd3)
            begin
                stream_invalid <= 1'b1;
            end
            else
            begin
                stream_invalid <= 1'b0;
            end
            fps <= fps_cnt;
            fps_cnt <= 32'd0;
        end
        else
        begin
            if(frame_end)
            begin
               fps_cnt <= fps_cnt +1'b1; 
               fps_total_cnt <= fps_total_cnt +1'b1;        
            end
        end
    end
end


//
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        col <= 16'b0;
        col_cnt <= 16'b0;
    end
    else
    begin
        if(axis_xfer && s_axis_tlast)
        begin
            col <= col_cnt + 1'b1;
            col_cnt <= 16'b0;
        end
        else if(axis_xfer)
        begin
            col_cnt <= col_cnt + 1'b1;
        end
    end
end

always@(posedge aclk)
begin
    if(!aresetn)
    begin
        line <= 16'b0;
        line_cnt <= 16'b0;
    end
    else
    begin
        // SOF (tuser) before line-end (tlast): same-beat SOF+tlast must reset frame, not bump line_cnt.
        if(axis_xfer && (m_axis_tuser[0] == 1'b1))
        begin
            line <= line_cnt;
            line_cnt <= 16'b0;
        end
        else if(axis_xfer && s_axis_tlast)
        begin
            line_cnt <= line_cnt + 1'b1;
        end
    end
end

// RGB 命中统计 ROI: slv_reg16..19 为 16 位, 闭区间 [x_start,x_end]x[y_start,y_end].
// roi_pix_x / roi_pix_y 与 col_cnt, line_cnt 一致: 从 0 起的像素列/行下标 (见 COL_PIX_SHIFT).
// 因此 Lite 应写入与计数器同一套 0 起始闭区间; 若上位机从 1 起编号, 请在软件中先减 1 再写寄存器.
// 切勿在硬件对四路统一减 1: xs=0 时下溢为 16'hFFFF, roi_cfg_ok 为假, in_rgb_roi 退化为全幅 (非零计数).
// 上电复位值由 ROI_RST_* 参数 / slv_reg16..19 决定; 非法 (xe<xs 或 ye<ys) 时 in_rgb_roi 退化为全幅.
wire [15:0] lite_roi_xs;
wire [15:0] lite_roi_xe;
wire [15:0] lite_roi_ys;
wire [15:0] lite_roi_ye;

reg [15:0] roi_xs_r;
reg [15:0] roi_xe_r;
reg [15:0] roi_ys_r;
reg [15:0] roi_ye_r;
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        roi_xs_r <= ROI_RST_XS;
        roi_xe_r <= ROI_RST_XE;
        roi_ys_r <= ROI_RST_YS;
        roi_ye_r <= ROI_RST_YE;
    end
    else
    begin
        roi_xs_r <= lite_roi_xs;
        roi_xe_r <= lite_roi_xe;
        roi_ys_r <= lite_roi_ys;
        roi_ye_r <= lite_roi_ye;
    end
end

// 当前 beat 对应的像素列 (扩展为 32 位便于与 ROI 比较)
wire [31:0] roi_pix_x = ({16'd0, col_cnt} << COL_PIX_SHIFT);
// 当前 beat 对应的行号
wire [31:0] roi_pix_y = (axis_xfer && m_axis_tuser[0]) ? 32'd0 : {16'd0, line_cnt};
// ROI 上下界合法 (各维均为 start<=end)
wire        roi_cfg_ok = (roi_xe_r >= roi_xs_r) && (roi_ye_r >= roi_ys_r);
// 落在矩形内, 或非法 ROI 时退化为全幅允许计数
wire        in_rgb_roi = !roi_cfg_ok
    || ((roi_pix_x >= {16'd0, roi_xs_r}) && (roi_pix_x <= {16'd0, roi_xe_r})
     && (roi_pix_y >= {16'd0, roi_ys_r}) && (roi_pix_y <= {16'd0, roi_ye_r}));

// RGB 帧内命中: ROI 内; 视频像素 (低 24 位 RBG) 各通道落在 (统计不要求 mask 色排除)
// [rgb_target_q - pixel_threshold_q, rgb_target_q + pixel_threshold_q] (按通道绝对差实现)
wire [7:0] tgt_R = rgb_target_q[23:16];
wire [7:0] tgt_B = rgb_target_q[15:8];
wire [7:0] tgt_G = rgb_target_q[7:0];
wire [7:0] rgb_dR = (pixel_cmp_R > tgt_R) ? (pixel_cmp_R - tgt_R) : (tgt_R - pixel_cmp_R);
wire [7:0] rgb_dB = (pixel_cmp_B > tgt_B) ? (pixel_cmp_B - tgt_B) : (tgt_B - pixel_cmp_B);
wire [7:0] rgb_dG = (pixel_cmp_G > tgt_G) ? (pixel_cmp_G - tgt_G) : (tgt_G - pixel_cmp_G);
wire within_rgb_target_band = (rgb_dR <= pixel_threshold_q) &&
                              (rgb_dB <= pixel_threshold_q) &&
                              (rgb_dG <= pixel_threshold_q);
wire rgb_stat_pixel = axis_xfer && axis_compare_enable_q  && in_rgb_roi && within_rgb_target_band;

reg [31:0] rgb_pixle_total;
reg [31:0] rgb_pixle_total_cnt;
// Latch completed-frame count on SOF beat when that beat is not rgb_stat_pixel (if has priority).
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        rgb_pixle_total     <= 'd0;
        rgb_pixle_total_cnt <= 'd0;
    end
    else
    begin
        if(!axis_compare_enable_q)
        begin
            rgb_pixle_total     <= 'd0;
            rgb_pixle_total_cnt <= 'd0;
        end
        else if(axis_xfer && m_axis_tuser[0]) begin
            rgb_pixle_total     <= rgb_pixle_total_cnt;
            rgb_pixle_total_cnt <= rgb_stat_pixel ? 32'd1 : 32'd0;
        end
        else if(rgb_stat_pixel)
        begin
            rgb_pixle_total_cnt <= rgb_pixle_total_cnt + 1'b1;
        end

    end
end

wire [15:0] lite_point_x;
wire [15:0] lite_point_y;
reg  [15:0] point_x_r;
reg  [15:0] point_y_r;
reg  [23:0] point_pixel_latched;
wire at_point_pixel = axis_xfer && (roi_pix_x == {16'd0, point_x_r}) && (roi_pix_y == {16'd0, point_y_r});
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        point_x_r <= 16'd0;
        point_y_r <= 16'd0;
        point_pixel_latched <= 24'd0;
    end
    else
    begin
        point_x_r <= lite_point_x;
        point_y_r <= lite_point_y;
        if(at_point_pixel)
            point_pixel_latched <= s_axis_tdata[23:0];
    end
end

wire [31:0] col_pixels_comb;
assign col_pixels_comb = stream_invalid ? 32'b0 : ({16'd0, col} << COL_PIX_SHIFT);
wire [31:0] err_col_pixels_comb;
assign err_col_pixels_comb = stream_invalid ? 32'b0 : ({16'd0, err_col} << COL_PIX_SHIFT);

reg [31:0] stat_col_lite;
reg [31:0] stat_line_lite;
reg [31:0] stat_fps_lite;
reg [31:0] stat_err_col_lite;
reg [31:0] stat_err_line_lite;
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        stat_col_lite      <= 32'b0;
        stat_line_lite     <= 32'b0;
        stat_fps_lite      <= 32'b0;
        stat_err_col_lite  <= 32'b0;
        stat_err_line_lite <= 32'b0;
    end
    else
    begin
        stat_col_lite      <= col_pixels_comb;
        stat_line_lite     <= stream_invalid ? 32'b0 : {16'd0, line};
        stat_fps_lite      <= stream_invalid ? 32'b0 : fps;
        stat_err_col_lite  <= err_col_pixels_comb;
        stat_err_line_lite <= stream_invalid ? 32'b0 : {16'd0, err_line};
    end
end
////////////////

// Explicit width adapters into AXI_LITE (eliminates implicit truncate/zero-extend warnings).
wire [31:0] lite_s_axis_tdata_1    = {8'b0, s_axis_tdata[47:24]};
wire [31:0] lite_s_axis_tdata_0    = {8'b0, s_axis_tdata[23:0]};
wire [31:0] lite_error_data_hold   = error_data_hold[31:0];
wire [31:0] lite_stream_in_data_hold = stream_in_data_hold[31:0];
wire [15:0] lite_err_col           = stat_err_col_lite[15:0];
wire [15:0] lite_err_line          = stat_err_line_lite[15:0];

// Instantiation of Axi Bus Interface S00_AXI
	AXI_LITE_REG_v1_0_S00_AXI # ( 
		.C_RST_ROI_XS(ROI_RST_XS),
		.C_RST_ROI_XE(ROI_RST_XE),
		.C_RST_ROI_YS(ROI_RST_YS),
		.C_RST_ROI_YE(ROI_RST_YE),
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) AXI_LITE_REG_v1_0_S00_AXI_inst (
	    .col(stat_col_lite),
        .line(stat_line_lite),
        .fps(stat_fps_lite),
        .stream_valid(!stream_invalid),
        .intr_level (frame_error_flag),
        .axis_compare_enable (axis_compare_enable),
        .intr_clear (intr_clear),
        .fps_total_cnt(fps_total_cnt),
        .s_axis_tdata_1(lite_s_axis_tdata_1),
        .s_axis_tdata_0(lite_s_axis_tdata_0),
        .error_data_hold(lite_error_data_hold),
        .stream_in_data_hold(lite_stream_in_data_hold),
        .pixel_threshold(pixel_threshold),
        .err_col(lite_err_col),
        .err_line(lite_err_line),
        .rgb_cnt_pixel(w_axi_rgb_target),
        .rgb_pixle_total(rgb_pixle_total),
        .rgb_not_pixel(w_axi_rgb_mask),
        .roi_x_start(lite_roi_xs),
        .roi_x_end(lite_roi_xe),
        .roi_y_start(lite_roi_ys),
        .roi_y_end(lite_roi_ye),
        .point_x(lite_point_x),
        .point_y(lite_point_y),
        .point_pixel(point_pixel_latched),
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready)
	);

// Re-time S00-driven controls onto aclk (1-cycle latency vs slv_reg write).
always@(posedge aclk)
begin
    if(!aresetn)
    begin
        axis_compare_enable_q <= 1'b0;
        intr_clear_q    <= 1'b0;
        pixel_threshold_q       <= 8'd0;
        rgb_target_q            <= 24'd0;
        rgb_mask_q              <= 24'd0;
    end
    else
    begin
        axis_compare_enable_q <= axis_compare_enable;
        intr_clear_q  <= intr_clear;
        pixel_threshold_q     <= pixel_threshold;
        rgb_target_q            <= w_axi_rgb_target;
        rgb_mask_q              <= w_axi_rgb_mask;
    end
end

endmodule
