#!/bin/bash
for f in ../../Applications/*; do
    if [ -d "$f" ]; then
        # echo $f
        cd $f/
        result=${PWD##*/}
        echo $result
        cd build/
        rm -rf *
        rm .config
        rm ../config.h
        rm ../*.em*
        rm ../*.ld
        rm ../*.jlink
        cd ../../../Tools/SEGGER/
    fi
done