/***************************************************************************//**
 *
 * @file eta_utils_bootloader.h
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

#include <stdio.h>
#include <string.h>
#include "eta_chip.h"
#include "eta_bsp.h"
#include "eta_csp_inc.h"
#include "eta_utils_terminal.h"

#ifndef __ETA_UTILS_BOOTLOADER_H__
#define __ETA_UTILS_BOOTLOADER_H__

#ifdef __cplusplus
extern "C"
{
#endif

//
//! Bootloader version.
//
#define _BOOTLOADER_VER "1.0.0"

//
//! The command prefix.
//
#define _CMD_PREFIX AT+

//
//! Blob length.
//
#define BLOB_LENGTH ETA_CSP_FLASH_PAGE_SIZE_BYTES

//
//! Address for the image.
//
#define IMAGE_START 0x01005000

//
// Magic address abd value.
//
#define _BOOTLOADER_MAGIC_ADDR 0x10000000 + 0x20000
#define _BOOTLOADER_MAGIC_VAL  0xBADC0DE

//
// Command concat.
//
#define STRINGIFY(x)          #x
#define TOKENPASTE3(x, y)     #x#y
#define COMMAND(PREFIX, NAME) { TOKENPASTE3(PREFIX, NAME), NAME ## _Cmd }

//
// Command functions.
//
static void HELP_Cmd(int argc, const char * const *argv);
static void BOOTMODE_Cmd(int argc, const char * const *argv);
static void APPINFO_Cmd(int argc, const char * const *argv);
static void APPMODE_Cmd(int argc, const char * const *argv);
static void BINBLOB_Cmd(int argc, const char * const *argv);
static void EOU_Cmd(int argc, const char * const *argv);
static void VER_Cmd(int argc, const char * const *argv);

//******************************************************************************
//
//! Boot image structure.
//
//******************************************************************************
typedef struct
{
    //
    //! Starting address for the image.
    //
    uint32_t  ui32LinkAddress;

    //
    //! Length of the image in bytes.
    //
    uint32_t ui32NumBytes;

    //
    //! CRC-32 Value for the image.
    //
    int32_t ui32CRC;

    //
    //! Stack pointer address.
    //
    uint32_t *pui32StackPointer;

    //
    //! Reset vector address.
    //
    uint32_t *pui32ResetVector;

    //
    //! Is the image ready to run.
    //
    bool bReadyToRun;

    //
    //! App name.
    //
    char pcName[15];

    //
    //! Target name.
    //
    char pcTarget[15];
}
tBootloaderImage;

//******************************************************************************
//
//! Boot blob structure.
//
//******************************************************************************
typedef struct
{
    //
    //! Current sequence number.
    //
    uint32_t ui32SequenceNum;

    //
    //! CRC-32 Value for the blob.
    //
    int32_t ui32CRC;

    //
    //! Encoded blob.
    //
    char *pcEncoded;

    //
    //! Decoded blob.
    //
    uint8_t pcDecoded[BLOB_LENGTH];

}
tBootloaderBlob;

//******************************************************************************
//
//! Boot flash status structure.
//
//******************************************************************************
typedef struct
{
    //
    //! Last sequence number flashed.
    //
    uint32_t ui32SequenceNumFlashed;

    //
    //! Current program address
    //
    uint32_t ui32ProgramAddress;

    //
    //! Bytes to be flashed.
    //
    uint32_t ui32BytesToFlash;

}
tBootloaderFlashStatus;

//******************************************************************************
//
//! Boot command structure.
//
//******************************************************************************
typedef struct
{
  char *pcName;
  void (*pfnFunction) (int argc, const char * const *argv);
}
tBootloaderCmd;

//*****************************************************************************
//
// External function declarations.
//
//*****************************************************************************

//
// Call this from an application to reboot into the bootloader.
//
extern void EtaUtilsBootloaderInvoke(void);

//
// The main bootloader function.  Should NOT be called from an application.
//
extern void EtaUtilsBootloaderMain(tUart *psUart);

#ifdef __cplusplus
}
#endif

#endif // __ETA_UTILS_BOOTLOADER_H__

/** @}*/
