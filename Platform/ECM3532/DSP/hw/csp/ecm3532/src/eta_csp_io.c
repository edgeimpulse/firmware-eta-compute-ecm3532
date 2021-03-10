/***************************************************************************//**
 *
 * @file eta_csp_io.c
 *
 * @brief This file contains eta_csp_io module implementations.
 *
 * Copyright (C) 2020 Eta Compute, Inc
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
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ecm3532io-dsp Input/Output (IO)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include "reg.h"
#include "eta_csp_io.h"
#include "eta_csp_mailbox.h"
#include "eta_chip.h"
#include "eta_csp_mailbox_cmd.h"

// #ifndef DSP_PRINT_BUFFER_SIZE
// #define DSP_PRINT_BUFFER_SIZE (256)
// #endif
// #ifndef DSP_PRINT_BUFFER_USABLE
// #define DSP_PRINT_BUFFER_USABLE (256)
// #endif
// #ifndef DSP_PRINT_BUFFER_IN_YMEM
// #define DSP_PRINT_BUFFER_IN_YMEM (0)
// #endif

//////////////////////////////////////////////////////////////////////
// Run time control of printf targets
#ifdef PRINT_VIA_M3_MBOX
static bool bPrintViaM3Mbox = true;
#else
static bool bPrintViaM3Mbox = false;
#endif

#ifdef PRINT_VIA_DSP_UART
static bool bPrintViaDspUart = true;
#else
static bool bPrintViaDspUart = false;
#endif


// These functions select printf, puts, putc targets.
// More than one can be enabled for any application.

/*************************************************************************//**
 *
 *  EtaCspIoPrintfViaM3Mbox - Enable M3 Mailbox as a printf target.
 *
 *  @param bEnable - set to true to print via the M3 Mailbox.
 *
 ****************************************************************************/
void
EtaCspIoPrintfViaM3Mbox(bool bEnable)
{
    bPrintViaM3Mbox = bEnable;
}

/***************************************************************************//**
 *
 * EtaCspIoPrintfViaM3MboxGet - return enable state
 *
 * @return bool
 *
 ******************************************************************************/
bool
EtaCspIoPrintfViaM3MboxGet(void)
{
    return(bPrintViaM3Mbox);
}

/*************************************************************************//**
 *
 *  EtaCspIoPrintfViaDspUart - Enable DSP Uart as a printf target.
 *
 *  @param bEnable - set to true to print via the DSP UART
 *
 * The DSP UART is also known as the back door UART.
 ****************************************************************************/
void
EtaCspIoPrintfViaDspUart(bool bEnable)
{
    bPrintViaDspUart = bEnable;
}

/***************************************************************************//**
 *
 * EtaCspIoPrintfViaDspUartGet - return enable state
 *
 * @return bool
 *
 ******************************************************************************/
bool
EtaCspIoPrintfViaDspUartGet(void)
{
    return(bPrintViaDspUart);
}

//
// Run time control of printf targets
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Turn on / off all prints
// If verbose is 1, prints are enabled. This is usually done inside
// main, but can be done here.
bool g_bEtaCspIoVerbose = true; // By default, print all debug.

// This function may be called any time after init_tb();

/***************************************************************************//**
 *
 *  EtaCspIoVerboseEnable - Enables global variable allowing prints
 *
 ******************************************************************************/
void
EtaCspIoVerboseEnable(void)
{
    g_bEtaCspIoVerbose = true;
}

/***************************************************************************//**
 *
 *  EtaCspIoVerboseDisable - Disables global variable allowing silent dropping
 * of prints
 *
 ******************************************************************************/
void
EtaCspIoVerboseDisable(void)
{
    g_bEtaCspIoVerbose = true;
}

// Turn on / off all prints
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// PRINT_VIA_M3_MBOX
// These functions are needed only when the DSP is printing to a main
// UART via the M3 / Mailbox commands.
//
// A single printf (or puts) from the DSP should not exceed
// DSP_PRINT_BUFFER_SIZE
// or there will be a total lockup.
//
// strings printed by the DSP. The M3 then sends it to the UART.
#if DSP_PRINT_BUFFER_IN_YMEM
uchar chess_storage(YMEM) glob_printbuff_data [DSP_PRINT_BUFFER_SIZE];
#else
uchar chess_storage(XMEM) glob_printbuff_data [DSP_PRINT_BUFFER_SIZE];
#endif

volatile int glob_printbuff_wrptr; // Print buffer write pointer
volatile int glob_printbuff_rdptr; // Print buffer read pointer
int glob_printbuff_msgstart; // Local variable to store off current
                             // wrptr

/***************************************************************************//**
 *
 *  eta_csp_printviam3_inc_indx - Circular increment of write or read ptr by 1
 *
 *  @param curr_ptr - Current array pointer value
 *
 *  @return pointer incremented by 1
 *
 ******************************************************************************/

