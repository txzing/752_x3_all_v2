@echo off
setlocal

call D:\Vivado\Vitis\2020.1\settings64.bat
cd /d D:\Vivado\Vitis\2020.1\bin

set MMI=D:\workdir\cariad\ccu\two_dp_rx_v2\vitis\sdk_workspace\vitis_proj\_ide\bitstream\system_wrapper.mmi
set BIT=D:\workdir\cariad\ccu\two_dp_rx_v2\vitis\sdk_workspace\vitis_proj\_ide\bitstream\system_wrapper.bit
set ELF=D:\workdir\cariad\ccu\two_dp_rx_v2\vitis\sdk_workspace\vitis_proj\Debug\vitis_proj.elf
set OUT=D:\workdir\cariad\ccu\two_dp_rx_v2\vitis\sdk_workspace\vitis_proj\_ide\bitstream\download.bit

if not exist "%MMI%" (echo ERROR: missing %MMI% & exit /b 1)
if not exist "%BIT%" (echo ERROR: missing %BIT% & exit /b 1)
if not exist "%ELF%" (echo ERROR: missing %ELF% & exit /b 1)

call updatemem.bat -force -meminfo "%MMI%" -data "%ELF%" -proc system_i/processor_subsystem/microblaze_0 -bit "%BIT%" -out "%OUT%"
if errorlevel 1 (
    echo updatemem failed, errorlevel=%errorlevel%
    exit /b 1
)

echo Generated: %OUT%
exit /b 0
