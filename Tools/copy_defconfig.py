#!/usr/bin/env python

import sys	
import shutil
# import binascii
import os
# import struct
# import serial.tools.list_ports
# import glob
# import inquirer

src     = sys.argv[1]
cwd = os.getcwd()

# print(os.path.abspath(src))
# print(os.path.abspath(cwd))

src_files = os.listdir(src)
for file_name in src_files:
    if not file_name.startswith('.'):
        full_file_name = os.path.join(src, file_name)
        dest = cwd + "/" + file_name
        if os.path.exists(dest):
            os.remove(dest)
        os.symlink(full_file_name, dest)