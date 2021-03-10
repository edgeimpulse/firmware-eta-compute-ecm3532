#!/usr/bin/perl
#
# Simple program to take the DSP pmem, xmem and ymem files (like verilog readmem files but
# with no leading @ADDR) and convert them into a c includeable header file.
#
# John   11-2-2017
#

if (@ARGV != 2)
{
  printf STDERR ("Usage:  create_dsp_include.pl PROJECT_NAME\n");
  printf STDERR ("        will look for bin/pmem.hex, xmem.hex, ymem.hex\n");
  printf STDERR ("        and will write c include into directory include\n");
  exit (1);
};


$PROJECT_NAME = @ARGV[0];
$SECTION_NAME = @ARGV[1];
open(C_FILE, "> dsp_fw.c");

printf STDOUT ("Making dsp_fw.c (ECM3532) for $PROJECT_NAME\n");


print C_FILE <<END_OF_HEADER;

/***************************************************************************//**
 *
 * \@file dsp_fw.c
 *
 * \@brief This is dsp_fw.c (ECM3532) for DSP project $PROJECT_NAME
 *
 * Copyright (C) \${year} Eta Compute, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This is part of revision \${version} of the Tensai Software Development Kit.
 *
 ******************************************************************************/

#include <stdint.h>
#include "eta_csp_dsp.h"
END_OF_HEADER


foreach $MEM ("pmem")
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
  printf C_FILE ("static const uint32_t $MEM\_image_data [] __attribute__((section(\"%s\"))) = {\n",$SECTION_NAME);
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
      if ( ! length $LINE )
      {
        next(); ## drop this line.
      }
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

## XMEM / YMEM are output 1 byte, so they need some processing to work.
foreach $MEM ("xmem", "ymem")
{
  $FILE = uc($MEM);
  $FILE = "bin/$PROJECT_NAME.elf.$FILE";

  if ((open (INPUT_FILE, $FILE)) == 0) {
    printf STDERR ("ERROR:  Could not open \"$FILE\"\n");
    exit (1);
  }

  printf C_FILE ("\n");
  printf C_FILE ("static const uint16_t $MEM\_image_data []  __attribute__((section(\"%s\"))) = {\n",$SECTION_NAME);

  $LINE_ADDR=0;
  $LINE_ODD=0;
  $LINE_BYTE=0;
  $PREV_BYTE=0;
  $WORD_COUNT=0;
  $CURR_ADDR=0;

  while (<INPUT_FILE>)
  {
    s/\n//;

    $LINE=$_;

    ## Data will look like @ADDR HEXHEXHEX with either 16 bit or 32 bit words.
    ## We'll parse everything the same, but X/YMEM need endianess swapping.
    if (/@([0-9a-fA-F]*)\s+([0-9a-fA-F]*)/)
    {
      $LINE_ADDR = (hex $1) >> 1;
      $LINE_BYTE = hex $2;
      $LINE_ODD  = (hex $1) & 1;
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
        #printf C_FILE ("//  %d %x  %d %x", $CURR_ADDR, $CURR_ADDR, $LINE_ADDR, $LINE_ADDR);
      }
    }

    if ($LINE_ODD)
    {
      if ($WORD_COUNT == 0)
      {
        printf C_FILE ("    0x%0.2x%0.2x", $LINE_BYTE, $PREV_BYTE);
      }
      else
      {
        printf C_FILE (",\n    0x%0.2x%0.2x", $LINE_BYTE, $PREV_BYTE);
      }
      $WORD_COUNT++;
      $CURR_ADDR++;
    }
    else
    {
      $PREV_BYTE = $LINE_BYTE;
    }
  }

  ## After we are done, if there is an even byte present, lets print it ( we print it twice because we need a filler)
  if ($LINE_ODD)
  {
  }
  else
  {
    if ($WORD_COUNT == 0)
    {
      printf C_FILE ("    0x%0.2x%0.2x", $PREV_BYTE, $PREV_BYTE);
    }
    else
    {
      printf C_FILE (",\n    0x%0.2x%0.2x", $PREV_BYTE, $PREV_BYTE);
    }
    $WORD_COUNT++;
    $CURR_ADDR++;
  }



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
printf C_FILE ("const tDspMem dsp_mem __attribute__((section(\"%s\"))) = { .pcImageName = \"%s\",\n", $SECTION_NAME,$PROJECT_NAME);
printf C_FILE ("                          .ui32PmemSize  = DSP_PMEM_IMAGE_SIZE ,  \n");
printf C_FILE ("                          .ui32XmemSize  = DSP_XMEM_IMAGE_SIZE ,  \n");
printf C_FILE ("                          .ui32YmemSize  = DSP_YMEM_IMAGE_SIZE ,  \n");
printf C_FILE ("                          .pui32PmemData  = &pmem_image_data[0] ,  \n");
printf C_FILE ("                          .pui16XmemData  = &xmem_image_data[0] ,  \n");
printf C_FILE ("                          .pui16YmemData  = &ymem_image_data[0] } ;\n");
printf C_FILE ("\n");
printf C_FILE ("\n");
