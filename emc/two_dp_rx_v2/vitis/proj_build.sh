#!/bin/bash

#source /opt/Xilinx/Vivado/xxx/settings64.sh
source setenv.sh

#exit | source clean.sh -b -d
bash -c "source clean.sh -b -d && exit"

if [[ "$SETENV" -eq "" ]]; then
    echo -e "\033[41;36m OSTYPE Unkown: $OSTYPE \033[0m"
    exit 1
else
    case "$OSTYPE" in
    linux*) 
        xsct create_and_build_SW_proj.tcl
        ;;
    msys*) 
        alias xmd=xmd.bat
        alias xsct=xsct.bat
        alias xsdb=xsdb.bat
        xsct.bat create_and_build_SW_proj.tcl
        ;;
    cygwin*) 
        alias xmd=xmd.bat
        alias xsct=xsct.bat
        alias xsdb=xsdb.bat
        xsct.bat create_and_build_SW_proj.tcl
        ;;
    *) 
        echo -e "\033[41;36m OSTYPE Unkown: $OSTYPE \033[0m"
        exit 1
        ;;
    esac
fi


#xsct create_and_build_SW_proj.tcl
if [ $? != 0 ]
then
    echo -e "\n"
    echo -e "\033[41;36m proj build fail!!! Press any key to exit \033[0m"
    exit 1
else
    echo -e "\n"
    echo -e "\033[42;31m build output done!!! Press any key to exit \033[0m"
#    echo -e "\033[42;31m proj build done!!! \033[0m"
fi


# #exit  | source post_build.sh
# echo "" | source post_build.sh

# if [ $? != 0 ]
# then
#     echo -e "\n"
#     echo -e "\033[41;36m build output fail!!! Press any key to exit \033[0m"
#     exit 1
# else
#     echo -e "\n"
#     echo -e "\033[42;31m build output done!!! Press any key to exit \033[0m"
# fi

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

#exit | source clean.sh -b
bash -c "source clean.sh -b && exit"


