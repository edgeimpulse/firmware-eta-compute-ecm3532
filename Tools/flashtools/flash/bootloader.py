import select
import sys
import serial
import serial.tools.list_ports
from ymodem import YMODEM
import os
import time
import inquirer
import platform
from shutil import which

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

ft232r = list(filter(lambda x: 'FT232R' in x, portList))

if len(ft232r) == 1:
    answers = { 'port': ft232r[0] }
elif len(portList) != 0:
    questions = [
      inquirer.List('port',
                    message="Select your device",
                    choices=portList,
                ),
    ]
    answers = inquirer.prompt(questions)
else:
    print("\n\rNo connected devices found, exiting...\n\r\n\r")
    exit(1)

if not answers['port']:
    exit(1)

# print(ports[x])
myorder = "{}"
pStr = answers['port']
port = pStr.split(' ', 1)[0]

print('Using device', port)

bootloader_type = 0
fileCount = len(sys.argv)
newBootloaderString = "#### BOOTLOADER V2 ####"
oldBootloaderString = "Supported Commands:"

binFileJustNames = []
count = 1
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
    print("\nClick the RESET button on your Eta compute development board (marked by POR)", flush=True)
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
        raise Exception("Failed to open file!")
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

waitForBanner()

if bootloader_type == 0 :
    update_tools = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'bootloader_update_tools')
    bl_download = os.path.join(update_tools, 'bl_download.py')
    frag_1 = os.path.join(update_tools, '01_partition.bin')
    frag_2 = os.path.join(update_tools, '02_m3_fw.bin')
    update_app = os.path.join(update_tools, 'bootloader_update_app.bin')

    print("Old bootloader detected\n\rProceed with bootloader upgrade?\n\r")
    answer = input("Enter Y/N : ")
    response = answer.lower()
    if response == "y":
        ser.close()
        os.system('eta-flash-tool --firmware-path "' + update_app + '"')
        print('\n\rApplying bootloader update...\n\r')
        if(platform.system() == 'Windows'):
            os.system('python "' + bl_download + '" ' + port + ' "' + frag_1 + '" "' + frag_2 + '"')
        else:
            os.system('python3 "' + bl_download + '" ' + port + ' "' + frag_1 + '" "' + frag_2 + '"')
        print('Bootloader updated')
        ser = serial.Serial(port, 460800)
        waitForBanner()
    elif response == 'n':
        exit(1)
    else:
        print('Invalid input', response)
        exit(1)

for x in range(len(binFileJustNames)):
    ymodem_send(binFileJustNames[x])

ymodem_sender.end_file()

ser.close()
