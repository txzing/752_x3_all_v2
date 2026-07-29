@echo off
setlocal EnableExtensions
REM ============================================================
REM ModelSim batch sim for rotate_stream_accel (90deg, 3 frames)
REM Default ModelSim: D:\Modelsim\win64\vsim.exe
REM Override: set MODELSIM_VSIM=C:\path\to\vsim.exe
REM ============================================================

set "SCRIPT_DIR=%~dp0"
set "ROOT=%SCRIPT_DIR%.."
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
set "ROTATE_STREAM_ROOT=%ROOT%"

if defined MODELSIM_VSIM (
  set "VSIM=%MODELSIM_VSIM%"
) else (
  set "VSIM=D:\Modelsim\win64\vsim.exe"
)

if not exist "%VSIM%" (
  echo ERROR: ModelSim not found: %VSIM%
  echo Set MODELSIM_VSIM to your vsim.exe path and retry.
  exit /b 1
)

if not exist "%ROOT%\rtl\rotate_stream_accel.v" (
  echo ERROR: missing %ROOT%\rtl\rotate_stream_accel.v
  exit /b 1
)
if not exist "%ROOT%\sim\tb_rotate_stream.sv" (
  echo ERROR: missing %ROOT%\sim\tb_rotate_stream.sv
  exit /b 1
)

echo ROOT=%ROOT%
echo VSIM=%VSIM%
cd /d "%ROOT%\sim" || exit /b 1

if exist modelsim_transcript.log del /f /q modelsim_transcript.log
if exist work rmdir /s /q work 2>nul

"%VSIM%" -c -do "run_modelsim.do"
set "EC=%ERRORLEVEL%"

findstr /C:"TEST PASSED" modelsim_transcript.log >nul 2>&1
if errorlevel 1 (
  echo.
  echo ==== MODEL SIM FAILED ^(no TEST PASSED^) ====
  if exist modelsim_transcript.log type modelsim_transcript.log
  exit /b 1
)

echo.
echo ==== MODEL SIM PASSED ====
exit /b 0
