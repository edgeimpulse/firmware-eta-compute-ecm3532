/***************************************************************************//**
 *
 * @file eta_devices_spiflash.c
 *
 * @brief Generic SPI Flash interface
 *
 * Copyright (C) 2019 Eta Compute, Inc
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
#include "config.h"

#if CONFIG_SPI_FLASH
#include "spi_flash.h"
#include "gpio_hal.h"
#include "task.h"
#include "errno.h"
#include "print_util.h"

static tEtaStatus spiFlashWriteDataPPMaxLen(uint32_t offset, uint16_t length, void *data);
static tEtaStatus waitTillWIP(uint32_t waitTimeMs, bool sleep);
static tEtaStatus waitTillWEL(uint32_t waitTimeMs, bool sleep);

static spiFlashConfig_t spiFlashConfig = {
    .spiNum = eSpiInvalid,
    .chipSel = eSpiChipSelInvalid,
    .lock = NULL
};

#if CONFIG_SPI_FLASH_DEBUG
struct _spiFlashDebugData_
{
    uint32_t cmd;
    spiFlashWrapperIdentifier_t inProgress;
    uint32_t cmdStatus;
    uint32_t offset;
    void* buffer;
    uint32_t length;
    int32_t registerDone;
    uint32_t chipSel;
    uint32_t spiNum;
}spiFlashDebugData;
#endif

tEtaStatus spiFlashRegister(tSpiNum iNum, tSpiChipSel spiChipSel)
{
    tEtaStatus status;

    spiFlashConfig.spiNum = eSpiInvalid;
    spiFlashConfig.chipSel = eSpiChipSelInvalid;

    spiFlashConfig.lock = xSemaphoreCreateMutex();
    if(spiFlashConfig.lock == NULL)
    {
#if CONFIG_SPI_FLASH_DEBUG
        spiFlashDebugData.registerDone = -1;
#endif
        return eEtaFailure;
    }
    UNLOCK_SPI_FLASH();

    if(HalGpioOutInit(spiChipSel, 1) == -EINVAL)
    {
#if CONFIG_SPI_FLASH_DEBUG
        spiFlashDebugData.registerDone = -1;
#endif
        return eEtaSpiInvalidConfig;
    }

    configASSERT((iNum < eSpiMax) && (iNum > eSpiInvalid));

    spiFlashConfig.spiNum = iNum;
    spiFlashConfig.chipSel = spiChipSel;

#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.spiNum = iNum;
    spiFlashDebugData.chipSel = spiChipSel;
    spiFlashDebugData.registerDone = 1;
#endif
    return eEtaSuccess;
}

tEtaStatus spiFlashWriteEnable(void)
{
    WREN_t wren;
    tEtaStatus status;

    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    waitTillWIP(MAX_WAIT_DELAY,1);

    wren.cmd = WREN_CMD;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);

    status = HalSpiXfer(spiFlashConfig.spiNum, &wren, sizeof(WREN_t), NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = WREN_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif

    return status;
}

tEtaStatus spiFlashWriteDisable(void)
{
    WRDI_t wrdi;
    tEtaStatus status;

    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    waitTillWIP(MAX_WAIT_DELAY,1);

    wrdi.cmd = WRDI_CMD;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &wrdi, sizeof(WRDI_t), NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = WRDI_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashReadDeviceID(RDID_RES_t *rdid_res)
{
    RDID_t rdid;
    tEtaStatus status;

    configASSERT(rdid_res != NULL);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    rdid.cmd = RDID_CMD;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &rdid, sizeof(RDID_t), rdid_res, 3, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();

#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = RDID_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

static tEtaStatus spiFlashSendStatusCommand(void)
{
    RDSR_t rdsr;
    tEtaStatus status;

    rdsr.cmd = RDSR_CMD;
    status = HalSpiXfer(spiFlashConfig.spiNum, &rdsr, sizeof(RDSR_t), NULL, 0, NULL, NULL);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = RDSR_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}


static tEtaStatus spiFlashReadByte(void *data)
{
    RDSR_t rdsr;
    tEtaStatus status;

    configASSERT(data != NULL);

    status = HalSpiXfer(spiFlashConfig.spiNum, NULL, 0, data, 1, NULL, NULL);
    return status;
}

tEtaStatus spiFlashReadStatusRegister(void *rdsr_res)
{
    RDSR_t rdsr;
    tEtaStatus status;

    configASSERT(rdsr_res != NULL);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    rdsr.cmd = RDSR_CMD;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &rdsr, sizeof(RDSR_t), rdsr_res, 1, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();

#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = RDSR_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashReadControlRegister(RDCR_RES_t *rdcr_res)
{
    RDCR_t rdcr;
    tEtaStatus status;

    configASSERT(rdcr_res != NULL);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    rdcr.cmd = RDCR_CMD;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &rdcr, sizeof(RDCR_t), rdcr_res, 2, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = RDCR_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashWriteStatusControlRegister(WRSR_t *wrsr)
{
    tEtaStatus status;

    configASSERT(wrsr != NULL);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    wrsr->cmd = WRSR_CMD;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, wrsr, sizeof(WRSR_t), NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();

#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = WRSR_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashReadData(uint32_t offset, uint32_t length, void *buff)
{
    tEtaStatus status;
    READ_t read;

    configASSERT(buff != NULL);
    configASSERT(length > 0);
    configASSERT((offset + length) <= MAX_OFFSET_SIZE);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.buffer = buff;
    spiFlashDebugData.length = length;
    spiFlashDebugData.offset = offset;
    spiFlashDebugData.inProgress = WRAPPER_READ;
#endif
    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    read.cmd = READ_CMD;
    read.addr2 = (offset >> 16);
    read.addr1 = (offset >> 8);
    read.addr0 = (offset);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &read, sizeof(READ_t), buff, length, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = READ_CMD;
    spiFlashDebugData.cmdStatus = status;
    spiFlashDebugData.inProgress = WRAPPER_NONE;
#endif
    return status;
}

tEtaStatus spiFlashFastReadData(uint32_t offset, uint32_t length, void *buff)
{
    tEtaStatus status;
    FAST_READ_t fRead;

    configASSERT(buff != NULL);
    configASSERT(length > 0);
    configASSERT((offset + length) <= MAX_OFFSET_SIZE);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.buffer = buff;
    spiFlashDebugData.length = length;
    spiFlashDebugData.offset = offset;
    spiFlashDebugData.inProgress = WRAPPER_READ;
#endif
    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    fRead.cmd = FAST_READ_CMD;
    fRead.addr2 = (offset >> 16);
    fRead.addr1 = (offset >> 8);
    fRead.addr0 = (offset);
    fRead.dummy = 0;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &fRead, sizeof(FAST_READ_t), buff, length, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = FAST_READ_CMD;
    spiFlashDebugData.cmdStatus = status;
    spiFlashDebugData.inProgress = WRAPPER_NONE;
#endif
    return status;
}

tEtaStatus spiFlashSectorErase(uint32_t offset)
{
    tEtaStatus status;
    SE_t se;

    configASSERT(offset < MAX_OFFSET_SIZE);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_SECTOR_ERASE;
    spiFlashDebugData.offset = offset;
#endif
    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    spiFlashWriteEnable();

    waitTillWEL(MAX_WAIT_DELAY,0);

    se.cmd = SE_CMD;
    se.addr2 = (offset >> 16);
    se.addr1 = (offset >> 8);
    se.addr0 = (offset);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &se, sizeof(SE_t), NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_NONE;
    spiFlashDebugData.cmd = SE_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashBlockErase32K(uint32_t offset)
{
    tEtaStatus status;
    BE32K_t be32k;

    configASSERT(offset < MAX_OFFSET_SIZE);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_BLOCK32_ERASE;
    spiFlashDebugData.offset = offset;
#endif
    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    spiFlashWriteEnable();

    waitTillWEL(MAX_WAIT_DELAY,0);

    be32k.cmd = BE32K_CMD;
    be32k.addr2 = (offset >> 16);
    be32k.addr1 = (offset >> 8);
    be32k.addr0 = (offset);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &be32k, sizeof(BE32K_t), NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_NONE;
    spiFlashDebugData.cmd = BE32K_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    UNLOCK_SPI_FLASH();

    return status;
}

tEtaStatus spiFlashBlockErase(uint32_t offset)
{
    tEtaStatus status;
    BE_t be;

    configASSERT(offset < MAX_OFFSET_SIZE);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_BLOCK_ERASE;
    spiFlashDebugData.offset = offset;
#endif
    LOCK_SPI_FLASH();

    waitTillWIP(MAX_WAIT_DELAY,1);

    spiFlashWriteEnable();

    waitTillWEL(MAX_WAIT_DELAY,0);

    be.cmd = BE_CMD;
    be.addr2 = (offset >> 16);
    be.addr1 = (offset >> 8);
    be.addr0 = (offset);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &be, sizeof(BE_t), NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    UNLOCK_SPI_FLASH();
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_NONE;
    spiFlashDebugData.cmd = BE_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashChipErase(void)
{
    tEtaStatus status = eEtaSuccess;
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_CHIP_ERASE;
#endif
    LOCK_SPI_FLASH();

    for(int i=0;i<MAX_NUM_BLOCK; i++)
    {
        status = spiFlashBlockErase(i*BLOCK_LEN_BYTES);
        if(status != eEtaSuccess)
        {
            break;
        }
    }

    UNLOCK_SPI_FLASH();
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.inProgress = WRAPPER_NONE;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashDeepPowerDownMode(void)
{
    DP_t dp;
    tEtaStatus status;

    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    waitTillWIP(MAX_WAIT_DELAY,1);

    dp.cmd = DP_CMD;

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &dp, sizeof(DP_t), NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);
    return status;
}

tEtaStatus spiFlashExitDeepPowerDownMode(void)
{
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    vTaskDelay(1);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);

    return eEtaSuccess;
}

static tEtaStatus waitTillWIP(uint32_t waitTimeMs, bool sleep)
{
    TickType_t tick,currTick;
    RDSR_RES_t rdsr_res;
    tEtaStatus status;

    tick = xTaskGetTickCount() + waitTimeMs * CONFIG_TICK_RATE_HZ/1000;
    currTick = xTaskGetTickCount();

    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    spiFlashSendStatusCommand();
    while(tick > currTick)
    {
        spiFlashReadByte(&rdsr_res);
        if(rdsr_res.bits.wip)
        {
            status = eEtaSpiDoneTimeout;
            if(sleep)
            {
                vTaskDelay(1);
            }
        }
        else
        {
            status = eEtaSuccess;
            break;
        }
        currTick = xTaskGetTickCount();
    }
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);
    return status;
}

static tEtaStatus waitTillWEL(uint32_t waitTimeMs, bool sleep)
{
    TickType_t tick,currTick;
    RDSR_RES_t rdsr_res;
    tEtaStatus status;

    tick = xTaskGetTickCount() + waitTimeMs * CONFIG_TICK_RATE_HZ/1000;
    currTick = xTaskGetTickCount();

    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    spiFlashSendStatusCommand();
    while(tick > currTick)
    {
        spiFlashReadByte(&rdsr_res);
        if(!rdsr_res.bits.wel)
        {
            spiFlashWriteEnable();
            status = eEtaSpiDoneTimeout;
            if(sleep)
            {
                vTaskDelay(1);
            }
        }
        else
        {
            status = eEtaSuccess;
            break;
        }
        currTick = xTaskGetTickCount();
    }
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);
    return status;
}

static tEtaStatus spiFlashWriteDataPPMaxLen(uint32_t offset, uint16_t length, void *data)
{
    tEtaStatus status;
    PP_t pp;

    configASSERT(data != NULL);
    configASSERT(length <= PP_MAX_LEN);
    configASSERT(length > 0);
    configASSERT((offset + length) <= MAX_OFFSET_SIZE);
    configASSERT(spiFlashConfig.spiNum != eSpiInvalid);
    configASSERT(spiFlashConfig.chipSel != eSpiChipSelInvalid);

    waitTillWIP(MAX_WAIT_DELAY,1);

    spiFlashWriteEnable();

    waitTillWEL(MAX_WAIT_DELAY,0);

    pp.cmd = PP_CMD;
    pp.addr2 = (offset >> 16);
    pp.addr1 = (offset >> 8);
    pp.addr0 = (offset);

    HalSpiAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel, CS_ACTIVE_LOW);
    status = HalSpiXfer(spiFlashConfig.spiNum, &pp, sizeof(PP_t), NULL, 0, NULL, NULL);
    status = HalSpiXfer(spiFlashConfig.spiNum, data, length, NULL, 0, NULL, NULL);
    HalSpiDeAssertCS(spiFlashConfig.spiNum, spiFlashConfig.chipSel);
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmd = PP_CMD;
    spiFlashDebugData.cmdStatus = status;
#endif
    return status;
}

tEtaStatus spiFlashWriteData(uint32_t offset, uint32_t length, void *data)
{
    tEtaStatus status;
    uint16_t writeLength;

    configASSERT(data != NULL);
    configASSERT(length > 0);
    configASSERT((offset + length) <= MAX_OFFSET_SIZE);

    writeLength = PP_MAX_LEN - (offset & PP_MAX_LEN_MASK);
    if(writeLength > length)
    {
        writeLength = length;
    }
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.buffer = data;
    spiFlashDebugData.length = length;
    spiFlashDebugData.offset = offset;
    spiFlashDebugData.inProgress = WRAPPER_WRITE;
#endif
    LOCK_SPI_FLASH();

    while(length)
    {
        status = spiFlashWriteDataPPMaxLen(offset, writeLength, data);
        if(status != eEtaSuccess)
        {
            break;
        }

        length = length - writeLength;
        offset += writeLength;
        data += writeLength;
        writeLength = (length > PP_MAX_LEN) ? PP_MAX_LEN : length;
    }

    UNLOCK_SPI_FLASH();
#if CONFIG_SPI_FLASH_DEBUG
    spiFlashDebugData.cmdStatus = status;
    spiFlashDebugData.inProgress = WRAPPER_NONE;
#endif
    return status;
}

tEtaStatus spiFlashWriteSectors(void* data, uint16_t sectorNumber, uint16_t sectorCount)
{
    tEtaStatus status;
    uint32_t offset;
    uint32_t length;

    configASSERT(data != NULL);
    configASSERT(sectorCount > 0);
    configASSERT((sectorNumber + sectorCount) <= MAX_SECTOR_NUM);

    offset = sectorNumber * SECTOR_LEN_BYTES;
    length = sectorCount * SECTOR_LEN_BYTES;

    status = spiFlashWriteData(offset,length,data);

    return status;
}

tEtaStatus spiFlashReadSectors(void* data, uint16_t sectorNumber, uint16_t sectorCount)
{
    tEtaStatus status;
    uint32_t offset;
    uint32_t length;

    configASSERT(data != NULL);
    configASSERT(sectorCount > 0);
    configASSERT((sectorNumber + sectorCount) <= MAX_SECTOR_NUM);

    offset = sectorNumber * SECTOR_LEN_BYTES;
    length = sectorCount * SECTOR_LEN_BYTES;

    status = spiFlashReadData(offset,length,data);

    return status;
}

tEtaStatus spiFlashEraseSectors(uint16_t sectorNumber, uint16_t sectorCount)
{
    tEtaStatus status;
    uint32_t offset;

    configASSERT(sectorCount > 0);
    configASSERT((sectorNumber + sectorCount) <= MAX_SECTOR_NUM);

    while(sectorCount && (sectorNumber % 8 != 0))
    {
        offset = sectorNumber * SECTOR_LEN_BYTES;
        status = spiFlashSectorErase(offset);
        if(status != eEtaSuccess)
        {
            return status;
        }
        sectorCount--;
        sectorNumber++;
    }

    while(sectorCount && (sectorCount / 8 != 0) && (sectorNumber % 16 != 0))
    {
        offset = sectorNumber * SECTOR_LEN_BYTES;
        status = spiFlashBlockErase32K(offset);
        if(status != eEtaSuccess)
        {
            return status;
        }
        sectorCount -= 8;
        sectorNumber += 8;
    }

    while(sectorCount && (sectorCount / 16 != 0))
    {
        offset = sectorNumber * SECTOR_LEN_BYTES;
        status = spiFlashBlockErase(offset);
        if(status != eEtaSuccess)
        {
            return status;
        }
        sectorCount -= 16;
        sectorNumber += 16;
    }

    while(sectorCount && (sectorCount / 8 != 0))
    {
        offset = sectorNumber * SECTOR_LEN_BYTES;
        status = spiFlashBlockErase32K(offset);
        if(status != eEtaSuccess)
        {
            return status;
        }
        sectorCount -= 8;
        sectorNumber += 8;
    }

    while(sectorCount)
    {
        offset = sectorNumber * SECTOR_LEN_BYTES;
        status = spiFlashSectorErase(offset);
        if(status != eEtaSuccess)
        {
            return status;
        }
        sectorCount--;
        sectorNumber++;
    }

    return eEtaSuccess;
}
#endif
