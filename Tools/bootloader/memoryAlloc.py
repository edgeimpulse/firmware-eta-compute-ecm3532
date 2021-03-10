#!/usr/bin/env python

import sys	
import binascii
import os
import serial.tools.list_ports
import glob
import inquirer

blFlashSize = 0x6000
totalFlashSize = 0x80000
totalRamSize = 0x40000
remainingFlashSize = totalFlashSize
remainingRamSize = totalRamSize

ramStartAddress = 0x10000000
flashStartAddress = 0x01000000
ramEndAddress = ramStartAddress + totalRamSize

flashAvailableLoc = flashStartAddress
ramAvailableLoc = ramStartAddress

config_loc = sys.argv[1]

def checkLine(name):
	searchLine = ""
	# print("string :", name)
	with open(config_loc, "r") as myfile:
	    for line in myfile:
	        if name in line:
	            searchLine = line
	            break
	return searchLine

appTypeLine = checkLine("CONFIG_BOOTLOADER_APP")
if appTypeLine:
	flashStartAddress = flashStartAddress + blFlashSize
	remainingFlashSize = totalFlashSize - blFlashSize
	flashAvailableLoc = flashAvailableLoc + blFlashSize

print("\n\rRemaining Flash : ",remainingFlashSize,"\n\r"," Remaining Ram : ",remainingRamSize,"\n\r")

################ M3 #####################
m3Type = 1
m3Loc = checkLine("CONFIG_RAM_ONLY=")
if m3Loc:
	print ("Here1")
	m3Type = 0

m3Loc = checkLine("CONFIG_RAM_SHM_ONLY=")
if m3Loc:
	print ("Here2")
	m3Type = 0

m3RamSize = 0
m3FlashSize = 0

if m3Type == 1:
	questions = [
	  inquirer.Text('size', message="M3 Code Size?"),
	]
	sizeAnswer = inquirer.prompt(questions)
	m3FlashSize = int(sizeAnswer['size'])

	questions = [
	  inquirer.Text('size', message="M3 Data Size?"),
	]
	sizeAnswer = inquirer.prompt(questions)
	m3RamSize = int(sizeAnswer['size'])
	flashAvailableLoc = flashAvailableLoc + m3FlashSize
	remainingFlashSize = remainingFlashSize - m3FlashSize
	flashAlignment = 0
	if flashAvailableLoc % 4096 != 0:
		flashAlignment = 4096 - flashAvailableLoc % 4096
	flashAvailableLoc =  flashAvailableLoc + flashAlignment
	remainingFlashSize = remainingFlashSize - flashAlignment
	ramAvailableLoc = ramAvailableLoc + m3RamSize
	remainingRamSize = remainingRamSize - m3RamSize
else:
	questions = [
	  inquirer.Text('size', message="M3 Code+Data Size?"),
	]
	sizeAnswer = inquirer.prompt(questions)
	m3RamSize = int(sizeAnswer['size'])
	ramAvailableLoc = ramAvailableLoc + m3RamSize
	remainingRamSize = remainingRamSize - m3RamSize

# print("m3StartLoc",hex(m3StartLoc))
# print("m3Size",hex(m3Size))
# print("flashAvailableLoc",hex(flashAvailableLoc))
# print("ramAvailableLoc",hex(ramAvailableLoc))
print("\n\rRemaining Flash : ",remainingFlashSize,"\n\r"," Remaining Ram : ",remainingRamSize,"\n\r")

# exit(0)

################ DSP #####################
questions = [
  inquirer.List('loc',
                message="Choose the DSP location?",
                choices=["FLASH","RAM"],
            ),
]
locAnswer = inquirer.prompt(questions)
questions = [
  inquirer.Text('size', message="DSP Size?"),
]
sizeAnswer = inquirer.prompt(questions)
sizeVal = int(sizeAnswer['size'])

if locAnswer['loc'] == "FLASH":
	dspStartLoc = flashAvailableLoc
	dspSize = sizeVal
	flashAvailableLoc = flashAvailableLoc + sizeVal
	remainingFlashSize = remainingFlashSize - sizeVal
	flashAlignment = 0
	if flashAvailableLoc % 4096 != 0:
		flashAlignment = 4096 - flashAvailableLoc % 4096
	flashAvailableLoc =  flashAvailableLoc + flashAlignment
else:
	ramAvailableLoc = ramAvailableLoc + sizeVal
	dspStartLoc = ramAvailableLoc
	dspSize = sizeVal
	remainingRamSize = remainingRamSize - sizeVal

# print("dspStartLoc",hex(dspStartLoc))
# print("dspSize",hex(dspSize))
# print("flashAvailableLoc",hex(flashAvailableLoc))
# print("ramAvailableLoc",hex(ramAvailableLoc))
print("\n\rRemaining Flash : ",remainingFlashSize,"\n\r"," Remaining Ram : ",remainingRamSize,"\n\r")

