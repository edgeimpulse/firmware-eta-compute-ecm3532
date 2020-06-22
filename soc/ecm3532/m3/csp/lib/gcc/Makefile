################################################################################
#
# @file Makefile
#
# @brief This file contains the commands to build the gcc CSP library.
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
# This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
#
################################################################################

LIB_NAME     := eta_m3_csp.a
CC           := arm-none-eabi-gcc
CXX          := arm-none-eabi-g++
AR           := arm-none-eabi-ar
DEPEND_OPTS  := -MMD -MP
INCLUDES     := -I../../../../../common/inc \
                -I../../../../common/csp/inc \
                -I../../inc \
                -I../../../reg/inc
OPTIMIZATION := -O3
OPTS		 := -mcpu=cortex-m3 -mthumb -mlittle-endian $(OPTIMIZATION) \
	         -fmessage-length=0 -fsigned-char -ffunction-sections \
		 -fdata-sections -g3 -Werror -Wswitch -DPRTCC -std=gnu11 \
		 -DFIRMWARE_BUILD -falign-functions=4 \
                 -Wswitch -Wunused-variable -Wenum-compare -Wbool-operation \
                 -Wunused-but-set-variable -Wreturn-type \
		 -Wparentheses -Wmaybe-uninitialized -Wall \
                 -fno-strict-aliasing


CC_OPT      :=	-std=gnu11
CXX_OPT     :=	-std=c++11

SOURCES := $(wildcard ../../src/*.c)  \
           $(wildcard ../../src/*.cpp)  \
           $(wildcard ../../src/*.cc)  \
           $(wildcard ../../src/*.s)  \

INCLUDE_FILES := ../../../../../common/inc/* \
                ../../../../common/csp/inc/* \
                ../../inc/* \
                ../../../reg/inc/*

OBJECTS :=  $(patsubst ../../src/%.c,bin/%.o,$(wildcard ../../src/*.c))  \
            $(patsubst ../../src/%.s,bin/%.o,$(wildcard ../../src/*.s))  \

.PHONY: all bin
all: bin $(OBJECTS) $(LIB_NAME)

$(LIB_NAME): $(OBJECTS)
	@$(AR) rcs $@ $^ || (echo "Compile error! $?"; exit 1)
	@echo "Success..."


bin:
	@echo "Compiling ECM3532 GCC CSP..."
	@mkdir -p bin

clean:
	rm -f bin/*.d bin/*.o $(LIB_NAME)

# For C++ source files.
bin/%.o:	../../src/%.cpp $(INCLUDE_FILES)
	@mkdir -p $(dir $@)
	$(CXX) $(OPTS) $(CXX_OPTS) $(INCLUDES) -c $< -o $@
bin/%.o:	../../src/%.cc $(INCLUDE_FILES)
	@mkdir -p $(dir $@)
	$(CXX) $(OPTS) $(CXX_OPTS) $(INCLUDES) -c $< -o $@

# For C source files.
bin/%.o:	../../src/%.c $(INCLUDE_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(OPTS) $(CC_OPTS) $(DEPEND_OPTS) $(INCLUDES) -c $< -o $@

# For ASM source files.
bin/%.o:	../../src/%.s $(INCLUDE_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(OPTS) $(CC_OPTS) $(DEPEND_OPTS) $(INCLUDES) -c $< -o $@


vcast:
	cd vcast_work/regression_cov/; pwd; ./csp_coverage.csh 
	make

vcast_import:

vcast_clean:
	-cd vcast_work/regression_cov/; pwd; clicast -lc -e csp_coverage.vcp cover instrument none
	-cd vcast_work/regression_cov/; pwd; rm -r CCAST_.CFG csp_coverage commands.tmp *.vcp
	make clean
	make 

vcast_review:

