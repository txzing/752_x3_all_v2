#!/bin/bash

## assume you have Vivado 2020.1 installed in D:\Xilinx\, which is necessary

# you need one of bash.exe from following way
## D:\Xilinx\Vivado\2020.1\tps\win64\git-2.16.2\bin\bash.exe
## assume you have Git installed in C:\Program Files\Git, offering bash.exe (not git-bash.exe)
## assume you have cygwin64 installed in D:\cygwin64\, offering bash.exe
## assume you have msys64 installed in D:\msys64\, offering bash.exe (* cmd /c cannot work, so cannot use query_vars_msys() )

# open bash.exe and 
# DIR=`cygpath -ua "D:\path\to\project"` && cd $DIR
# source setenv.sh

query_vars_msys() {
  #(cmd.exe /c 'call D:\Xilinx\Vivado\2020.1\settings64.bat && "C:\Program Files\Git\bin\bash.exe" -c "printenv PATH"')
  #(cmd.exe /c "call D:\Xilinx\Vivado\2020.1\settings64.bat && D:\Xilinx\Vivado\2020.1\tps\win64\git-2.16.2\bin\bash.exe -c 'printenv PATH'")
  #(cmd.exe /c 'call D:\Xilinx\Vivado\2020.1\settings64.bat && "C:\Program Files\Git\bin\bash.exe" -c "/usr/bin/printenv PATH"')
  #(cmd.exe /c "call D:\Xilinx\Vivado\2020.1\settings64.bat && D:\msys64\usr\bin\bash.exe -c '/usr/bin/printenv PATH'")
  (cmd.exe /c "call D:\Xilinx\Vivado\2020.1\settings64.bat && D:\Xilinx\Vivado\2020.1\tps\win64\git-2.16.2\bin\bash.exe -c '/usr/bin/printenv PATH'")
}

query_vars_cygwin() {
  #cmd.exe /c "call D:\Xilinx\Vivado\2020.1\settings64.bat && sleep 1 && D:\Xilinx\Vivado\2020.1\tps\win64\git-2.16.2\bin\bash.exe -c 'printenv PATH'"
  #(cmd.exe /c "call D:\Xilinx\Vivado\2020.1\settings64.bat && D:\Xilinx\Vivado\2020.1\tps\win64\git-2.16.2\bin\bash.exe -c 'printenv PATH'")
  (cmd.exe /c "call D:\Xilinx\Vivado\2020.1\settings64.bat && D:\cygwin64\bin\bash.exe -c '/usr/bin/printenv PATH'")
}

echo -e "\033[42;31mScript ver 0.1.002 \nMake sure the env path is correct before running!! \033[0m"

#if [ "xx" = "xxx" ] | [ "xx" = "xxx" ]; then
#   echo "yes!"
#else
#   echo "no!"
#fi

