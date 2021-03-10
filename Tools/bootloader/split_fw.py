#!/usr/bin/env python

import sys	
import binascii
import os
import struct

#print("split_fw.py")
#print(sys.argv[1])
#print(sys.argv[2])

bin_loc = sys.argv[1]
map_loc		= sys.argv[2]
pbuf_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/pbuf.bin"
dsp_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/dsp_fw.bin"
m3_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/m3_fw.bin"
base            = os.path.splitext(os.path.abspath(sys.argv[1]))[0]

pbufAddress = 0xFFFFFFFF
pbufLength = 0x0
dspAddress = 0xFFFFFFFF
dspLength = 0x0;

txtImageLength = 0;

txtBlock = 0

with open(map_loc, "r") as myfile:
    for line in myfile:
        if '.text           0x' in line:
            txtBlock = 1;
            # print("\n\r", line, end="")
            break

if txtBlock != 1:
    #print("flash_bl.py")
    exit(0)

splitWords = line.split()
txtAddressString = splitWords[1]
txtAddress = int(txtAddressString, 16) & 0x00FFFFFF
txtLocation = int(txtAddressString, 16) >> 24
# print("txtAddress",hex(txtAddress))
txtLengthString = splitWords[2]
txtLength = int(txtLengthString, 16)
# print("txtLength",hex(txtLength))

#print("txtLocation", txtLocation & 0xF0)

if txtLocation & 0xF0:
    m3_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/02_m3_fw_ram.bin"
    cmd = "cp " + bin_loc + " " + m3_file 
    os.system(cmd)
    exit(0)

pbufFound = 0;

with open(map_loc, "r") as myfile:
    for line in myfile:
        if ' .pbuf          0x' in line:
            pbufFound = 1;
            #print("\n\r", line, end="")
            break

if pbufFound == 1:
    splitWords = line.split()
    pbufAddressString = splitWords[1]
    pbufAddress = int(pbufAddressString, 16) & 0x00FFFFFF
    pbufLocation = int(pbufAddressString, 16) & 0xFF000000 >> 24
    #print("pbufAddress",hex(pbufAddress))
    pbufLengthString = splitWords[2]
    pbufLength = int(pbufLengthString, 16)
    # print("pbufLength",hex(pbufLength))

    myfile.close()

    if pbufLocation & 0xF0:
        pbuf_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/04_pbuf_ram.bin"
    else:
        pbuf_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/04_pbuf_flash.bin"

    with open(bin_loc, "rb") as myfile, open(pbuf_file, "wb") as file:
        allBinData = myfile.read()
        for x in range((pbufAddress - txtAddress), (pbufAddress - txtAddress) + pbufLength):
            # #print(hex(x))
            file.write(struct.pack("=B",allBinData[x]))
    file.close()
    myfile.close()
    #print("Written to ", pbuf_file)
# else:
    #print("pbuf not found")

dspBlock = 0

with open(map_loc, "r") as myfile:
    for line in myfile:
        if ' .dsp           0x' in line:
            dspBlock = 1;
            #print("\n\r", line, end="")
            break

if dspBlock == 1:
    splitWords = line.split()
    dspAddressString = splitWords[1]
    dspAddress = int(dspAddressString, 16) & 0x00FFFFFF
    dspLocation = int(dspAddressString, 16) >> 24
    # print("dspAddress",hex(dspAddress))
    dspLengthString = splitWords[2]
    dspLength = int(dspLengthString, 16)
    # print("dspLength",hex(dspLength))
    #print("dspLocation", dspLocation)

    myfile.close()

    if dspLocation & 0xF0:
        dsp_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/03_dsp_fw_ram.bin"
    else:
        dsp_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/03_dsp_fw_flash.bin"

    with open(bin_loc, "rb") as myfile, open(dsp_file, "wb") as file:
        allBinData = myfile.read()
        for x in range((dspAddress-txtAddress), (dspAddress-txtAddress)+dspLength):
            file.write(struct.pack("=B",allBinData[x]))
    file.close()
    myfile.close()
    #print("Written to ", dsp_file)
# else:
    #print("dsp not found")


if txtLocation & 0xF0:
    m3_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/02_m3_fw_ram.bin"
else:
    m3_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/02_m3_fw_flash.bin"

# TODO : find a way to figure out exact size of m3 image using multiple no load builds in linker

dspBlock = 0

with open(map_loc, "r") as myfile:
    for line in myfile:
        if '.dspBlock       0x' in line:
            dspBlock = 1;
            # print("\n\r", line, end="")
            break

if dspBlock == 1:
    splitWords = line.split()
    dspAddressString = splitWords[1]
    dspAddress = int(dspAddressString, 16) & 0x00FFFFFF
    dspLocation = int(dspAddressString, 16) >> 24
    # print("dspAddress",hex(dspAddress))
    dspLengthString = splitWords[2]
    dspLength = int(dspLengthString, 16)
    # print("dspLength",hex(dspLength))
    # print("dspLocation", dspLocation)
    myfile.close()

#print("dspAddress",hex(dspAddress))
#print("txtAddress",hex(txtAddress))
# if dspLength != 0:
#     txtImageLength = dspAddress - txtAddress
# else:
#     txtImageLength = txtLength

if (dspLength == 0x0) and (pbufLength == 0x0):
    with open(bin_loc, "rb") as myfile:
        allBinData = myfile.read()
        txtImageLength = len(allBinData)
        myfile.close()
else:
    if dspAddress < pbufAddress :
        txtImageLength = dspAddress - txtAddress
    else :
        txtImageLength = pbufAddress - txtAddress

# print("txtImageLength", hex(txtImageLength))

# print("txtImageLength",hex(txtImageLength))
with open(bin_loc, "rb") as myfile, open(m3_file, "wb") as file:
    allBinData = myfile.read()
    for x in range(0, txtImageLength):
        # #print(hex(x))
        file.write(struct.pack("=B",allBinData[x]))
file.close()
myfile.close()
#print("Written to ", m3_file)

if txtLocation & 0xF0:
    print("Not adding CRC as its a RAM build")
else:
    #print("Adding CRC")
    os.system('pwd')
    cmd = "python3 ../../../Tools/bootloader/crc.py " + m3_file
    os.system(cmd)