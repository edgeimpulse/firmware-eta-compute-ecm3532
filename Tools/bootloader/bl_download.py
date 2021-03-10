#!/usr/bin/env python

import sys
import binascii
import os
import struct
import serial.tools.list_ports
import glob
import inquirer
import platform

file_loc     = sys.argv[1]
bl_update = sys.argv[2]

ports = list(serial.tools.list_ports.comports())
count = 0

portList = []

# print("\n\n")

for p in ports:
    # string = "" + str(count) + " : "
    # print(string)
    myorder = "{}"
    pStr = myorder.format(p)
    # print(string + pStr)
    portList.append(pStr)
    count = count + 1

# print(portList,"\n\n")

if len(portList) != 0:
    questions = [
      inquirer.List('port',
                    message="Choose the serial port using UP/DOWN keys and press ENTER",
                    choices=portList,
                ),
    ]
    answers = inquirer.prompt(questions)
else:
    print("\n\rNo COM port found, exiting...\n\r\n\r")
    exit(0)

# print(ports[x])
myorder = "{}"
pStr = answers['port']
dev = pStr.split(' ', 1)[0]
# dev = answers['port']

# print(dev,"\n\r")

binFileNames = glob.glob(file_loc+"/0*.bin")
# print("binFileNames ",binFileNames)

partitionFileName = ""
binFileJustNames = []

for i in binFileNames:
    if os.stat(i).st_size != 0:
        if i.find("01_partition.bin") != -1:
            partitionFileName = i
        else:
            baseName = os.path.basename(i)
            binFileJustNames.append(baseName)

# print("binFileNames",binFileNames)

# binFileJustNames = []
filePath = os.path.dirname(partitionFileName)
# print("partitionFileName ", partitionFileName)
# for i in binFileNames:
#     baseName = os.path.basename(i)
#     # print(i)
#     binFileJustNames.append(baseName)

binFileJustNames.sort()

chosenBinFiles = []

name = ""

if len(binFileNames) != 0:
    if bl_update == "1":
        chosenBinFiles.append("02_m3_fw.bin")
    else :
        # questions = [
        #   inquirer.Checkbox('interests',
        #                     message="Use UP/DOWN keys and use SPACEBAR to select or deselect the files to be downloaded",
        #                     choices=binFileJustNames,
        #                     ),
        # ]
        # answers = inquirer.prompt(questions)
        # chosenBinFiles = answers['interests']

        # print(chosenBinFiles)
        for x in range(len(binFileJustNames)):
            name = name + " " + filePath + "/" +binFileJustNames[x]
    # name = ""

    # for x in range(len(chosenBinFiles)):
    #     name = name + " " + filePath + "/" +chosenBinFiles[x]
        # print("name ", name)

    # print("Files downloaded : ", name)

    stty_command = "sudo stty -F "+dev+" 460800 cs8 -parenb -cstopb crtscts"
    if platform.system() == 'Darwin':
        stty_command = "stty -f "+dev+" 460800 cs8 -parenb -cstopb crtscts"

    sb_command = "sb"
    if platform.system() == 'Darwin':
        sb_command = "lsz"

    if bl_update == "1":
        os.system(stty_command + " && echo \"****************************************\" && echo \"**** Wait for 10 seconds for update ****\" && echo \"****************************************\" && echo \"\" &&  " + sb_command + " -b --ymodem " +partitionFileName+" "+name+ " > "+dev+" < "+dev)
    else:
        os.system(stty_command + " && echo \"****************************************\" && echo \"**** Press and Release POR_N switch ****\" && echo \"****************************************\" && echo \"\" &&  " + sb_command + " -b --ymodem " +partitionFileName+" "+name+ " > "+dev+" < "+dev)

else :
    print("No bin file found")