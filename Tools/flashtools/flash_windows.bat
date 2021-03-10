@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
setlocal
REM go to the folder where this bat script is located
cd /d %~dp0

FOR %%I IN (.) DO SET DIRECTORY_NAME=%%~nI%%~xI

where /q python
IF ERRORLEVEL 1 (
    GOTO PYTHONNOTINPATHERROR
)

where /q eta-flash-tool
IF ERRORLEVEL 1 (
    GOTO ETANOTINPATHERROR
)

where /q arm-none-eabi-objcopy
IF ERRORLEVEL 1 (
    GOTO OBJCOPYNOTINPATHERROR
)

echo If you see a ModuleNotFoundError error, run the following commands to add Python packages
echo     python -m pip install pyserial==3.4
echo     python -m pip install inquirer==2.7.0
echo .

set "CURR_DIR=%cd%"
set BUILD_DIR=..\..\Applications\edge-impulse-ingestion\build
python -u ..\bootloader\genPartitionTable.py %BUILD_DIR%\edge-impulse-ingestion.map

cd %BUILD_DIR%

IF EXIST 02_m3_fw.bin (
    DEL 02_m3_fw.bin
)
IF EXIST 03_dsp_fw.bin (
    DEL 03_dsp_fw.bin
)
IF EXIST 04_pbuf.bin (
    DEL 04_pbuf.bin
)

arm-none-eabi-objcopy -O binary --only-section=.dspBlock edge-impulse-ingestion.elf 03_dsp_fw.bin
arm-none-eabi-objcopy -O binary --only-section=.pbufBlock edge-impulse-ingestion.elf 04_pbuf.bin
arm-none-eabi-objcopy -O binary --remove-section=.pbufBlock --remove-section=.dspBlock edge-impulse-ingestion.elf 02_m3_fw.bin

cd %CURR_DIR%
python -u flash\bootloader.py %BUILD_DIR%\01_partition.bin %BUILD_DIR%\02_m3_fw.bin %BUILD_DIR%\03_dsp_fw.bin %BUILD_DIR%\04_pbuf.bin

@pause
exit /b 0

:PYTHONNOTINPATHERROR
echo Cannot find 'python' in your PATH. Install Python before you continue
echo Installation instructions: https://www.python.org/downloads/
@pause
exit /b 1

:ETANOTINPATHERROR
echo Cannot find 'eta-flash-tool' in your PATH. Install the Edge Impulse CLI before you continue
echo Installation instructions: https://docs.edgeimpulse.com/docs/cli-installation
@pause
exit /b 1

:OBJCOPYNOTINPATHERROR
echo Cannot find 'arm-none-eabi-objcopy' in your PATH. Install GNU ARM toolchain before you continue
@pause
exit /b 1