# exit(0)
################ PBUF #####################
questions = [
  inquirer.List('loc',
                message="Choose the PBUF location?",
                choices=["FLASH","RAM"],
            ),
]
locAnswer = inquirer.prompt(questions)

questions = [
  inquirer.Text('size', message="PBUF Size?"),
]
sizeAnswer = inquirer.prompt(questions)
sizeVal = int(sizeAnswer['size'])

if locAnswer['loc'] == "FLASH":
	pbufStartLoc = flashAvailableLoc
	pbufSize = sizeVal
	flashAvailableLoc = flashAvailableLoc + sizeVal
	remainingFlashSize = remainingFlashSize - sizeVal
	flashAlignment = 0
	if flashAvailableLoc % 4096 != 0:
		flashAlignment = 4096 - flashAvailableLoc % 4096
	flashAvailableLoc =  flashAvailableLoc + flashAlignment
else:
	ramAvailableLoc = ramAvailableLoc + sizeVal
	pbufStartLoc = ramAvailableLoc
	pbufSize = sizeVal
	remainingRamSize = remainingRamSize - sizeVal

# print("pbufStartLoc",hex(pbufStartLoc))
# print("pbufSize",hex(pbufSize))
# print("flashAvailableLoc",hex(flashAvailableLoc))
# print("ramAvailableLoc",hex(ramAvailableLoc))
print("\n\rRemaining Flash : ",remainingFlashSize,"\n\r"," Remaining Ram : ",remainingRamSize,"\n\r")

################ HEAP SIZE #####################
heap_length = 0
if appTypeLine:
	heapMinLine = checkLine("CONFIG_APP_MIN_HEAP_SIZE")
	if heapMinLine:
		heap_split = heapMinLine.split('=')
		heap_length = int(heap_split[1], 16)

print ("Minimum Heap size : ",heap_length)
questions = [
  inquirer.Text('heapSize', message="HEAP Size?"),
]
answers = inquirer.prompt(questions)
heapSize = int(answers['heapSize'])
heapStartLoc = ramEndAddress - heapSize
remainingRamSize = remainingRamSize - heapSize
print("\n\rRemaining Flash : ",remainingFlashSize,"\n\r"," Remaining Ram : ",remainingRamSize,"\n\r")



print("  M3_RAM_START : ",hex(ramStartAddress))
print("   M3_RAM_SIZE : ",hex(m3RamSize))

print("    HEAP_START : ",hex(heapStartLoc))
print("     HEAP_SIZE : ",hex(heapSize))

print("M3_FLASH_START : ",hex(flashStartAddress))
print(" M3_FLASH_SIZE : ",hex(m3FlashSize))

print("  DSP_FW_START : ",hex(dspStartLoc))
print(" DSP_FW_LENGTH : ",hex(dspSize))

print("    PBUF_START : ",hex(pbufStartLoc))
print("   PBUF_LENGTH : ",hex(pbufSize))

print("\n\r###############################################################")
print("## Do \"make menuconfig\" again for the changes to take effect ##")
print("###############################################################\n\r")

def replaceString(oldString, newString):
	reading_file = open(config_loc, "r")

	new_file_content = ""
	for line in reading_file:
		stripped_line = line.strip()
		if oldString in stripped_line:
			new_line = newString
		else:
			new_line = stripped_line
		new_file_content += new_line +"\n"
	reading_file.close()

	writing_file = open(config_loc, "w")
	writing_file.write(new_file_content)
	writing_file.close()

replaceString("CONFIG_M3_RAM_START=","CONFIG_M3_RAM_START="+hex(ramStartAddress))
replaceString("CONFIG_M3_RAM_SIZE=","CONFIG_M3_RAM_SIZE="+hex(m3RamSize))
replaceString("CONFIG_HEAP_START=","CONFIG_HEAP_START="+hex(heapStartLoc))
replaceString("CONFIG_HEAP_SIZE=","CONFIG_HEAP_SIZE="+hex(heapSize))
replaceString("CONFIG_M3_FLASH_START=","CONFIG_M3_FLASH_START="+hex(flashStartAddress))
replaceString("CONFIG_M3_FLASH_SIZE=","CONFIG_M3_FLASH_SIZE="+hex(m3FlashSize))
replaceString("CONFIG_DSP_FW_START=","CONFIG_DSP_FW_START="+hex(dspStartLoc))
replaceString("CONFIG_DSP_FW_LENGTH=","CONFIG_DSP_FW_LENGTH="+hex(dspSize))
replaceString("CONFIG_PBUF_START=","CONFIG_PBUF_START="+hex(pbufStartLoc))
replaceString("CONFIG_PBUF_LENGTH=","CONFIG_PBUF_LENGTH="+hex(pbufSize))