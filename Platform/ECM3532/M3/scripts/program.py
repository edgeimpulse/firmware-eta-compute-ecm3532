#!/usr/bin/env python3

################################################################################
#
# @file program
#
# @brief Program binary on Eta Compute evaluation board.
#
# Copyright (C) 2019 Eta Compute, Inc
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This is part of revision ${version} of the Tensai Software Development Kit.
#
################################################################################

import telnetlib
import argparse
import logging
import os
import platform
import re
import sys
import subprocess
import threading
import time
import tempfile

# Setup local logger
logger = logging.getLogger(__name__)
#logging.basicConfig(level=logging.DEBUG)

mem = {"ecm3531": {"flash_start": "0x01000000",
                     "flash_length": "512K",
                     "sram_start": "0x10000000",
                     "sram_length": "128K",
                     "magic0": {"addr": "0x1001FFF0", "val": "0xc001c0de"},
                     "magic1": {"addr": "0x1001FFF4", "val": "0xc001c0de"},
                     "magic2": {"addr": "0x1001FFF8", "val": "0xDEADBEEF"},
                     "magic3": {"addr": "0x1001FFFC", "val": "0xC369A517"}},

        "ecm3532": {"flash_start": "0x01000000",
                    "flash_app": "0x01006000",
                    "flash_length": "512K",
                    "sram_start": "0x10000000",
                    "sram_length": "256K",
                    "magic0": {"addr": "0x1003FFF0", "val": "0xc001c0de"},
                    "magic1": {"addr": "0x1003FFF4", "val": "0xc001c0de"},
                    "magic2": {"addr": "0x1003FFF8", "val": "0xDEADBEEF"},
                    "magic3": {"addr": "0x1003FFFC", "val": "0xC369A517"}}}


class JLinkError(Exception):
    """Class to represent an error from JLink.  Base-class for all JLink errors.
    """
    pass


