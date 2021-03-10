/***************************************************************************//**
 *
 * @file eta_csp_flash.c
 *
 * @brief This file contains eta_csp_flash module implementations.
 *
 * Copyright (C) 2018 Eta Compute, Inc
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
 * @addtogroup ecm3532flash-m3 Flash
 * @ingroup ecm3532csp-m3
 * @{
 ******************************************************************************/

#include "eta_chip.h"
#include "eta_csp_flash.h"
#include "eta_csp.h"
#include "memio.h"
#include "eta_csp_uart.h"
#include "eta_csp_io.h"


//
// Define the function pointers to reach into the bootrom helpers.
//
tpfFlashBrWsHelper pfnFlashBrWsHelper;
tpfFlashBrLoadHelper pfnFlashBrLoadHelper;
tpfFlashBrStoreHelper pfnFlashBrStoreHelper;
tpfFlashBrVersionHelper pfnFlashBrVersionHelper;

tpfFlashBrErase pfnFlashBrErase;
tpfFlashBrProgram pfnFlashBrProgram;
tpfFlashBrRead pfnFlashBrRead;

// new for ECM3532
tpfFlashBrDebrick pfnFlashBrDebrick;
tpfFlashBrErase pfnFlashBrEraseBkpt;
tpfFlashBrProgram pfnFlashBrProgramBkpt;
tpfFlashBrRead pfnFlashBrReadBkpt;

