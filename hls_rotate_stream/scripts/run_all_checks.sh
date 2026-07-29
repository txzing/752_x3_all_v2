#!/usr/bin/env bash
# One-shot regression: host C-sim + RTL sim
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
bash "$ROOT/scripts/run_host_csim.sh"
bash "$ROOT/scripts/run_rtl_sim.sh"
echo "ALL_STREAM_ROTATE_CHECKS_PASSED"
