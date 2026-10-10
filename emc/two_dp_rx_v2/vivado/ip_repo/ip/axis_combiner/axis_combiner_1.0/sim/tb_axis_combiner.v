// tb_axis_combiner.v
// 复现 axis_combiner 输出 tuser/帧率统计异常：对比 s0 与 m 的 SOF 上升沿计数
`timescale 1ns / 1ps

module tb_axis_combiner;

localparam integer CLK_NS      = 10;      // 100MHz
localparam integer CHANNEL_IN  = 24;
localparam integer CHANNEL_OUT = 48;
localparam integer FIFO_D      = 64;
localparam integer H_ACT       = 8;
localparam integer V_ACT       = 8;       // 64 像素/帧
localparam integer NUM_FRAMES  = 8;
localparam integer GAP_CYCLES  = 2;

reg                         clk;
reg                         rst_n;

reg  [CHANNEL_IN-1:0]       s0_tdata;
reg                         s0_tvalid;
wire                        s0_tready;
reg                         s0_tuser;
reg                         s0_tlast;

reg  [CHANNEL_IN-1:0]       s1_tdata;
reg                         s1_tvalid;
wire                        s1_tready;
reg                         s1_tuser;
reg                         s1_tlast;

wire [CHANNEL_OUT-1:0]      m_tdata;
wire                        m_tvalid;
reg                         m_tready;
wire                        m_tuser;
wire                        m_tlast;

reg  s0_tuser_d, m_tuser_d;
wire s0_sof_edge_raw = ~s0_tuser_d & s0_tuser;
wire m_sof_edge_raw  = ~m_tuser_d  & m_tuser;
wire s0_sof_edge_vld = s0_tvalid & s0_tready & s0_tuser;
wire m_sof_edge_vld  = m_tvalid  & m_tready  & m_tuser;

integer s0_sof_raw, m_sof_raw;
integer s0_sof_vld, m_sof_vld;
integer s0_beats, m_beats;
integer scenario;
integer tb_fail;

axis_combiner #(
    .CHANNEL_IN_W (CHANNEL_IN),
    .CHANNEL_OUT_W(CHANNEL_OUT),
    .FIFO_D       (FIFO_D)
) u_dut (
    .clk            (clk),
    .rst_n          (rst_n),
    .s0_axis_tdata  (s0_tdata),
    .s0_axis_tvalid (s0_tvalid),
    .s0_axis_tready (s0_tready),
    .s0_axis_tuser  (s0_tuser),
    .s0_axis_tlast  (s0_tlast),
    .s1_axis_tdata  (s1_tdata),
    .s1_axis_tvalid (s1_tvalid),
    .s1_axis_tready (s1_tready),
    .s1_axis_tuser  (s1_tuser),
    .s1_axis_tlast  (s1_tlast),
    .m_axis_tdata   (m_tdata),
    .m_axis_tvalid  (m_tvalid),
    .m_axis_tready  (m_tready),
    .m_axis_tuser   (m_tuser),
    .m_axis_tlast   (m_tlast)
);

initial clk = 1'b0;
always #(CLK_NS/2) clk = ~clk;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        s0_tuser_d <= 1'b0;
        m_tuser_d  <= 1'b0;
        s0_sof_raw <= 0;
        m_sof_raw  <= 0;
        s0_sof_vld <= 0;
        m_sof_vld  <= 0;
        s0_beats   <= 0;
        m_beats    <= 0;
    end else begin
        s0_tuser_d <= s0_tuser;
        m_tuser_d  <= m_tuser;
        if (s0_sof_edge_raw) s0_sof_raw <= s0_sof_raw + 1;
        if (m_sof_edge_raw)  m_sof_raw  <= m_sof_raw  + 1;
        // valid 门控：仅握手拍上的 SOF 拍计数（用上一拍 tuser 边沿近似）
        if (s0_tvalid & s0_tready & s0_tuser & ~s0_tuser_d) s0_sof_vld <= s0_sof_vld + 1;
        if (m_tvalid  & m_tready  & m_tuser  & ~m_tuser_d)  m_sof_vld  <= m_sof_vld  + 1;
        if (s0_tvalid & s0_tready) s0_beats <= s0_beats + 1;
        if (m_tvalid  & m_tready)  m_beats  <= m_beats  + 1;
    end
end

reg [2:0] ready_div;
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) ready_div <= 3'd0;
    else        ready_div <= ready_div + 3'd1;
end

always @(*) begin
    if (scenario == 1) m_tready = (ready_div != 3'd0);
    else               m_tready = 1'b1;
end

task automatic clear_axis;
    begin
        s0_tdata  = {CHANNEL_IN{1'b0}};
        s0_tvalid = 1'b0;
        s0_tuser  = 1'b0;
        s0_tlast  = 1'b0;
        s1_tdata  = {CHANNEL_IN{1'b0}};
        s1_tvalid = 1'b0;
        s1_tuser  = 1'b0;
        s1_tlast  = 1'b0;
    end
endtask

// 带超时的握手等待，避免 FIFO 满后死等
task automatic wait_ready;
    input integer which; // 0=s0 1=s1
    integer tmo;
    begin
        tmo = 0;
        if (which == 0) begin
            while (!s0_tready && tmo < 5000) begin
                @(posedge clk);
                tmo = tmo + 1;
            end
        end else begin
            while (!s1_tready && tmo < 5000) begin
                @(posedge clk);
                tmo = tmo + 1;
            end
        end
        if (tmo >= 5000) begin
            $display("WARN: wait_ready(%0d) timeout", which);
            tb_fail = 1;
        end
    end
endtask

task automatic send_frame;
    input integer port_sel;
    input integer frame_id;
    input integer inject_tuser_glitch;
    integer r, c;
    integer pix;
    integer abort;
    begin
        abort = 0;
        for (r = 0; r < V_ACT && !abort; r = r + 1) begin
            for (c = 0; c < H_ACT && !abort; c = c + 1) begin
                pix = (frame_id << 16) | (r << 8) | c;
                @(posedge clk);
                wait_ready(port_sel);
                if (tb_fail) begin
                    abort = 1;
                end else if (port_sel == 0) begin
                    s0_tvalid <= 1'b1;
                    s0_tuser  <= (r == 0 && c == 0);
                    s0_tlast  <= (c == H_ACT - 1);
                    s0_tdata  <= pix[CHANNEL_IN-1:0];
                end else begin
                    s1_tvalid <= 1'b1;
                    s1_tuser  <= (r == 0 && c == 0);
                    s1_tlast  <= (c == H_ACT - 1);
                    s1_tdata  <= pix[CHANNEL_IN-1:0] ^ 24'hAA55AA;
                end
            end
        end
        @(posedge clk);
        if (port_sel == 0) begin
            s0_tvalid <= 1'b0;
            s0_tuser  <= 1'b0;
            s0_tlast  <= 1'b0;
        end else begin
            s1_tvalid <= 1'b0;
            s1_tuser  <= 1'b0;
            s1_tlast  <= 1'b0;
        end
        repeat (GAP_CYCLES) @(posedge clk);

        if (inject_tuser_glitch && port_sel == 0 && !abort) begin
            @(posedge clk);
            s0_tvalid <= 1'b0;
            s0_tuser  <= 1'b1;
            @(posedge clk);
            s0_tuser  <= 1'b0;
            @(posedge clk);
        end
    end
endtask

// 预填 s1：写到接近 alfull 即停，不满死等
task automatic preload_s1;
    integer n;
    integer pix;
    begin
        n = 0;
        s1_tvalid <= 1'b0;
        while (n < (FIFO_D - 10) && s1_tready) begin
            @(posedge clk);
            if (!s1_tready) begin
                s1_tvalid <= 1'b0;
            end else begin
                pix = n;
                s1_tvalid <= 1'b1;
                s1_tuser  <= (n == 0);
                s1_tlast  <= ((n % H_ACT) == (H_ACT - 1));
                s1_tdata  <= pix[CHANNEL_IN-1:0];
                n = n + 1;
            end
        end
        @(posedge clk);
        s1_tvalid <= 1'b0;
        s1_tuser  <= 1'b0;
        s1_tlast  <= 1'b0;
        $display("INFO: preload_s1 wrote %0d beats", n);
        repeat (5) @(posedge clk);
    end
endtask

task automatic run_scenario;
    input integer sc;
    integer f;
    integer use_glitch;
    integer s0_raw0, m_raw0, s0_vld0, m_vld0;
    begin
        scenario   = sc;
        use_glitch = (sc == 2);
        tb_fail    = 0;

        rst_n = 1'b0;
        clear_axis;
        repeat (5) @(posedge clk);
        rst_n = 1'b1;
        repeat (5) @(posedge clk);

        // 锁存复位后的计数基线（应为 0）
        s0_raw0 = s0_sof_raw;
        m_raw0  = m_sof_raw;
        s0_vld0 = s0_sof_vld;
        m_vld0  = m_sof_vld;

        $display("INFO: scenario=%0d preload s1", sc);
        preload_s1;

        $display("INFO: scenario=%0d drive frames glitch=%0d", sc, use_glitch);
        fork
            begin : s1_bg
                integer k;
                for (k = 0; k < NUM_FRAMES + 2; k = k + 1) begin
                    if (tb_fail) disable s1_bg;
                    send_frame(1, 200 + k, 0);
                end
            end
            begin : s0_fg
                for (f = 0; f < NUM_FRAMES; f = f + 1) begin
                    if (tb_fail) disable s0_fg;
                    send_frame(0, f, use_glitch);
                end
            end
        join

        repeat (300) @(posedge clk);

        $display("--------------------------------------------------");
        $display("SCENARIO %0d RESULT (expect frames=%0d)", sc, NUM_FRAMES);
        $display("  s0 SOF raw=%0d  vld=%0d  beats=%0d",
                 s0_sof_raw - s0_raw0, s0_sof_vld - s0_vld0, s0_beats);
        $display("  m  SOF raw=%0d  vld=%0d  beats=%0d",
                 m_sof_raw - m_raw0, m_sof_vld - m_vld0, m_beats);
        if ((m_sof_raw - m_raw0) > NUM_FRAMES)
            $display("  >>> REPRO: m raw-SOF %0d > expect %0d (inflate)",
                     m_sof_raw - m_raw0, NUM_FRAMES);
        else
            $display("  >>> m raw-SOF matches expect frames");
        if ((m_sof_raw - m_raw0) > (s0_sof_raw - s0_raw0))
            $display("  >>> m raw > s0 raw (combiner added edges)");
        if ((m_sof_vld - m_vld0) != (s0_sof_vld - s0_vld0))
            $display("  >>> valid-gated SOF mismatch m=%0d s0=%0d",
                     m_sof_vld - m_vld0, s0_sof_vld - s0_vld0);
        $display("--------------------------------------------------");
    end
endtask

initial begin
    $display("=== tb_axis_combiner start ===");
    scenario = 0;
    tb_fail  = 0;
    clear_axis;
    rst_n = 1'b0;
    m_tready = 1'b1;

    run_scenario(0);
    run_scenario(1);
    run_scenario(2);

    $display("=== tb_axis_combiner done ===");
    $finish;
end

initial begin
    #(CLK_NS * 500000); // 5ms
    $display("FAIL: TB timeout");
    $finish;
end

endmodule
