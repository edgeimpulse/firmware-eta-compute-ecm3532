/***************************************************************************//**
 *
 * @file eta_utils_bootloader.c
 *
 * @brief UART bootloader.
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
 * This is part of revision ${version} of the Tensai Software Development Kit.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup etautils-bootloader UART Bootloader
 * @ingroup etautils
 * @{
 ******************************************************************************/

#include "eta_utils_bootloader.h"

//
// Available  commands.
//
tBootloaderCmd g_psCommands[] =
{
    COMMAND(_CMD_PREFIX, BOOTMODE),
    COMMAND(_CMD_PREFIX, APPINFO),
    COMMAND(_CMD_PREFIX, APPMODE),
    COMMAND(_CMD_PREFIX, BINBLOB),
    COMMAND(_CMD_PREFIX, EOU),
    COMMAND(_CMD_PREFIX, VER),
    COMMAND(_CMD_PREFIX, HELP)
};

//
//! The command prefix.
//
#define _BOOT_NUM_CMDS sizeof(g_psCommands) / sizeof(tBootloaderCmd)

//
// UART for the terminal.
//
tUart *g_psBootloaderUart;

//
// Bootloader image.
//
tBootloaderImage g_sBootImage;

//
// Bootloader blob.
//
tBootloaderBlob g_sBootBlob;

//
// Bootloader flash status.
//
volatile tBootloaderFlashStatus g_sBootFlashStatus;

//
// Bootloader flash status.
//
uint8_t g_pui8PageBytes[BLOB_LENGTH];

//
// Is a character waiting from the terminal?
//
volatile bool g_bTerminalCharWaiting;

//
// Variable to track boot mode persistance.
//
volatile bool g_bStayInBoot;

//
// Create microrl object and pointer on it.
//
microrl_t g_rlTerminal;
microrl_t *g_prlTerminal = &g_rlTerminal;

//
// Base64 encoding table.
//
static char g_pcBase64EncodeTable[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                        '4', '5', '6', '7', '8', '9', '+', '/'};
//
// Base64 decoding table.
//
static char g_pcBase64DecodeTable[256];

/***************************************************************************//**
 *
 * CommandHelpPrint - Print the help prompt.
 *
 ******************************************************************************/
static void
CommandHelpPrint(void)
{
    EtaUtilsTerminalPrint("Supported Commands:\n\r");
    EtaUtilsTerminalPrint("\tAT+HELP         - Show help\r");
    EtaUtilsTerminalPrint("\tAT+VER          - Print the version\r");
    EtaUtilsTerminalPrint(
                          "\tAT+APPINFO      - Set the new application info (4 parameters).\r");
    EtaUtilsTerminalPrint(
                          "\tAT+BINBLOB      - Retrieve a binary blob (3 parameters).\r");
    EtaUtilsTerminalPrint("\tAT+EOU          - End of upload.\r");
    EtaUtilsTerminalPrint("\tAT+APPMODE      - Switch to application.\r");
    EtaUtilsTerminalPrint("\tAT+BOOTMODE     - Switch to bootloader.\r");
}

/***************************************************************************//**
 *
 * Base64Decode - Decode the base64 data.
 *
 ******************************************************************************/
