################################################################################
#
# @file Makefile
#
# @brief This file contains the commands to build the ECM3532 for
#        Embedded Studio.
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
# This is part of revision 1.0.0a6 of the Tensai Software Development Kit.
#
################################################################################
SHELL=/bin/bash

K := $(shell which emBuild)

ifeq ($(K),)
all clean:
	$(info emBuild tools not found, skipping...)
else
all:
	@echo "Compiling ECM3532 Embedded Studio CSP..."
	@emBuild -config "Release" eta_m3_csp.emProject || (echo "Compile error! $?"; exit 1)
	@echo "Success..."

clean:
	@echo Cleaning... ;
	@emBuild -config "Release" eta_m3_csp.emProject -clean
	@echo "Success..."
endif

