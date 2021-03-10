/***************************************************************************//**
 *
 * @file eta_csp_i2c.c
 *
 * @brief This file contains eta_csp_i2c module implementations.
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
 * @addtogroup ecm3532i2c-dsp Inter-integrated Circuit (I2C)
 * @ingroup ecm3532csp-dsp
 * @{
 ******************************************************************************/

#include "reg.h"
#include "reg_eta_ecm3532_dsp.h"
#include "eta_csp_status.h"
#include "eta_csp_isr.h"
#include "eta_csp_i2c.h"
#include "eta_csp_mailbox.h"
#include "eta_csp_io.h"


//
// If 1, we'll do an additional read per interrupt to determine how much TX FIFO
// avail or RX FIFO ready and read that amount. If 0, we do one write/read per
// interrupt. Greedy tends to be more efficient, but does add one read which is
// not strictly required since we know the interrupt watermarks.
//
#define ETA_CSP_I2C_EN_GREEDY_INT 1

// Global pointer to command structs for I2C2
//volatile EtaCspI2cCmd_t *g_pEtaCspI2cCmd[1];
volatile tI2cCmd gsI2cCmd;

/*
int
etaCspI2cPollUntilDone(void)
{
    int tempdata0;
    uint16_t timeout = 10000;

    do
    {
        tempdata0 = REG_I2C2_STATUS;
    }
    while((--timeout) &&
          ((tempdata0 & BM_I2C2_STATUS_START_BUSY) ==
           BM_I2C2_STATUS_START_BUSY));

    if(timeout == 0)
    {
        ETA_CSP_I2C_LEGACY_DEBUG etaPrintf(
                                           "ERROR: Timed out on waiting for I2C%d TX to finish (SR=%x)\r\n", 2, tempdata0);
        return(-1);
    }
    else
    {
        return(0);
    }
}
*/

/*
int
etaCspI2cPollUntilRxHasData()
{
    int tempdata0;
    uint16_t timeout = 1000;

    do
    {
        tempdata0 = REG_I2C2_STATUS;
    }
    while((--timeout) && ((tempdata0 & BM_I2C2_STATUS_RXFIFO_HAS_1DATA) == 0));

    if(timeout == 0)
    {
        ETA_CSP_I2C_LEGACY_DEBUG etaPrintf(
                                           "ERROR: Timed out on waiting for I2C%d RX to be ready for data (STATUS=%x)\r\n", 2, tempdata0);
        return(-1);
    }
    else
    {
        return(0);
    }
}
*/

/*
int
etaCspI2cPollUntilTxRdyForData()
{
    int tempdata0;
    uint16_t timeout = 1000;

    do
    {
        tempdata0 = REG_I2C2_STATUS;
    }
    while((--timeout) && ((tempdata0 & BM_I2C2_STATUS_TXFIFO_HAS_1SPACE) == 0));

    if(timeout == 0)
    {
        ETA_CSP_I2C_LEGACY_DEBUG etaPrintf(
                                           "ERROR: Timed out on waiting for I2C%d TX to have space for data (STATUS=%x)\r\n", 2, tempdata0);
        return(-1);
    }
    else
    {
        return(0);
    }
}
*/

// #endif // NOTDEF

// etaCspI2cTransferPoll
// This logis sends / receives data to/from the external I2C device.
// It is setup to issue commands, necessarily a TX portion first and then a RX
// portion.
// In this way, the same data buffer can be used, or seperate buffers if
// desired.
// The sequence requires at least one TX (typically the Device ID)
// The sequence sent is:
// 1. Send tx_len of data from tx_data_array. Received data is ignored.
// 2. Send rx_len of dummy data (0xFF). Received data is pushed onto
// rx_data_array
// Additionally, a restart will be issued after restart_after_byte bytes are
// sent. If set to 0, no restart will be sent.
//
// As an example, a Adesto RM24C32C is a SEEPROM that uses 2 address bytes
// (called A0 and A1) below. Assuming it has its device
// address bits configured as 0, then a typical write of two bytes of data
// (0xD0, 0xD1) command would be:
// Write 0xa4 to TXRX_1BYTE_FIFO_DATA                 // Device ID, Write
// Write 0xa0 to TXRX_1BYTE_FIFO_DATA                 // SEEPROM Address
// Write 0xa1 to TXRX_1BYTE_FIFO_DATA                 // SEEPROM Address
// Write 0xd0 to TXRX_1BYTE_FIFO_DATA                 // Data0
// Write 0xd1 to TXRX_1BYTE_FIFO_DATA                 // Data1
// Write 0x04 to CMD_FIFO_DATA                        // Issue STOP
// A typical random read (which involes writing the desired address and then
// triggering a restart to read the data) of the same data would be:
// Write 0xa4 to TXRX_1BYTE_FIFO_DATA                 // Device ID, Write
// Write 0xa0 to TXRX_1BYTE_FIFO_DATA                 // SEEPROM Address
// Write 0xa1 to TXRX_1BYTE_FIFO_DATA                 // SEEPROM Address
// Write 0x01 to CMD_FIFO_DATA                        // Issue Restart
// Write 0xa5 to TXRX_1BYTE_FIFO_DATA                 // Device ID, Read
// Write 0xff to TXRX_1BYTE_FIFO_DATA                 // Dummy data (will be
// filled in by remote side)
// Write 0xd0 to CMD_FIFO_DATA                        // Issue command to drive
// NACK to 0 and enable RX FIFO
// Write 0xff to TXRX_1BYTE_FIFO_DATA                 // Dummy data (will be
// filled in by remote side)
// Write 0x34 to CMD_FIFO_DATA                        // Send NACK=1 on last
// read and issue STOP

// This comes from eta_i2c_csp.h
//// These defines are distilled into the 4 or 5 we need.
// #define I2C_CMD_ENABLE_RXFIFO
// (BFV_I2C2_CMD_FIFO_DATA_CMD_RXFIFO_ENABLE_RXFIFO  <<
// BP_I2C2_CMD_FIFO_DATA_CMD_RXFIFO )
// #define I2C_CMD_DISABLE_RXFIFO
// (BFV_I2C2_CMD_FIFO_DATA_CMD_RXFIFO_DISABLE_RXFIFO <<
// BP_I2C2_CMD_FIFO_DATA_CMD_RXFIFO )
// #define I2C_CMD_NACK_LOW
// (BFV_I2C2_CMD_FIFO_DATA_CMD_NACK_DRIVE_NACK_LOW   <<
// BP_I2C2_CMD_FIFO_DATA_CMD_NACK   )
// #define I2C_CMD_NACK_HIGH
// (BFV_I2C2_CMD_FIFO_DATA_CMD_NACK_DRIVE_NACK_HIGH  <<
// BP_I2C2_CMD_FIFO_DATA_CMD_NACK   )
// #define I2C_CMD_EXP_1NACK      (BM_I2C2_CMD_FIFO_DATA_EXP_1NACK
// )
// #define I2C_CMD_STOP           (BFV_I2C2_CMD_FIFO_DATA_CMD_SRD_STOP
// << BP_I2C2_CMD_FIFO_DATA_CMD_SRD    )
// #define I2C_CMD_DUMMYBYTE      (BFV_I2C2_CMD_FIFO_DATA_CMD_SRD_DUMMYBYTE
// << BP_I2C2_CMD_FIFO_DATA_CMD_SRD    )
//
// #define I2C_CMD_RESTART        (BFV_I2C2_CMD_FIFO_DATA_CMD_SRD_RESTART
// << BP_I2C2_CMD_FIFO_DATA_CMD_SRD    )
// #define I2C_CMD_BEGIN_READ     (I2C_CMD_ENABLE_RXFIFO | I2C_CMD_NACK_LOW)
// #define I2C_CMD_END_WRITE       I2C_CMD_STOP
// #define I2C_CMD_END_READ       (I2C_CMD_NACK_HIGH | I2C_CMD_STOP |
// I2C_CMD_EXP_1NACK)

