#!/usr/bin/perl
#
# Simple program to take the DSP pmem, xmem and ymem files (like verilog readmem files but
# with no leading @ADDR) and convert them into a c includeable header file.
#
# John   11-2-2017
#

if (@ARGV != 1)
{
  printf STDERR ("Usage:  create_dsp_include.pl PROJECT_NAME\n");
  printf STDERR ("        will look for bin/pmem.hex, xmem.hex, ymem.hex\n");
  printf STDERR ("        and will write c include into directory include\n");
  exit (1);
};


$PROJECT_NAME = @ARGV[0];
open(C_FILE, "> dsp_fw.c");


printf STDOUT ("Making dsp_fw.c for $PROJECT_NAME\n");


print C_FILE <<END_OF_HEADER;

////////////////////////////////////////////////////////////////////////////
// The confidential and proprietary information contained in this file may
// only be used by a person authorized under and to the extent permitted
// by a subsisting licensing agreement from Eta Compute, Inc
//
// Copyright (C) 2018 Eta Compute, Inc, All Rights Reserved
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Eta Compute, Inc.
////////////////////////////////////////////////////////////////////////////

// This is dsp_fw.c for DSP project $PROJECT_NAME

#include <stdint.h>
#include "eta_csp_dsp.h"


END_OF_HEADER


foreach $MEM ("pmem", "xmem", "ymem")
{
  $FILE = uc($MEM);
  $FILE = "bin/$PROJECT_NAME.elf.$FILE";

  if ((open (INPUT_FILE, $FILE)) == 0) {
    printf STDERR ("ERROR:  Could not open \"$FILE\"\n");
    exit (1);
  }

  $LINE_ADDR=0;
  $BYTE0=0;
  $BYTE1=0;
  $BYTE2_ON=0;
  $WORD_COUNT=0;
  $CURR_ADDR=0;
  $LINE_ADDR=0;
  printf C_FILE ("\n");
  if ($MEM eq "pmem")
  {
    printf C_FILE ("static const uint32_t $MEM\_image_data [] = {\n");
  }
  else
  {
    printf C_FILE ("static const uint16_t $MEM\_image_data [] = {\n");
  }
  while (<INPUT_FILE>)
  {
    s/\n//;

    $LINE=$_;

    ## Data will look like @ADDR HEXHEXHEX with either 16 bit or 32 bit words.
    ## We'll parse everything the same, but X/YMEM need endianess swapping.
    if (/@([0-9a-fA-F]*)\s+([0-9a-fA-F]*)/)
    {
      $LINE_ADDR=hex($1);
      $LINE=$2;
    }
    elsif (/\@0+/)
    {
      next(); ## drop this empty line.
    }

    if ($CURR_ADDR < $LINE_ADDR)
    {
      printf C_FILE ("// Filling in dummy data from Addr:%x to %x\n", $CURR_ADDR, $LINE_ADDR-1);
      ## First, fill in missing spaces if they jump in verilog file.
      while ($CURR_ADDR < $LINE_ADDR)
      {
        if ($WORD_COUNT == 0)
        {
          printf C_FILE ("    0x00");
        }
        else
        {
          printf C_FILE (",\n    0x00");
        }
        $WORD_COUNT++;
        $CURR_ADDR++;
      }
    }

    if ($WORD_COUNT == 0)
    {
      printf C_FILE ("    0x$LINE");
    }
    else
    {
      printf C_FILE (",\n    0x$LINE");
    }
    $WORD_COUNT++;
    $CURR_ADDR++;

  };

  ## If we are here and word count sitll emtpy, array is empty. Add 1 entry or else we get badness
  if ($WORD_COUNT==0)
  {
    printf C_FILE ("    0x0 //EMPTY\n");
    $WORD_COUNT++;
  }
  printf C_FILE ("};\n");
  printf C_FILE ("\n");
  $MEM_SIZE{$MEM} = $WORD_COUNT;

}

printf C_FILE ("#define DSP_PMEM_IMAGE_SIZE (%d)\n", $MEM_SIZE{pmem});
printf C_FILE ("#define DSP_XMEM_IMAGE_SIZE (%d)\n", $MEM_SIZE{xmem});
printf C_FILE ("#define DSP_YMEM_IMAGE_SIZE (%d)\n", $MEM_SIZE{ymem});
printf C_FILE ("\n");
printf C_FILE ("#define DSP_IMAGE_NAME \"%s\"\n", $PROJECT_NAME);
printf C_FILE ("\n");
printf C_FILE ("const tDspMem dsp_mem = { .pcImageName = \"%s\",\n", $PROJECT_NAME);
printf C_FILE ("                          .ui32PmemSize  = DSP_PMEM_IMAGE_SIZE ,  \n");
printf C_FILE ("                          .ui32XmemSize  = DSP_XMEM_IMAGE_SIZE ,  \n");
printf C_FILE ("                          .ui32YmemSize  = DSP_YMEM_IMAGE_SIZE ,  \n");
printf C_FILE ("                          .pui32PmemData  = &pmem_image_data[0] ,  \n");
printf C_FILE ("                          .pui16XmemData  = &xmem_image_data[0] ,  \n");
printf C_FILE ("                          .pui16YmemData  = &ymem_image_data[0] } ;\n");
printf C_FILE ("\n");
printf C_FILE ("\n");
