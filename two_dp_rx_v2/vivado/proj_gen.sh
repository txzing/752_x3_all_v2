#!/bin/bash

cd "$(dirname "$0")"
mkdir -p ip_cache

source ./setenv.sh
source ./clean.sh -b -w

vivado -mode batch -source create_proj.tcl

if [[ $? -ne 0 ]]; then
  echo -e "\n\033[41;36m proj gen fail!!! \033[0m"
  exit 1
fi

echo -e "\n\033[42;31m proj gen done!!! \033[0m"
source ./clean.sh -b
read -r -n 1 -p "Press any key..."