class JLink:
    def __init__(self, connected, device, interface, speed, jlink_exe=None, jlink_path=""):
        """
        JLink interface class.

        Class for interfacing with Segger JLink USB interface.

        Keyword arguments:
        connected -- target specific messages returned when target has been successfully detected
        device -- target device name
        speed -- speed of connection in kHz
        jlink_exe -- name of JLink executable (deafult None = determined by OS)
        jlink_path -- path to JLink executable (default blank = assumes exe is in system path)

        Example usage:

        from jlink import JLink
        interface = JLink("Cortex-M3 r2p0, Little endian", "LPC1343",
                          "swd", "1000", jlink_path="/home/py/jlink/jlink_linux")
        if interface.is_connected():
            interface.program(["dummy.hex"])
        """
        self._connected = connected

        self.commands = []

        # Get JLink executable name
        if jlink_exe is None:
            system = platform.system()
            if system == "Windows":
                jlink_exe = "JLink.exe"
            else:
                jlink_exe = "JLinkExe"

        # Construct full path to JLinkExe tool
        self._jlink_path = os.path.join(jlink_path, jlink_exe)
        logger.info("Using path to JLinkExe: {0}".format(self._jlink_path))

        # Construct command line parameters
        temp_params = "-device {0} -if {1} -speed {2}".format(
            device, interface, speed)
        self._jlink_params = []
        self._jlink_params.extend(temp_params.split())
        logger.info("JLinkExe parameters: {0}".format(temp_params))

        # Check that specified executable exists
        self._test_jlinkexe()

    def _test_jlinkexe(self):
        """Check if JLinkExe is found at the specified path"""

        # Spawn JLinkExe process and raise an exception if not found
        args = [self._jlink_path]
        args.append("?")
        try:
            process = subprocess.Popen(args, stdout=subprocess.PIPE)
            process.wait()
            logger.info("Success")
        except OSError:
            raise JLinkError(
                "'{0}' missing. Ensure J-Link folder is in your system path.".format(self._jlink_path))

    def _run(self, cmd, timeout_sec):
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        try:
            outs, errs = proc.communicate(timeout=timeout_sec) # will raise error and kill any process that runs longer than 60 seconds
        except:
            proc.kill()
            outs, errs = proc.communicate()
        logger.info(outs)
        return outs

    def run_script(self, filename, timeout=60):
        """Run specified JLink script. Returns output of the JLinkExe process. If execution takes longer than specified amount of seconds, process is killed and an exception is thrown. Setting timeout to None disables the timeout check."""

        # Spawn JLinkExe process and capture its output
        args = [self._jlink_path]
        args.extend(self._jlink_params)
        args.append(filename)

        try:
            output = self._run(args, timeout)
        except:
            logger.debug("JLink response: {0}".format(output))
            raise JLinkError("Something went wrong when trying to execute the provided JLink script")

        return output

    def run_commands(self, timeout=30):
        """Run the provided list of commands. The provided commands should be a list of strings, which can be executed by JLink. If execution takes longer than specified amount of seconds, process is killed and an exception is thrown. Setting timeout to None disables the timeout check."""

        # Create temporary file for the run_script method
        temp_file = tempfile.NamedTemporaryFile(mode="w", delete=False)
        self.commands = "\n".join(self.commands)
        temp_file.write(self.commands)
        temp_file.close()
        logger.debug("Temporary script file name: {0}".format(temp_file.name))
        logger.debug("Running JLink commands: {0}".format(self.commands))
        self.commands = []
        return self.run_script(temp_file.name, timeout)

    def is_connected(self):
        """Returns true if the specified device is connected to the programmer. Device needs to be a string recognised by JLink, interface can be either S for SWD or J for JTAG."""
        self.add_command("connect")
        self.add_command("q")
        output = self.run_commands(timeout=50)
        return self._connected.encode() in output


    def erase(self):
        """Erase entire flash of the target."""

        self.reset()
        self.add_command("erase")
        print("erase done\r\n")
        self.reset()
        self.quit()

    def add_command(self, command):
        self.commands.append(command)

    def reset(self):
        #
        # HACK. Force a toggle of the reset line.
        #
        self.add_command("r0")
        self.sleep(250)
        self.add_command("r1")
        self.sleep(250)

    def halt(self):
        self.add_command("h")

    def wait_halt(self, timeout=4000):
        self.add_command("WaitHalt {}".format(timeout))

    def reset_halt(self):
        self.reset()
        self.halt()

    def reset_type(self, Rtype):
        self.add_command("RSetType {0}".format(Rtype))

    def quit(self):
        self.add_command("q")

    def sleep(self, time):
        self.add_command("sleep {}".format(time))

    def execute_commands(self, timeout=4000, program=False):
        output = self.run_commands(timeout=timeout)

        if(program):
            # Check if programming succeeded
            pass_messages = ["J-Link: Flash download: Total time needed:", "O.K."]
            write_failed = "Writing target memory failed.".encode()
            flash_match = "Skipped. Contents already match".encode()

            if write_failed in output:
                logger.info("Writing flash failed")
                return -1
            elif flash_match in output:
                logger.info("Target already programmed")
                return 0
            elif all(message.encode() in output for message in pass_messages):
                logger.info("Target programmed")
                return 0
            else:
                logger.info("Programming target failed")
                return -1

    def program(self, hex_files=[], bin_files=[]):
        """Program target with specified list of hex and/or bin files.
        hex_files is a list of paths to .hex files.
        bin_files is a list of tuples with the first value being the path to the .bin file and the second value being the integer starting address for the bin files"""

        # Construct a list of commands
        # self.commands.append("r")

        # Add each hex file
        for f in hex_files:
            f = os.path.abspath(f)
            self.commands.append('loadfile "{0}"'.format(f))

        # Add each bin file
        for f, addr in bin_files:
            f = os.path.abspath(f)
            self.commands.append('loadbin "{0}" 0x{1:08X}'.format(f, addr))

    def addr_write(self, addr, value):
        """Write target address to specified value.
        addr is the address.
        value is the value to write"""

        # configure the write
        # commands = ["h"]
        self.commands.append('w4 0x{0:08X}, 0x{1:08X}'.format(addr, value))

        # Run commands
        # self.run_commands(commands)


################################################################################
#
# Send command to OpenOCD.
#
################################################################################
def send_ocd_cmd(line):
    ocd_sock.write(bytes(line, encoding='utf-8'))
    print(ocd_sock.read_until(b'> ').decode('utf-8'), end='')

