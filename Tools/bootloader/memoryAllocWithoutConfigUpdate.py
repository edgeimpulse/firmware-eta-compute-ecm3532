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

questions = [
  inquirer.List('bl',
                message="Choose the application type?",
                choices=["STANDALONE","BOOTLOADER_APP"],
            ),
]
answers = inquirer.prompt(questions)

if answers['bl'] == "BOOTLOADER_APP":
	flashStartAddress = flashStartAddress + blFlashSize
	remainingFlashSize = totalFlashSize - blFlashSize
	flashAvailableLoc = flashAvailableLoc + blFlashSize

print("\n\rRemaining Flash : ",remainingFlashSize,"\n\r"," Remaining Ram : ",remainingRamSize,"\n\r")

################ M3 #####################
questions = [
  inquirer.List('loc',
                message="Choose the M3 location?",
                choices=["FLASH","RAM"],
            ),
]
locAnswer = inquirer.prompt(questions)

m3RamSize = 0
m3FlashSize = 0

if locAnswer['loc'] == "FLASH":
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
questions = [
  inquirer.Text('heapSize', message="HEAP Size?"),
]
answers = inquirer.prompt(questions)
heapSize = int(answers['heapSize'])
heapStartLoc = ramEndAddress - heapSize
# print("heapSize",heapSize,"\n\r")
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
