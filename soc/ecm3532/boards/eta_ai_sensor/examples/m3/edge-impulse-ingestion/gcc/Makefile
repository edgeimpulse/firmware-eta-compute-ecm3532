################################################################################
#
# @file Makefile
#
# @brief This file contains the commands to build the GCC example.
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

#
# Subdirectories.
#
SUBDIRS := $(dir $(wildcard */[Mm]akefile))


all realclean clean: $(SUBDIRS)
	@echo "Completed gcc make"
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
.PHONY: $(SUBDIRS) all realclean clean