if [[ "$SETENV" -eq "" ]]; then
    #echo "yes!"
    #echo SETENV=$SETENV
    echo "SETENV has not been set before, setting ..."

    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "OSTYPE = $OSTYPE"
        source /opt/Xilinx/Vivado/2020.1/settings64.sh
    elif [[ "$OSTYPE" == "msys" ]]; then
        echo "OSTYPE = $OSTYPE"
        #source 'D:\Xilinx\Vivado\2020.1\settings64.sh'
        #echo "setenv_bash.bat" | cmd 
        #exit

        ## bash.exe from Git can call cmd.exe with /c option, so can use query_vars_msys()
        #export PATH="$PATH:$(query_vars_msys)"
        #if [ $? != 0 ]
        #then
        #    echo -e "\n"
        #    echo -e "\033[41;36m setting vars fail!!! \033[0m"
        #    exit 1
        #fi
        
        #export PATH="/mingw64/bin:/usr/bin:/c/Users/dengl/bin:/bin:/d/Xilinx/Model_Composer/2020.1/bin:/d/Xilinx/Vitis/2020.1/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/d/Xilinx/Vitis/2020.1/gnuwin/bin:/d/Xilinx/Vivado/2020.1/bin:/d/Xilinx/Vivado/2020.1/lib/win64.o:/d/Xilinx/DocNav:/d/Python/Python38/Scripts:/d/Python/Python38:/c/windows/system32:/c/windows:/c/windows/System32/Wbem:/c/windows/System32/WindowsPowerShell/v1.0:/c/windows/System32/OpenSSH:/cmd:/c/Program Files/TortoiseGit/bin:/d/Python/Python38/Lib/site-packages/qt5_applications/Qt/bin:/c/Program Files/Meld:/c/Program Files/Docker/Docker/resources/bin:/d/opencv440/x64/mingw/lib:/d/opencv440/x64/mingw/bin:/c/Users/dengl/AppData/Local/Microsoft/WindowsApps:/c/Users/dengl/AppData/Local/Programs/Microsoft VS Code/bin:/c/Program Files (x86)/SSH Communications Security/SSH Secure Shell:/c/Users/dengl/AppData/Local/GitHubDesktop/bin"
        
        #export PATH="/bin:/d/Xilinx/Model_Composer/2020.1/bin:/d/Xilinx/Vitis/2020.1/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/d/Xilinx/Vitis/2020.1/gnuwin/bin:/d/Xilinx/Vivado/2020.1/bin:/d/Xilinx/Vivado/2020.1/lib/win64.o:/d/Xilinx/DocNav:$PATH"
        
        #export PATH="$PATH:/bin:/d/Xilinx/Model_Composer/2020.1/bin:/d/Xilinx/Vitis/2020.1/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/d/Xilinx/Vitis/2020.1/gnuwin/bin:/d/Xilinx/Vivado/2020.1/bin:/d/Xilinx/Vivado/2020.1/lib/win64.o:/d/Xilinx/DocNav"
        
        ## bash.exe from msys2 cannot run cmd.exe with /c option, so just set PATH direcitly. bash.exe from Git all so can use this PATH
        export PATH="/bin:$PATH:/d/Xilinx/Model_Composer/2020.1/bin:/d/Xilinx/Vitis/2020.1/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/d/Xilinx/Vitis/2020.1/gnuwin/bin:/d/Xilinx/Vivado/2020.1/bin:/d/Xilinx/Vivado/2020.1/lib/win64.o:/d/Xilinx/DocNav"
        # how to findout? ref to setenv_bash.bat, comment CALL vivado bat, compare difference

        #echo $PATH
        
    elif [[ "$OSTYPE" == "cygwin" ]]; then
        echo "OSTYPE = $OSTYPE"
        #export PATH=$PATH:/cygdrive/c/windows/system32
        #export PATH="$(query_vars_cygwin):/cygdrive/c/windows/system32"

        ## bash.exe from Cygwin can call cmd.exe with /c option, so can use query_vars_msys()
        #export PATH="$PATH:$(query_vars_cygwin)"
        #if [ $? != 0 ]
        #then
        #    echo -e "\n"
        #    echo -e "\033[41;36m setting vars fail!!! \033[0m"
        #    exit 1
        #fi
        
        #export PATH="/usr/local/bin:/usr/bin:/cygdrive/d/Xilinx/Model_Composer/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnuwin/bin:/cygdrive/d/Xilinx/Vivado/2020.1/bin:/cygdrive/d/Xilinx/Vivado/2020.1/lib/win64.o:/cygdrive/d/Xilinx/DocNav:/cygdrive/d/Python/Python38/Scripts:/cygdrive/d/Python/Python38:/cygdrive/c/windows/system32:/cygdrive/c/windows:/cygdrive/c/windows/System32/Wbem:/cygdrive/c/windows/System32/WindowsPowerShell/v1.0:/cygdrive/c/windows/System32/OpenSSH:/cygdrive/c/Program Files/Git/cmd:/cygdrive/c/Program Files/TortoiseGit/bin:/cygdrive/d/Python/Python38/Lib/site-packages/qt5_applications/Qt/bin:/cygdrive/c/Program Files/Meld:/cygdrive/c/Program Files/Docker/Docker/resources/bin:/cygdrive/d/opencv440/x64/mingw/lib:/cygdrive/d/opencv440/x64/mingw/bin:/cygdrive/c/Users/dengl/AppData/Local/Microsoft/WindowsApps:/cygdrive/c/Users/dengl/AppData/Local/Programs/Microsoft VS Code/bin:/cygdrive/c/Program Files (x86)/SSH Communications Security/SSH Secure Shell:/cygdrive/c/Users/dengl/AppData/Local/GitHubDesktop/bin"
        
        #export PATH="$PATH:/cygdrive/d/Xilinx/Model_Composer/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnuwin/bin:/cygdrive/d/Xilinx/Vivado/2020.1/bin:/cygdrive/d/Xilinx/Vivado/2020.1/lib/win64.o:/cygdrive/d/Xilinx/DocNav"
        
        
        ## just set PATH direcitly. bash.exe from Git all so can use this PATH
        export PATH="/bin:$PATH:/cygdrive/d/Xilinx/Model_Composer/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/arm/nt/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/microblaze/linux_toolchain/nt64_le/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-linux-gnueabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch32/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-linux/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/aarch64/nt/aarch64-none/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnu/armr5/nt/gcc-arm-none-eabi/bin:/cygdrive/d/Xilinx/Vitis/2020.1/tps/win64/cmake-3.3.2/bin:/cygdrive/d/Xilinx/Vitis/2020.1/gnuwin/bin:/cygdrive/d/Xilinx/Vivado/2020.1/bin:/cygdrive/d/Xilinx/Vivado/2020.1/lib/win64.o:/cygdrive/d/Xilinx/DocNav"
        #echo $PATH
        
    else
        # Unkown. such as freebsd darwin
        # echo "OSTYPE = $OSTYPE"
        echo -e "\033[41;36m OSTYPE Unkown: $OSTYPE \033[0m"
        exit 1
    fi
    
    export SETENV=1

else
    #echo "no!"
    echo SETENV=$SETENV
    
fi


#vivado -mode tcl

