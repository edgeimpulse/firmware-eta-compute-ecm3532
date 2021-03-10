#!/usr/bin/env python

import sys	
import binascii
import os

# configFile = "../config.h"
map_loc		= sys.argv[1]
filePath = os.path.dirname(map_loc)
outputFile = filePath+"/01_partition.bin"
# print ("outputFile ", outputFile)
# print("map_loc",map_loc)

def getAddress(name):
	searchLine = ""
	# print("string :", name)
	with open(map_loc, "r") as myfile:
	    for line in myfile:
	        if name in line:
	            searchLine = line
	            break
	return searchLine

# # num = 0x1122334455667788
# num = 0x1133557799BBDDFF
# length_bytes = num.to_bytes(8, byteorder='little', signed=True)

# file = open(temp_file, "wb")

# count = 1026
# for x in range(count):
# 	file.write(length_bytes);

# file.close();

flash_start  = 0
flash_length = 0
ram_start  	= 0
ram_length 	= 0
m3_start		= 0
m3_length	= 0
dsp_start  	= 0
dsp_length 	= 0
pbuf_start	  	= 0
pbuf_length	  	= 0
shm_start = 0
shm_length = 0

flash_line = getAddress("FLASH            0x")
if flash_line:
	flash_split = flash_line.split()
	flash_start = int(flash_split[1], 16)
	flash_length = int(flash_split[2], 16)

ram_line = getAddress("RAM              0x")
if ram_line:
	ram_split = ram_line.split()
	ram_start = int(ram_split[1], 16)
	ram_length = int(ram_split[2], 16)

shm_line = getAddress("SHM              0x")
if shm_line:
	shm_split = shm_line.split()
	shm_start = int(shm_split[1], 16)
	shm_length = int(shm_split[2], 16)

# dsp_line = getAddress(" .dsp           0x")
# if dsp_line:
# 	dsp_split = dsp_line.split()
# 	dsp_start = int(dsp_split[1], 16)
# 	dsp_length = int(dsp_split[2], 16)
# else:
dsp_line = getAddress("DSP              0x")
# print("dsp_line",dsp_line)
if dsp_line:
	dsp_split = dsp_line.split()
	dsp_start = int(dsp_split[1], 16)
	dsp_length = int(dsp_split[2], 16)
else:
	dsp_start = 0
	dsp_length = 0

# pbuf_line = getAddress(" .pbuf         0x")
# if pbuf_line:
# 	pbuf_split = pbuf_line.split()
# 	pbuf_start = int(pbuf_split[1], 16)
# 	pbuf_length = int(pbuf_split[2], 16)
# else:
pbuf_line = getAddress("PBUF             0x")
if pbuf_line:
	pbuf_split = pbuf_line.split()
	pbuf_start = int(pbuf_split[1], 16)
	pbuf_length = int(pbuf_split[2], 16)
else:
	pbuf_start = 0
	pbuf_length = 0

textLine = getAddress(".reset          0x")
if textLine:
	splitWords = textLine.split()
	txtAddress = int(splitWords[1], 16)
	txtLength = int(splitWords[2], 16)
else:
	textLine = getAddress(".text           0x")
	if textLine:
		splitWords = textLine.split()
		txtAddress = int(splitWords[1], 16)
		txtLength = int(splitWords[2], 16)
	else:
		print("genPartitionTable.py : ERROR - No text section found")

if txtAddress & 0xF0000000:
	m3_start = ram_start
	m3_length = ram_length + shm_length
else:
	m3_start = flash_start
	# m3_length = dsp_start - m3_start
	m3_length = flash_length

print("Partition Table")
print("   m3_start :", hex(m3_start))
print("  m3_length :", hex(m3_length))
print("  dsp_start :", hex(dsp_start))
print(" dsp_length :", hex(dsp_length))
print(" pbuf_start :", hex(pbuf_start))
print("pbuf_length :", hex(pbuf_length))
print("\n\r")

file = open(outputFile, "wb")
temp_bytes = m3_start.to_bytes(4, byteorder='little', signed=False)
file.write(temp_bytes);
temp_bytes = m3_length.to_bytes(4, byteorder='little', signed=False)
file.write(temp_bytes);
temp_bytes = dsp_start.to_bytes(4, byteorder='little', signed=False)
file.write(temp_bytes);
temp_bytes = dsp_length.to_bytes(4, byteorder='little', signed=False)
file.write(temp_bytes);
temp_bytes = pbuf_start.to_bytes(4, byteorder='little', signed=False)
file.write(temp_bytes);
temp_bytes = pbuf_length.to_bytes(4, byteorder='little', signed=False)
file.write(temp_bytes);
file.close()