#!/bin/bash
for f in ../../../Applications/*; do
    if [ -d "$f" ]; then
        # echo $f
        cd $f/
        result=${PWD##*/}
        echo $result
        cd build/

        cmake ..
    fi
done