// FIXME, currently an unexpected ACK is ignored and will lock up this polling
// ... maybe check only if TX FIFO does not have space?

/*
int
etaCspI2cTransferPoll(char *tx_data_array, int tx_len, char *rx_data_array,
                          int rx_len, int restart_after_byte)
{
    int byte_count;
    int tempdata0;
    int timeout;
    char tx_byte;
    char rx_byte;

    // Check that the command is acceptable
    // These can be removed if onerous
    if(tx_len == 0)
    {
        ETA_CSP_I2C_LEGACY_DEBUG etaPrintf(
                                           "ERROR: etaCspI2cTransferPoll for instance=%d has no tx data (tx_len %d). Not supported.", 2, tx_len);

        // users should send at least device id.
        return(-1);
    }

    if(tx_len < restart_after_byte)
    {
        ETA_CSP_I2C_LEGACY_DEBUG etaPrintf(
                                           "ERROR: etaCspI2cTransferPoll for instance=%d has restart after tx data (tx_len %d, restart_after_byte %d). Not supported.", 2, tx_len, restart_after_byte);

        // users should send at least device id.
        return(-1);
    }

// if (tx_len > 64*1024)
// {
// etaPrintf ("ERROR: etaCspI2cTransferPoll for instance=%d exceeded max
// tx_len of 64*1024 with %d", 2, tx_len);
// return -1;
// }
//
// if (rx_len > 64*1024)
// {
// etaPrintf ("ERROR: etaCspI2cTransferPoll for instance=%d exceeded max
// rx_len of 64*1024 with %d", 2, rx_len);
// return -1;
// }
// Check that the command is acceptable

    // Make sure previous command is done (optional)
    if(etaCspI2cPollUntilDone())
    {
        return(-1);
    }

    // Start command (start could be done later, but it will wait for first byte
    // sent to it).
    REG_I2C2_STATUS = BM_I2C2_STATUS_START_BUSY;
    byte_count = 0;

    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                         "etaCspI2cTransferPoll beginning tx\r\n");
    timeout = 10000;
    while((--timeout) && (tx_len))
    {
        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1);
        }

        tx_byte = *tx_data_array++;

        REG_I2C2_TXRX_1BYTE_FIFO_DATA = tx_byte;
        tx_len--;

        IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                             "etaCspI2cTransferPoll tx data sent (%d left)\r\n", tx_len);

        byte_count = byte_count + 1;
        if(byte_count == restart_after_byte)
        {
            if(etaCspI2cPollUntilTxRdyForData())
            {
                return(-1);
            }
            REG_I2C2_CMD_FIFO_DATA = I2C_CMD_RESTART;
            IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                                 "etaCspI2cTransferPoll restart issued\r\n");
        }
    }
    if(timeout == 0)
    {
        return(1);
    }

    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                         "etaCspI2cTransferPoll done with tx\r\n");

    // Now deal with RX.
    // If no rx, end wrtie
    if(rx_len == 0)
    {
        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1);
        }

        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_END_WRITE;
        IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                             "etaCspI2cTransferPoll corner case with rx_len=0\r\n");
        return(0);
    }

    // Each RX byte needs 1 TX dummy byte to start the state machine.
    // We will send the first and the last dummy byte manually (by writing to
    // TXRX_1BYTE_FIFO_DATA)
    // the others will be invoked from a dummy byte command.

    // The following commands queue up the reads, but do not process them.
    // This assumes there is a reasonable length of RX_FIFO to not be a blocking
    // issue while we pump commands into the TX_FIFO
    // If RX_FIFO depth is reduced, this may produce issues.

    // In unique cornercase of 1 RX length, we send all the commands at once
    // with one

    if(rx_len == 1)
    {
        IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                             "etaCspI2cTransferPoll beginning rx with rx_len=1\r\n");
        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1); // FIXME ... improve
                        // by reading tx fifo
                        // entries instead,
                        // we' know if we had
                        // enough for everone
        }
        REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_BEGIN_READ | I2C_CMD_END_READ;
    }
    else
    {
        IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                             "etaCspI2cTransferPoll beginning rx with rx_len>2\r\n");
        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1);
        }
        REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_BEGIN_READ;

        REG_I2C2_DUMMY_BYTES = rx_len - 1;
        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_DUMMYBYTE;

        if(etaCspI2cPollUntilTxRdyForData())
        {
            return(-1);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_END_READ;
    }

    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                         "etaCspI2cTransferPoll waiting for rx data\r\n");
    timeout = 10000;
    while((--timeout) && (rx_len))
    {
        if(etaCspI2cPollUntilRxHasData())
        {
            return(-1);
        }

        // pop data off the read stack
        rx_byte = REG_I2C2_TXRX_1BYTE_FIFO_DATA;
        *rx_data_array++ = rx_byte;

        rx_len--;
        IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                             "etaCspI2cTransferPoll rx data received (%d left)\r\n", rx_len);
    }

    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                         "etaCspI2cTransferPoll done waiting for rx data\r\n");
    return(timeout == 0);
}
*/

