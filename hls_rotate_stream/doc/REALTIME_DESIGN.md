# 实时视频流旋转 — 全盘设计

## 1. 需求目标

| 项 | 目标 |
|----|------|
| 接口 | AXIS RGB888 1ppc 进 / 出，挂在 PL 视频链 |
| 软件 | **只配一次**（分辨率/角度/缓冲基址/enable），之后无 CPU 参与 |
| 实时性 | 输入帧率可持续吞吐（目标 **≥30 fps @ 1920×3840**，300 MHz） |
| 延迟 | 90° 固有 **≥1 帧**；稳态不堆帧、可背压 |
| 缓冲 | 双进 + 双出（4 FB），采集/旋转/输出 **三引擎并发** |

## 2. 为何旧方案不够“实时”

旧 IP：`Capture → Rotate → Emit` **串行**同一帧。  
一帧像素约 \(1920\times3840\approx7.37\times10^6\)，若每阶段 II=1：

- 串行总拍数 ≈ \(3\times7.37\text{M}\) → @300 MHz 仅约 **13 fps**
- 且吐出期间 **不再收流** → 上游必须停或丢帧

## 3. 实时架构（已按此改 RTL / HLS）

```text
                 ┌────────────┐
  s_axis ───────►│  CAPTURE   │──► IN0 / IN1  (ping-pong)
                 └────────────┘         │
                                        ▼
                 ┌────────────┐   frame_done
                 │  ROTATE    │◄────────┘
                 └────────────┘
                        │
                        ▼
                   OUT0 / OUT1
                        │
                 ┌────────────┐
                 │   EMIT     │──────► m_axis
                 └────────────┘
```

- **CAPTURE**：有空闲 IN 槽就收；帧末置 `in_valid`；无槽则 `tready=0` 背压  
- **ROTATE**：`in_valid` 且有空闲 OUT 槽 → 自动开转（帧同步触发）  
- **EMIT**：`out_valid` 则吐流；与 CAPTURE 并行  
- **enable=1 + 一次 ap_start 锁存参数** 后常开，无需每帧软件 Start  

稳态吞吐 ≈ \(\min(f_{cap}, f_{rot}, f_{emit})\)。三者皆 II=1 像素时：

\[
f_{\max}\approx\frac{300\times10^6}{7.37\times10^6}\approx40\ \text{fps}
\]

满足 30 fps 余量。

## 4. DDR 带宽（板级 HLS `m_axi`）

单帧 ≈ 21.1 MB。30 fps 时粗算 4 路（写 IN、读 IN、写 OUT、读 OUT）：

\[
4\times21.1\times30\approx 2.5\ \text{GB/s}
\]

建议：`m_axi` 数据宽 ≥128 bit，挂 PS HP，独立 outstanding；FB 放连续 DDR 区（4×stride）。

仿真用 RTL 用片上 `mem[]` 验证并发语义；**1920×3840 上板必须用 HLS 导出的 `m_axi` 版或等价 DDR DMA**。

## 5. 延迟与策略

- 首帧出图：至少 **1 整帧采集 + 旋转** 之后  
- 上游过快：背压（推荐）或可后续加 “丢最旧 IN”  
- 下游慢：OUT 占满 → ROTATE 停 → IN 占满 → 背压输入  

## 6. 验证项

1. 功能：多帧 90/180/270 与 golden 一致  
2. **流水重叠**：第 N 帧 EMIT 未结束前，第 N+1 帧 CAPTURE 已开始（探针计数）  
3. 常开：仅一次 `ap_start`，连续 N 帧  
4. 本机 ModelSim：`scripts/run_modelsim.bat`  
5. Vitis HLS：`run_stream_cosim.tcl` / `run_export_ip.tcl` 后替换 ip_repo  

## 7. 与 792 链路衔接

```text
CSI/TPG 1ppc RGB24 → rotate_stream_accel → video_out VDMA / 显示
                      m_axi → HP/SMC
                      s_axi_lite ← PS（仅上电配置）
```

软件：`rotate_stream_init_once()` 写 height/width/dir/fb_base，Start+AutoRestart/enable。
