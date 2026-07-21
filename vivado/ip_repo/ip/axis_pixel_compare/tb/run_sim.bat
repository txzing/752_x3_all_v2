@echo off
setlocal
set VSIM=D:\Modelsim\win64\vsim.exe
set TB_DIR=%~dp0
cd /d "%TB_DIR%"

if not exist "%VSIM%" (
  echo ERROR: vsim not found at %VSIM%
  exit /b 2
)

REM Do not use -onfinish exit: TB $finish must return to run_sim.do for PASS check
"%VSIM%" -c -do "do run_sim.do"
set RC=%ERRORLEVEL%
echo exit_code=%RC%
exit /b %RC%