int
eta_csp_printviam3_inc_indx(int curr_ptr)
{
    if(curr_ptr == (DSP_PRINT_BUFFER_USABLE - 1))
    {
        curr_ptr = 0;
    }
    else
    {
        curr_ptr++;
    }
    return(curr_ptr);
}

/***************************************************************************//**
 *
 *  eta_csp_printviam3_start_message - Called at the beginning of print to
 *                                     record current wrptr
 *
 ******************************************************************************/

void
eta_csp_printviam3_start_message(void)
{
    // Store the start of this print message
    glob_printbuff_msgstart = glob_printbuff_wrptr;

    return;
}

/***************************************************************************//**
 *
 *  eta_csp_printviam3_end_message - Ends the process of sending print to M3 via
 *                                   mailbox (i.e. cleanup and send mailbox)
 *
 ******************************************************************************/

void
eta_csp_printviam3_end_message(void)
{
    // We need to "print" end of string so we can deliminate commands
    etaPutc('\0');

    uint16_t mailbox_cmd;
    uint32_t mailbox_message;
    int byteaddress;

    // DSP byte address is confusing.
    // First things first, we need to find address of glob_printbuff_data which
    // will be in bytes.
    // Then we take the index, which is also in bytes.
    // But, to make it easier for M3, we'll tell the the system address. The DSP
    // expresses a 16 bit halfword
    // in a 32 bit system address space. So this means that when we specify the
    // starting address, we'll always have
    // and extra 0 inseted at bit position[1].
    //
    // byteaddress = globaddr + msgstart
    // systemaddr = byteaddress[14:1],1'b0,byteaddress[0];

    byteaddress = ((int)&glob_printbuff_data[0]) + glob_printbuff_msgstart;

    // This is byte aligned, 32 bit AHB address
    mailbox_message =
        DSP_PRINT_BUFFER_IN_YMEM ? DSP_YMEM_START : DSP_XMEM_START;
    mailbox_message |= ((byteaddress & ~0x1) << 1);
    mailbox_message |= ((byteaddress &  0x1));

    mailbox_cmd = MAILBOX_DSP2M3CMD_PRINTVIAM3;

    // Let the m3 know we want to print something.
    EtaCspMboxDsp2M3(mailbox_cmd, mailbox_message);

    return;
}

/***************************************************************************//**
 *
 *  eta_csp_printviam3_putc - PUTC variant of printf specific for Print via M3
 *
 *  @param ui8Byte - character to print
 *
 *  Note this funciton will stall until space is available in the buffer and
 *  may stall forever if space is never available.
 *
 ******************************************************************************/

void
eta_csp_printviam3_putc(uchar ui8Byte)
{
    int new_wrptr;
    int byteaddress;
    int mailbox_16bit_message;
    uint32_t mbox_message;
    uchar tempchar;

    // If we get to end of DSP buffer, we jump back to the beginning.
    // We "hardcode" a constant at the end of the buffer that is the
    // "stop, jump back to address xyz" so from putc perspective, we
    // simply push onto circular buffer.

    // He we check to make sure we have space to print. If we don't, we stall.
    // OPTME, permenant silent stall... what can we do better here?
    // We could print to the DSP/DEBUG UART, but there is no reasonable
    // expected time that the M3 would print (M3 may be busy). Maybe we could
    // wait 1 sec, but we have no mechanism to determine what 1 sec is, so
    // we'd have to assume 1 sec at slowest and fastest DSP freq, which is a
    // timeout of 100s of millions of counts.... feels like a cluster to
    // do nothing but maybe print if we are enabled... leave as is.
    new_wrptr = eta_csp_printviam3_inc_indx(glob_printbuff_wrptr);
    while(new_wrptr == glob_printbuff_rdptr) // Stall forever (interrupt should
                                             // replenish credits)
    {
    }
    glob_printbuff_data [glob_printbuff_wrptr] = ui8Byte;
    glob_printbuff_wrptr = eta_csp_printviam3_inc_indx(glob_printbuff_wrptr);
}

/***************************************************************************//**
 *
 *  EtaCspPrintViaM3ProcessPrintCredit - processes M3 print credits
 *
 *  @param credit - indicates number of characters M3 prints
 *
 ******************************************************************************/
void
EtaCspPrintViaM3ProcessPrintCredit(int credit)
{
    int temp_rdptr;

    temp_rdptr = glob_printbuff_rdptr + credit;

    // Modulo - temp_rdptr = temp_rdptr % (DSP_PRINT_BUFFER_USABLE);

    if(temp_rdptr >= DSP_PRINT_BUFFER_USABLE)
    {
        temp_rdptr = temp_rdptr - DSP_PRINT_BUFFER_USABLE;
    }

    glob_printbuff_rdptr = temp_rdptr;

    // We could use this to increment, but we can do it in one add vs X inc by 1
    // for (index0=0; index0 < credit; index0++)
    // {
    // glob_printbuff_rdptr = eta_csp_printviam3_inc_indx
    // (glob_printbuff_rdptr);
    // }
}

