#!/usr/bin/env bash
# Host C-sim (g++) — no Vitis HLS required
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
python3 scripts/gen_tb_data.py --height 64 --width 128 --out-dir tb_data
mkdir -p build
g++ -O2 -std=c++14 -Iinclude -o build/stream_csim \
  src/rotate_stream_csim_model.cpp src/xf_rotate_stream_tb.cpp
./build/stream_csim tb_data 0 3 64 128
./build/stream_csim tb_data 1 3 64 128
./build/stream_csim tb_data 2 3 64 128
echo "HOST_CSIM_ALL_PASSED"
