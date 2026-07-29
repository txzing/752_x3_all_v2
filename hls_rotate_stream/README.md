# hls_rotate_stream

**Realtime** AXIS RGB888 rotate: CAPTURE ∥ ROTATE ∥ EMIT + IN/OUT double-buffer.
Configure once (`ap_start` + enable), then continuous video — no per-frame CPU kick.

See [doc/REALTIME_DESIGN.md](doc/REALTIME_DESIGN.md) for bandwidth / FPS / DDR analysis.

## Layout

| Path | Role |
|------|------|
| `src/xf_rotate_stream_accel.cpp` | HLS top (`rotate_stream_accel`) |
| `src/rotate_stream_csim_model.cpp` | Host C-sim model (g++) |
| `src/xf_rotate_stream_tb.cpp` | Multi-frame host TB |
| `src/xf_rotate_stream_axis_tb.cpp` | AXIS TB for Vitis HLS cosim |
| `include/` | Config + rotate kernel |
| `rtl/rotate_stream_accel.v` | Verified RTL (sim / interim IP HDL) |
| `sim/tb_rotate_stream.sv` | ModelSim / iverilog multi-frame TB |
| `scripts/` | C-sim / cosim / export / RTL run |

## Verify (this environment)

```bash
# Host multi-frame C-sim (90/180/270)
bash scripts/run_host_csim.sh

# RTL sim (ModelSim if available, else iverilog)
bash scripts/run_rtl_sim.sh
```

Expected: `HOST_CSIM_ALL_PASSED`, `TEST PASSED`, `IVERILOG_RTL_SIM_PASSED`.

## Vitis HLS (Windows / machine with 2020.1)

```bat
vitis_hls -f scripts/run_stream_csim.tcl
vitis_hls -f scripts/run_stream_cosim.tcl
vitis_hls -f scripts/run_export_ip.tcl
```

Copy exported IP into `vivado/ip_repo/ip/` (or overwrite `rotate_stream_accel_v1_0`).

## ModelSim（本地 Windows）

本云环境无 ModelSim；你本机有的话直接跑：

```bat
cd <repo>\hls_rotate_stream\scripts
run_modelsim.bat
```

默认 `vsim`：`D:\Modelsim\win64\vsim.exe`。若路径不同：

```bat
set MODELSIM_VSIM=C:\path\to\vsim.exe
run_modelsim.bat
```

成功时应看到 `==== MODEL SIM PASSED ====`，并在 `sim\modelsim_transcript.log` 中有 `TEST PASSED`。

看波形：

```bat
run_modelsim_gui.bat
```

## BD / SW integration

1. Vivado: `source vivado/scripts/insert_rotate_stream.tcl` then `insert_rotate_stream_accel`
2. Connect 1ppc RGB24 AXIS in/out, clock/reset, AXI-Lite, m_axi→HP
3. Rebuild XSA; BSP gets `XPAR_XROTATE_STREAM_ACCEL_*`
4. App: `rotate_stream_init_once()` once after `vdma_config()` — already wired in `main.c` under that XPAR guard

## Notes

- Full 1920×3840 needs external DDR (`m_axi` after HLS export). Checked-in RTL uses on-chip multi-buffer to prove **concurrent** semantics (`dbg_overlap_cycles > 0`).
- End-to-end latency ≥ 1 frame (90°); sustained FPS targets ≥30 @ 300 MHz with II=1 stages (see design doc).
- Realtime regression: `bash scripts/run_full_sim_verify.sh` or `run_modelsim.bat`.
