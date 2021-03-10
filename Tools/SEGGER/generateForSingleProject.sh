#!/bin/bash
cd ../../Applications/$1/
result=${PWD##*/}
echo $result
if [ ! -d "build" ]; then
  mkdir build
fi
cd build/
rm -rf *
rm .config
rm ../config.h
rm ../*.em*
rm ../*.ld
rm ../*.jlink

cmake ..

FILE="../configs/""$result""_defconfig"
# echo $FILE
if [ -f "$FILE" ]; then
    echo "$FILE exists."
    make loadconfig CONFIG="$FILE"
fi
make ses