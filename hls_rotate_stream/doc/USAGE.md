# rotate_stream_accel 使用说明（含多 PPC）

## 1. 这个 IP 做什么

把 **AXIS 视频流** 做 90°/180°/270° 旋转后仍以 AXIS 输出。  
软件 **只配置一次**（分辨率、角度、DDR 缓冲、使能），之后 PL 内 **CAPTURE ∥ ROTATE ∥ EMIT** 自动跑实时流。

```text
CSI/TPG  --AXIS-->  rotate_stream_accel  --AXIS-->  显示 VDMA / video_out
                         |  m_axi (板级 DDR 多缓冲)
                         |  s_axi_lite (仅上电配置)
```

## 2. 软件怎么用（配置一次）

```c
/* main 里 vdma/视频初始化之后调用一次即可 */
rotate_stream_init_once();
```

内部会写：

| 寄存器含义 | 典型值（竖屏转横屏） |
|-----------|----------------------|
| height / width | 3840 / 1920（输入） |
| direction | 0=90CW，1=180，2=270CW |
| fb_base / fb_bytes | DDR 连续 4 个 FB |
| enable + Start(+AutoRestart) | 常开 |

**不要**在 `while(1)` 里反复配角度或每帧 `Start`。

## 3. 硬件连接要点

1. `ap_clk`：视频/处理时钟（如 150/200/300 MHz）  
2. `s_axis_*`：输入 Video AXIS（`TUSER`=SOF，`TLAST`=EOL）  
3. `m_axis_*`：输出 Video AXIS（旋转后宽高可能对调）  
4. `m_axi`（HLS 导出板级 IP）：挂 PS HP / SmartConnect  
5. `s_axi_control`：挂 PS，分配基址后编进 XSA  

BD 辅助：`vivado/scripts/insert_rotate_stream.tcl`

## 4. 多 PPC（NPC）——吞吐与“频率”

**PPC / NPC** = 每个时钟拍处理几个像素。

| NPC | AXIS `TDATA` 宽 | 同频像素吞吐 |
|-----|-----------------|--------------|
| 1 | 24 bit | \(F_{clk}\) |
| 2 | 48 bit | \(2\times F_{clk}\) |
| 4 | 96 bit | \(4\times F_{clk}\) |

近似最大帧率：

\[
FPS_{max} \approx \frac{F_{clk}\times NPC}{H_{in}\times W_{in}}
\]

例：1920×3840，\(F_{clk}=150\,\text{MHz}\)

| NPC | 约 FPS（仅像素吞吐上限） |
|-----|-------------------------|
| 1 | ≈ 20 |
| 2 | ≈ 41 |
| 4 | ≈ 81 |

因此：

- **不是**“多 PPC 就能把时钟拉得无限高”，而是 **同样帧率可用更低时钟（更好走时序）**，或 **同样时钟得到更高帧率**。  
- 实际上限还受 **DDR 带宽**、旋转引擎、下游准备好约束。多 PPC 后 AXIS 侧更轻松，DDR 往往成为瓶颈。

约束：

- `width`（输入）必须被 `NPC` 整除  
- 90°/270° 后输出宽 = 原高，也必须被 `NPC` 整除（1920/3840 对 NPC=1/2/4 都合适）

RTL / 综合参数：

```verilog
rotate_stream_accel #(
  .NPC   (2),      // 1, 2, or 4
  .AXIS_W(48),     // NPC * 24
  .MAX_H (3840),
  .MAX_W (1920)
) u_rot ( ... );
```

HLS：`ROTATE_NPC` / `ROTATE_AXIS_W`（见 `include/xf_rotate_stream_config.h`）。

## 5. 与现有 2ppc CSI 链路怎么接

典型 792 路径：CSI **2ppc@48b** →（可选 subset）→ **1ppc** → VDMA。  

两种接法：

1. **IP 配 NPC=2**：直接吃 48-bit 流（少一次 2→1 转换，时钟可更低）  
2. **IP 配 NPC=1**：保持现有 1ppc，改动最小，帧率靠提高 `ap_clk` 或接受较低 FPS  

推荐实时 30 fps@1920×3840：优先 **NPC=2 + 约 150–200 MHz**，并保证 HP DDR 带宽。

## 6. 验证

```bat
REM 本机 ModelSim
hls_rotate_stream\scripts\run_modelsim.bat
```

```bash
# 功能 + 实时重叠
bash hls_rotate_stream/scripts/run_full_sim_verify.sh
# 多 PPC TB（NPC=2）
bash hls_rotate_stream/scripts/run_ppc_sim.sh
```

## 7. 延迟

90° 至少 **1 整帧** 延迟（必须收满才能开始出旋转后的第一行）。多 PPC 降低的是 **吞吐时间**，不消除这 1 帧固有延迟。
