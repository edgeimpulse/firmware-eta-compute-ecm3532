#!/usr/bin/python3
import os
import telnetlib
import argparse
import platform

def send_ocd_cmd(line):
    ocd_sock.write(bytes(line,encoding = 'utf-8'))
    print(ocd_sock.read_until(b'> ').decode('utf-8'), end='')

def get_ocd_response():
    print(ocd_sock.read_until(b'> ').decode('utf-8'), end='')

#get hooked up to openocd daemon
ocd_sock = telnetlib.Telnet(host='localhost', port=4444)
get_ocd_response() # clean it out
parser = argparse.ArgumentParser(description='Programming arguments')
group = parser.add_argument_group()
group.add_argument('--soc', choices=["ecm3531", "ecm3532"], default='ecm3531',
        help="SoC")
args = parser.parse_args()

SoC = args.soc

if SoC == "ecm3532":
    ocd_commands = ["halt\n",
                "flash erase_sector 0 0 127\n",
                "mww 0x1003fff8 0\n",
                "mdw 0x01000000 16\n",
                "reset\n"]
else:
    ocd_commands = ["halt\n",
                "flash erase_sector 0 0 127\n",
                "mww 0x1001fff8 0\n",
                "mdw 0x01000000 16\n",
                "reset\n"]

# OK now do what we came here for!!!
for x in ocd_commands:
    print(x)
    send_ocd_cmd(x)
