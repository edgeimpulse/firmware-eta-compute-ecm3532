#!/bin/bash
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

        cmake ..

        FILE="../configs/""$result""_defconfig"
        # echo $FILE
        if [ -f "$FILE" ]; then
            echo "$FILE exists."
            make loadconfig CONFIG="$FILE"
        fi
        make -j
        cd ../../../Tools/SEGGER/
    fi
done