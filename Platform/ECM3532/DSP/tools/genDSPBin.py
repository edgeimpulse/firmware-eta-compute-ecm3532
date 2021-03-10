import os
import sys	

elfFileName = sys.argv[1]
baseName = os.path.basename(elfFileName)
projectName = os.path.splitext(baseName)[0]
# print("projectName",projectName)

def writeNum(numString,size):
	hexString = "0x" + numString
	hexNum = int(hexString, 16)
	hexBytes = hexNum.to_bytes(size, byteorder='little', signed=False)
	dspFile.write(hexBytes);

def writeHexNum(hexNum, size):
	hexBytes = hexNum.to_bytes(size, byteorder='little', signed=False)
	dspFile.write(hexBytes);

def stringToNum(numString):
	hexString = "0x" + numString
	num = int(hexString, 16)
	return num

def getOffsetData(line):
	line = line.strip()
	line = line.replace('@','')
	# print(line)
	x = line.split(" ")
	offset = stringToNum(x[0])
	data = stringToNum(x[1])
	return offset, data

def padZeros(num,size):
	zeroString = "0"
	# print(num,size)
	if(num > 0):
		for x in range(num):
			writeNum(zeroString,size)

dspFile = open("dsp_mem.bin", "wb")

pmemFile = elfFileName+".PMEM"
# print("pmemFile",pmemFile)

size = os.path.getsize(pmemFile)
# print(size)

pmemCount = 0

if(size > 0):
	file1 = open(pmemFile, 'r') 

	file1.readline()
	offset, data = getOffsetData(file1.readline())
	padZeros(offset, 1)
	pmemCount = offset

	file1 = open(pmemFile, 'r') 
	file1.readline()
	Lines = file1.readlines() 

	# if(Lines[0] != ""):
	# 	# print("Valid pmem file")

	for line in Lines: 
		offset, data = getOffsetData(line)
		diff = offset - pmemCount
		padZeros(diff, 4)
		writeHexNum(data,4)
		pmemCount = pmemCount + diff + 1
		# print(pmemCount)
	file1.close()
else:
	writeHexNum(0,4)
	pmemCount = 1

# print("pmemCount:",pmemCount)

xmemFile = elfFileName+".XMEM"

size = os.path.getsize(xmemFile)
# print(size)

xmemCount = 0

if(size > 0):
	file1 = open(xmemFile, 'r') 

	offset, data = getOffsetData(file1.readline())
	padZeros(offset, 1)
	xmemCount = offset

	file1.seek(0)
	Lines = file1.readlines() 
	# print(Lines[0])

	# if(Lines[0] != ""):
	# 	print("Valid xmem file")

	for line in Lines: 
		offset, data = getOffsetData(line)
		diff = offset - xmemCount
		padZeros(diff, 1)
		writeHexNum(data,1)
		xmemCount = xmemCount + diff + 1
		# print(xmemCount)
	file1.close()
else:
	writeHexNum(0,2)
	xmemCount = 2

padding = xmemCount%2
padZeros(padding, 1)
xmemCount = xmemCount + padding
# print("xmemCount:",xmemCount)

ymemFile = elfFileName+".YMEM"

size = os.path.getsize(ymemFile)
# print(size)

ymemCount = 0

if(size > 0):
	file1 = open(ymemFile, 'r') 

	offset, data = getOffsetData(file1.readline())
	padZeros(offset, 1)
	ymemCount = offset

	file1.seek(0)
	Lines = file1.readlines() 
	# print(Lines[0])

	# if(Lines[0] != ""):
	# 	print("Valid ymem file")

	for line in Lines: 
		offset, data = getOffsetData(line)
		diff = offset - ymemCount
		padZeros(diff, 1)
		writeHexNum(data,1)
		ymemCount = ymemCount + diff + 1
		# print(ymemCount)
	file1.close()
else:
	writeHexNum(0,2)
	ymemCount = 2

padding = ymemCount%2
padZeros(padding, 1)
ymemCount = ymemCount + padding
# print("ymemCount:",ymemCount)

xmemCount = xmemCount >> 1;
ymemCount = ymemCount >> 1;

dspFile.close()
file1.close()

# searchLine = ""
# configFile = os.path.dirname(elfFileName) + "/../../config.h"

# name = "#define CONFIG_DSP_FW_START "
# address_start = 0
# with open(configFile, "r") as myfile:
#     for line in myfile:
#         if name in line:
#             searchLine = line
#             break

# if searchLine:
# 	address_split = searchLine.split()
# 	address_start = int(address_split[2], 16)
# else:
# 	print("ERROR CONFIG_DSP_FW_START not found in config.h file")
# 	exit(0)

startingAddress = 0
# print("stringAddress", hex(startingAddress))

# projectName = "hello_world"

dspFile = open("dsp_padded.bin", "wb")

stringAddress = startingAddress + 28
writeHexNum(stringAddress,4)
writeHexNum(pmemCount,4)
writeHexNum(xmemCount,4)
writeHexNum(ymemCount,4)

stringLength = len(projectName) + 1

pmemAddress = stringAddress + stringLength
xmemAddress = pmemAddress + pmemCount * 4
ymemAddress = xmemAddress + xmemCount * 2
# print("pmemAddress",hex(pmemAddress))
# print("xmemAddress",hex(xmemAddress))
# print("ymemAddress",hex(ymemAddress))
writeHexNum(pmemAddress,4)
writeHexNum(xmemAddress,4)
writeHexNum(ymemAddress,4)

dspFile.write(projectName.encode('utf-8'))
padZeros(1,1)

dspFile.close()

# print("pmemCount",pmemCount)
# print("xmemCount",xmemCount)
# print("ymemCount",ymemCount)

with open("dsp_fw.bin", "wb") as myfile, open("dsp_padded.bin", "rb") as file2, open("dsp_mem.bin", "rb") as file3:
    myfile.write(file2.read())
    myfile.write(file3.read())

myfile.close()
file2.close()