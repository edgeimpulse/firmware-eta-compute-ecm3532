################################################################################
#
# @file Makefile
#
# @brief This file contains the commands to build the keil CSP library.
#
# Copyright (C) 2018 Eta Compute, Inc
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

LIB_NAME     := eta_m3_csp
OUTPUT_DIR   := bin

.PHONY: all
all: $(LIB_NAME)

#
# Build a release.
#
release: all

#
# Clean the artifacts.
#
clean:
	rm -rf $(OUTPUT_DIR)

#
# Build the library.
#
$(LIB_NAME):
ifeq ($(OS),Windows_NT)
ifeq (, $(shell which UV4))
	$(info  No UV4 in PATH, add UV4 to PATH to build from the command line)
else
	@echo "Compiling ECM3532 Keil CSP..."
	@uv4 -j0 -cr $(LIB_NAME).uvprojx || (echo "Compile error! $?"; exit 1)
	@echo "Success..."
endif
else
	@echo "Not running on Windows, skipping Keil..."
endif

