@echo off
setlocal EnableExtensions
REM GUI ModelSim for waveform debug
set "SCRIPT_DIR=%~dp0"
set "ROOT=%SCRIPT_DIR%.."
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
set "ROTATE_STREAM_ROOT=%ROOT%"

if defined MODELSIM_VSIM (
  set "VSIM=%MODELSIM_VSIM%"
) else (
  set "VSIM=D:\Modelsim\win64\vsim.exe"
)

cd /d "%ROOT%\sim" || exit /b 1
"%VSIM%" -do "run_modelsim_gui.do"
