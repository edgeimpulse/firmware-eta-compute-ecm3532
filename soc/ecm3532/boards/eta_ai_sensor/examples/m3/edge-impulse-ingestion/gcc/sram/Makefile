################################################################################
#
# @file Makefile
#
# @brief This file contains the commands to build the edge_impulse_ingestion example.
#
# Copyright (C) 2020 Eta Compute, Inc
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
# This is part of revision 1.0.0a3 of the Tensai Software Development Kit.
#
################################################################################

TOP = ../../../../../../../..

PROJECT_NAME = edge_impulse_ingestion

CPU = cortex-m3

LINKER_SCRIPT =	ecm3532.ld
STARTUP_FILE = startup_gcc.c

COMPILER = arm-none-eabi

CC = $(COMPILER)-gcc
CXX = $(COMPILER)-g++
LD = $(COMPILER)-ld
AR = $(COMPILER)-ar
CP = $(COMPILER)-objcopy
OD = $(COMPILER)-objdump

DEFINES = -DECM3532 -DSRAM -DPROJ_NAME=\"edge_impulse_ingestion\" 

INCLUDES = -I../../inc
INCLUDES+= -I../../../../../../../m3/reg/inc
INCLUDES+= -I../../../../../../../m3/csp/inc
INCLUDES+= -I../../../../../../../common/csp/inc
INCLUDES+= -I../../../../../../../../../soc/common/inc
INCLUDES+= -I../../../../../../eta_evb/eta_bsp/inc
INCLUDES+= -I../../../../../eta_bsp/inc
INCLUDES+= -I../../../../../../../../../utils/inc
INCLUDES+= -I../../../../../../../../../devices/inc
INCLUDES+= -I../../../../../../../../../3rd_party/tiny_printf
INCLUDES+= -I../../../../../../../../../3rd_party/microrl/src

VPATH = ../../src
VPATH+= ../../../../../../eta_evb/eta_bsp/src
VPATH+= ../../../../../eta_bsp/src
VPATH+= ../../../../../../../../../utils/src
VPATH+= ../../../../../../../../../devices/src
VPATH+= ../../../../../../../../../3rd_party/tiny_printf
VPATH+= ../../../../../../../../../3rd_party/microrl/src

SRC = main.c
SRC+= eta_bsp.c
SRC+= eta_bsp_button.c
SRC+= eta_bsp_display.c
SRC+= eta_bsp_led.c
SRC+= eta_bsp_power.c
SRC+= eta_bsp_um2121.c
SRC+= eta_utils_state.c
SRC+= eta_utils_stdio.c
SRC+= eta_utils_terminal.c
SRC+= eta_devices_lsm6dso.c
SRC+= eta_devices_nhd0216.c
SRC+= printf.c
SRC+= microrl.c
SRC+= startup_gcc.c

C_SRC = $(filter %.c,$(SRC))
CPP_SRC = $(filter %.cpp,$(SRC))
CC_SRC = $(filter %.cc,$(SRC))
A_SRC = $(filter %.s,$(SRC))

OBJS = $(C_SRC:%.c=bin/%.o)
OBJS+= $(CPP_SRC:%.cpp=bin/%.o)
OBJS+= $(CC_SRC:%.cc=bin/%.o)
OBJS+= $(A_SRC:%.s=bin/%.o)

DEPS = $(C_SRC:%.c=bin/%.d)
DEPS+= $(CPP_SRC:%.cpp=bin/%.d)
DEPS+= $(CC_SRC:%.cc=bin/%.d)
DEPS+= $(A_SRC:%.s=bin/%.d)

M3_CSP_LIB_DIR = $(TOP)/ecm3532/m3/csp/lib/gcc
M3_CSP_LIB = $(M3_CSP_LIB_DIR)/eta_m3_csp.a

LIBS = $(M3_CSP_LIB)
LIBS+= 

##### Options #####
OPTS =	-mthumb -mcpu=$(CPU) -mlittle-endian -fmessage-length=0 -fsigned-char
OPTS+= -ffunction-sections -fdata-sections -g3 -fno-common -MMD -MP -Werror
OPTS+= -Ofast
OPTS+= $(DEFINES)
OPTS+= $(INCLUDES)
CC_OPTS = -std=gnu11
CXX_OPTS = -std=c++11 -U__STRICT_ANSI__

LD_OPTS	= -mthumb -mcpu=$(CPU) -mlittle-endian -Ofast
LD_OPTS+= -fsigned-char -ffunction-sections -fdata-sections -g3 -fno-common
LD_OPTS+= -nostartfiles -static --specs=nosys.specs -fmessage-length=0 -flto
LD_OPTS+= -v -Xlinker -Map=bin/lsm6dso.map -Wl,--gc-sections
LD_OPTS+= -Wl,--start-group $(LIBS) -Wl,--end-group

CP_OPTS = --verbose -Obinary

OD_OPTS = -S -d

##### Rules #####
.PHONY: all clean realclean
all: bin bin/$(PROJECT_NAME).bin

bin:
	mkdir -p $@

bin/%.o: %.c bin/%.d
	@echo " Compiling $<" with $(CC) ;\
	$(CC) -c $(OPTS) $(CC_OPTS) $< -o $@

bin/%.o: %.cpp bin/%.d
	@echo " Compiling $<" with $(CXX) ;\
	$(CXX) -c $(OPTS) $(CXX_OPTS) $< -o $@

bin/%.o: %.cc bin/%.d
	@echo " Compiling $<" with $(CXX) ;\
	$(CXX) -c $(OPTS) $(CXX_OPTS) $< -o $@

bin/%.o: %.s bin/%.d
	@echo " Assembling $<" with $(CC);\
	$(CC) -c $(OPTS) $(CC_OPTS) $< -o $@

bin/$(PROJECT_NAME).elf : $(OBJS) $(LIBS)
	@echo " Linking $@" with $(CXX);\
	$(CXX) -Wl,-T,$(LINKER_SCRIPT) -o $@ $(OBJS) $(LD_OPTS)

bin/$(PROJECT_NAME).bin: bin/$(PROJECT_NAME).elf
	@echo " Copying $@..." ;\
	$(CP) $(CP_OPTS) $< $@ ;\
	$(OD) $(OD_OPTS) $< > bin/$(PROJECT_NAME).txt

realclean clean:
	rm -f $(OBJS) $(DEPS) \
		bin/$(PROJECT_NAME).bin bin/$(PROJECT_NAME).elf \
		bin/$(PROJECT_NAME).txt bin/$(PROJECT_NAME).map

bin/%d: ;

$(M3_CSP_LIB):
	$(MAKE) -C $(M3_CSP_LIB_DIR)

ifeq ($(OS), Windows_NT)
debug:
	@powershell $(TOP)/../bin/gdb_debug.ps1 ECM3532 $(CURDIR)
else
debug:
	@$(TOP)/../bin/gdb_debug.sh "ECM3532" $(CURDIR)
endif

# Automatically include any generated dependencies
-include $(DEPS)


