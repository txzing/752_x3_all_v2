#!/bin/bash

## assume you have Vivado 2020.1 (and Vitis) installed in D:\Xilinx\, which is necessary

# you need one of bash.exe from following way
## D:\Xilinx\Vivado\2020.1\tps\win64\git-2.16.2\bin\bash.exe
## assume you have Git installed in C:\Program Files\, offering bash.exe(* not git-bash.exe, git-bash.exe can work but cannot display xsct output)
## assume you have cygwin64 installed in D:\cygwin64\, offering bash.exe(* vitis proj build can not generate elf for the first time, build twice)
## assume you have msys64 installed in D:\msys64\, offering bash.exe(* cmd /c cannot work, so cannot use query_vars_msys() )

# you can get OSTYPE by command `bash -c "echo $OSTYPE"`
# here is an avilable list and coresponding OS
# darwin*       # Mac OS
# darwin9       # iOS
# solaris*      # Solaris
# linux-gnu     # Android
# linux-android # Android (termux) 
# linux-gnu     # Linux
# cygwin        # CYGWIN
# msys          # MSYS2
# linux-gnu     # Win10 bash
# openbsd*      # OpenBSD
# FreeBSD       # FreeBSD
# netbsd        # NetBSD
# ...


# open bash.exe and 
# DIR=`cygpath -ua "D:\path\to\project"` && cd $DIR
# source setenv.sh

query_vars_msys() {
  (cmd.exe /c "call D:\Xilinx\Vitis\2020.1\settings64.bat && D:\Xilinx\Vivado\2020.1\tps\win64\git-2.16.2\bin\bash.exe -c '/usr/bin/printenv PATH'")
}

query_vars_cygwin() {
  (cmd.exe /c "call D:\Xilinx\Vitis\2020.1\settings64.bat && D:\cygwin64\bin\bash.exe -c '/usr/bin/printenv PATH'")
}

echo -e "\033[42;31mScript ver 0.1.002 \nMake sure the env path is correct before running!! \033[0m"

#if [[ "$OSTYPE" == "linux-gnu"* ]]; then
#    source /opt/Xilinx/Vitis/2020.1/settings64.sh
#fi

#case "$OSTYPE" in
#solaris*) echo "SOLARIS" ;;
#darwin*) echo "OSX" ;;
#linux*) echo "LINUX" ;;
#bsd*) echo "BSD" ;;
#msys*) echo "WINDOWS" ;;
#cygwin*) echo "ALSO WINDOWS" ;;
#*) echo "unknown: $OSTYPE" ;;
#esac

#case "$OSTYPE" in
#    solaris*) 
#        echo "SOLARIS" 
#        ;;
#    darwin*) 
#        echo "OSX" 
#        ;;
#    linux*) 
#        echo "LINUX" 
#        ;;
#    bsd*) 
#        echo "BSD" 
#        ;;
#    msys*) 
#        echo "MSYS2" 
#        ;;
#    cygwin*) 
#        echo "CYGWIN" 
#        ;;
#    *) 
#        echo "unknown: $OSTYPE" 
#        ;;
#esac

if [[ "$SETENV" -eq "" ]]; then
    echo "SETENV has not been set before, setting ..."

    case "$OSTYPE" in
        linux*) 
            echo "OSTYPE = $OSTYPE"
            source /opt/Xilinx/Vitis/2020.1/settings64.sh
            export SETENV=1
            ;;
        msys*) 
            echo "OSTYPE = $OSTYPE"
            #export PATH="$PATH:$(query_vars_msys):/c/Users/dengl/AppData/Roaming/MobaXterm/slash/bin/"
            #export PATH="$PATH:$(query_vars_msys)"
            
            export PATH="/bin:$PATH:/d/Xilinx/Model_Composer/2020.1/bin:/d/Xilinx/Vitis/2020.1/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/d/Xilinx/Vitis/2020.1/gnuwin/bin:/d/Xilinx/Vivado/2020.1/bin:/d/Xilinx/Vivado/2020.1/lib/win64.o:/d/Xilinx/DocNav"
            if [ $? != 0 ]
            then
                echo -e "\n"
                echo -e "\033[41;36m setting vars fail!!! \033[0m"
                exit 1
            fi
            alias xmd=xmd.bat
            alias xsct=xsct.bat
            alias xsdb=xsdb.bat
            export SETENV=1
            ;;
        cygwin*) 
            echo "OSTYPE = $OSTYPE"
            #export PATH="$PATH:$(query_vars_cygwin):/cygdrive/c/Users/dengl/AppData/Roaming/MobaXterm/slash/bin/"
            #export PATH="$PATH:$(query_vars_cygwin)"
            
            export PATH="/bin:$PATH:/cygdrive/d/Xilinx/Model_Composer/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnuwin/bin:/cygdrive/d/Xilinx/Vivado/2020.1/bin:/cygdrive/d/Xilinx/Vivado/2020.1/lib/win64.o:/cygdrive/d/Xilinx/DocNav"
            if [ $? != 0 ]
            then
                echo -e "\n"
                echo -e "\033[41;36m setting vars fail!!! \033[0m"
                exit 1
            fi
            alias xmd=xmd.bat
            alias xsct=xsct.bat
            alias xsdb=xsdb.bat
            export SETENV=1
            ;;
        *) 
            #echo "unknown: $OSTYPE" 
            echo -e "\033[41;36m OSTYPE Unkown: $OSTYPE \033[0m"
            exit 1
            ;;
    esac
else
    echo SETENV=$SETENV
fi

#xsct

