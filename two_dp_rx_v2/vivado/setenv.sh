#!/bin/bash
# Vivado environment.

_script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
_xilinx_env=""
for _candidate in \
  "${_script_dir}/../env/xilinx_env.sh" \
  "${_script_dir}/env/xilinx_env.sh"; do
  if [[ -f "$_candidate" ]]; then
    _xilinx_env="$_candidate"
    break
  fi
done
if [[ -z "$_xilinx_env" ]]; then
  echo "ERROR: xilinx_env.sh not found. Need repo env/ or vitis/env/" >&2
  exit 1
fi
# shellcheck source=/dev/null
source "$_xilinx_env"
xilinx_env_init vivado
