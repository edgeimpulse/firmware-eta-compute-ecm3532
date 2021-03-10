#!/usr/bin/env python

import sys	
import binascii
import os
import serial.tools.list_ports
import glob
import inquirer

# #print(sys.argv[1])

application_loc = sys.argv[1]
output_loc		= sys.argv[1]
temp_file       = os.path.dirname(os.path.abspath(sys.argv[1]))+"/crc.bin"
# output_file     = os.path.dirname(os.path.abspath(sys.argv[1]))+"/combined_binary.bin"
base            = os.path.splitext(os.path.abspath(sys.argv[1]))[0]

#print ("output_loc", output_loc)

# #print(temp_file)

def CRC32_from_file(filename):
    buf = open(filename,'rb').read()
    buf = (binascii.crc32(buf) & 0xFFFFFFFF)
    return "%08X" % buf

count = 0

with open(application_loc, "rb") as f:
    byte = f.read(1)
    while byte != b"":
        count = count + 1
        byte = f.read(1)

# #print(count)
crc = CRC32_from_file(application_loc)

# #print(crc)

crc = "0x" + crc

crc_hex = int(crc, 16)

length_bytes = count.to_bytes(8, byteorder='little', signed=True)
crc_bytes = crc_hex.to_bytes(8, byteorder='little', signed=True)

file = open(temp_file, "wb")
file.write(length_bytes);
file.write(crc_bytes);

count = (256 - 16)>>3;

for x in range(count):
	file.write(crc_bytes);

file.close();


with open(application_loc, "rb") as file2:
	app = file2.read()

with open(temp_file, "rb") as myfile:
	crc = myfile.read()

binFile = application_loc
#print("binFile", binFile)

file = open(binFile, "wb")
file.write(crc)
file.write(app)

#print("\n\rSuccess : written to "+binFile+"\n\r")