################################################################################
#
# Read response from OpenOCD.
#
################################################################################
def get_ocd_response():
    print(ocd_sock.read_until(b'> ').decode('utf-8'), end='')

################################################################################
#
# Main
#
################################################################################
if __name__ == "__main__":

    #
    # Parse arguments.
    #
    parser = argparse.ArgumentParser(description='Programming arguments')
    group = parser.add_argument_group()

    group.add_argument('--bin', help="binary")

    group.add_argument('--soc', choices=["ecm3531", "ecm3532"], default='ecm3532',
                        help="SOC")

    group.add_argument('--ide', choices=["keil", "iar", "gcc", "es"], default='gcc',
                        help="IDE")

    group.add_argument('--interface', choices=["jlink", "ocd", ""], default='jlink',
                        help="Interface to use (J-Link or OpenOCD)")

    group.add_argument('--type', choices=["f", "fbs", "s", "a"], default='f',
                       help="variant (f=flash, fbs=flash_boot_sram, s=sram, a=flash_app)")

    group.add_argument('-s', '--stop', action='store_true',
                        help="stop the core after programming")

    group.add_argument('-wfh', '--wait_for_halt', default='',
                        help="Start the J-Link server and wait for the core to halt")

    group.add_argument('-me', '--mass_erase', action='store_true',
                        help="mass erase the device")

    args = parser.parse_args()

    #
    # Mass erase.
    #
    mass_erase = args.mass_erase

    #
    # IDE
    #
    ide = args.ide
    if(ide == 'es'):
        ide = "embedded_studio"

    #
    # Halt?
    #
    if args.stop:
        stop = "halt\n"
    else:
        stop = "reset\n"

    #
    # Get path to project bin file.
    #
    cur_dir = os.getcwd()
    base = os.path.basename(cur_dir)

    #
    # Determine SoC
    #
    SoC = args.soc
    #if('ecm3531' in cur_dir):
    #    SoC = 'ecm3531'
    #else:
    #    SoC = 'ecm3532'
    print("Using {} SoC".format(SoC))

    #
    # Project variant.
    #
    if (args.type != ''):
        variant = args.type
        if(variant == 's'):
            variant = 'sram'
        elif variant == 'fbs':
            variant = 'flash_boot_sram'
        elif variant == 'a':
            variant = 'flash_app'
        else:
            variant = 'flash'

    #
    # Locate the binary.
    #
    if args.bin:
        work_dir = cur_dir + "/" + args.bin
        bin_file = work_dir

    else:
        #
        # Project variant.
        #
        if (variant == ''):
            if('flash_boot_sram' in cur_dir):
                variant = 'flash_boot_sram'
            elif ('sram' in cur_dir):
                variant = 'sram'
            else:
                variant = 'flash'

        if base == 'bin':
            work_dir = cur_dir
        elif base == variant:
            work_dir = cur_dir + '/bin'
        elif base == ide:
            work_dir = cur_dir + variant + '/bin'
        elif base == 'tmp':
            print("Not allowed in /tmp.")
        try:
            work_dir = cur_dir + '/' + ide + '/' + variant + '/bin'
            project_name = os.path.basename(work_dir.rsplit('/', 3)[0])
            elf_file = work_dir + '/' + project_name + '.elf'
            bin_file = work_dir + '/' + project_name + '.bin'
        except:
            print("\r\nERROR: Cant determine variant. Please specify a file " \
                  "to program or change your current directory to a project." \
                  "\r\n")
            exit()

    #
    # Make sure binary has the correct length.
    #
    if(args.interface == 'ocd'):
        if(mass_erase):
            ocd_commands = ["halt\n",
                            "flash erase_sector 0 0 127\n",
                            "reset\n",
                            ]
        else:
            print("\r\nProgramming file {} \r\nfile size".format(bin_file),os.stat(bin_file).st_size)
            if((os.stat(bin_file).st_size % 4) != 0):
                print("\n\nbin_file length not aligned fixing it\n\n")
                with open(bin_file, 'r+b') as fp:
                    out_data = bytearray(fp.read())
                out_data.extend(b'\x01\x02')
                with open(bin_file, 'wb') as fp:
                    fp.write(out_data)


            #
            # Commands to send.
            #
            if(variant == 'flash_boot_sram'):
                ocd_commands = ["halt\n",
                                "flash erase_sector 0 0 127\n",
                                "flash write_image {} {}\n".format(bin_file, int(mem[SoC]["flash_start"], 0)),
                                "etacorem3 store 0x50010000 0\n",
                                "mww {} 0\n".format(mem[SoC]["magic1"]["addr"]),
                                stop]
            elif variant == 'sram':
                ocd_commands = ["halt\n",
                                "load_image {} {}\n".format(bin_file, int(mem[SoC]["sram_start"], 0)),
                                "mww {} {}\n".format(mem[SoC]["magic2"]["addr"], mem[SoC]["magic2"]["val"]),
                                "mww {} {}\n".format(mem[SoC]["magic3"]["addr"], mem[SoC]["magic3"]["val"]),
                                stop]
            elif variant == 'flash_app':
                ocd_commands = ["halt\n",
                                "flash write_image erase {} {}\n".format(bin_file, int(mem[SoC]["flash_app"], 0)),
                                "mww {} 0\n".format(mem[SoC]["magic1"]["addr"]),
                                stop]
            else:
                ocd_commands = ["halt\n",
                                "flash write_image erase {} {}\n".format(bin_file, int(mem[SoC]["flash_start"], 0)),
                                "mww {} 0\n".format(mem[SoC]["magic1"]["addr"]),
                                stop]

        #
        # Connect to daemon and clean out buffer.
        #
        ocd_sock = telnetlib.Telnet(host='localhost', port=4444)
        get_ocd_response()

        #
        # Execute the commands.
        #
        for x in ocd_commands:
            print(x)
            send_ocd_cmd(x)

    else:
        interface = JLink("Cortex-M3 r2p1, Little endian.", "ECM3532", "swd", "1000")

        #
        # Is the J-Link connected?
        #
        if interface.is_connected():
            print("\r\nConnected to Target!\r\n")

            #
            # Mass erase?
            #
            if(mass_erase):
                print("Performing a mass erase\r\n")
                interface.erase()
                interface.reset()

            #
            # Wait for halt?
            #
            elif(args.wait_for_halt != ''):
                print("Waiting for halt\r\n")
                interface.wait_halt(args.wait_for_halt)

            #
            # Perform a normal program.
            #
            else:
                print("\r\nProgramming file {} \r\nfile size".format(bin_file),os.stat(bin_file).st_size)
                if((os.stat(bin_file).st_size % 4) != 0):
                    print("\n\nbin_file length not aligned fixing it\n\n")
                    with open(bin_file, 'r+b') as fp:
                        out_data = bytearray(fp.read())
                    out_data.extend(b'\x01\x02')
                    with open(bin_file, 'wb') as fp:
                        fp.write(out_data)


                interface.halt()
                if(variant == 'sram'):
                    interface.program(bin_files=[(bin_file, int(mem[SoC]["sram_start"], 0))])
                    interface.addr_write(int(mem[SoC]["magic2"]["addr"], 0), int(mem[SoC]["magic2"]["val"], 0))
                    interface.addr_write(int(mem[SoC]["magic3"]["addr"], 0), int(mem[SoC]["magic3"]["val"], 0))
                elif (variant == 'flash_app'):
                    print ("\n\rDownloading via flash app\n\r")
                    interface.program(bin_files=[(bin_file, int(mem[SoC]["flash_app"], 0))])
                    interface.addr_write(int(mem[SoC]["magic1"]["addr"], 0), 0)
                    interface.reset()
                else:
                    interface.program(bin_files=[(bin_file, int(mem[SoC]["flash_start"], 0))])
                    interface.addr_write(int(mem[SoC]["magic1"]["addr"], 0), 0)
                    interface.reset()

                if(args.stop):
                    #
                    # Reset and halt.
                    #
                    interface.reset_halt()
                else:
                    #
                    # Reset
                    #
                    interface.reset()

            #
            # Quit
            #
            interface.quit()

            #
            # Run the commands.
            #
            interface.execute_commands(4000)

        else:
            print("\r\nERROR: Not connected!\r\n")
