.PHONY: clean all
.SUFFIXES:
.SUFFIXES: .c .o  .h .a

CC = chesscc
SETUPSCRIPT     := /tools/synopsys/asip_programmer_O-2018.09-SP3-5/linux64/chess_env_LNa64.sh
CF16_LIB_LOC    := /tools/dsp_software/CoolFlux/cf16_vO-2018.09-1/lib
CFLAGS = $(MYCFLAGS) -P $(CF16_LIB_LOC) -p cf16 +s +W noodle,+c +W llvm,-O1
REGS_PATH       := ../../../reg/inc/
INCDIRS = -I../inc

NXPLIB_LIB = libnxplib.a
OBJDIR_NXPLIB=obj_nxplib
PATH_TO_NXPLIB=../src
SOURCES_NXPLIB=$(shell ls $(PATH_TO_NXPLIB)/*.c)
OBJECTS_NXPLIB=$(patsubst %.c,$(OBJDIR_NXPLIB)/%.o,$(notdir $(SOURCES_NXPLIB)))


all: $(NXPLIB_LIB) 

$(OBJDIR_NXPLIB)/%.o: $(PATH_TO_NXPLIB)/%.c
	source $(SETUPSCRIPT); $(CC) -o $@ -c $(CFLAGS) $(INCDIRS) $< 

$(NXPLIB_LIB): $(OBJECTS_NXPLIB)
	@echo Creating NXP library
	@source $(SETUPSCRIPT); $(CC) -a -o $(NXPLIB_LIB) $(OBJECTS_NXPLIB) $(CFLAGS)

clean:
	@-@/bin/rm -rf $(OBJDIR_NXPLIB) $(NXPLIB_LIB)
	@-@/bin/rm -rf *.a.mu