/*
//     etaCspI2cSpeedSetup - Sets up I2C speed assuming 4MHz clock.
//     Baud in KHz is typically 100,400,1000, but could be anything.
//
//     If this gets bad, these are hardcoded values for 100K and 400K that work in
//     TB
//     Actual boards with their variable pull up strengths may vary.
//
//     100K
//     CYCLE_TIME        = 39;
//     IBG_TIME          = CYCLE_TIME;
//     PE_TIME           = 16;
//     NE_TIME           = 31;
//     RX_TIME           = 25;
//     TX_TIME           = 1;
//     MIN_SCL_HI        = 4;
//     HYSTERSIS_LEVEL   = 8;
//     400K
//     CYCLE_TIME        = 9;
//     IBG_TIME          = CYCLE_TIME;
//     PE_TIME           = 4;
//     NE_TIME           = 8;
//     RX_TIME           = 5;
//     TX_TIME           = 1;
//     MIN_SCL_HI        = 0;
//     HYSTERSIS_LEVEL   = 1;

int
etaCspI2cSpeedSetup(int baud_in_khz)
{
    int config2_temp;
    int scl_config_temp;

    char CYCLE_TIME;
    char IBG_TIME;
    char PE_TIME;
    char NE_TIME;
    char TX_TIME;
    char RX_TIME;
    char MIN_SCL_HI;
    char SCL_HYSTERSIS_LEVEL;
    char SDA_HYSTERSIS_LEVEL;
    char TEMP_TIME;

    // Read default and clear out fields we may change
    config2_temp = REG_I2C2_CONFIG2;
    config2_temp &= ~BM_I2C2_CONFIG2_SDA_HYSTERISIS_LEVEL;
    config2_temp &= ~BM_I2C2_CONFIG2_DISABLE_SDA_HYSTERSIS;
    config2_temp &= ~BM_I2C2_CONFIG2_SCL_HYSTERISIS_LEVEL;
    config2_temp &= ~BM_I2C2_CONFIG2_DISABLE_SCL_HYSTERSIS;

    scl_config_temp = REG_I2C2_SCL_CONFIG;
    scl_config_temp &= ~BM_I2C2_SCL_CONFIG_DISABLE_SCL_HIGH_CHK;
    scl_config_temp &= ~BM_I2C2_SCL_CONFIG_HARD_DRIVE_SCL;
    scl_config_temp &= ~BM_I2C2_SCL_CONFIG_MIN_SCL_HIGH;

    // We take the known good points at 400KHz and 100KHz and assume that it is
    // a generally
    // linear timing between those points. So we calc TEMP_TIME which is
    // effectively percentage
    // desired baud is between 100 and 400KHz numbers. From that we take the:
    // 400KHz number and add that to the percentage of 400KHz-100KHz numbers.
    // So PE time is 4 @ 400KHz and 16 @ 100KHz, so we add 4 + (16-4) *
    // percentage which gives us
    // the desired freq.

    if(baud_in_khz < 500) // Use algorithm for lower speed interfaces
    {
        CYCLE_TIME = (8000 / baud_in_khz) - 1; // Assumes 8MHz source
                                               // clock.
        IBG_TIME = CYCLE_TIME;
        TEMP_TIME = ((400 - baud_in_khz) / 3);
        PE_TIME = (4 + ((TEMP_TIME * (16 - 4))) / 100) * 2;
        NE_TIME = (8 + ((TEMP_TIME * (31 - 8))) / 100) * 2;
        RX_TIME = (5 + ((TEMP_TIME * (25 - 5))) / 100) * 2;
        TX_TIME = (1 + ((TEMP_TIME * (1 - 1))) / 100) * 2;
        MIN_SCL_HI = (0 + ((TEMP_TIME * (4 - 0))) / 100) * 2;
        SCL_HYSTERSIS_LEVEL = (1 + ((TEMP_TIME * (8 - 1))) / 100) * 2;
        SDA_HYSTERSIS_LEVEL = (1 + ((TEMP_TIME * (8 - 1))) / 100) * 2;

        // Enable hystersis
        if(SCL_HYSTERSIS_LEVEL > 0)
        {
            config2_temp |= (16 - SCL_HYSTERSIS_LEVEL) <<
                            BP_I2C2_CONFIG2_SCL_HYSTERISIS_LEVEL;
        }
        else
        {
            config2_temp |= BM_I2C2_CONFIG2_DISABLE_SCL_HYSTERSIS;
        }

        if(SDA_HYSTERSIS_LEVEL > 0)
        {
            config2_temp |= (16 - SDA_HYSTERSIS_LEVEL) <<
                            BP_I2C2_CONFIG2_SDA_HYSTERISIS_LEVEL;
        }
        else
        {
            config2_temp |= BM_I2C2_CONFIG2_DISABLE_SDA_HYSTERSIS;
        }
    }
    else // if (baud_in_khz >= 500) // Use prerecorded values for high speed.
    {
        CYCLE_TIME = (8000 / baud_in_khz) - 1; // Assumes 4MHz source clock.
        IBG_TIME = 0; // We want max speed, so lets turn down IBG
        PE_TIME = 2;
        NE_TIME = 4;
        RX_TIME = 6; // This is done at this time because we are sampling
                     // "late" due to metastable FF.
        TX_TIME = 0;
        MIN_SCL_HI = 0;

        // Disable Hysterisis
        SCL_HYSTERSIS_LEVEL = 0;
        SDA_HYSTERSIS_LEVEL = 0;
        config2_temp |= BM_I2C2_CONFIG2_DISABLE_SCL_HYSTERSIS;
        config2_temp |= BM_I2C2_CONFIG2_DISABLE_SDA_HYSTERSIS;

        if(baud_in_khz >= 700) // This is a SWAG, but we work fine without these
                               // at 666KHz
        {
            scl_config_temp |= BM_I2C2_SCL_CONFIG_DISABLE_SCL_HIGH_CHK;
            scl_config_temp |= BM_I2C2_SCL_CONFIG_HARD_DRIVE_SCL;
        }
    }

    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("I2C%d timing parameters\r\n", 2);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("  CYCLE_TIME :%d\r\n", CYCLE_TIME);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("  IBG_TIME   :%d\r\n", IBG_TIME);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("  PE_TIME    :%d\r\n", PE_TIME);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("  NE_TIME    :%d\r\n", NE_TIME);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("  RX_TIME    :%d\r\n", RX_TIME);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("  TX_TIME    :%d\r\n", TX_TIME);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf("  MIN_SCL_HI :%d\r\n", MIN_SCL_HI);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                         "  SCL_HYSTERSIS (glitch avoidance) %d clocks (%d ns) \r\n", SCL_HYSTERSIS_LEVEL, SCL_HYSTERSIS_LEVEL * 250);
    IF_I2C_DETAIL_LEGACY_DEBUG etaPrintf(
                                         "  SDA_HYSTERSIS (glitch avoidance) %d clocks (%d ns) \r\n", SDA_HYSTERSIS_LEVEL, SDA_HYSTERSIS_LEVEL * 250);

    REG_I2C2_CLK_CONFIG1 = (IBG_TIME << BP_I2C2_CLK_CONFIG1_IBG_TIME) |
                           (CYCLE_TIME << BP_I2C2_CLK_CONFIG1_CYCLE_TIME);
    REG_I2C2_CLK_CONFIG2 = (TX_TIME <<  BP_I2C2_CLK_CONFIG2_TX_TIME)  |
                           (RX_TIME << BP_I2C2_CLK_CONFIG2_RX_TIME);
    REG_I2C2_CLK_CONFIG3 = (PE_TIME <<  BP_I2C2_CLK_CONFIG3_PE_TIME)  |
                           (NE_TIME << BP_I2C2_CLK_CONFIG3_NE_TIME);

    REG_I2C2_SCL_CONFIG = scl_config_temp | MIN_SCL_HI;
    REG_I2C2_CONFIG2 = config2_temp;

    return(0);
}
*/

/*
int
etaCspI2cMasterSetup(int baud_in_khz)
{
    int result = 0;

    volatile int tempdata0;

    result |= etaCspI2cSpeedSetup(baud_in_khz);

    REG_I2C2_CONFIG = BM_I2C2_CONFIG_STALL_IF_RXFULL |
                      BM_I2C2_CONFIG_HOLD_AT_ACK;

    // Perform soft reset of SM and set stall TX if RX FULL
    tempdata0 = REG_I2C2_CONFIG2;
    REG_I2C2_CONFIG2 = tempdata0 | BM_I2C2_CONFIG2_SOFT_RST;
    REG_I2C2_CONFIG2 = tempdata0;

    return(result);
}
*/

/*
int
etaCspI2cNackCheckDisable()
{
    volatile int tempdata0;

    tempdata0 = REG_I2C2_CONFIG;
    tempdata0 |= BM_I2C2_CONFIG_IGNORE_NACK;
    REG_I2C2_CONFIG = tempdata0;

    return(0);
}
*/

