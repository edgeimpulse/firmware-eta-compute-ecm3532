#!/bin/bash

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

pathToFile=$(dirname $1)
fileName=$(basename $1)
hexFileName=$(basename $1 .elf)

PYTHON3=$(which python3 || true)

if [ ! -x "$PYTHON3" ]; then
    echo "Cannot find 'python3' in your PATH. Install Python before you continue."
    echo "Installation instructions: https://www.python.org/downloads/"
    exit 1
fi

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

# Generate a partition table file
python3 $SCRIPTPATH/genPartitionTable.py $pathToFile/$hexFileName.map

rm -f 02_m3_fw.bin 2> /dev/null
rm -f 03_dsp_fw.bin 2> /dev/null
rm -f 04_pbuf.bin 2> /dev/null

# Generate bin file from elf
arm-none-eabi-objcopy -O binary --only-section=.dspBlock $pathToFile/$hexFileName.elf $pathToFile/03_dsp_fw.bin 2> /dev/null
arm-none-eabi-objcopy -O binary --only-section=.pbufBlock $pathToFile/$hexFileName.elf $pathToFile/04_pbuf.bin 2> /dev/null
arm-none-eabi-objcopy -O binary --remove-section=.pbufBlock --remove-section=.dspBlock $pathToFile/$hexFileName.elf $pathToFile/02_m3_fw.bin 2> /dev/null

python3 $SCRIPTPATH/bl_download.py $pathToFile 0
