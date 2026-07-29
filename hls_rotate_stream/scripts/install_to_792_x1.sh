#!/usr/bin/env bash
# Copy verified stream-rotate IP + SW helpers into a 792_x1 tree
set -euo pipefail
SRC_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DST="${1:-}"
if [[ -z "$DST" ]]; then
  echo "Usage: $0 /path/to/792_x1"
  exit 1
fi
mkdir -p "$DST/vivado/ip_repo/ip"
rm -rf "$DST/vivado/ip_repo/ip/rotate_stream_accel_v1_0"
cp -a "$SRC_ROOT/vivado/ip_repo/ip/rotate_stream_accel_v1_0" "$DST/vivado/ip_repo/ip/"
mkdir -p "$DST/vivado/scripts"
cp -a "$SRC_ROOT/vivado/scripts/insert_rotate_stream.tcl" "$DST/vivado/scripts/"
mkdir -p "$DST/vitis/src/vitis_proj/src/rotate"
cp -a "$SRC_ROOT/vitis/src/vitis_proj/src/rotate/"* "$DST/vitis/src/vitis_proj/src/rotate/" 2>/dev/null || true
mkdir -p "$DST/hls_rotate_stream"
cp -a "$SRC_ROOT/hls_rotate_stream/." "$DST/hls_rotate_stream/"
echo "Installed rotate_stream into $DST"
echo "Next: open BD, source insert_rotate_stream.tcl, connect AXIS, rebuild XSA, call rotate_stream_init_once()"
