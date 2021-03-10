import os
import sys

dev = sys.argv[1]

os.system("eta-flash-tool --firmware-path bootloader_update_tools/bootloader_update_app.bin")
os.system("python3 bootloader_update_tools/bl_download.py bootloader_update_tools/ 1 " + dev)