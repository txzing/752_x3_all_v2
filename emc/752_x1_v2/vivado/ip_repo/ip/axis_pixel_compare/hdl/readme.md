# axis_pixel_compare IP 说明

**版本:** 2.20 (与 `component.xml` 中 `spirit:version` 一致)  
**最后更新:** 2026-07-20  
**RTL 源文件:** `axis_pixel_compare.v`, `AXI_LITE_REG_v1_0_S00_AXI.v`

---

## 1. 功能概述

本 IP 接在 AXI-Stream 视频通路上, 对每拍像素数据做 **透传**, 同时提供三类能力:

| 功能 | 说明 |
|------|------|
| **帧差比较** | 比较参考与实时视频各通道差值, 超阈累计错误像素; 达到 `ERR_PIXEL_CNT` 才产生中断 (默认 1=首错) |
| **RGB 区域统计** | 在 ROI 内统计与目标色接近的像素个数 (按通道容差) |
| **点采样** | 指定 (x,y), 读回流经该点的视频像素值 |

PS 通过 **AXI4-Lite (S00_AXI)** 配置阈值, 目标色, 忽略色, ROI, 采样坐标等; 统计量与错误信息经同一总线读回.

---

## 2. 外部接口

### 2.1 AXI-Stream (视频流)

| 接口 | 方向 | 说明 |
|------|------|------|
| `s_axis_*` | 输入 | 待比较的视频流 |
| `m_axis_*` | 输出 | 与输入同拍透传 (valid/ready 握手一致) |
| `aclk` / `aresetn` | 输入 | 视频域时钟与复位 (低有效) |

**数据格式 (默认 WIDTH=48):**

```
s_axis_tdata[47:24]  参考半字 (Reference), 24 位 RBG
s_axis_tdata[23:0]   实时视频 (Live), 24 位 RBG
```

- **RBG 打包:** 高字节 R, `[15:8]` 为 B, `[7:0]` 为 G.
- **`s_axis_tuser[0]`:** 帧起始 SOF; `s_axis_tlast`: 行结束 EOL.
- **PPC 参数:** 每 beat 像素数 1/2/4/8; COL/ERR_COL 读回按 PPC 换算为像素列号.

### 2.2 AXI4-Lite (控制/状态)

| 信号 | 说明 |
|------|------|
| `s00_axi_*` | 标准 AXI4-Lite 从接口, 32 位数据, 7 位地址 (128 字节) |
| `s00_axi_aclk` / `s00_axi_aresetn` | 寄存器时钟域 |

寄存器偏移见 `drivers/.../axis_pixel_compare.h`.

### 2.3 中断

| 信号 | 说明 |
|------|------|
| `intr` | 本帧错误像素数达到 `ERR_PIXEL_CNT` 且比较使能时置位; **下一帧 SOF** 或写 `INTR_CLEAR` 清除 |

内部 `frame_end` 信号为 `m_axis_tuser[0]` 上升沿 (下一帧 SOF), 不是行末 `tlast`.

---

## 3. 核心算法

### 3.1 帧差比较 (报错 / 中断)

1. 写 `STATUS` bit0 = 1 使能比较.
2. 若视频像素落在 **忽略色容差带** 内, 则 **不参与** 帧差, 不计入错误:  
   各通道 `|video - RGB_NOT_PIXEL| <= PIXEL_THRESHOLD`
3. 否则, 若任一通道 `|ref - video| > PIXEL_THRESHOLD`, 则本帧错误像素计数 +1:
   - **首错** 时锁存 `ERROE_DATA_HOLD` / `STREAM_IN_DATA_HOLD` / `ERR_COL` / `ERR_LINE`
   - 当 `err_cnt >= ERR_PIXEL_CNT` (默认 1) 时, 本帧 **首次** 置位 `STATUS` bit1 并产生 `intr`
   - `ERR_PIXEL_CNT_TOTAL` 在下一帧 SOF 锁存上一帧累计错误数 (与 `RGB_PIXEL_TOTAL` 同语义; 帧内读到的是上一帧)
4. 写 `INTR_CLEAR` (0x10, bit0=1) 或 **下一帧 SOF** 清除中断相关状态.

### 3.2 RGB 区域命中统计

- **目标色:** `RGB_CNT_PIXEL` (slv_reg13, 24 位 RBG)
- **容差:** 与帧差共用 `PIXEL_THRESHOLD` (slv_reg10, 8 位)
- **统计条件 (同时满足):**
  - 比较已使能 (`axis_compare_enable_q`)
  - 当前 beat 在 ROI 内 (见 3.3)
  - 各通道 `|video - RGB_CNT_PIXEL| <= PIXEL_THRESHOLD`
