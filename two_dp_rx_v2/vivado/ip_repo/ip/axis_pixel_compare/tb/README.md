# axis_pixel_compare simulation

## Run

```bat
cd tb
run_sim.bat
```

Runs **smoke** then **stress** (separate logs under `logs/`).

Pass criteria: each log contains `TB_PASS` and no `TB_FAIL`.

## Coverage

### Smoke (`tb_axis_pixel_compare.v`)
- ERR_PIXEL_CNT threshold IRQ + TOTAL SOF latch
- Regression: legacy IRQ, HOLD, INTR_CLEAR, ignore colour, passthrough, POINT, RGB

### Stress (`tb_axis_pixel_compare_stress.v`)
- SOF / last-pixel error, backpressure, 1px frame
- thr=0 / thr>>errors, mid-frame thr drop, mid-frame disable
- INTR_CLEAR same-frame, burst frames, all-error frame
- ignore colour, TH boundary, STATUS bit1 sticky
- **S14 idle-tuser without tvalid** — `frame_end` 已用 `axis_xfer` 门控，应 PASS
- **S16 SOF then tvalid gap** — SOF 一拍后空档再续传，IRQ/坐标/TOTAL 应正常