/*
int
etaCspI2cNackCheckEnable()
{
    volatile int tempdata0;

    tempdata0 = REG_I2C2_CONFIG;
    tempdata0 &= ~BM_I2C2_CONFIG_IGNORE_NACK;
    REG_I2C2_CONFIG = tempdata0;

    return(0);
}
*/

/*
int
etaCspI2cLineClear()
{
    int result = 0;
    volatile int tempdata0;

    result |= etaCspI2cNackCheckDisable();
    REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
    REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
    REG_I2C2_CMD_FIFO_DATA = I2C_CMD_STOP;
    REG_I2C2_STATUS = BM_I2C2_STATUS_START_BUSY;
    result |= etaCspI2cPollUntilDone();
    result |= etaCspI2cNackCheckEnable();
    return(result);
}
*/

/***************************************************************************//**
 *
 * EtaCspI2cSpeedConfig - Configure the speed for the I2C module.
 *
 * @param iClockSpeed is the I2C clock speed
 * @param iSrcClockSpeed is the source clock speed
 *
 * > To set an I2C speed greater than 100kHz source clock speed
 *   has to be 4MHz or greater.
 *
 * @return Returns the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cSpeedConfig(tI2cClk iClockSpeed, tI2cSrcClk iSrcClockSpeed)
{
    char CYCLE_TIME;
    char IBG_TIME;
    char PE_TIME;
    char NE_TIME;
    char TX_TIME;
    char RX_TIME;
    char MIN_SCL_HI;
    char SCL_HYSTERSIS_LEVEL;
    char SDA_HYSTERSIS_LEVEL;
    char TEMP_TIME;

    //
    // If source clock is less than 4MHz, then I2C speeds greater than 100kHz
    // are not allowed.
    //
    if((iSrcClockSpeed < 4000) && (iClockSpeed > 100))
    {
        return eEtaI2cInvalidConfig;
    }

    //
    // Use algorithm for lower speed interfaces.
    //
    if(iClockSpeed < 500)
    {
        CYCLE_TIME = (iSrcClockSpeed / iClockSpeed) - 1; // 4MHz source clock.
        IBG_TIME = CYCLE_TIME;
        TEMP_TIME = ((400 - iClockSpeed) / 3);
        PE_TIME = 4 + ((TEMP_TIME * (16 - 4))) / 100;
        NE_TIME = 8 + ((TEMP_TIME * (31 - 8))) / 100;
        RX_TIME = 5 + ((TEMP_TIME * (25 - 5))) / 100;
        TX_TIME = 1 + ((TEMP_TIME * (1  - 1))) / 100;
        MIN_SCL_HI = 0 + ((TEMP_TIME * (4 - 0))) / 100;
        SCL_HYSTERSIS_LEVEL = 1 + ((TEMP_TIME * (8 - 1))) / 100;
        SDA_HYSTERSIS_LEVEL = 1 + ((TEMP_TIME * (8 - 1))) / 100;

        if(iSrcClockSpeed==eI2cSrcClk2MHz)
        {
            PE_TIME = (PE_TIME - 1) / 2;
            NE_TIME = (NE_TIME - 1) / 2;
            RX_TIME = (RX_TIME - 1) / 2;
            //TX_TIME = (TX_TIME - 1) / 2;
            MIN_SCL_HI = (MIN_SCL_HI - 1) / 2;
            SCL_HYSTERSIS_LEVEL = (SCL_HYSTERSIS_LEVEL - 1) / 2;
            SDA_HYSTERSIS_LEVEL = (SDA_HYSTERSIS_LEVEL - 1) / 2;
        }

        if(iSrcClockSpeed==eI2cSrcClk8MHz)
        {
            PE_TIME = (PE_TIME * 2) + 1;
            NE_TIME = (NE_TIME * 2) + 1;
            RX_TIME = (RX_TIME * 2) + 1;
            TX_TIME = (TX_TIME * 2) + 1;
            MIN_SCL_HI = (MIN_SCL_HI * 2) + 1;
            SCL_HYSTERSIS_LEVEL = (SCL_HYSTERSIS_LEVEL * 2) + 1;
            SDA_HYSTERSIS_LEVEL = (SDA_HYSTERSIS_LEVEL * 2) + 1;
        }
    }

    //
    // If (iClockSpeed >= 500) Use prerecorded values.
    //
    else
    {
        CYCLE_TIME = 3;
        IBG_TIME = CYCLE_TIME;
        PE_TIME = 1;
        NE_TIME = 2;
        RX_TIME = 3;
        TX_TIME = 0;
        MIN_SCL_HI = 0;

        //
        // Disable Hysteresis.
        //
        SCL_HYSTERSIS_LEVEL = 0;
        SDA_HYSTERSIS_LEVEL = 0;

        if(iSrcClockSpeed==eI2cSrcClk8MHz)
        {
            CYCLE_TIME = (CYCLE_TIME * 2) + 1;
            IBG_TIME = (IBG_TIME * 2) + 1;
            PE_TIME = (PE_TIME * 2) + 1;
            NE_TIME = (NE_TIME * 2) + 1;
            RX_TIME = (RX_TIME * 2) - 1;
            TX_TIME = (TX_TIME * 2) + 1;
            MIN_SCL_HI = (MIN_SCL_HI * 2) + 1;
            SCL_HYSTERSIS_LEVEL = 1;
            SDA_HYSTERSIS_LEVEL = 1;
        }

    }

    REG_I2C2_CLK_CONFIG1 = (IBG_TIME << BP_I2C2_CLK_CONFIG1_IBG_TIME) |
                           (CYCLE_TIME << BP_I2C2_CLK_CONFIG1_CYCLE_TIME);
    REG_I2C2_CLK_CONFIG2 = (TX_TIME << BP_I2C2_CLK_CONFIG2_TX_TIME) |
                           (RX_TIME << BP_I2C2_CLK_CONFIG2_RX_TIME);
    REG_I2C2_CLK_CONFIG3 = (PE_TIME << BP_I2C2_CLK_CONFIG3_PE_TIME) |
                           (NE_TIME << BP_I2C2_CLK_CONFIG3_NE_TIME);
    REG_I2C2_SCL_CONFIG = (MIN_SCL_HI << BP_I2C2_SCL_CONFIG_MIN_SCL_HIGH);

    //
    // Enable hysteresis.
    //
    if(SCL_HYSTERSIS_LEVEL > 0)
    {
        REG_I2C2_CONFIG2 =
            (REG_I2C2_CONFIG2 & (~BM_I2C2_CONFIG2_SCL_HYSTERISIS_LEVEL)) |
            ((16 - SCL_HYSTERSIS_LEVEL) <<
                BP_I2C2_CONFIG2_SCL_HYSTERISIS_LEVEL);
        REG_I2C2_CONFIG2 =
            (REG_I2C2_CONFIG2 & (~BM_I2C2_CONFIG2_DISABLE_SCL_HYSTERSIS));
    }
    else
    {
        REG_I2C2_CONFIG2 =
            (REG_I2C2_CONFIG2 & (~BM_I2C2_CONFIG2_DISABLE_SCL_HYSTERSIS)) |
            (1 << BP_I2C2_CONFIG2_DISABLE_SCL_HYSTERSIS);
    }

    //
    // Enable hysteresis.
    //
    if(SDA_HYSTERSIS_LEVEL > 0)
    {
        REG_I2C2_CONFIG2 =
            (REG_I2C2_CONFIG2 & (~BM_I2C2_CONFIG2_SDA_HYSTERISIS_LEVEL)) |
            ((16 - SDA_HYSTERSIS_LEVEL) <<
                BP_I2C2_CONFIG2_SDA_HYSTERISIS_LEVEL);
        REG_I2C2_CONFIG2 =
            (REG_I2C2_CONFIG2 & (~BM_I2C2_CONFIG2_DISABLE_SDA_HYSTERSIS));
    }
    else
    {
        REG_I2C2_CONFIG2 =
            (REG_I2C2_CONFIG2 & (~BM_I2C2_CONFIG2_DISABLE_SDA_HYSTERSIS)) |
            (1 << BP_I2C2_CONFIG2_DISABLE_SDA_HYSTERSIS);
    }

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 * EtaCspI2cDonePoll - Poll for done.
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cDonePoll()
{
    uint32_t ui32Timeout = 1000000;

    while((--ui32Timeout) &&
          ((REG_I2C2_STATUS & BM_I2C2_STATUS_START_BUSY) != 0) &&
          ((REG_I2C2_STATUS & BM_I2C2_STATUS_SM_IN_NACK_HOLD) == 0))
    {
    }

    if(ui32Timeout <= 0)
    {
        return(eEtaI2cDoneTimeout);
    }
    else
    {
        if((REG_I2C2_STATUS & BM_I2C2_STATUS_SM_IN_NACK_HOLD) != 0) // We
                                                                    // encountered
                                                                    // a NACK,
                                                                    // stop
                                                                    // command,
                                                                    // report
                                                                    // back.
        {
            REG_I2C2_CONFIG2 |=  BM_I2C2_CONFIG2_SOFT_RST;
            REG_I2C2_CONFIG2 &= ~BM_I2C2_CONFIG2_SOFT_RST;
            return(eEtaI2cUnexpectedNack);
        }
        else
        {
            return(eEtaSuccess);
        }
    }
}

/***************************************************************************//**
 *
 * EtaCspI2cRxDataPoll() - Poll for RX data ready.
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cRxDataPoll()
{
    uint32_t ui32Timeout = 1000000;

    while((--ui32Timeout) &&
          ((REG_I2C2_STATUS & BM_I2C2_STATUS_RXFIFO_HAS_1DATA) == 0))
    {
    }

    if(ui32Timeout <= 0)
    {
        return(eEtaI2cRecTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspI2cTxReadyPoll() - Poll for TX ready.
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cTxReadyPoll()
{
    uint32_t ui32Timeout = 10000000;

    while((--ui32Timeout) &&
          ((REG_I2C2_STATUS & BM_I2C2_STATUS_TXFIFO_HAS_1SPACE) == 0))
    {
    }

    if(ui32Timeout <= 0)
    {
        return(eEtaI2cSendTimeout);
    }
    else
    {
        return(eEtaSuccess);
    }
}

/***************************************************************************//**
 *
 * EtaCspI2cLineClear - Triggers an I2C clearing event to clear any residual
 * commands
 *                      issued before a soft reset
 *
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cLineClear()
{
    tEtaStatus iResult;

    REG_I2C2_CONFIG = (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_IGNORE_NACK) |
                      (1 << BP_I2C2_CONFIG_IGNORE_NACK);

    REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
    REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
    REG_I2C2_CMD_FIFO_DATA = I2C_CMD_STOP;
    REG_I2C2_STATUS = (1 << BP_I2C2_STATUS_START_BUSY);

    iResult = EtaCspI2cDonePoll();
    if(iResult != eEtaSuccess)
    {
        return(iResult);
    }

    REG_I2C2_CONFIG = (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_IGNORE_NACK) |
                      (0 << BP_I2C2_CONFIG_IGNORE_NACK);

    //
    // Return the result.
    //
    return(iResult);
}

/***************************************************************************//**
 *
 * EtaCspI2cInit - Initialize the I2C module.
 *
 * @param iClockSpeed is the I2C clock speed
 * @param iSrcClockSpeed is the source clock speed
 * @return Returns eEtaSuccess if initialization is successful.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cInit(tI2cClk iClockSpeed, tI2cSrcClk iSrcClockSpeed)
{
    tEtaStatus iResult;

    iResult = EtaCspI2cSpeedConfig(iClockSpeed, iSrcClockSpeed);
    if(iResult != eEtaSuccess)
    {
        return(iResult);
    }

    REG_I2C2_CONFIG = BM_SPI2_CONFIG2_STALL_IF_RXFULL |
                      BM_I2C2_CONFIG_HOLD_AT_ACK;

    REG_I2C2_CONFIG2 |=  BM_I2C2_CONFIG2_SOFT_RST;
    REG_I2C2_CONFIG2 &= ~BM_I2C2_CONFIG2_SOFT_RST;

    //
    // I2C setup.
    //
    EtaCspI2cDonePoll();
    EtaCspI2cTxReadyPoll();

    //
    // Clear the Line from any aborted previous command.
    //
    EtaCspI2cLineClear();

    //
    // Return the result.
    //
    return(iResult);
}

/***************************************************************************//**
 *
 * EtaCspI2cTransferDmaConfig - Enable DMA transfers from/to I2C device.
 *
 * NOTE: Call this function first, if you need to read from (write to) a
 * I2C device using a DMA transfer. This function also puts the internal
 * state machine to "new I2C transaction start" state. So, make sure any
 * pending I2C transactions are finished before calling this. You can use
 * EtaCspI2cDonePoll() for this.
 ******************************************************************************/