- **注意:** 统计 **不** 排除忽略色; 与帧差逻辑独立.
- **实现 (当前 2.19 RTL):**
  - 组合逻辑逐 beat 判定 `rgb_stat_pixel`.
  - 每个 SOF beat (`axis_xfer && tuser`) 锁存 `rgb_pixle_total <= rgb_pixle_total_cnt`, 计数器 seed 为 1 或 0.
  - **已知:** SOF 拍 `roi_pix_y` 仍用拍初 `line_cnt` (上一帧末行数), 有限 ROI 下 SOF 首像素可能不计入 → 稳态比 ROI 像素总数少 1 (100×100 → **9999**).
  - 比较关闭时读回为 0; ROI 自 AXI 每拍更新.
- **读回:** `RGB_PIXEL_TOTAL` 为 **上一帧** 计数; 首帧 SOF 前读回可能为 0; 第 2 帧 SOF 后仍可能为 10000 (首帧 SOF 时 line_cnt=0 全计入), 第 3 帧 SOF 后稳态 **9999** (120×100 TB).

### 3.3 ROI 区域

- 寄存器: `ROI_X_START` / `ROI_X_END` / `ROI_Y_START` / `ROI_Y_END` (各 16 位, **0 起算闭区间** `[xs,xe]x[ys,ye]`)
- 与内部 `col_cnt`, `line_cnt` 一致; 上位机 1 起算请在软件中先减 1.
- **上电默认值:** IP 参数 `ROI_RST_*` (默认 xs=ys=0, xe=ye=99).
- **非法配置** (xe&lt;xs 或 ye&lt;ys): `in_rgb_roi` 退化为 **全幅** 统计 (非零计数).
- **切勿** 在硬件对 ROI 统一减 1: xs=0 下溢为 16'hFFFF 会使配置非法并触发全幅退化.

### 3.4 点采样

| 寄存器 | 偏移 | 读写 | 说明 |
|--------|------|------|------|
| `POINT_X` | 0x50 | W/R | 采样列, 0 起算 |
| `POINT_Y` | 0x54 | W/R | 采样行, 0 起算 |
| `POINT_PIXEL` | 0x58 | R | 流经 (x,y) 时锁存的视频 RBG |

每次 `axis_xfer` 且坐标匹配时更新, 保留最近一次采样值.

### 3.5 COL / LINE 与 ERR_COL / ERR_LINE

**col_cnt / line_cnt (0 基, 与 ROI/点采样/ERR 共用):**
- 1920x720 一帧末像素坐标为 **1919 x 719** (0 起).
- `col_cnt` 行内 beat 索引; `line_cnt` 行索引; EOL 时 `line_cnt++`.

**COL 读回:**
- EOL 时 `col <= col_cnt + 1` (如 1919 -> **1920**).

**LINE 读回:**
- SOF 时 `line <= line_cnt` 锁存上一帧统计值.
- 硬件为 0 基计数结果; **上位机显示 WxH 时可对 LINE (及必要时 COL) +1** 更直观 (719+1=720 行).
- `line_cnt` 与 ROI/`ERR_LINE` 同一套 0 基语义, 硬件不对 ERR_LINE 做 +1.

**ERR_COL / ERR_LINE:**
- **首错当拍**锁存 `ERR_COL = col_cnt`, `ERR_LINE = line_cnt` (拍初值, **0 基**, 与 ROI 一致).
- 行内/行末 (非 SOF) 错误坐标与像素下标一致 (如第 20 行第 10 列 → ERR_LINE=20, ERR_COL=10).
- **已知:** SOF 拍首错时 `ERR_LINE` 可能为上一帧末 `line_cnt` (如 100), 而非 0; `ERR_COL` 在 SOF 拍通常为 0.
- `frame_error_flag` / STATUS bit1 保持至下一 SOF; `INTR_CLEAR` 清 hold/坐标, **不清** STATUS bit1.
- 下一帧 SOF 或 `INTR_CLEAR` 清零 ERR 坐标.

---

## 4. 寄存器映射摘要

