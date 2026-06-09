#!/bin/bash
set -e

cd "$(dirname "$0")"
mkdir -p ip_cache

source ./setenv.sh
source ./clean.sh -b

vivado -mode batch -source create_and_build_proj.tcl

if [[ $? -ne 0 ]]; then
  echo -e "\n\033[41;36m proj build fail!!! \033[0m"
  exit 1
fi

echo -e "\n\033[42;31m proj build done!!! \033[0m"
source ./clean.sh -b
read -r -n 1 -p "Press any key..."
