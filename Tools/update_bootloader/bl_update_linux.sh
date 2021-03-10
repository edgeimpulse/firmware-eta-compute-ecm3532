#!/bin/bash
set -e

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

ETA_FLASH_TOOL=$(which eta-flash-tool || true)
PYTHON3=$(which python3 || true)

if [ ! -x "$ETA_FLASH_TOOL" ]; then
    echo "Cannot find 'eta-flash-tool' in your PATH. Install the Edge Impulse CLI before you continue."
    echo "Installation instructions: https://docs.edgeimpulse.com/docs/cli-installation"
    exit 1
fi

if [ ! -x "$PYTHON3" ]; then
    echo "Cannot find 'python3' in your PATH. Install Python before you continue."
    echo "Installation instructions: https://www.python.org/downloads/"
    exit 1
fi

cd $SCRIPTPATH/data

echo "Checking Python dependencies..."

HAS_PYSERIAL=$(pip3 list | grep -F pyserial || true)
HAS_INQUIRER=$(pip3 list | grep -F inquirer || true)

if [ -z "$HAS_PYSERIAL" ]; then
    pip3 install pyserial==3.4
fi

if [ -z "$HAS_INQUIRER" ]; then
    pip3 install inquirer==2.7.0
fi

echo "Checking Python dependencies OK"
echo ""

eta-flash-tool --firmware-path bootloader_update_app.bin
python3 bl_download.py . 1
