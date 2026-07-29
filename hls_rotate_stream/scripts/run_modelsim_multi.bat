@echo off
setlocal EnableExtensions
REM ModelSim multi-scenario regression for rotate_stream_accel
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
  exit /b 1
)

echo ROOT=%ROOT%
echo VSIM=%VSIM%
cd /d "%ROOT%\sim" || exit /b 1
if not exist results mkdir results
if exist results\modelsim_multi_transcript.log del /f /q results\modelsim_multi_transcript.log
if exist work rmdir /s /q work 2>nul

"%VSIM%" -c -do "run_modelsim_multi.do"
set "EC=%ERRORLEVEL%"

findstr /C:"TEST PASSED" results\modelsim_multi_transcript.log >nul 2>&1
if errorlevel 1 (
  echo.
  echo ==== MODEL SIM MULTI FAILED ====
  if exist results\modelsim_multi_transcript.log type results\modelsim_multi_transcript.log
  exit /b 1
)

echo.
findstr /C:"SCENARIO" results\modelsim_multi_transcript.log
findstr /C:"SUMMARY" results\modelsim_multi_transcript.log
echo ==== MODEL SIM MULTI PASSED ====
exit /b 0