/***************************************************************************//**
 *
 *  EtaCspFlashInit - Initialize the flash pointers.
 *
 *  @return eEtaSuccess or flash specific error status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspFlashInit(void)
{
    uint32_t *pui32Ptr;
    int32_t i32I;

    //
    // Find the branch table in bootrom (limit search to 2KB).
    //
    pui32Ptr = (uint32_t *)0x100;
    for(i32I = 0; i32I < 512; i32I++)
    {
        if(*pui32Ptr == 0x43415445)
        {
            //
            // 1st 4 bytes of "ETACOMPUTE".
            //
            break;
        }
        else pui32Ptr++;
    }
    if(*pui32Ptr == 0x43415445)
    {
        //
        // Point to 2nd 4 bytes of string.
        //
        pui32Ptr++;
        if(*pui32Ptr != 0x55504d4f)
        {
            return(eEtaFailure);
        }

        //
        // Point to 3nd 4 bytes of string.
        //
        pui32Ptr++;
        if(*pui32Ptr != 0x00004554)
        {
            return(eEtaFailure);
        }

        //
        // Point to revision info.
        //
        pui32Ptr++;

        //
        // Point to pointer to branch table.
        //
        pui32Ptr++;

        //
        // Pointer to the flash helper branch table.
        //
        pui32Ptr = (uint32_t *)*pui32Ptr;
    }
    else
    {
        //
        // Pointer to the flash helper branch table.  This will work for ECM3531
        // and ECM3532 but not for later chips.
        //
        pui32Ptr = (uint32_t *)MEMIO32(0x98);
    }

    //
    // Once the start of the branch table is known, everything else is easy.
    //
    pfnFlashBrWsHelper = (tpfFlashBrWsHelper)pui32Ptr[0];
    pfnFlashBrLoadHelper = (tpfFlashBrLoadHelper)pui32Ptr[1];
    pfnFlashBrStoreHelper = (tpfFlashBrStoreHelper)pui32Ptr[2];
    pfnFlashBrVersionHelper = (tpfFlashBrVersionHelper)pui32Ptr[3];

    pfnFlashBrErase = (tpfFlashBrErase)pui32Ptr[5];
    pfnFlashBrProgram = (tpfFlashBrProgram)pui32Ptr[6];
    pfnFlashBrRead = (tpfFlashBrRead)pui32Ptr[7];

    // new for ECM3532
    pfnFlashBrDebrick = (tpfFlashBrDebrick)pui32Ptr[8];
    pfnFlashBrEraseBkpt = (tpfFlashBrErase)pui32Ptr[9];
    pfnFlashBrProgramBkpt = (tpfFlashBrProgram)pui32Ptr[10];
    pfnFlashBrReadBkpt = (tpfFlashBrRead)pui32Ptr[11];

    //
    // Return success.
    //
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspFlashProgram - Copies \a ui32Count bytes from \a pui8Src to the
 *                       ui32Address in flash \a fd.
 *
 *  @param ui32Address the address in flash to which we will copy the source
 *                     bytes.  This value must be 32-bit aligned.  For now this
 *                     must start at the beginning of a page.
 *  @param pui8Src The data buffer to copy from. Note this can be byte aligned.
 *  @param ui32Count The number of bytes to copy. MAX == 4096, i.e. page size.
 *  @param bBkpt set to one to use the breakpoint version of bootrom helper.
 *  @return eEtaSuccess or flash specific error status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspFlashProgram(uint32_t ui32Address, uint8_t *pui8Src, uint32_t ui32Count,
		   bool bBkpt)
{
    //
    // 1024 32-bit words. Aligned 64-bits.
    //
    uint32_t pui32Buffer[ETA_CSP_FLASH_PAGE_SIZE_WORDS];
    uint32_t *pui32SrcPtr;
    uint32_t *pui32DstPtr;
    int32_t i32I;
    uint8_t *pui8Ptr;
    uint32_t ui32Work;

    if(ui32Address < ETA_CSP_FLASH_BASE)
    {
        return(eEtaFlashAddressError+0);
    }
    if(ui32Address >= ETA_CSP_FLASH_MAX)
    {
        return(eEtaFlashAddressError+1);
    }
    if(ui32Count > ETA_CSP_FLASH_PAGE_SIZE_BYTES)
    {
        return(eEtaFlashPageSizeError+0);
    }

    //
    // Now check that area to be programmed doesn't cross a page boundary.
    // Handle starting offset from beginning of a page.
    //
    ui32Work = (ui32Address & ~ETA_CSP_FLASH_PAGE_SIZE_MASK) + ui32Count;
    if(ui32Work > ETA_CSP_FLASH_PAGE_SIZE_BYTES)
    {
        return(eEtaFlashAddressError+2);
    }

    //
    // This is redundant, checks for address to high or crossing a page boundary
    //   above will cover this
    //
    // if((ui32Address + ui32Count) > ETA_CSP_FLASH_MAX)
    // {
    //     return(16);
    // }

    //
    // Copy in the old stuff (read action of the read/modify/write).
    // do this as 32-bit reads and writes.
    //
    pui32SrcPtr = (uint32_t *)(ui32Address & ETA_CSP_FLASH_PAGE_SIZE_MASK);
    pui32DstPtr = pui32Buffer;
    for(i32I = 0; i32I < (ETA_CSP_FLASH_PAGE_SIZE_WORDS); i32I++)
    {
        *pui32DstPtr++ = *pui32SrcPtr ++;
    }


    //
    // Now overwrite it with the good stuff (modify part of read/modify/write).
    // Allow it to start on partial page boundary.
    //
    pui8Ptr = (uint8_t *)pui32Buffer;
    pui8Ptr += (ui32Address & ~ETA_CSP_FLASH_PAGE_SIZE_MASK);
    for(i32I = 0; i32I < ui32Count; i32I++)
    {
        *pui8Ptr++ = *pui8Src++;
    }

    //
    // Now do the write part of read/modify/write.
    //
    if(bBkpt)
    {
        pfnFlashBrProgramBkpt((ui32Address & ETA_CSP_FLASH_PAGE_SIZE_MASK),
                          (uint32_t *)pui32Buffer,
                          ETA_CSP_FLASH_PAGE_SIZE_WORDS, // number of 32-bit words
                          0, // program normal space
                          ETA_CSP_FLASH_TNVS_COUNT,
                          ETA_CSP_FLASH_TPGS_COUNT,
                          ETA_CSP_FLASH_TPROG_COUNT,
                          ETA_CSP_FLASH_TNVH_COUNT,
                          ETA_CSP_FLASH_TRCV_COUNT);
    }
    else
    {
        pfnFlashBrProgram((ui32Address & ETA_CSP_FLASH_PAGE_SIZE_MASK),
                          (uint32_t *)pui32Buffer,
                          ETA_CSP_FLASH_PAGE_SIZE_WORDS, // number of 32-bit words
                          0, // program normal space
                          ETA_CSP_FLASH_TNVS_COUNT,
                          ETA_CSP_FLASH_TPGS_COUNT,
                          ETA_CSP_FLASH_TPROG_COUNT,
                          ETA_CSP_FLASH_TNVH_COUNT,
                          ETA_CSP_FLASH_TRCV_COUNT);
    }

    //
    // Return success.
    //
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspFlashInfoProgram - Copies \a ui32Count bytes from \a pui8Src to the
 *                           address in flash INFO page \a fd.
 *
 *  @param ui32Address the address in flash to which we will copy the source
 *                     bytes.  For now this must start at the beginning of the
 *                     first page.
 *  @param pui8Src is the data pui64Buffer to copy from.  Note this can be byte
 *                 aligned.
 *  @param ui32Count is the number of bytes to copy.
 *  @param bBkpt set to one to use the breakpoint version of bootrom helper.
 *  @return eEtaSuccess or flash specific error status.
 *
 *  @note This must start at ETA_CSP_FLASH_BASE and the page size cannot be
 *        greater than ETA_CSP_FLASH_PAGE_SIZE_BYTES.
 *
 ******************************************************************************/
