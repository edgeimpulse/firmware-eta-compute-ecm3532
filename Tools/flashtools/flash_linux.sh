#!/bin/bash
set -e

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
PYTHON3=$(which python3 || true)

if [ ! -x "$PYTHON3" ]; then
    echo "Cannot find 'python3' in your PATH. Install Python before you continue."
    echo "Installation instructions: https://www.python.org/downloads/"
    exit 1
fi

echo "Creating partitions..."
HAS_OBJCOPY=$(which arm-none-eabi-objcopy || true)
if [ -z "$HAS_OBJCOPY" ]; then
    echo "Cannot find 'arm-none-eabi-objcopy' in your PATH. Install GNU ARM toolchain before you continue."
    exit 1
fi

BUILD_DIR=$SCRIPTPATH/../../Applications/edge-impulse-ingestion/build
cd $BUILD_DIR

# Generate a partition table file
python3 $SCRIPTPATH/../bootloader/genPartitionTable.py $BUILD_DIR/edge-impulse-ingestion.map

rm -f 02_m3_fw.bin 2> /dev/null
rm -f 03_dsp_fw.bin 2> /dev/null
rm -f 04_pbuf.bin 2> /dev/null

# Generate bin file from elf
arm-none-eabi-objcopy -O binary --only-section=.dspBlock edge-impulse-ingestion.elf 03_dsp_fw.bin 2> /dev/null
arm-none-eabi-objcopy -O binary --only-section=.pbufBlock edge-impulse-ingestion.elf 04_pbuf.bin 2> /dev/null
arm-none-eabi-objcopy -O binary --remove-section=.pbufBlock --remove-section=.dspBlock edge-impulse-ingestion.elf 02_m3_fw.bin 2> /dev/null
echo "Creating partitions OK"
echo ""

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

echo "Flashing board... If you receive a 'Permission denied' error, you can set up udev rules via:"
echo "    printf 'KERNEL==\"ttyUSB[0-9]*\",MODE=\"0666\"\\\nKERNEL==\"ttyACM[0-9]*\",MODE=\"0666\"' | sudo tee -a /etc/udev/rules.d/50-myusb.rules"
echo "(unplug the device for this to take effect)"
echo ""

python3 -u $SCRIPTPATH/flash/bootloader.py $BUILD_DIR/01_partition.bin $BUILD_DIR/02_m3_fw.bin $BUILD_DIR/03_dsp_fw.bin $BUILD_DIR/04_pbuf.bin

echo ""
echo "Flashed your Eta Compute development board."
