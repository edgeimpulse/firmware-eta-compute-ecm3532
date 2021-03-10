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

count = 0;
count_bytes = count.to_bytes(4, byteorder='little', signed=False)

file = open(output_loc, "wb")
for i in range(57856):
    file.write(count_bytes)

#print("\n\rSuccess : written to "+binFile+"\n\r")