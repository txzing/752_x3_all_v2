@echo off

start bash -i -c "exit | source clean.sh -b"

set ip_cache_dir=%cd%\ip_cache

if not exist %ip_cache_dir% (
    echo %ip_cache_dir% is not exist! create new one!
    mkdir ip_cache
)

if %SETENV%==[] (
    echo SETENV=%SETENV%
    call D:\Xilinx\Vivado\2020.1\settings64.bat
    set SETENV=1
) else (
    echo SETENV=%SETENV%
)

::vivado -mode tcl -source create_and_build_proj.tcl
vivado -mode batch -source create_and_build_proj.tcl

start bash -i -c "source clean.sh -b"

