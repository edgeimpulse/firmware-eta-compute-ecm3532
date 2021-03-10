#!/usr/bin/python3

import os
import telnetlib

def send_ocd_cmd(line):
    ocd_sock.write(bytes(line,encoding = 'utf-8'))
    print(ocd_sock.read_until(b'> ').decode('utf-8'), end='')

def get_ocd_response():
    print(ocd_sock.read_until(b'> ').decode('utf-8'), end='')

#get hooked up to openocd daemon
ocd_sock = telnetlib.Telnet(host='localhost', port=4444)
get_ocd_response() # clean it out

# git path to project elf file
cur_dir = os.getcwd()
base = os.path.basename(cur_dir)
if base == 'freertos':
    work_dir = cur_dir + '/build'
elif base == 'scripts':
    work_dir = cur_dir + '/../build'
elif base == 'build':
    work_dir = cur_dir
else:
    print("you are in wrong directory  = ", cur_dir)
    print("run from freertos directory")
    exit(1)
elf_file = work_dir + '/ecm3532.elf'
print("elf_file = ",elf_file)


# use these to download and run the elf fle
ocd_commands = ["halt\n",
                "load_image {}\n".format(elf_file),
                "mww 0x1003FFF8 0xDEADBEEF\n",
                "mww 0x1003FFFC 0xC369A517\n",
                "reset\n"]

# OK now do what we came here for!!!
for x in ocd_commands:
    print(x)
    send_ocd_cmd(x)
