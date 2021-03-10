#!/bin/sh

pathToFile=$(dirname $1)
fileName=$(basename $1)
hexFileName=$(basename $1 .elf)
scriptPath=$(dirname $0)

# Generate a partition table file
python3 $scriptPath/genPartitionTable.py $pathToFile/$hexFileName.map

rm 02_m3_fw.bin 2> /dev/null
# rm 03_dsp_fw.bin 2> /dev/null
rm 04_pbuf.bin 2> /dev/null

# Generate bin file from elf
# arm-none-eabi-objcopy -O binary --only-section=.dspBlock $pathToFile/$hexFileName.elf $pathToFile/03_dsp_fw.bin 2> /dev/null
arm-none-eabi-objcopy -O binary --only-section=.pbufBlock $pathToFile/$hexFileName.elf $pathToFile/04_pbuf.bin 2> /dev/null
arm-none-eabi-objcopy -O binary --remove-section=.pbufBlock --remove-section=.dspBlock $pathToFile/$hexFileName.elf $pathToFile/02_m3_fw.bin 2> /dev/null