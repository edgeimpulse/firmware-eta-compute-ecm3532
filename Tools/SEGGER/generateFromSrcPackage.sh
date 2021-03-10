#!/bin/bash
cd  $1/Tools/SEGGER/
for f in ../../Applications/*; do
    if [ -d "$f" ]; then
        # echo $f
        cd $f/
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
        rm -rf *
        rm .config
        cd ../../../Tools/SEGGER/
    fi
done