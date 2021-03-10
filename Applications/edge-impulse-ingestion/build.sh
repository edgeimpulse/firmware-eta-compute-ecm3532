#!/bin/bash

cd build
make
cd ..
JLinkExe -CommandFile CommandFile.jlink

