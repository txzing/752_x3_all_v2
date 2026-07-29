#!/usr/bin/env bash
# Realtime verification: host C-sim + RTL concurrent pipeline (all dirs)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ART="$ROOT/sim/results"
mkdir -p "$ART"
bash "$ROOT/scripts/run_host_csim.sh" | tee "$ART/host_csim.log"
grep -q HOST_CSIM_ALL_PASSED "$ART/host_csim.log"

cd "$ROOT/sim"
iverilog -g2012 -o rot_rt ../rtl/rotate_stream_accel.v tb_rotate_stream.sv
vvp ./rot_rt 2>&1 | tee "$ART/rtl_realtime.log"
grep -q "TEST PASSED" "$ART/rtl_realtime.log"
grep -q "overlap_cycles=" "$ART/rtl_realtime.log"
# overlap must be > 0
python3 - <<'PY'
import re
t=open("results/rtl_realtime.log").read()
m=re.search(r"overlap_cycles=(\d+)", t)
assert m and int(m.group(1))>0, t
print("OVERLAP_OK", m.group(1))
PY

echo "ALL_REALTIME_SIM_CHECKS_PASSED" | tee "$ART/SIM_REPORT.txt"
cat "$ART/rtl_realtime.log" >> "$ART/SIM_REPORT.txt"