| 偏移 | 名称 | R/W | 说明 |
|------|------|-----|------|
| 0x00 | COL | R | 上一完成行的 beat/像素数 (EOL 时 col_cnt+1) |
| 0x04 | LINE | R | 上一帧 line_cnt (0 基; SW 显示可 +1 得行数) |
| 0x08 | FPS | R | 帧率 |
| 0x0C | STATUS | R | [0]使能 [1]本帧有错 [2]流有效 |
| 0x10 | INTR_CLEAR | W | 写 bit0 清 intr/hold/ERR 坐标; **不清** STATUS bit1 (frame_error_flag) |
| 0x14 | FPS_TOTAL_CNT | R | 累计帧数 |
| 0x18 | PIXEL_POINT | R | 总线上当前参考半字 |
| 0x1C | STREAM_IN_DATA | R | 总线上当前视频半字 |
| 0x20 | ERROE_DATA_HOLD | R | 首错时刻参考像素 |
| 0x24 | STREAM_IN_DATA_HOLD | R | 首错时刻视频像素 |
| 0x28 | PIXEL_THRESHOLD | W/R | 各通道容差 (0~255) |
| 0x2C | ERR_COL | R | 首错列 (0 基像素列) |
| 0x30 | ERR_LINE | R | 首错行 (0 基, 与 ROI 一致, 不做 +1) |
| 0x34 | RGB_CNT_PIXEL | W/R | RGB 统计目标色 |
| 0x38 | RGB_PIXEL_TOTAL | R | 上一帧 ROI 命中数 (SOF 边界锁存) |
| 0x3C | RGB_NOT_PIXEL | W/R | 忽略色中心 |
| 0x40~0x4C | ROI_* | W/R | 统计区域 xs/xe/ys/ye |
| 0x50~0x58 | POINT_* | W/R | 点采样 x/y/pixel |
| 0x5C | ERR_PIXEL_CNT | W/R | 错误像素个数阈值 (默认 1); `err_cnt>=` 本值才 IRQ |
| 0x60 | ERR_PIXEL_CNT_TOTAL | R | 上一帧错误像素累计 (下一帧 SOF 锁存) |

---

## 5. IP 可配置参数 (Vivado GUI)

| 参数 | 默认 | 说明 |
|------|------|------|
| WIDTH | 48 | AXI-Stream 数据位宽 |
| PPC | 1 | 每 beat 像素数: 1 / 2 / 4 / 8 |
| TUSER_WIDTH | 1 | tuser 位宽 |
| FREQ_HZ | 100000000 | FPS 计数用 (通常与 aclk 一致) |
| ROI_RST_XS / XE / YS / YE | 0 / 99 / 0 / 99 | ROI 上电复位 (0 起闭区间) |

---

## 6. 典型使用流程

1. 配置 `PIXEL_THRESHOLD`, `RGB_NOT_PIXEL` (可选), `RGB_CNT_PIXEL`, ROI 四寄存器.
2. 可选: 写 `POINT_X`, `POINT_Y`.
3. 写 `STATUS` bit0 = 1 启动比较.
4. 帧差错误: 响应 `intr`, 读 `ERR_*`, `*_HOLD`, 再写 `INTR_CLEAR`.
5. 读 `RGB_PIXEL_TOTAL` (至少等 2 帧); 读 `POINT_PIXEL`.

---

## 7. 文件说明

| 文件 | 内容 |
|------|------|
| `axis_pixel_compare.v` | 顶层: 透传, 帧差, RGB 统计, 点采样, AXI 例化 |
| `AXI_LITE_REG_v1_0_S00_AXI.v` | AXI4-Lite 从机与寄存器译码 |
| `readme.md` | 本说明 (中文, GB2312) |

RTL 内注释为英文; 含中文的源文件编码为 **GB2312**.

---

## 8. 版本记录 (摘要)

| 日期 | 版本 | 主要变更 |
|------|------|----------|
| 2026-05-13 | 2.5 | PPC, stat_* lite 打拍, 中断与 752_x1 对齐 |
| 2026-05-19 | 2.8 | 帧率监测, DEBUG 属性 |
| 2026-05-22 | 2.9 | 修复单帧多次中断; ROI 上电复位; RGB/忽略色容差; 点采样 |
| 2026-06-02 | 2.19 | **当前 RTL:** 组合 RGB 统计 + SOF 锁存; 忽略色按通道 TH; 实时 ROI; line_cnt SOF 先于 EOL |
| 2026-07-20 | 2.20 | ERR_PIXEL_CNT 阈值 (默认 1); `err_cnt>=` 才 IRQ; ERR_PIXEL_CNT_TOTAL 本帧累计 |

说明: 2.10~2.18 曾描述带 RGB 流水线/shadow 寄存器的实验实现, **不在当前 netlist 中**. 勿引用 `tuser_arm`, `frame_latch`, `is_sof_cap`, `rgb_pixle_total_lite` 等已移除信号.

详细变更见 `axis_pixel_compare.v` 头部 `Version history`.
