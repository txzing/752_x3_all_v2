# axis_combiner 仿真（SOF / 帧率复现）

## 运行

```bat
cd bk\vivado\ip_repo\ip\axis_combiner\axis_combiner_1.0\sim
D:\Modelsim\win64\vsim.exe -c -onfinish exit -do "cd {%~dp0}; do run_tb.do"
```

或在 PowerShell：

```powershell
$sim = "...\axis_combiner_1.0\sim"
Set-Location $sim
& D:/Modelsim/win64/vsim.exe -c -onfinish exit -do "cd {$sim}; do run_tb.do"
```

- 使用 `xilinx_sync_fifo_beh.v` 替代 XPM，无需 `xpm` 仿真库
- `.do` 使用限时 `run`，禁止 `run -all`

## 场景结论（已跑通）

| 场景 | 条件 | m raw SOF | 结论 |
|------|------|-----------|------|
| 0 | 干净视频，ready=1 | = 期望帧数 | **干净流不翻倍** |
| 1 | 间歇反压 | = 期望帧数 | 反压 alone 不翻倍 |
| 2 | 帧间 `tvalid=0` 时打 `tuser` 毛刺 | **= 2× 期望** | **可复现 2×** |

机制：DUT 输出 `m_axis_tuser <= s0_axis_tuser`（不看 `tvalid`），与板上 monitor 一样用裸 `tuser` 上升沿计 fps。
