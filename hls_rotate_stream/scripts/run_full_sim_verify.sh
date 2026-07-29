#!/usr/bin/env bash
# Full simulation verification for rotate_stream_accel
# - Host C-sim (multi-frame, dirs 0/1/2)
# - RTL iverilog (same TB/RTL as ModelSim), dirs 0/1/2
# - ModelSim if vsim is on PATH
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ART="${ROTATE_SIM_ART:-$ROOT/sim/results}"
mkdir -p "$ART"
SUMMARY="$ART/SIM_REPORT.txt"
: > "$SUMMARY"
log() { echo "$*" | tee -a "$SUMMARY"; }

log "========================================"
log " rotate_stream_accel simulation report"
log " date: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
log " root: $ROOT"
log "========================================"

log ""
log "[1] Host multi-frame C-sim (dirs 0/1/2)"
bash "$ROOT/scripts/run_host_csim.sh" 2>&1 | tee "$ART/host_csim.log"
grep -q "HOST_CSIM_ALL_PASSED" "$ART/host_csim.log"
tail -n 20 "$ART/host_csim.log" >> "$SUMMARY"
log "HOST_CSIM: PASS"

log ""
log "[2] RTL sim (iverilog = same sources as ModelSim)"
cd "$ROOT/sim"
python3 <<'PY'
from pathlib import Path
import re

src = Path("tb_rotate_stream.sv").read_text()

def make(direction: int, out_h: int, out_w: int, map_lines: str) -> None:
    t = src
    t = t.replace("localparam integer DIR      = 0;", f"localparam integer DIR      = {direction};")
    t = re.sub(r"localparam integer OUT_H\s*=\s*[^;]+;", f"localparam integer OUT_H    = {out_h};", t, count=1)
    t = re.sub(r"localparam integer OUT_W\s*=\s*[^;]+;", f"localparam integer OUT_W    = {out_w};", t, count=1)
    old = """                        // dir=0 90CW
                        isx = oy;
                        isy = HEIGHT - 1 - ox;"""
    if old not in t:
        raise SystemExit("map block missing")
    t = t.replace(old, map_lines, 1)
    Path(f"tb_auto_dir{direction}.sv").write_text(t)
    print(f"wrote tb_auto_dir{direction}.sv")

make(0, 64, 32, """                        // dir=0 90CW
                        isx = oy;
                        isy = HEIGHT - 1 - ox;""")
make(1, 32, 64, """                        // dir=1 180
                        isx = WIDTH - 1 - ox;
                        isy = HEIGHT - 1 - oy;""")
make(2, 64, 32, """                        // dir=2 270CW
                        isx = WIDTH - 1 - oy;
                        isy = ox;""")
PY

for d in 0 1 2; do
  log "--- RTL dir=$d ---"
  iverilog -g2012 -o "rot_auto_d${d}" ../rtl/rotate_stream_accel.v "tb_auto_dir${d}.sv"
  vvp "./rot_auto_d${d}" 2>&1 | tee "$ART/rtl_dir${d}.log"
  grep -q "TEST PASSED" "$ART/rtl_dir${d}.log"
  grep "TEST PASSED\|Frame\|Mismatch\|ModelSim" "$ART/rtl_dir${d}.log" >> "$SUMMARY" || true
  log "RTL dir=$d: PASS"
done

log ""
if command -v vsim >/dev/null 2>&1; then
  log "[3] ModelSim vsim"
  vsim -c -do run_modelsim.do 2>&1 | tee "$ART/modelsim.log"
  if grep -q "TEST PASSED" modelsim_transcript.log 2>/dev/null || grep -q "TEST PASSED" "$ART/modelsim.log"; then
    log "MODELSIM: PASS"
  else
    log "MODELSIM: FAIL"
    exit 1
  fi
else
  log "[3] ModelSim: not in PATH (cloud) — RTL iverilog covered same TB"
  log "    On your PC: hls_rotate_stream\\scripts\\run_modelsim.bat"
fi

log ""
log "========================================"
log " ALL_SIM_CHECKS_PASSED"
log " artifacts: $ART"
log "========================================"
