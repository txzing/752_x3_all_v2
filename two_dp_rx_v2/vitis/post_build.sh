#!/bin/bash

#source /opt/Xilinx/Vivado/xxx/settings64.sh
source setenv.sh

top=$PWD

cd ./sdk_workspace/fsbl/_ide/bitstream
updatemem -meminfo system_wrapper.mmi -data ../../Debug/fsbl.elf -proc system_i/processor_subsystem/microblaze_0 -bit system_wrapper.bit -out download.bit -force
mkdir -p ../flash

case "$OSTYPE" in
linux*) 
	echo -e "the_ROM_image:\n{\n$PWD/download.bit\n}\n" > ../flash/bootimage.bif
    ;;
msys*)
	BITFILE="`cygpath -w ${top}/sdk_workspace/fsbl/_ide/bitstream/download.bit`"
	(echo "the_ROM_image:" && \
    echo "{" && echo " ${BITFILE}" && \
    echo "}" && echo "") > ../flash/bootimage.bif
    ;;
cygwin*) 
	BITFILE="`cygpath -w ${top}/sdk_workspace/fsbl/_ide/bitstream/download.bit`"
	(echo "the_ROM_image:" && \
    echo "{" && echo " ${BITFILE}" && \
    echo "}" && echo "") > ../flash/bootimage.bif
    ;;
*) 
    echo -e "\033[41;36m OSTYPE Unkown: $OSTYPE \033[0m"
    exit 1
    ;;
esac


bootgen -arch fpga -image ../flash/bootimage.bif -w -o ../flash/BOOT.bin -interface spi 

if [ $? != 0 ]
then
    echo -e "\n"
    echo -e "\033[41;36m gen BOOT.bin fail!!! Press any key to exit \033[0m"
    exit 1
else
    echo -e "\n"
    echo -e "\033[42;31m gen BOOT.bin done!!! \033[0m"
fi

cd $top

#cd -
mkdir -p output
rm -rf output/*
echo -e "\n"
echo -e "\033[42;31m clear output done!!! \033[0m"

cp ./sdk_workspace/vitis_proj/Debug/vitis_proj.elf ./output/app.bin
if [ $? != 0 ]
then
    echo -e "\n"
    echo -e "\033[41;36m cp app.bin fail!!! Press any key to exit \033[0m"
    exit 1
fi

du -b ./output/app.bin | awk '{print substr($1,$2)}' | xargs -I {} printf "%x\n" {} > ./output/app.txt
if [ $? != 0 ]
then
    echo -e "\n"
    echo -e "\033[41;36m calc size fail!!! Press any key to exit \033[0m"
    exit 1
fi

cp ./sdk_workspace/fsbl/_ide/flash/BOOT.bin ./output/fsbl.bin
if [ $? != 0 ]
then
    echo -e "\n"
    echo -e "\033[41;36m fsbl.bin fail!!! Press any key to exit \033[0m"
    exit 1
else
    echo -e "\n"
    echo -e "\033[42;31m cp fsbl.bin done!!! Press any key to exit \033[0m"
fi

du -b ./output/fsbl.bin | awk '{print substr($1,$2)}' | xargs -I {} printf "%x\n" {} > ./output/fsbl.txt
if [ $? != 0 ]
then
    echo -e "\n"
    echo -e "\033[41;36m calc size fail!!! Press any key to exit \033[0m"
    exit 1
fi

# BOOT.bin for remote update
cp ./sdk_workspace/fsbl/_ide/bitstream/download.bit ./output/
vivado -mode batch -source write_cfgmem.tcl
if [ $? != 0 ]
then
    echo -e "\n"
    echo -e "\033[41;36m Generate BOOT.bin fail!!! Press any key to exit \033[0m"
    exit 1
else
    echo -e "\n"
    echo -e "\033[42;31m Generate BOOT.bin done!!! Press any key to exit \033[0m"
fi

bash -c "source clean.sh -b && exit"

# function pause(){
# #	read -p "$*"
#     printf "$*\n"
# #    read -n 1
#     read
# #    printf "\n"
# }
# pause 'Press any key to continue'
function pause(){
    read -n 1
}
pause