int
Base64Decode(const char *data, size_t input_length, uint8_t *output_buffer,
             size_t output_buffer_length) {

    if (input_length % 4 != 0) return -1;

    size_t output_length = input_length / 4 * 3;
    if (output_length != output_buffer_length) {
        return -1;
    }
    if (data[input_length - 1] == '=') output_length--;
    if (data[input_length - 2] == '=') output_length--;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : g_pcBase64DecodeTable[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : g_pcBase64DecodeTable[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : g_pcBase64DecodeTable[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : g_pcBase64DecodeTable[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
            + (sextet_b << 2 * 6)
            + (sextet_c << 1 * 6)
            + (sextet_d << 0 * 6);

        if (j < output_length) output_buffer[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) output_buffer[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) output_buffer[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return (int)(output_length);
}

/***************************************************************************//**
 *
 * crc32 - Perform a CRC on the requested data.
 *
 * @param s TODO
 * @param n number of bytes.
 * @return TODO
 *
 ******************************************************************************/
uint32_t
Crc32(const uint8_t *s,size_t n) {
    uint32_t crc=0xFFFFFFFF;
    for(size_t i=0;i<n;i++) {
        uint8_t ch=s[i];
        for(size_t j=0;j<8;j++) {
            uint32_t b=(ch^crc)&1;
            crc>>=1;
            if(b) crc=crc^0xEDB88320;
            ch>>=1;
        }
    }
    return ~crc;
}

/***************************************************************************//**
 *
 * AtoiFast - TODO
 *
 * @param p TODO
 * @return TODO
 *
 ******************************************************************************/
int
AtoiFast(const char *str)
{
    int val = 0;
    while( *str ) {
        val = val*10 + (*str++ - '0');
    }
    return val;
}

/***************************************************************************//**
 *
 * TokenGet - TODO
 *
 * @param str TODO
 * @return TODO
 *
 ******************************************************************************/
char *
TokenGet(char *str)
{
    char *pcToken;
    char *pcEnd;
    char *pcEqual;
    char *pcComma;
    char *pcSpace;
    uint32_t ui32Pos;

    pcEqual = strstr(str, "=");
    if(pcEqual > (str + 15))
    {
        pcEqual = NULL;
    }
    pcComma = strstr(str, ",");

    if(pcEqual && pcComma)
    {
        pcToken = pcEqual + 1;
        ui32Pos = pcComma - pcToken;
        pcToken[ui32Pos] = 0;
    }
    else if(pcEqual)
    {
        pcToken = pcEqual + 1;
    }

    else if(pcComma)
    {
        pcToken = str;
        ui32Pos = pcComma - pcToken;
        pcToken[ui32Pos] = 0;
    }

    else
    {
        pcToken = str;
    }

    //
    // Trim leading whitespace.
    //
    if(pcToken[0] == ' ')
    {
        pcToken = &pcToken[1];
    }

    //
    // Return the token.
    //
    return(pcToken);
}

/***************************************************************************//**
 *
 * BootloaderCharHandler - The ISR handler for the terminal.
 *
 ******************************************************************************/
void
BootloaderCharHandler(tIrqNum iIrqNum)
{
    //
    // Indicate there is at least one character wainting in from the terminal.
    //
    g_bTerminalCharWaiting = true;

    //
    // Disable and clear the interrupt.
    //
    EtaCspUartIntDisable(g_psBootloaderUart, 0x7);
    EtaCspUartIntStatusClear(g_psBootloaderUart, 0x7);
}

/***************************************************************************//**
 *
 * BootloaderImageValidate - Check an image for validness.
 *
 * @return Return **true** if an image is valid and **false** otherwise.
 *
 ******************************************************************************/
static bool
BootloaderImageValidate(void)
{
    uint32_t ui32ResetVector, ui32StackPointer;
    uint32_t *pui32LinkAddress;

    //
    // Set the link address.
    //
    pui32LinkAddress = (uint32_t *)IMAGE_START;

    //
    // Make sure the link address is in flash.
    //
    if((uint32_t)pui32LinkAddress > 0x01080000)
    {
        return(false);
    }

    ui32StackPointer = pui32LinkAddress[0];
    ui32ResetVector = pui32LinkAddress[1];

    //
    // Make sure the stack is in SRAM.
    //
    if((ui32StackPointer < 0x10000000) || (ui32StackPointer > 0x1FFFFFFF))
    {
        return(false);
    }

    //
    // Make sure the reset vector points somewhere after the start of the image.
    //
    if((ui32ResetVector < (uint32_t)pui32LinkAddress))
    {
        return(false);
    }

    //
    // If those tests pass, we're probably safe to run.
    //
    return(true);
}

/***************************************************************************//**
 *
 * BootloaderImageRun - Execute an image that has been written to flash.
 *
 * @param psImage is a pointer to the image structure.
 *
 ******************************************************************************/
#ifdef __CC_ARM
#else
void __attribute__((naked))
BootloaderImageRun(uint32_t ui32Ptr)
{
    //
    // when called, we have a pointer to the vector table of the program in SRAM.
    // use this function to call the program in SRAM (it never returns).
    //
    __asm(
            // r0 points to the vector table of the program in SRAM.

            // Load SP from stack pointer value in reset vector in SRAM
            "    ldr      sp, [r0, #0]\n"

            // Load R1 from PC value in reset vector in SRAM
            "    ldr      r1, [r0, #4]\n"

            // Update VTOR with pointer to SRAM
            "    movw     r2, #0xED08\n"
            "    movt     r2, #0xE000\n"
            "    str      r0, [r2]\n"

            // Branch to the reset handler entry point in SRAM
            "    bx       r1");
}

#endif

/***************************************************************************//**
 *
 * BLOBSIZESend - TODO
 *
 * @param argv TODO
 *
 ******************************************************************************/
static void
BLOBSIZESend(uint32_t ui32BlobSize)
{
    char pcBlobSize[8];

    sprintf(pcBlobSize, "%d", ui32BlobSize);
    EtaUtilsTerminalPrint("AT+BLOBSIZE=");
    EtaUtilsTerminalPrint(pcBlobSize);
    EtaUtilsTerminalPrint("\r");
}

/***************************************************************************//**
 *
 * ACK_Cmd - TODO
 *
 ******************************************************************************/
void
ACK_Cmd(void)
{
    EtaUtilsTerminalPrint("AT+ACK\r");
}

/***************************************************************************//**
 *
 * NACK_Cmd - TODO
 *
 ******************************************************************************/
void
NACK_Cmd(void)
{
    EtaUtilsTerminalPrint("AT+NACK\r");
}

/***************************************************************************//**
 *
 * ACKBLOB_Cmd - TODO
 *
 * @param ui32SequenceNum TODO
 *
 ******************************************************************************/
void
ACKBLOB_Cmd(uint32_t ui32SequenceNum)
{
    char pcSequenceNum[8];

    sprintf(pcSequenceNum, "%d", ui32SequenceNum);
    EtaUtilsTerminalPrint("AT+ACKBLOB=");
    EtaUtilsTerminalPrint(pcSequenceNum);
    EtaUtilsTerminalPrint("\r");
}

/***************************************************************************//**
 *
 * NACKBLOB_Cmd - TODO
 *
 * @param ui32SequenceNum TODO
 *
 ******************************************************************************/
void
NACKBLOB_Cmd(uint32_t ui32SequenceNum)
{
    char pcSequenceNum[8];

    sprintf(pcSequenceNum, "%d", ui32SequenceNum);
    EtaUtilsTerminalPrint("AT+NACKBLOB=");
    EtaUtilsTerminalPrint(pcSequenceNum);
    EtaUtilsTerminalPrint("\r");
}

/***************************************************************************//**
 *
 * HELP_Cmd - TODO
 *
 * @param pcInfo TODO
 *
 ******************************************************************************/
void
HELP_Cmd(int argc, const char * const *argv)
{
    char pcPrintMe[35];

    //
    // Print the bootloader info.
    //
    EtaUtilsTerminalPrint("\n\nEta Compute Bootloader - v");
    sprintf(pcPrintMe, "%s\n", _BOOTLOADER_VER);
    EtaUtilsTerminalPrint(pcPrintMe);
    sprintf(pcPrintMe, "Compiled on %s at %s\n", __DATE__, __TIME__);
    EtaUtilsTerminalPrint(pcPrintMe);

    //
    // Print the help menu.
    //
    CommandHelpPrint();
}

/***************************************************************************//**
 *
 * BOOTMODE_Cmd - TODO
 *
 * @param argv TODO
 *
 ******************************************************************************/
static void
BOOTMODE_Cmd(int argc, const char * const *argv)
{
    //
    // Stay in boot mode.
    //
    g_bStayInBoot = true;

    //
    // ACK the command.
    //
    ACK_Cmd();
}

/***************************************************************************//**
 *
 * APPINFO_Cmd - TODO
 *
 * @param argv TODO
 *
 ******************************************************************************/
static void
APPINFO_Cmd(int argc, const char * const *argv)
{
    uint32_t ui32I;
    int t_checksum;
    char pcCrc[17];

    //
    // Is there enough args?
    //
    if(argc != 4)
    {
        EtaUtilsTerminalPrint("Invalid number of args.\n");
        NACK_Cmd();
        return;
    }

    //
    // Reset the flashed sequence number.
    //
    g_sBootFlashStatus.ui32SequenceNumFlashed = 0;
    g_sBootFlashStatus.ui32BytesToFlash = 0;
    g_sBootFlashStatus.ui32ProgramAddress = IMAGE_START;

    //
    // Fill in the image info.
    //
    ui32I = 0;
    strcpy(g_sBootImage.pcName, TokenGet((char *)argv[ui32I++]));
    strcpy(g_sBootImage.pcTarget, TokenGet((char *)argv[ui32I++]));
    strcpy(pcCrc, TokenGet((char *)argv[ui32I++]));
    sscanf(pcCrc, "%d", &t_checksum);
    g_sBootImage.ui32CRC = (uint32_t)t_checksum;
    g_sBootImage.ui32NumBytes = AtoiFast(TokenGet((char *)argv[ui32I++]));
    if(g_sBootImage.ui32NumBytes == 0)
    {
        EtaUtilsTerminalPrint("Invalid number of bytes.\n");
        NACK_Cmd();
    }
    else
    {
        BLOBSIZESend(BLOB_LENGTH);

        //
        // TODO: Hack for Edge Impulse host.
        //
        EtaUtilsTerminalPrint("\n");
        ACK_Cmd();
    }
}

/***************************************************************************//**
 *
 * APPMODE_Cmd - TODO
 *
 * @param argv TODO
 *
 ******************************************************************************/
static void
APPMODE_Cmd(int argc, const char * const *argv)
{
    //
    // Is the image ready to run?
    //
    if(g_sBootImage.bReadyToRun)
    {
        //
        // ACK and run the image.
        //
        ACK_Cmd();

        //
        // Reset the warm start.
        //
        EtaCspRtcRestartModeSet(eRestartCold);

        //
        // Zero out the magic address.
        //
        MEMIO32(_BOOTLOADER_MAGIC_ADDR) = 0x0;

        //
        // Run the image.
        //
        BootloaderImageRun(g_sBootImage.ui32LinkAddress);
    }
    else
    {
        //
        // NACK as the image is not ready.
        //
        EtaUtilsTerminalPrint("Image not ready.  Run AT+EOU first.\n");
        NACK_Cmd();
    }
}

/***************************************************************************//**
 *
 * BINBLOB_Cmd - TODO
 *
 * @param argv TODO
 *
 ******************************************************************************/
static void
BINBLOB_Cmd(int argc, const char * const *argv)
{
    uint32_t ui32I, ui32CRC, ui32DecodedLen;
    volatile uint32_t ui32BytesToCopy, ui32BytesLeft;
    int32_t i32DecodedCrc;
    char pcCrc[35];
    int t_checksum;
    char pcPrintMe[35];

    //
    // Is there enough args?
    //
    if(argc != 3)
    {
        EtaUtilsTerminalPrint("Invalid number of args.\n");
        NACK_Cmd();
        return;
    }

    //
    // Fill in the blob info.
    //
    ui32I = 0;
    g_sBootBlob.ui32SequenceNum = AtoiFast(TokenGet((char *)argv[ui32I++]));
     if(g_sBootBlob.ui32SequenceNum == 1680)
    {
        EtaUtilsTerminalPrint("");
    }
    strcpy(pcCrc, TokenGet((char *)argv[ui32I++]));
    sscanf(pcCrc, "%d", &t_checksum);
    g_sBootBlob.ui32CRC = (uint32_t)t_checksum;
    g_sBootBlob.pcEncoded = TokenGet((char *)argv[ui32I++]);

    //
    // If we have already programmed this sequence number, ACK and wait for the
    // next one.
    //
    if((g_sBootBlob.ui32SequenceNum != 0) &&
       (g_sBootFlashStatus.ui32SequenceNumFlashed ==
        g_sBootBlob.ui32SequenceNum))
    {
        sprintf(pcPrintMe, "Already received and programmed blob #%d\n",
                g_sBootBlob.ui32SequenceNum);
        EtaUtilsTerminalPrint(pcPrintMe);
        NACKBLOB_Cmd(g_sBootBlob.ui32SequenceNum);
        return;
    }

    //
    // If we have already programmed this sequence number, ACK and wait for the
    // next one.
    //
    if(g_sBootBlob.ui32SequenceNum !=
       (g_sBootFlashStatus.ui32SequenceNumFlashed + 1))
    {
        sprintf(pcPrintMe, "Invalid sequence number, expected %lld but " \
                           "received %ld\n",
                g_sBootFlashStatus.ui32SequenceNumFlashed + 1,
                g_sBootBlob.ui32SequenceNum);
        EtaUtilsTerminalPrint(pcPrintMe);
        NACKBLOB_Cmd(g_sBootBlob.ui32SequenceNum);
        return;
    }

    //
    // Do we have room for the data?
    //
    ui32DecodedLen = strlen(g_sBootBlob.pcEncoded) / 4 * 3;
    if(ui32DecodedLen > BLOB_LENGTH)
    {
        EtaUtilsTerminalPrint("Cannot allocate decoded data buffer\n");
        NACKBLOB_Cmd(g_sBootBlob.ui32SequenceNum);
        return;
    }

    //
    // Decode the blob.
    //
    ui32DecodedLen = Base64Decode(g_sBootBlob.pcEncoded,
                                   strlen(g_sBootBlob.pcEncoded),
                                   g_sBootBlob.pcDecoded, ui32DecodedLen);
    if(ui32DecodedLen < 0)
    {
        sprintf(pcPrintMe, "Failed to decode message (%d)\n", ui32DecodedLen);
        EtaUtilsTerminalPrint(pcPrintMe);
        NACKBLOB_Cmd(g_sBootBlob.ui32SequenceNum);
        return;
    }

    //
    // Check the CRC
    //
    i32DecodedCrc = Crc32(g_sBootBlob.pcDecoded, ui32DecodedLen);
    if(i32DecodedCrc != g_sBootBlob.ui32CRC)
    {
        sprintf(pcPrintMe, "Checksum failed, expected %lld but decoded CRC32" \
                           "was %lu\n", g_sBootBlob.ui32CRC, i32DecodedCrc);
        EtaUtilsTerminalPrint(pcPrintMe);
        NACKBLOB_Cmd(g_sBootBlob.ui32SequenceNum);
        return;
    }

    //
    // Does the start or end page need to be erased?
    //
    if(MEMIO32(g_sBootFlashStatus.ui32ProgramAddress) != 0xFFFFFFFF)
    {
        //
        // Erase the next two pages of flash.
        //
        EtaCspFlashPageErase(g_sBootFlashStatus.ui32ProgramAddress);
        EtaCspFlashPageErase(g_sBootFlashStatus.ui32ProgramAddress +
                             ETA_CSP_FLASH_PAGE_SIZE_BYTES);
    }

    //
    // Copy all the bytes into the array if theres room.
    //
    if((g_sBootFlashStatus.ui32BytesToFlash + ui32DecodedLen) <=
       ETA_CSP_FLASH_PAGE_SIZE_BYTES)
    {
        ui32BytesToCopy = ui32DecodedLen;
    }

    //
    // Copy what we have room for.
    //
    else
    {
        ui32BytesToCopy = (ETA_CSP_FLASH_PAGE_SIZE_BYTES -
                           g_sBootFlashStatus.ui32BytesToFlash);
    }

    //
    // Perform the copy.
    //
    memcpy(&g_pui8PageBytes[g_sBootFlashStatus.ui32BytesToFlash],
           (uint8_t *)g_sBootBlob.pcDecoded, ui32BytesToCopy);

    //
    // Save the number of bytes to flash.
    //
    g_sBootFlashStatus.ui32BytesToFlash+=ui32BytesToCopy;

    //
    // Program the bytes.
    //
    if(g_sBootFlashStatus.ui32BytesToFlash == ETA_CSP_FLASH_PAGE_SIZE_BYTES)
    {
        EtaCspFlashProgramFast(g_sBootFlashStatus.ui32ProgramAddress,
                               (uint32_t *)g_pui8PageBytes);
        g_sBootFlashStatus.ui32ProgramAddress+=ETA_CSP_FLASH_PAGE_SIZE_BYTES;

        //
        // Copy the remaining bytes to the page array to be programmed on the
        // next page flash.
        //
        memcpy(g_pui8PageBytes,
               (uint8_t *)&g_sBootBlob.pcDecoded[ui32BytesToCopy],
               ui32DecodedLen - ui32BytesToCopy);
        g_sBootFlashStatus.ui32BytesToFlash = ui32DecodedLen - ui32BytesToCopy;
    }

    //
    // Save the flashed sequence number and send the ACK.
    //
    g_sBootFlashStatus.ui32SequenceNumFlashed = g_sBootBlob.ui32SequenceNum;
    ACKBLOB_Cmd(g_sBootBlob.ui32SequenceNum);
}

/***************************************************************************//**
 *
 * EOU_Cmd - TODO
 *
 * @param argv TODO
 *
 ******************************************************************************/
static void
EOU_Cmd(int argc, const char * const *argv)
{
    int32_t i32DecodedCrc;
    char pcPrintMe[35];

    //
    // Program the bytes if we didnt have a multiple of a page.
    //
    if(g_sBootFlashStatus.ui32BytesToFlash != 0)
    {
        EtaCspFlashProgramFast(g_sBootFlashStatus.ui32ProgramAddress,
                               (uint32_t *)g_pui8PageBytes);
    }

    //
    // Set the link address.
    //
    g_sBootImage.ui32LinkAddress = IMAGE_START;

    //
    // Check the image CRC.
    //
    i32DecodedCrc = Crc32((uint8_t *)g_sBootImage.ui32LinkAddress,
                           g_sBootImage.ui32NumBytes);
    if(i32DecodedCrc != g_sBootImage.ui32CRC)
    {
        sprintf(pcPrintMe, "Checksum failed, expected %lld but decoded CRC32" \
                           "was %lu\n", g_sBootImage.ui32CRC, i32DecodedCrc);
        EtaUtilsTerminalPrint(pcPrintMe);
        NACK_Cmd();
        return;
    }

    //
    // Validate the image.
    //
    if(BootloaderImageValidate())
    {
        g_sBootImage.bReadyToRun = true;
        ACK_Cmd();
    }
    else
    {
        EtaUtilsTerminalPrint("Image not valid\n");
        NACK_Cmd();
    }
}

/***************************************************************************//**
 *
 * VER_Cmd - TODO
 *
 * @param argv TODO
 *
 ******************************************************************************/
static void
VER_Cmd(int argc, const char * const *argv)
{
    //
    // Print the bootloader version.
    //
    EtaUtilsTerminalPrint(_BOOTLOADER_VER);
    EtaUtilsTerminalPrint("\r");
}

/***************************************************************************//**
 *
 * BootloaderCmdExecute - Execute callback for microrl library.
 *
 ******************************************************************************/
int
BootloaderCmdExecute(int argc, const char * const *argv)
{
    uint32_t ui32I, ui32J;
    char *pcCmd;

    //
    // Just iterate through argv word and compare it with your commands.
    //
    ui32I = 0;
    while(ui32I < argc)
    {
        //
        // Loop through known commands.
        //
        for(ui32J = 0; ui32J < _BOOT_NUM_CMDS; ui32J++)
        {
            //
            // Does the command sent contain a known command?
            //
            pcCmd = strstr(argv[ui32I], g_psCommands[ui32J].pcName);
            if(pcCmd)
            {
                //
                // TODO: Hack for Edge Impulse host.
                //
                EtaUtilsTerminalPrint("\n");
                g_psCommands[ui32J].pfnFunction(argc, argv);
                EtaCspRtcTmrDelayMs(10);
                return(0);
            }
        }

        ui32I++;
    }
    return(0);
}

/***************************************************************************//**
 *
 * BootloaderInit - Initialize the bootloader.
 *
 ******************************************************************************/
static void
BootloaderInit(tUart *psUart)
{
    //
    // If the magic number is correct, stay here and wait for the update.
    //
    if(MEMIO32(_BOOTLOADER_MAGIC_ADDR) == _BOOTLOADER_MAGIC_VAL)
    {
        g_bStayInBoot = true;
    }
    else
    {
        g_bStayInBoot = false;
    }

    //
    // Set the UART Baud.
    //
    EtaBspUartInit(eUartBaud460800);

    //
    // Save the UART to use.
    //
    g_psBootloaderUart = psUart;
    EtaUtilsTerminalInit(psUart);

    //
    // Setup NVIC for terminal interrupt.
    //
    if(g_psBootloaderUart->iNum == eUartNum0)
    {
        EtaCspIsrHandlerSet(eIrqNumUart0, BootloaderCharHandler);
        EtaCspNvicIntClear(eIrqNumUart0);
        EtaCspNvicIntEnable(eIrqNumUart0);
    }
    else if(g_psBootloaderUart->iNum == eUartNum1)
    {
        EtaCspIsrHandlerSet(eIrqNumUart1, BootloaderCharHandler);
        EtaCspNvicIntClear(eIrqNumUart1);
        EtaCspNvicIntEnable(eIrqNumUart1);
    }

    //
    // Enable all RX interrupts.
    //
    EtaCspUartIntStatusClear(g_psBootloaderUart, 0x7);
    EtaCspUartIntEnable(g_psBootloaderUart, 0x7);

    //
    // Fill out the b64 decode table.
    //
    for (int i = 0; i < 64; i++) {
        g_pcBase64DecodeTable[(unsigned char)g_pcBase64EncodeTable[i]] = i;
    }

    //
    // Print the help.
    //
    HELP_Cmd(0, 0);

    //
    // Call init with g_ptr to microrl instance and print callback.
    //
    microrl_init(g_prlTerminal, EtaUtilsTerminalPrint);

    //
    // Set callback for execute.
    //
    microrl_set_execute_callback(g_prlTerminal, BootloaderCmdExecute);

    //
    // Set up function pointers to flash helpers.
    //
    if (EtaCspFlashInit() != eEtaSuccess) {
        EtaUtilsTerminalPrint("ERROR: EtaCspFlashInit\r\n");
        while(1)
        {
        }
    }
}

/***************************************************************************//**
 *
 * EtaUtilsBootloaderInvoke - Reboot into the bootloader.
 *
 ******************************************************************************/
void
EtaUtilsBootloaderInvoke(void)
{
    //
    // Disable interrupts.
    //
    EtaCspM3IntDisable();

    //
    // Write the magic number at the magic address.
    // This will tell the bootloader that we want to update the image.
    //
    MEMIO32(_BOOTLOADER_MAGIC_ADDR) = _BOOTLOADER_MAGIC_VAL;

    //
    // Reset the warm start.
    //
    EtaCspRtcRestartModeSet(eRestartCold);

    //
    // Force a Reset.
    //
    EtaCspNvicM3Reset();
}

/***************************************************************************//**
 *
 * EtaUtilsBootloaderMain - Run the bootloader.
 *
 * Should not be called from an application. This is the main loop for the
 * bootloader.  To reboot into the bootloader call: EtaUtilsBootloaderInvoke
 *
 ******************************************************************************/
void
EtaUtilsBootloaderMain(tUart *psUart)
{
    //
    // Initialize the bootloader.
    //
    BootloaderInit(psUart);

    //
    // Reset the timer.
    //
    EtaCspTimerInitMs();

    //
    // Loop waiting for terminal input.
    //
    while(1)
    {
        //
        // Is there a character from the terminal waiting?
        //
        if(g_bTerminalCharWaiting)
        {
            //
            // Reset the timer.
            //
            EtaCspTimerInitMs();

            //
            // Read characters until the RX FIFO is empty.
            //
            while(!EtaCspUartRxFifoIsEmpty(g_psBootloaderUart))
            {
                //
                // Put received char from stdin to microrl lib.
                //
                microrl_insert_char(g_prlTerminal, EtaUtilsTerminalCharGet(false));
            }

            //
            // Reenable interrupts.
            //
            EtaCspUartIntStatusClear(g_psBootloaderUart, 0x7);
            EtaCspUartIntEnable(g_psBootloaderUart, 0x7);

            //
            // Interrupt handled.
            //
            g_bTerminalCharWaiting = false;
        }

        //
        // Leave boot mode and timeout?
        //
        if(!g_bStayInBoot && EtaCspTimerCountGetMs() > 5000)
        {
            //
            // Is there a valid image?
            //
            if(BootloaderImageValidate())
            {
                EtaUtilsTerminalPrint("Image is valid.\r\n");
                EtaUtilsTerminalPrint("Jumping to Image\r\n");
                BootloaderImageRun(IMAGE_START);
            }
        }
    }
}

/** @}*/
