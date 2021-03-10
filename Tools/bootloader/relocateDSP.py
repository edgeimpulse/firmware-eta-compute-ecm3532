#!/usr/bin/env python

import sys	
import binascii
import os
import struct

# print("relocateDSP.py : ", end="")
#print(sys.argv[1])
#print(sys.argv[2])

def smallest(num1, num2, num3):
    if (num1 < num2) and (num1 < num3):
        smallest_num = num1
    elif (num2 < num1) and (num2 < num3):
        smallest_num = num2
    else:
        smallest_num = num3
    return smallest_num


bin_loc = sys.argv[1]
map_loc		= sys.argv[2]
pbuf_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/pbuf.bin"
dsp_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/dsp_fw.bin"
m3_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/m3_fw.bin"
base            = os.path.splitext(os.path.abspath(sys.argv[1]))[0]

dspBlock = 0

def writeHexNum(hexNum, size):
    hexBytes = hexNum.to_bytes(size, byteorder='little', signed=False)
    dspFile.write(hexBytes);

txtImageLength = 0;

resetBlock = 0
txtBlock = 0

with open(map_loc, "r") as myfile:
    for line in myfile:
        if '.reset          0x' in line:
            resetBlock = 1
            txtBlock = 1
            #print("\n\r", line, end="")
            break

if resetBlock != 1:
    with open(map_loc, "r") as myfile:
        for line in myfile:
            if '.text           0x' in line:
                txtBlock = 1;
                #print("\n\r", line, end="")
                break

# print("line" , line)

if txtBlock != 1:
    print("ERROR : no text or reset block found")
    exit(0)

splitWords = line.split()
txtAddressString = splitWords[1]
txtAddress = int(txtAddressString, 16)
txtLocation = int(txtAddressString, 16) >> 24
# print("txtAddress",hex(txtAddress))
txtLengthString = splitWords[2]
txtLength = int(txtLengthString, 16)
#print("txtLength",hex(txtLength))
#print("txtLocation", txtLocation & 0xF0)
myfile.close()

 # .pbuf          0x
pbufBlock = 0;
with open(map_loc, "r") as myfile:
    for line in myfile:
        if '.pbufBlock      0x' in line:
            pbufBlock = 1;
            break

if pbufBlock == 1:
    splitWords = line.split()
    if (line.find("load address") == -1):
        pbufAddressString = splitWords[1]
    else:
        pbufAddressString = splitWords[5]
    pbufAddress = int(pbufAddressString, 16)
    pbufLocation = int(pbufAddressString, 16) >> 24
    # print("pbufAddress",hex(pbufAddress))
    myfile.close()
else:
    pbufAddress = 0xFFFFFFFF

with open(map_loc, "r") as myfile:
    for line in myfile:
        if '.dspBlock       0x' in line:
            dspBlock = 1;
            #print("\n\r", line, end="")
            break

if dspBlock == 1:
    splitWords = line.split()
    if (line.find("load address") == -1):
        dspAddressString = splitWords[1]
    else:
        dspAddressString = splitWords[5]
    dspAddress = int(dspAddressString, 16)
    dspLocation = int(dspAddressString, 16) >> 24
    # print("dspAddress",hex(dspAddress))
    myfile.close()

    dspOffset = 0

    startAddress = smallest(dspAddress,txtAddress,pbufAddress)
    dspOffset = dspAddress - startAddress

    # print("\n")
    # print("dspAddress",hex(dspAddress))
    # print("txtAddress",hex(txtAddress))
    # if dspAddress > txtAddress:
    #     dspOffset = dspAddress - txtAddress
    # else:
    #     dspOffset = 0 

    with open(bin_loc, "rb") as myfile:
        allBinData = myfile.read()
        offset = dspOffset
        # print("offset",hex(offset))

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

        #print("stringAddress", hex(stringAddress))
        #print("pmemAddress", hex(pmemAddress))
        #print("xmemAddress", hex(xmemAddress))
        #print("ymemAddress", hex(ymemAddress))

        myfile.close()

        with open(bin_loc, "wb") as dspFile:
            dspFile.write(allBinData[0:dspOffset])
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
    # print("DSP section not found, nothing to relocate\n\r")
    exit(0)