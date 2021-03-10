.PHONY: clean all
.SUFFIXES:
.SUFFIXES: .c .o  .h .a

CC = chesscc
SETUPSCRIPT     := /tools/synopsys/asip_programmer_O-2018.09-SP3-5/linux64/chess_env_LNa64.sh
CF16_LIB_LOC    := /tools/dsp_software/CoolFlux/cf16_vO-2018.09-1/lib
PATH_TO_MATHLIB := /tools/dsp_software/CoolFlux/cf16_vO-2018.09-1/NXP/mathlib/src
CFLAGS = $(MYCFLAGS) -P $(CF16_LIB_LOC) -p cf16 +s +W noodle,+c +W llvm,-O1
REGS_PATH       := ../../../reg/inc/
INCDIRS = -I../inc -I../../nxp/inc

MATHLIB_LIB = libmathlib.a
OBJDIR_MATHLIB=obj_mathlib
SOURCES_MATHLIB=$(shell ls $(PATH_TO_MATHLIB)/*.c)
OBJECTS_MATHLIB=$(patsubst %.c,$(OBJDIR_MATHLIB)/%.o,$(notdir $(SOURCES_MATHLIB)))

all: $(MATHLIB_LIB)

$(OBJDIR_MATHLIB)/%.o: $(PATH_TO_MATHLIB)/%.c
	source $(SETUPSCRIPT); $(CC) -o $@ -c $(CFLAGS) $(INCDIRS) $< 

$(MATHLIB_LIB): $(OBJECTS_MATHLIB)
	@echo Creating Mathlib library
	@source $(SETUPSCRIPT); $(CC) -a -o $(MATHLIB_LIB) $(OBJECTS_MATHLIB) $(CFLAGS)

clean:
	@-@/bin/rm -rf $(OBJDIR_MATHLIB) $(MATHLIB_LIB)
	@-@/bin/rm -rf *.a.mu

