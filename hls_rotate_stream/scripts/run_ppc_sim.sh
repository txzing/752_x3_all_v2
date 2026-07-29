#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/sim"
echo "=== NPC=1 ==="
iverilog -g2012 -o rot_n1 ../rtl/rotate_stream_accel.v tb_rotate_stream.sv
vvp ./rot_n1 | tee npc1.log
grep -q "TEST PASSED" npc1.log
echo "=== NPC=2 ==="
iverilog -g2012 -o rot_n2 ../rtl/rotate_stream_accel.v tb_rotate_stream_npc2.sv
vvp ./rot_n2 | tee npc2.log
grep -q "TEST PASSED" npc2.log
echo "PPC_SIM_ALL_PASSED"
