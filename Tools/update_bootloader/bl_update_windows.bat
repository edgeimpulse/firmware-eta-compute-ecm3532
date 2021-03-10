@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
setlocal
REM go to the folder where this bat script is located
cd /d %~dp0

FOR %%I IN (.) DO SET DIRECTORY_NAME=%%~nI%%~xI

where /q eta-flash-tool
IF ERRORLEVEL 1 (
    GOTO ETANOTINPATHERROR
)

where /q python3
IF ERRORLEVEL 1 (
    GOTO PYTHONNOTINPATHERROR
)

echo Checking Python dependencies...

set PYSERIAL=(pip3 list  2> nul) | findstr "pyserial"
IF %PYSERIAL% == "" (
    pip3 install pyserial==3.4
)
set INQUIRER=(pip3 list  2> nul) | findstr "inquirer"
IF %INQUIRER% == "" (
    pip3 install inquirer==2.7.0
)

echo Checking Python dependencies OK

eta-flash-tool --firmware-path bootloader_update_app.bin
python3 bl_download.py . 1

exit /b 0

:ETANOTINPATHERROR
echo Cannot find 'eta-flash-tool' in your PATH. Install the Edge Impulse CLI before you continue
echo Installation instructions: https://docs.edgeimpulse.com/docs/cli-installation
@pause
exit /b 1

:PYTHONNOTINPATHERROR
echo Cannot find 'python3' in your PATH. Install Python before you continue
echo Installation instructions: https://www.python.org/downloads/
@pause
exit /b 1