void
EtaCspI2cTransferDmaConfig(void)
{
    // enable TX/RX Ready (dst_rdy/src_rdy to DMA control)
    REG_I2C2_INT_EN = BM_I2C2_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN_DSTRDY |
                      BM_I2C2_INT_EN_RX_DATA_EN_SRCRDY;

    // Set TX Level to be 2 (Data and Cmd)
    REG_I2C2_CONFIG |= (2 << BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);

    // Start command
    REG_I2C2_STATUS = BM_I2C2_STATUS_START_BUSY;
}

/***************************************************************************//**
 *
 * EtaCspI2cTransferPoll - Read and or write to an I2C device.
 *
 * @param pui8TxData Pointer to the data to transmit.
 * @param ui16TxLen Transmit length in bytes.
 * @param pui8RxData Point to the received data.
 * @param ui16RxLen Receive length in bytes.
 * @param ui8RestartAfterByte Perform a repeated start after this mayn bytes.
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cTransferPoll(uint8_t *pui8TxData, uint16_t ui16TxLen,
                      uint8_t *pui8RxData, uint16_t ui16RxLen,
                      uint16_t ui8RestartAfterByte)
{
    uint32_t ui32ByteCount, ui32Timeout;
    uint8_t ui8TxByte, ui8RxByte;
    tEtaStatus localI2cStatus;

    //
    // Check that the command is acceptable.
    //
    if(ui16TxLen == 0)
    {
        return(eEtaI2cInvalidConfig);
    }

    if(ui16TxLen < ui8RestartAfterByte)
    {
        return(eEtaI2cInvalidConfig);
    }

// these are impossible to hit with 16 bit integers
// if(ui16TxLen > 64 * 1024)
// {
// #ifdef ETA_CSP_I2C_DETAILED_DEBUG
// etaPrintf("(DEBUG Info) ERROR: EtaCspI2cTransferPoll, exceeds max " \
//                          "ui16TxLen=%d\r\n", ui16TxLen);
// #endif
// return(eEtaI2cInvalidConfig);
// }
//
// if(ui16RxLen > 64 * 1024)
// {
// #ifdef ETA_CSP_I2C_DETAILED_DEBUG
// etaPrintf("(DEBUG Info) ERROR: EtaCspI2cTransferPoll, exceeds max " \
//                          "ui16RxLen=%d\r\n", ui16RxLen);
// #endif
// return(eEtaI2cInvalidConfig);
// }

    //
    // Make sure previous command is done (optional).
    //
    if(localI2cStatus = EtaCspI2cDonePoll())
    {
        return(localI2cStatus);
    }

    //
    // Start command (waits for first byte).
    //
    REG_I2C2_STATUS = BM_I2C2_STATUS_START_BUSY;

    ui32ByteCount = 0;
    ui32Timeout = 10000;
    while((--ui32Timeout) && (ui16TxLen))
    {
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }

        ui8TxByte = *pui8TxData++;

        REG_I2C2_TXRX_1BYTE_FIFO_DATA = ui8TxByte;
        ui16TxLen--;

        ui32ByteCount = ui32ByteCount + 1;
        if(ui32ByteCount == ui8RestartAfterByte)
        {
            if(localI2cStatus = EtaCspI2cTxReadyPoll())
            {
                return(localI2cStatus);
            }

            REG_I2C2_CMD_FIFO_DATA = I2C_CMD_RESTART;
        }
    }

    // This timeout is for general programming reasons to prevent infinite loop
    if(ui32Timeout == 0)
    {
        return(eEtaFailure);
    }

    //
    // If no rx, end write.
    //
    if(ui16RxLen == 0)
    {
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }

        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_END_WRITE;
        return(eEtaSuccess);
    }

    if(ui16RxLen == 1)
    {
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_BEGIN_READ
                                 | I2C_CMD_END_READ;
    }
    else if(ui16RxLen == 2)
    {
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_BEGIN_READ;
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_END_READ;
    }
    else
    {
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_BEGIN_READ;

        REG_I2C2_DUMMY_BYTES = ui16RxLen - 2;
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_DUMMYBYTE;

        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
        if(localI2cStatus = EtaCspI2cTxReadyPoll())
        {
            return(localI2cStatus);
        }
        REG_I2C2_CMD_FIFO_DATA = I2C_CMD_END_READ;
    }

    if(pui8RxData == NULL)
    {
        return(eEtaSuccess);
    }

    ui32Timeout = 10000;
    while((--ui32Timeout) && (ui16RxLen))
    {
        if(localI2cStatus = EtaCspI2cRxDataPoll())
        {
            return(localI2cStatus);
        }

        //
        // Pop data off the read stack.
        //
        ui8RxByte = REG_I2C2_TXRX_1BYTE_FIFO_DATA;
        *pui8RxData++ = ui8RxByte;

        ui16RxLen--;
    }


    // This timeout is for general programming reasons to prevent infinite loop
    if(ui32Timeout == 0)
    {
        return(eEtaFailure);
    }

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 * EtaCspI2cDeviceAck - Read and or write to an I2C device.
 *
 * @param ui8TxData DevAddr and Read(write not) of device to poll for ack
 * @return Return the status.
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cDeviceAck(uint8_t ui8TxData)
{
    tEtaStatus result = eEtaSuccess;

    // Turn off NACK hold
    REG_I2C2_CONFIG = (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_IGNORE_NACK) |
                      (1 << BP_I2C2_CONFIG_IGNORE_NACK);

    // upper 7 is device id, lower bit is Read (write not)
    result |= EtaCspI2cTransferPoll(&ui8TxData, 1, &ui8TxData, 0, 1);

    result |= EtaCspI2cDonePoll();

    // Turn back on NACK hold
    REG_I2C2_CONFIG = (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_IGNORE_NACK) |
                      (0 << BP_I2C2_CONFIG_IGNORE_NACK);

    if(result)
    {
        return(eEtaFailure);
    }

    if(REG_I2C2_GENERAL_DEBUG & BM_I2C2_GENERAL_DEBUG_LAST_BYTE_HAD_NACK)
    {
        result = eEtaI2cUnexpectedNack;
    }
    else
    {
        result = eEtaSuccess;
    }

    return(result);
}

//// Returns 1 if it did ack, 0 if it did not and -1 if error.
// int i2c_did_device_ack (int deviceid){
// int index0;
// char tx_data;
// volatile int tempdata0;
// int result=0;
//
// result |= etaCspI2cNackCheckDisable();
//
// tx_data = deviceid;
// result |= EtaCspI2cTransferPoll(&tx_data, 1, &tx_data, 0, 1);
//
// result |= EtaCspI2cDonePoll();
//
// result |= etaCspI2cNackCheckEnable();
//
// if (result)
// {
// etaPrintf ("i2c_did_device_ack had nonzero result %d", result);
// return APP_I2C_RESULT_ERR;
// }
//
// tempdata0 = REG_I2C2_GENERAL_DEBUG;
// if (tempdata0 & BM_I2C2_GENERAL_DEBUG_LAST_BYTE_HAD_NACK)
// result = APP_I2C_RESULT_NOACK;
// else
// result = APP_I2C_RESULT_ACK;
//
// return result;
// }

/***************************************************************************//**
 *
 *  EtaCspI2cTransferInt - Start the I2C using Interrupt
 *
 *  @param pui8TxData pointer to TX Data Array
 *  @param ui16TxLen TX Data Length
 *  @param pui8RxData pointer to RX Data Array
 *  @param ui16RxLen RX Data Length
 *  @param ui16RestartAfterByte Indicates when restart will be asserted (set to
 *                              0 if no restart needed).
 *
 ******************************************************************************/

