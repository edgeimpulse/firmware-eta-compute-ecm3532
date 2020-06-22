################################################################################
#
# @file Makefile
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
# The main targert.
#
.PHONY: m3 dsp boards all
all: m3 dsp boards

#
# Make the M3 libraries.
#
m3:
	$(MAKE) -C m3

#
# Make the DSP libraries.
#
dsp:
	$(MAKE) -C m3

#
# Make the boards.
#
boards:
	$(MAKE) -C boards

clean:
	@for i in ${SUBDIRS}; do \
		$(MAKE) -C $${i} $@ || exit $? ;\
	done


