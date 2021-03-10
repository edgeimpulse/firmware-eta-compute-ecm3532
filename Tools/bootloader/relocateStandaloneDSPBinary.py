#!/usr/bin/env python

import sys	
import binascii
import os
import struct
import re

#print("relocateDSP.py : ", end="")
#print(sys.argv[1])
#print(sys.argv[2])

bin_loc = sys.argv[1]
ld_loc		= sys.argv[2]
dsp_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/dsp_fw.bin"
base            = os.path.splitext(os.path.abspath(sys.argv[1]))[0]

def writeHexNum(hexNum, size):
    hexBytes = hexNum.to_bytes(size, byteorder='little', signed=False)
    dspFile.write(hexBytes);

dspBlock = 0

with open(ld_loc, "r") as myfile:
    for line in myfile:
        if 'DSP' in line:
            dspBlock = 1;
            #print("\n\r", line, end="")
            break

if dspBlock == 1:
    start = line.find("ORIGIN") + len("ORIGIN")
    end = line.find(",")
    substring = line[start:end]

    start = substring.find("(") + len("(")
    end = substring.rfind(")")
    expression = substring[start:end]
    # print("expression",expression)
    dspAddress = eval(expression)
    # print("dspAddress",dspAddress)
    myfile.close()

    dspOffset = 0

    with open(bin_loc, "rb") as myfile:
        allBinData = myfile.read()
        offset = dspOffset
        # print("offset",offset)
        if len(allBinData) == 0:
            exit(0)

        stringLenth = (allBinData[offset + 3] << 24 | allBinData[offset + 2] << 16 | allBinData[offset + 1] << 8 | allBinData[offset + 0])
        stringAddress = dspAddress + stringLenth
        offset    = offset + 4
        pmemLen   = (allBinData[offset + 3] << 24 | allBinData[offset + 2] << 16 | allBinData[offset + 1] << 8 | allBinData[offset + 0])
        offset    = offset + 4
        xmemLen   = (allBinData[offset + 3] << 24 | allBinData[offset + 2] << 16 | allBinData[offset + 1] << 8 | allBinData[offset + 0])
        offset    = offset + 4
        ymemLen   = (allBinData[offset + 3] << 24 | allBinData[offset + 2] << 16 | allBinData[offset + 1] << 8 | allBinData[offset + 0])
        offset    = offset + 4
        pmemAddress   = dspAddress + (allBinData[offset + 3] << 24 | allBinData[offset + 2] << 16 | allBinData[offset + 1] << 8 | allBinData[offset + 0])
        offset    = offset + 4
        xmemAddress   = dspAddress + (allBinData[offset + 3] << 24 | allBinData[offset + 2] << 16 | allBinData[offset + 1] << 8 | allBinData[offset + 0])
        offset    = offset + 4
        ymemAddress   = dspAddress + (allBinData[offset + 3] << 24 | allBinData[offset + 2] << 16 | allBinData[offset + 1] << 8 | allBinData[offset + 0])

        # print("stringAddress", hex(stringAddress))
        # print("pmemAddress", hex(pmemAddress))
        # print("xmemAddress", hex(xmemAddress))
        # print("ymemAddress", hex(ymemAddress))

        myfile.close()

        with open(bin_loc, "wb") as dspFile:
            writeHexNum(stringAddress,4)
            writeHexNum(pmemLen,4)
            writeHexNum(xmemLen,4)
            writeHexNum(ymemLen,4)
            writeHexNum(pmemAddress,4)
            writeHexNum(xmemAddress,4)
            writeHexNum(ymemAddress,4)
            dspFile.write(allBinData[dspOffset+28:])
            dspFile.close()

    myfile.close()
    #print("Written to ", dsp_file)
else:
    print("DSP section not found, nothing to relocate\n\r")
    exit(0)