tEtaStatus
EtaCspFlashInfoProgram(uint32_t ui32Address, uint8_t *pui8Src,
                       uint32_t ui32Count, bool bBkpt)
{
    uint64_t pui64Buffer[ETA_CSP_FLASH_PAGE_SIZE_BYTES >> 3]; // 512 64-bit words
                                                              // aligned 64-bits
    uint32_t ui32NumExtra;
    uint32_t ui32MyCount;
    int32_t i32I;
    uint8_t *pui8Ptr;
    uint32_t ui32Work;

    ui32Work = (ui32Address & ~ETA_CSP_FLASH_PAGE_SIZE_MASK) + ui32Count;
    if(ui32Work > ETA_CSP_FLASH_PAGE_SIZE_BYTES)
    {
        return(eEtaFlashPageSizeError);
    }
    if(ui32Address != ETA_CSP_FLASH_BASE)
    {
        return(eEtaFlashAddressError);
    }

    ui32NumExtra = (ui32Count % 16) ? 1 : 0;
    ui32MyCount = (ui32Count >> 2) + ui32NumExtra;

    pui8Ptr = (uint8_t *)pui64Buffer;
    for(i32I = 0; i32I < ETA_CSP_FLASH_PAGE_SIZE_BYTES; i32I++)
    {
        *pui8Ptr++ = 0xFF;
    }
    pui8Ptr = (uint8_t *)pui64Buffer;
    for(i32I = 0; i32I < ui32Count; i32I++)
    {
        *pui8Ptr++ = *pui8Src++;
    }

    if(bBkpt)
    {
        pfnFlashBrProgramBkpt(ui32Address,
                          (uint32_t *)pui64Buffer,
                          ui32MyCount, // number of 32-bit words
                          1, // program INFO space
                          ETA_CSP_FLASH_TNVS_COUNT,
                          ETA_CSP_FLASH_TPGS_COUNT,
                          ETA_CSP_FLASH_TPROG_COUNT,
                          ETA_CSP_FLASH_TNVH_COUNT,
                          ETA_CSP_FLASH_TRCV_COUNT);
    }
    else
    {
        pfnFlashBrProgram(ui32Address,
                          (uint32_t *)pui64Buffer,
                          ui32MyCount, // number of 32-bit words
                          1, // program INFO space
                          ETA_CSP_FLASH_TNVS_COUNT,
                          ETA_CSP_FLASH_TPGS_COUNT,
                          ETA_CSP_FLASH_TPROG_COUNT,
                          ETA_CSP_FLASH_TNVH_COUNT,
                          ETA_CSP_FLASH_TRCV_COUNT);
    }

    //
    // Return success.
    //
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspFlashVersionGet - Returns a 32-bit version number of the BOOTROM code.
 *
 *  @return Returns a 32-bit version number of the BOOTROM code.
 *
 ******************************************************************************/
uint32_t
EtaCspFlashVersionGet(void)
{
    return(pfnFlashBrVersionHelper());
}

/***************************************************************************//**
 *
 *  EtaCspFlashBrLoad - Returns a 32-bit ui32Value of the supplied address.
 *
 *  @param ui32Address the address to be read.
 *  @return the value read from *ui32Address when it is cast to (uin32_t *).
 *
 ******************************************************************************/
uint32_t
EtaCspFlashBrLoad(uint32_t ui32Address)
{
    return(pfnFlashBrLoadHelper(ui32Address));
}

/***************************************************************************//**
 *
 *  EtaCspFlashBrStore - Stores a 32-bit value to the supplied address.
 *
 *  @param ui32Address the address to be written.
 *  @param ui32Value the value to be stored.
 *
 ******************************************************************************/
void
EtaCspFlashBrStore(uint32_t ui32Address, uint32_t ui32Value)
{
    pfnFlashBrStoreHelper(ui32Address, ui32Value);
}

/***************************************************************************//**
 *
 *  EtaCspFlashBrWsSet - Sets the 2-bit flash wait state selector.  This
 *                       function guarantees one is not running flash when this
 *                       happens.
 *
 *  @param ui32Select is the number of wait states for flash reads.
 *
 ******************************************************************************/
void
EtaCspFlashBrWsSet(uint32_t ui32Select)
{
    pfnFlashBrWsHelper(ui32Select);
}


/***************************************************************************//**
 *
 *  EtaCspFlashMassErase - Performs a mass erase of the entire FLASH except for
 *                         the INFO page.
 *
 ******************************************************************************/
void
EtaCspFlashMassErase(void)
{
    ETA_CSP_FLASH_MASS_ERASE();
}

/***************************************************************************//**
 *
 *  EtaCspFlashPageErase - Erase one page of the main flash.
 *
 *  @param ui32Address the address in flash which will be erased.
 *
 ******************************************************************************/
void
EtaCspFlashPageErase(uint32_t ui32Address)
{
    ETA_CSP_FLASH_PAGE_ERASE(ui32Address & ETA_CSP_FLASH_PAGE_SIZE_MASK);
}

/***************************************************************************//**
 *
 *  EtaCspFlashInfoErase - Erase the INFO page.
 *
 ******************************************************************************/
void
EtaCspFlashInfoErase(void)
{
    ETA_CSP_FLASH_INFO_ERASE();
}

/***************************************************************************//**
 *
 *  EtaCspFlashRead - Reads exactly four words from main flash using only the
 *                    Flash controller.
 *
 *  @param ui32Address is the address in flash from which to read.
 *  @param pui32Result is a pointer to the place to write 4, 32-bit words.
 *
 ******************************************************************************/
void
EtaCspFlashRead(uint32_t ui32Address, uint32_t *pui32Result)
{
    ETA_CSP_FLASH_READ(ui32Address, 0, pui32Result);
}

/***************************************************************************//**
 *
 *  EtaCspFlashInfoRead - Reads exactly four words from flash INFO page.
 *
 *  @param ui32Address is the address in flash INFO page from which to read.
 *  @param pui32Result is a pointer to the place to write 4, 32-bit words.
 *
 ******************************************************************************/
void
EtaCspFlashInfoRead(uint32_t ui32Address, uint32_t *pui32Result)
{
    ETA_CSP_FLASH_READ(ui32Address, 1, pui32Result);
}


/***************************************************************************//**
 *
 *  EtaCspFlashInfoGet - Reads N number of 8-bit bytes from flash INFO page.
 *
 *  @param ui32Address is the address in flash INFO page from which to read.
 *  @param pui8Result is a pointer to the place to write 8-bit words.
 *  @param ui32Count is the number of bytes to get from the INFO page.
 *
 ******************************************************************************/
void
EtaCspFlashInfoGet(uint32_t ui32Address, // starting bye address
                   uint8_t *pui8Result,
                   uint32_t ui32Count)   // number of bytes
{
    uint32_t ui32Result[4];
    uint8_t *pui8MyResult = (uint8_t *)ui32Result;
    uint32_t ui32InnerCount;
    uint32_t ui32I=0;

    ui32Address &= ~ETA_CSP_FLASH_PAGE_SIZE_MASK;
    ui32Address |=  ETA_CSP_FLASH_BASE;
    do
    {
        ui32InnerCount = 16 - (ui32Address & 0x0000000f);

	//
        // Read 16 bytes from INFO page.
	//
        EtaCspFlashInfoRead(ui32Address & 0xFFFFFFF0, ui32Result);


        //
	// Loop through copying up to 16 bytes.
	//
        do
	{
            *pui8Result++ = pui8MyResult[ui32Address & 0x0000000F];
            ui32Address++;
	    ui32I++;
	} while(--ui32InnerCount);
    } while(ui32I<ui32Count);

#ifdef NOTDEF
    for(ui32I=0;ui32I<ui32Count;ui32I++)
    {
         // yes, this is almost criminally inefficient
         EtaCspFlashInfoRead(ui32Address & 0xFFFFFFF0, ui32Result);
        *pui8Result++ = pui8Result[ui32Address & 0x0000000F];
    }
#endif
}

/***************************************************************************//**
 *
 *  EtaCspFlashRunningInFlash - Check PC to see if running in flash or SRAM.
 *
 *  @return  TRUE if running in flash
 *
 ******************************************************************************/
void *  startup_get_my_pc(void);
bool
EtaCspFlashRunningInFlash(void)
{
    bool     boolRunFlash;
    uint32_t ui32Work;

    ui32Work = (uint32_t) startup_get_my_pc();
    if( (ui32Work & 0xFF000000)==0x10000000){
       boolRunFlash = 0;
    }
    else{
       boolRunFlash = 1;
    }
    return (bool) boolRunFlash;
}

/***************************************************************************//**
 *
 *  EtaCspFlashEtaInfoBytesAreErased - Make sure all Eta Bytes in INFO  are erased
 *
 *  @return  TRUE if all bytes in first 1KB of INFO page are erased.
 *
 ******************************************************************************/
bool
EtaCspFlashEtaInfoBytesAreErased(void)
{
    bool     bFlashErased = true;
    int      ii;

    uint32_t result[4] = {
         0xBADBAD00
        ,0xBADBAD00
        ,0xBADBAD00
        ,0xBADBAD00
    };

    //
    // Step through INFO space four words at a time and confirm that it is all erased.
    for(ii=0;ii<(ETA_CSP_FLASH_PAGE_SIZE_WORDS);ii+=4){
        ETA_CSP_FLASH_READ((ETA_CSP_FLASH_BASE + (ii<<2) ), 1, result);
	if(result[0] != 0xFFFFFFFF) bFlashErased = false;
	if(result[1] != 0xFFFFFFFF) bFlashErased = false;
	if(result[2] != 0xFFFFFFFF) bFlashErased = false;
	if(result[3] != 0xFFFFFFFF) bFlashErased = false;
    }


    return bFlashErased;
}

/***************************************************************************//**
 *
 *  EtaCspFlashEtaInfoBytesAreAteLocked - Determine if ATE locked down the INFO
 *
 *  @return  TRUE if all bytes in first 1KB of INFO page are ATE Locked.
 *
 ******************************************************************************/
bool
EtaCspFlashEtaInfoBytesAreAteLocked(void)
{

    uint32_t result[4] = {
         0xBADBAD00
        ,0xBADBAD00
        ,0xBADBAD00
        ,0xBADBAD00
    };

    //
    // Read the 16 bytes containing the ATE lock word.
    //
    ETA_CSP_FLASH_READ((ETA_CSP_FLASH_BASE + (180<<2)), 1, result);

    //
    // See if the ATE lock pattern is present.
    //
    return (result[0] == 0xC001C0DE);
}

/***************************************************************************//**
 *
 *  EtaCspFlashProgramFast - Copies \a ui32Count bytes from \a pui8Src to the
 *                           ui32Address in flash \a fd.
 *
 *  @param ui32Address the address in flash to which we will copy the source
 *                     bytes.  This value must be 32-bit aligned.  this must
 *                     start at the beginning of a page.
 *  @param pui32Src The data buffer to copy from. Note this can be byte aligned.
 *  @return eEtaSuccess or flash specific error status.
 *
 * Program Flash with some restrictions that enhance speed. It copies exactly
 * and only one 4096 page per call. No less. No more. The starting address
 * must be page aligned.
 *
 ******************************************************************************/
tEtaStatus
EtaCspFlashProgramFast(uint32_t ui32Address, uint32_t *pui32Src)
{
    uint32_t ui32Rc;

    //
    // Validate parameters
    //
    if(ui32Address < ETA_CSP_FLASH_BASE)
    {
        return(eEtaFlashAddressError+0);
    }
    if((ui32Address+ETA_CSP_FLASH_PAGE_SIZE_BYTES) > ETA_CSP_FLASH_MAX)
    {
        return(eEtaFlashAddressError+1);
    }
    if((ui32Address & ~ETA_CSP_FLASH_PAGE_SIZE_MASK) != 0)
    {
        return(eEtaFlashAddressError+2);
    }

    //
    // Program the whole page.
    //
    ui32Rc = pfnFlashBrProgram((ui32Address & ETA_CSP_FLASH_PAGE_SIZE_MASK),
                               pui32Src,
                               ETA_CSP_FLASH_PAGE_SIZE_WORDS, // # 32-bit words
                               0, // program normal space
                               ETA_CSP_FLASH_TNVS_COUNT,
                               ETA_CSP_FLASH_TPGS_COUNT,
                               ETA_CSP_FLASH_TPROG_COUNT,
                               ETA_CSP_FLASH_TNVH_COUNT,
                               ETA_CSP_FLASH_TRCV_COUNT);
    //
    // Alert the world
    //
    return (ui32Rc) ? (ui32Rc + eEtaFlashAddressError) : eEtaSuccess;
}