// PRINT_VIA_M3_MBOX
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Print functions

/***************************************************************************//**
 *
 *  etaBaseConvert - print function. converts int to string of char based off of
 * base.
 *
 *  @param num - raw hexadecimal number (int)
 *  @param base - base to print (usually 8,10,16)
 *  @return pointer to string of characters ready for print.
 *
 ******************************************************************************/
static char *
etaBaseConvert(unsigned int num, int base)
{
    static char Representation[] = "0123456789ABCDEF\0";
    static char buffer[33];
    char *ptr;

    ptr = &buffer[32];
    *ptr = '\0';

    do
    {
        *--ptr = Representation[num % base];
        num /= base;
    }
    while(num > 0);
    return(ptr);
}

/***************************************************************************//**
 *
 *  etaBaseConvertLong - print function. converts long int to string of char
 * based off of base.
 *
 *  @param num - raw hexadecimal number (long int)
 *  @param base - base to print (usually 8,10,16)
 *  @return pointer to string of characters ready for print.
 *
 ******************************************************************************/
static char *
etaBaseConvertLong(unsigned long int num, int base)
{
    static char Representation[] = "0123456789ABCDEF\0";
    static char buffer[33];
    char *ptr;

    ptr = &buffer[32];
    *ptr = '\0';

    do
    {
        *--ptr = Representation[num % base];
        num /= base;
    }
    while(num > 0);
    return(ptr);
}

/***************************************************************************//**
 *
 *  etaPrintf - main printf function.
 *
 *  @param receives standard printf strings
 *
 *  Note: Binary not supported (but it could be if we wanted)
 *
 *  Note: %x and %d prints 16 bit integers. %lx and %ld prints 32 bit integers.
 *
 ******************************************************************************/
void
etaPrintf(const char *format, ...)
{
    char *traverse;
    unsigned int i;
    long unsigned int long_i;
    char *s;

    va_list arg;

    va_start(arg, format);
    if(g_bEtaCspIoVerbose)
    {
        if(bPrintViaM3Mbox)
        {
            eta_csp_printviam3_start_message();
        }

        traverse = (char *)format;
        while(*traverse != '\0')
        {
            while((*traverse != '%') && (*traverse != '\0'))
            {
                etaPutc(*traverse);
                if(*traverse == 0)
                {
                    break;
                }
                traverse++;
            }

            if(*traverse == 0)
            {
                break;
            }

            traverse++; // skip over % character

            // Module 2: Fetching and executing arguments
            switch(*traverse)
            {
                case 'c':
                {
                    i = va_arg(arg, int); // Fetch char argument
                    etaPutc(i);
                    break;
                }

                case 'd':
                {
                    i = va_arg(arg, int); // Fetch Decimal/Integer
                                          // argument
                    if(i < 0)
                    {
                        i = -i;
                        etaPutc('-');
                    }
                    _etaPuts(etaBaseConvert(i, 10));
                    break;
                }

                case 'o':
                {
                    i = va_arg(arg, unsigned int); // Fetch Octal representation
                    _etaPuts(etaBaseConvert(i, 8));
                    break;
                }

                case 's':
                {
                    s = va_arg(arg, char *); // Fetch string
                    _etaPuts(s);
                    break;
                }

                case 'x':
                {
                    i = va_arg(arg, unsigned int); // Fetch Hexadecimal
                                                   // representation
                    _etaPuts(etaBaseConvert(i, 16));
                    break;
                }

                case 'l':
                {
                    traverse++; // skip over l character
                    if(*traverse == 'x')
                    {
                        long_i = va_arg(arg, long unsigned int); // Fetch Long
                                                                 // Hexadecimal
                                                                 // representation
                        _etaPuts(etaBaseConvertLong(long_i, 16));
                    }
                    else if(*traverse == 'd')
                    {
                        long_i = va_arg(arg, long unsigned int); // Fetch Long
                                                                 // Hexadecimal
                                                                 // representation
                        if(long_i < 0)
                        {
                            long_i = -long_i;
                            etaPutc('-');
                        }
                        _etaPuts(etaBaseConvertLong(long_i, 10));
                    }
                    else
                    {
                        // Silently drop if not lx or ld.
                    }

                    break;
                }
            }
            traverse++; // skip over {c,d,o,s,x} character
        }

        if(bPrintViaM3Mbox)
        {
            eta_csp_printviam3_end_message();
        }

        // Module 3: Closing argument list to necessary clean-up
        va_end(arg);
    }
}

