@echo off
REM ModelSim regression (Windows)
set ROOT=%~dp0..
set VSIM=D:\Modelsim\win64\vsim.exe
if not exist "%VSIM%" (
  echo ModelSim not found at %VSIM%
  exit /b 1
)
"%VSIM%" -c -do "%ROOT%\sim\run_modelsim.do"
