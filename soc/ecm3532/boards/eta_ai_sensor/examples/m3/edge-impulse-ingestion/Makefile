################################################################################
#
# @file Makefile
#
# @brief This file contains the commands to build the examples.
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

SUBDIRS := $(dir $(wildcard */[Mm]akefile))

#
# GCC CSP directory.
#
ETA_CSP_DIR=../../../../../m3/csp/lib/gcc/

#
# IDE dirs.
#
GCC_DIR=gcc
KEIL_DIR=keil
IAR_DIR=iar
ES_DIR=embedded_studio

#
# Check dependencies and build if necessary.
#
.PHONY: all gcc keil iar es
all: all gcc keil iar es

gcc:
	@echo
	@echo "Building GCC..."
	@echo "Check CSP lib"
	make -C $(ETA_CSP_DIR)
	@echo "Make gcc here"
	! test -d $(GCC_DIR) || $(MAKE) -C gcc
	@echo
	@echo "...GCC compilation finished"
	@echo

keil:
	@echo
	@echo "Building Keil..."
	! test -d $(KEIL_DIR) || $(MAKE) -C keil
	@echo
	@echo "...Keil compilation finished"
	@echo

iar:
	@echo
	@echo "Building IAR..."
	! test -d $(IAR_DIR) || $(MAKE) -C iar
	@echo
	@echo "...IAR compilation finished"
	@echo

es:
	@echo
	@echo "Building Embedded Studio..."
	! test -d $(ES_DIR) || $(MAKE) -C embedded_studio
	@echo
	@echo "...Embedded Studio compilation finished"
	@echo

#
# Clean.
#
clean:
	@for i in ${SUBDIRS}; do \
		$(MAKE) -C $${i} $@ || exit $? ;\
	done


