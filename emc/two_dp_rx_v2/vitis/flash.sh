#!/bin/bash

#source /opt/Xilinx/Vivado/xxx/settings64.sh
source setenv.sh

cd output
echo "program_flash -f fsbl.bin -offset 0 -flash_type mt25ql256-spi-x1_x2_x4 -verify -cable type xilinx_tcf url TCP:127.0.0.1:3121"
program_flash -f fsbl.bin -offset 0 -flash_type mt25ql256-spi-x1_x2_x4 -verify -cable type xilinx_tcf url TCP:127.0.0.1:3121
echo "program_flash -f app.bin -offset 0x00400000 -flash_type mt25ql256-spi-x1_x2_x4 -verify -cable type xilinx_tcf url TCP:127.0.0.1:3121"
program_flash -f app.bin -offset 0x00D00000 -flash_type mt25ql256-spi-x1_x2_x4 -verify -cable type xilinx_tcf url TCP:127.0.0.1:3121

#echo -e "Press any key to exit ..."
#read -s -n1

function pause(){
#	read -p "$*"
    printf "$*\n"
#    read -n 1
    read
#    printf "\n"
}
pause 'Press any key to continue'
