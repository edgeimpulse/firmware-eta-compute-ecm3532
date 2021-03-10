import select
import sys
import serial
from ymodem import YMODEM
import os
import time

from shutil import which

bootloader_type = 0
port     = sys.argv[1]
fileCount = len(sys.argv)
newBootloaderString = "#### BOOTLOADER V2 ####"
oldBootloaderString = "Supported Commands:"

binFileJustNames = []
count = 2
while (count < fileCount):
    binFileJustNames.append(sys.argv[count])
    count = count + 1

binFileJustNames.sort()

def sender_getc(size):
    return ser.read(size) or None
 
def sender_putc(data):
    ser.write(data)

def waitForBanner():
    global bootloader_type
    print("\n\rPRESS and RELEASE the POR button")
    while True:
        line = ser.readline()
        # print(line)
        string = str(line, errors='ignore')
        if newBootloaderString in string : 
            bootloader_type = 1
            break
        if oldBootloaderString in string :
            bootloader_type = 0
            break

def ymodem_send(file):
    global ymodem_sender
    try:
        file_stream = open(file, 'rb')
    except IOError as e:
        raise Exception("Open file fail!")
    file_name = os.path.basename(file)
    file_size = os.path.getsize(file)
 
    try:
        ymodem_sender.send(file_stream, file_name,file_size)
    except Exception as e:
        file_stream.close()
        raise
    file_stream.close()

ser = serial.Serial(port, 460800)
ymodem_sender = YMODEM(sender_getc, sender_putc)

for x in range(len(binFileJustNames)): 
    ymodem_send(binFileJustNames[x])

ymodem_sender.end_file()

ser.close()