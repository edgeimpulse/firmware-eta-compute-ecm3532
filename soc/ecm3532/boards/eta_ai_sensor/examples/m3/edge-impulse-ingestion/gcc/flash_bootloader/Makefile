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

DEFINES = -DECM3532 -DUSE_ICM20602 -DUSE_PDM_MIC -DUSE_BLE -DUSE_SPIFLASH -DFLASH -DPROJ_NAME=\"edge_impulse_ingestion\" -DEI_SENSOR_AQ_STREAM=FILE

INCLUDES = -I../../inc
INCLUDES+= -I../../../../../../../m3/reg/inc
INCLUDES+= -I../../../../../../../m3/csp/inc
INCLUDES+= -I../../../../../../../common/csp/inc
INCLUDES+= -I../../../../../../../../../soc/common/inc
INCLUDES+= -I../../../../../eta_bsp/inc
INCLUDES+= -I../../../../../../../../../utils/inc
INCLUDES+= -I../../../../../../../../../devices/inc
INCLUDES+= -I../../../../../../../../../3rd_party/tiny_printf
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/ingestion-sdk-platform/eta-compute/
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/repl
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/ingestion-sdk-c
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/mbedtls_hmac_sha256_sw
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/mbedtls_hmac_sha256_sw/mbedtls
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/QCBOR/inc
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Include
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/Core/Include
INCLUDES+= -I../../src/sensors
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/anomaly
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/classifier
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/Core/Include
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Include
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/PrivateInclude
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Include
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp/spectral
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp/speechpy
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp/kissfft
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/porting
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/flatbuffers/include
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/gemmlowp
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/ruy
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/ruy/ruy
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/ruy/ruy/profiler
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/c
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/kernels
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/kernels/internal
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/core/api
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/micro
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/micro/memory_planner
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/micro/kernels
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/tflite-model
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse/model-parameters
INCLUDES+= -I../../../../../../../../../3rd_party/edge_impulse

VPATH = ../../src
VPATH+= ../../../../../eta_bsp/src
VPATH+= ../../../../../../../../../utils/src
VPATH+= ../../../../../../../../../devices/src
VPATH+= ../../../../../../../../../3rd_party/tiny_printf
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/ingestion-sdk-platform/eta-compute/
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/repl
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/ingestion-sdk-c
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/QCBOR
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/mbedtls_hmac_sha256_sw
VPATH+= ../../src/sensors
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/QCBOR/src
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/mbedtls_hmac_sha256_sw/mbedtls/src
VPATH+= ../../../../dsp/raw_audio/dsp_loader_image
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/anomaly
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/classifier
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/Core
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/BasicMathFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/BayesFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/ComplexMathFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/ControllerFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/DistanceFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/FilteringFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/SVMFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/ActivationFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/BasicMathFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/ConcatenationFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/ConvolutionFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/FullyConnectedFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/NNSupportFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/PoolingFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/ReshapeFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/CMSIS/NN/Source/SoftmaxFunctions
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp/dct
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp/spectral
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp/speechpy
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/dsp/kissfft
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/porting
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/porting/ecm3532
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/flatbuffers/include
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/gemmlowp
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/ruy
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/ruy/ruy
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/third_party/ruy/ruy/profiler
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/c
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/kernels
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/kernels/internal
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/core/api
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/micro
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/micro/memory_planner
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/micro/kernels
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow/lite/micro/testing
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/tflite-model
VPATH+= ../../../../../../../../../3rd_party/edge_impulse/model-parameters

SRC = main.cpp
SRC+= eta_bsp.c
SRC+= eta_bsp_button.c
SRC+= eta_bsp_led.c
SRC+= eta_bsp_power.c
SRC+= eta_utils_state.c
SRC+= eta_utils_stdio.c
SRC+= eta_utils_terminal.c
SRC+= eta_devices_icm20602.c
SRC+= eta_devices_spiflash.c
SRC+= eta_devices_a31r118.c
SRC+= eta_devices_nhd0216.c
SRC+= printf.c
SRC+= startup_gcc.c
SRC+= ei_device_eta_ecm3532.cpp
SRC+= repl.cpp
SRC+= ei_eta_fs_commands.cpp
SRC+= sensor_aq_mbedtls_hs256.cpp
SRC+= sensor_aq.cpp
SRC+= ei_sampler.cpp
SRC+= ei_inertialsensor.cpp
SRC+= ei_microphone.cpp
SRC+= ei_run_impulse.cpp
SRC+= qcbor_encode.c
SRC+= UsefulBuf.c
SRC+= ieee754.c
SRC+= dsp_fw.c
SRC+= ei_classifier_porting.cpp
SRC+= debug_log.cpp
SRC+= memory.cpp
SRC+= kiss_fft.cpp
SRC+= kiss_fftr.cpp
SRC+= fast-dct-fft.cpp
SRC+= arm_q15_to_float.c
SRC+= $(shell find ../../../../../../../../../3rd_party/edge_impulse/mbedtls_hmac_sha256_sw/mbedtls/src -name "*.c" | rev | cut -d"/" -f1 | rev)
SRC+= $(shell find ../../../../../../../../../3rd_party/edge_impulse/edge-impulse-sdk/tensorflow -name "*.c*" | rev | cut -d"/" -f1 | rev)

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
OPTS+= -O3
OPTS+= $(DEFINES)
OPTS+= $(INCLUDES)
CC_OPTS = -std=gnu11
CXX_OPTS = -std=c++11 -U__STRICT_ANSI__

LD_OPTS	= -mthumb -mcpu=$(CPU) -mlittle-endian -O3
LD_OPTS+= -fsigned-char -ffunction-sections -fdata-sections -g3 -fno-common
LD_OPTS+= -nostartfiles -static --specs=nosys.specs -fmessage-length=0 -flto
LD_OPTS+= -v -Xlinker -Map=bin/edge_impulse_ingestion.map -Wl,--gc-sections
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