tEtaStatus
EtaCspI2cTransferInt(uint8_t *pui8TxData, uint16_t ui16TxLen,
                     uint8_t *pui8RxData, uint16_t ui16RxLen,
                     uint16_t ui16RestartAfterByte)
{
    uint32_t ui32CurrByteCount;
    tEtaStatus localI2cStatus;

    // Check that the command is acceptable
    // These can be removed if onerous
    if(ui16TxLen == 0)
    {
        // users should send at least device id.
        return(eEtaI2cInvalidConfig);
    }

    if(ui16TxLen < ui16RestartAfterByte)
    {
        return(eEtaI2cInvalidConfig);
    }

// cannot hit this with 16 bit integers
// if (ui16TxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_I2C_DETAILED_DEBUG
// #endif
// return eEtaI2cInvalidConfig;
// }
//
// if (ui16RxLen > (64*1024)-1)
// {
// #ifdef ETA_CSP_I2C_DETAILED_DEBUG
// #endif
// return eEtaI2cInvalidConfig;
// }
// Check that the command is acceptable

    // Make sure previous command is done (optional)
    if(localI2cStatus = EtaCspI2cDonePoll())
    {
        return(localI2cStatus);
    }

    // Start command (start could be done later, but it will wait for first byte
    // sent to it).
    REG_I2C2_STATUS = BM_I2C2_STATUS_START_BUSY;
    ui32CurrByteCount = 0;

    if(ui16RxLen != 0)
    {
        REG_I2C2_DUMMY_BYTES = ui16RxLen - 1;
    }

    // Setup global array
    gsI2cCmd.pui8TxData = pui8TxData;
    gsI2cCmd.pui8RxData = pui8RxData;

    if(ui16RxLen == 0)
    {
        gsI2cCmd.bRxLenIs0 = 1; // Corner case of ui16RxLen=0
                                           // requires 1 extra transmit
        gsI2cCmd.bRxLenIs1 = 0;
        gsI2cCmd.bRxLenIsGt1 = 0;
        gsI2cCmd.ui16TxLen = ui16TxLen + 1;
    }
    else if(ui16RxLen == 1)
    {
        gsI2cCmd.bRxLenIs0 = 0;
        gsI2cCmd.bRxLenIs1 = 1; // Corner case of ui16RxLen=1
                                           // requires 2 extra transmit
        gsI2cCmd.bRxLenIsGt1 = 0;
        gsI2cCmd.ui16TxLen = ui16TxLen + 2;
    }
    else
    {
        gsI2cCmd.bRxLenIs0 = 0;
        gsI2cCmd.bRxLenIs1 = 0;
        gsI2cCmd.bRxLenIsGt1 = 1; // Common case of ui16RxLen>1
                                             // requires 4 extra transmit
        gsI2cCmd.ui16TxLen = ui16TxLen + 4;
    }

    gsI2cCmd.ui8TxIntSize = 8;
    gsI2cCmd.ui8RxIntSize = 8;
    gsI2cCmd.ui16RxLen = ui16RxLen;
    gsI2cCmd.bCmdActive = 1;
    gsI2cCmd.ui32CurrByteCount = 0;
    gsI2cCmd.ui16RestartAfterByte = ui16RestartAfterByte;
    gsI2cCmd.i2cStatus = eEtaSuccess;

    REG_I2C2_CONFIG = (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_RX_FIFO_INT_LEVEL) |
                      (gsI2cCmd.ui8TxIntSize <<
                       BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);


    REG_I2C2_INT_EN = BM_I2C2_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN |
                      BM_I2C2_INT_EN_UNACK_EN;
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntTxData - Process I2C Interrupt for TX Space Available
 *
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cProcessIntTxData(void)
{
    uint8_t space_in_fifo;

    if((gsI2cCmd.bCmdActive == 0) ||
       (gsI2cCmd.ui16TxLen == 0))
    {
        return(eEtaI2cCmdSequenceErr);
    }

    if(ETA_CSP_I2C_EN_GREEDY_INT) // This allows for burst writes, which is more
                                  // efficient, but not requires an extra read.
    {
        space_in_fifo = 16 -
                        (REG_I2C2_TX_FIFO_DEBUG &
                         BM_I2C2_TX_FIFO_DEBUG_ENTRIES);
    }
    else
    {
        space_in_fifo = gsI2cCmd.ui8TxIntSize; // Do work that
                                                          // interrupt said we
                                                          // have space for
    }


    do
    {
        // Deal with restart
        // First we check if we have a restart, then we check if it is ours. If
        // not, continue with transactions.
        if(gsI2cCmd.ui16RestartAfterByte != 0)
        {
            if(gsI2cCmd.ui32CurrByteCount ==
               gsI2cCmd.ui16RestartAfterByte)
            {
                gsI2cCmd.ui16RestartAfterByte = 0; // This should
                                                              // "disable"
                                                              // restart since 0
                                                              // is illegal for
                                                              // restart
                                                              // position
                REG_I2C2_CMD_FIFO_DATA = I2C_CMD_RESTART;


                break; // Exit loop if we entered here.
            }
        }

        // This is the more common case, so we'll check this first and only go
        // to the exceptions if this common case is not true.

        // These are various exceptions
        gsI2cCmd.ui32CurrByteCount =
            gsI2cCmd.ui32CurrByteCount + 1;

        if(gsI2cCmd.ui16TxLen > 4) // Note, this is the same as the
                                              // "else" clause at the bottom
                                              // below.
        {

            REG_I2C2_TXRX_1BYTE_FIFO_DATA = *gsI2cCmd.pui8TxData++;
            gsI2cCmd.ui16TxLen--;
            if(gsI2cCmd.ui16TxLen == 0) // Just sent last byte.
            {
                REG_I2C2_INT_EN = BM_I2C2_INT_EN_RX_HIT_LEVEL_EN |
                                  BM_I2C2_INT_EN_UNACK_EN;
                REG_I2C2_CONFIG =
                    (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_RX_FIFO_INT_LEVEL) |
                    (gsI2cCmd.ui8RxIntSize <<
                        BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);

            }
        }

        // Case where RX Length == 0
        else if((gsI2cCmd.bRxLenIs0 == 1) &&
                (gsI2cCmd.ui16TxLen == 1))
        {
            REG_I2C2_CMD_FIFO_DATA = I2C_CMD_END_WRITE;
            REG_I2C2_INT_EN = BM_I2C2_INT_EN_CMD_DONE | BM_I2C2_INT_EN_UNACK_EN;
            gsI2cCmd.ui16TxLen--;
            REG_I2C2_CONFIG =
                (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_RX_FIFO_INT_LEVEL) |
                (gsI2cCmd.ui8RxIntSize <<
                    BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);

            break; // Exit loop, we're done.
        }

        // Cases where RX Length == 1
        else if((gsI2cCmd.bRxLenIs1 == 1) &&
                (gsI2cCmd.ui16TxLen == 2))
        {
            REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
            gsI2cCmd.ui16TxLen--;

        }
        else if((gsI2cCmd.bRxLenIs1 == 1) &&
                (gsI2cCmd.ui16TxLen == 1))
        {
            REG_I2C2_CMD_FIFO_DATA = I2C_CMD_BEGIN_READ | I2C_CMD_END_READ;
            gsI2cCmd.ui16TxLen--;
            REG_I2C2_INT_EN = BM_I2C2_INT_EN_RX_HIT_LEVEL_EN |
                              BM_I2C2_INT_EN_UNACK_EN; //
                                                       // Go
                                                       // to
                                                       // RX
            REG_I2C2_CONFIG =
                (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_RX_FIFO_INT_LEVEL) |
                (gsI2cCmd.ui8RxIntSize <<
                    BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);

            break; // Exit loop, we're done.
        }

        // Cases where RX Length >1
        else if((gsI2cCmd.bRxLenIsGt1 == 1) &&
                (gsI2cCmd.ui16TxLen == 4))
        {
            REG_I2C2_TXRX_1BYTE_FIFO_DATA = 0xff;
            gsI2cCmd.ui16TxLen--;

        }
        else if((gsI2cCmd.bRxLenIsGt1 == 1) &&
                (gsI2cCmd.ui16TxLen == 3))
        {
            REG_I2C2_CMD_FIFO_DATA = I2C_CMD_BEGIN_READ;
            gsI2cCmd.ui16TxLen--;

        }
        else if((gsI2cCmd.bRxLenIsGt1 == 1) &&
                (gsI2cCmd.ui16TxLen == 2))
        {
            REG_I2C2_CMD_FIFO_DATA = I2C_CMD_DUMMYBYTE;
            gsI2cCmd.ui16TxLen--;

        }
        else if((gsI2cCmd.bRxLenIsGt1 == 1) &&
                (gsI2cCmd.ui16TxLen == 1))
        {
            REG_I2C2_CMD_FIFO_DATA = I2C_CMD_END_READ;
            gsI2cCmd.ui16TxLen--;
            REG_I2C2_INT_EN = BM_I2C2_INT_EN_RX_HIT_LEVEL_EN |
                              BM_I2C2_INT_EN_UNACK_EN; //
                                                       // Go
                                                       // to
                                                       // RX
            REG_I2C2_CONFIG =
                (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_RX_FIFO_INT_LEVEL) |
                (gsI2cCmd.ui8RxIntSize <<
                    BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);

            break; // Exit loop, we're done.
        }
        else
        {

            REG_I2C2_TXRX_1BYTE_FIFO_DATA = *gsI2cCmd.pui8TxData++;
            gsI2cCmd.ui16TxLen--;
            if(gsI2cCmd.ui16TxLen == 0) // Just sent last byte.
            {
                REG_I2C2_INT_EN = BM_I2C2_INT_EN_RX_HIT_LEVEL_EN |
                                  BM_I2C2_INT_EN_UNACK_EN;
                REG_I2C2_CONFIG =
                    (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_RX_FIFO_INT_LEVEL) |
                    (gsI2cCmd.ui8RxIntSize <<
                        BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);

                break; // Exit loop, we're done.
            }
        }
    }
    while(--space_in_fifo);

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntRxData - Process I2C Interrupt for RX Data Available
 *
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cProcessIntRxData(void)
{
    uint8_t space_in_fifo;
    uint16_t rx_word;


    if((gsI2cCmd.bCmdActive == 0) ||
       (gsI2cCmd.ui16RxLen == 0))
    {
        return(eEtaI2cCmdSequenceErr);
    }

    if(ETA_CSP_I2C_EN_GREEDY_INT) // This allows for burst writes, which is more
                                  // efficient, but not requires an extra read.
    {
        space_in_fifo =
            (REG_I2C2_RX_FIFO_DEBUG & BM_I2C2_RX_FIFO_DEBUG_ENTRIES);
    }
    else
    {
        space_in_fifo = gsI2cCmd.ui8RxIntSize; // Do work that
                                                          // interrupt said we
                                                          // have space for
        if(space_in_fifo > gsI2cCmd.ui16RxLen)
        {
            space_in_fifo = gsI2cCmd.ui16RxLen;
        }
    }


    do
    {
        if(space_in_fifo >= 2)
        {
            rx_word = REG_I2C2_RX_2BYTE_FIFO_DATA;


            *gsI2cCmd.pui8RxData++ = ((rx_word >> 0) & 0xff);
            *gsI2cCmd.pui8RxData++ = ((rx_word >> 8) & 0xff);
            gsI2cCmd.ui16RxLen -= 2;
            space_in_fifo -= 2;
        }
        else
        {
            rx_word = REG_I2C2_TXRX_1BYTE_FIFO_DATA;


            *gsI2cCmd.pui8RxData++ = ((rx_word >> 0) & 0xff);

            // If we have data to send, ignore receive data.
            gsI2cCmd.ui16RxLen--;
            space_in_fifo--;
        }

        if(gsI2cCmd.ui16RxLen == 0) // Just received last byte.
        {
            REG_I2C2_INT_EN = BM_I2C2_INT_EN_CMD_DONE | BM_I2C2_INT_EN_UNACK_EN;
        }

        if((space_in_fifo < 2) & (gsI2cCmd.ui16RxLen >= 2))
        {
            break; // If we drop low enough that we cannot bulk transfer, exit
                   // loop, wait for next int.
        }

        if(gsI2cCmd.ui16RxLen == 0)
        {
            break; // Exit loop, we're done.
        }
    }
    while(space_in_fifo);

    if(gsI2cCmd.ui8RxIntSize > gsI2cCmd.ui16RxLen)
    {
        REG_I2C2_CONFIG =
            (REG_I2C2_CONFIG & ~BM_I2C2_CONFIG_RX_FIFO_INT_LEVEL) |
            (gsI2cCmd.ui16RxLen << BP_I2C2_CONFIG_RX_FIFO_INT_LEVEL);
    }

    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntCmdDone - Process I2C Interrupt for Command Done
 *
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cProcessIntCmdDone(void)
{

    if((gsI2cCmd.bCmdActive == 0) ||
       (gsI2cCmd.ui16TxLen != 0) ||
       (gsI2cCmd.ui16RxLen != 0))
    {
        return(eEtaI2cCmdSequenceErr);
    }


    gsI2cCmd.i2cStatus = eEtaSuccess;
    gsI2cCmd.bCmdActive = 0;
    REG_I2C2_INT_EN = 0;
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntNack - Process I2C Interrupt for Unexpected Negative
 *                            Acknowledgement
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cProcessIntNack(void)
{

    // Disable Interrupts
    REG_I2C2_INT_EN = 0;

    // Clear State machine / cancel command
    REG_I2C2_CONFIG2 |=  BM_I2C2_CONFIG2_SOFT_RST;
    REG_I2C2_CONFIG2 &= ~BM_I2C2_CONFIG2_SOFT_RST;

    gsI2cCmd.i2cStatus = eEtaI2cUnexpectedNack;
    gsI2cCmd.bCmdActive = 0;
    REG_I2C2_INT_EN = 0;
    return(eEtaSuccess);
}

/***************************************************************************//**
 *
 *  EtaCspI2cProcessIntEntry - Entry for Interrupts for I2C
 *
 *
 ******************************************************************************/
tEtaStatus
EtaCspI2cProcessIntEntry(void)
{
    uint16_t bCmdActive_interrupt;
    tEtaStatus i2cStatus = eEtaSuccess;

    // Do one read per interrupt. The INT_ACT field may change during the
    // interrupt
    bCmdActive_interrupt = REG_I2C2_INT_ACT;


    if(bCmdActive_interrupt & BM_I2C2_INT_EN_UNACK_EN)
    {
        i2cStatus |= EtaCspI2cProcessIntNack();
    }
    if(bCmdActive_interrupt & BM_I2C2_INT_EN_TXFIFO_HAS_LEVEL_SPACE_EN)
    {
        i2cStatus |= EtaCspI2cProcessIntTxData();
    }
    if(bCmdActive_interrupt & BM_I2C2_INT_EN_RX_HIT_LEVEL_EN)
    {
        i2cStatus |= EtaCspI2cProcessIntRxData();
    }
    if(bCmdActive_interrupt & BM_I2C2_INT_EN_CMD_DONE)
    {
        i2cStatus |= EtaCspI2cProcessIntCmdDone();
    }

    return(i2cStatus);
}

/***************************************************************************//**
 *
 *  EtaCspI2cIsr- ISR handler for I2C interrupts.
 *
 *  @param iIrqNum is the interrupt number.
 *  @param int_line is the DSP interrupt line number.
 *
 ******************************************************************************/
void
EtaCspI2cIsr(tIrqNum iIrqNum, intline_T int_line)
{
    tEtaStatus i2cStatus = eEtaSuccess;
    i2cStatus = EtaCspI2cProcessIntEntry();
    if (i2cStatus != eEtaSuccess)
    {
        etaPrintf ("ERROR: Something went wrong while handling I2C interrupt.\r\n");
    }
    EtaCspIsrIntClear(iIrqNum, int_line);
}