/***************************************************************************//**
 *
 *  etaPutc - main etaPutc function.
 *
 *  @param char to print
 *
 ******************************************************************************/
void
etaPutc(const char x)
{
    if(bPrintViaM3Mbox)
    {
        eta_csp_printviam3_putc(x);
    }


    if(bPrintViaDspUart)
    {
        EtaCspIoDebugUartPutc(x);
    }
}

/***************************************************************************//**
 *
 *  _etaPuts - main _etaPuts function.
 *
 *  @param string to print
 *
 *  Note: _etaPuts is intended for "internal" prints not to be overriden by
 *        g_bEtaCspIoVerbose.
 *
 ******************************************************************************/
void
_etaPuts(const char *str)
{
    char x;

    while(x = *str++)
    {
        etaPutc(x);
    }
}

/***************************************************************************//**
 *
 *  etaPuts - user callable etaPuts function.
 *
 *  @param string to print
 *
 *  Note: This version does not call putc so it can be redirected seperately
 *        from printf.
 *
 ******************************************************************************/
void
etaPuts(const char *str)
{
    char x;

    if(g_bEtaCspIoVerbose)
    {
#ifdef PUTS_VIA_M3_MBOX
        eta_csp_printviam3_start_message();
#endif

        while(x = *str++)
        {
            if(bPrintViaM3Mbox)
            {
                eta_csp_printviam3_putc(x);
            }


            EtaCspIoDebugUartPutc(x);
            if(bPrintViaDspUart)
            {
                EtaCspIoDebugUartPutc(x);
            }
        }

#ifdef PUTS_VIA_M3_MBOX
        eta_csp_printviam3_end_message();
#endif
    }
}

/***************************************************************************//**
 *
 *  EtaCspIoDebugUartPutc - explicit call of Putc for the DSP DEBUG UART.
 *
 *  @param cChar - character to print
 *
 ******************************************************************************/
void
EtaCspIoDebugUartPutc(const char cChar)
{
    //
    // Wait for the transmitter to finish before sending in all cases.
    //
    while((REG_DSPCTRL_DSP_DSP_UART & BM_DSPCTRL_DSP_DSP_UART_BUSY) != 0)
    {
    }

    //
    // OK, send the character to the DSP UART. It is safe right now.
    //
    REG_DSPCTRL_DSP_DSP_UART = cChar;

    //
    // Wait for the transmitter to finish before exiting.
    // This is critical to being able to send characters on both the base
    // level and in an ISR context in the same program.
    //
    while((REG_DSPCTRL_DSP_DSP_UART & BM_DSPCTRL_DSP_DSP_UART_BUSY) != 0)
    {
    }
}

/***************************************************************************//**
 *
 *  EtaCspIoDebugUartPuts - explicit call of Puts for the DSP DEBUG UART.
 *
 *  @param pcString - string to print
 *
 ******************************************************************************/
void
EtaCspIoDebugUartPuts(const char *pcString)
{
    char cChar;

    while(cChar = *pcString++)
    {
        EtaCspIoDebugUartPutc(cChar);
    }
}

/***************************************************************************//**
 *
 *  EtaCspIoMboxPrintInit - If enabled, will initialize DSP Print
 *                                    buffer used by PRINT_VIA_M3_MBOX
 *
 ******************************************************************************/
void
EtaCspIoMboxPrintInit(void)
{
    uint16_t byteaddress;
    uint32_t mailbox_message;

    glob_printbuff_wrptr = 0;
    glob_printbuff_rdptr = 0;

    // This is byte aligned, 32 bit AHB address
    byteaddress = ((uint16_t)&glob_printbuff_data[0]);

    mailbox_message =
        DSP_PRINT_BUFFER_IN_YMEM ? DSP_YMEM_START : DSP_XMEM_START;
    mailbox_message |= ((byteaddress & ~0x1) << 1);
    mailbox_message |= ((byteaddress &  0x1));

    glob_printbuff_data [DSP_PRINT_BUFFER_SIZE - 1] = (mailbox_message >> 0) &
                                                      0xff;
    glob_printbuff_data [DSP_PRINT_BUFFER_SIZE - 2] = (mailbox_message >> 8) &
                                                      0xff;
    glob_printbuff_data [DSP_PRINT_BUFFER_SIZE - 3] = (mailbox_message >> 16) &
                                                      0xff;
    glob_printbuff_data [DSP_PRINT_BUFFER_SIZE - 4] = (mailbox_message >> 24) &
                                                      0xff;
    glob_printbuff_data [DSP_PRINT_BUFFER_SIZE - 5] = MAILBOX_DSP2M3CMD_PWRAP;
}

// Print functions
//////////////////////////////////////////////////////////////////////

