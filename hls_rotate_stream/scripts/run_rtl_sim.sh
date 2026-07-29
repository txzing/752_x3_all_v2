#!/usr/bin/env bash
# RTL sim: prefer ModelSim if present, else iverilog
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/sim"

if command -v vsim >/dev/null 2>&1; then
  echo "Using ModelSim vsim"
  vsim -c -do run_modelsim.do
  exit 0
fi

if [[ -x /D/Modelsim/win64/vsim.exe ]]; then
  echo "Using D:/Modelsim/win64/vsim.exe"
  /D/Modelsim/win64/vsim.exe -c -do run_modelsim.do
  exit 0
fi

echo "ModelSim not found — running iverilog (same TB/RTL)"
iverilog -g2012 -o rot_sim ../rtl/rotate_stream_accel.v tb_rotate_stream.sv
vvp ./rot_sim | tee iverilog_run.log
grep -q "TEST PASSED" iverilog_run.log
echo "IVERILOG_RTL_SIM_PASSED"
