#include "cf6_chess.h"

#include "eta_csp_status.h"
#include "eta_csp_isr.h"
#include "eta_csp_i2c.h"
#include "eta_csp_mailbox.h"
#include "dspI2c.h"
#include "dsp_config.h"


#define IFDBG  if (1)

// I2C setup
void i2c_setup(void)
{
    IFDBG etaPrintf("Configuring I2C2 at %d KHz.\r\n", CONFIG_I2C_2_FREQ);
    //EtaCspI2cInit(CONFIG_I2C_2_FREQ);
    //TODO: Read src clock from perpharal register.
    EtaCspI2cInit(CONFIG_I2C_2_FREQ, eI2cSrcClk8MHz);
  
///////////////////////////////////////
// Assume we are in bad state, clear line to make sure
// This can happen randomly, during initial power up or due to unfortunate
// timing on padmux
    etaPrintf("Issueing start/stop to kill any command in progress.\r\n");
    EtaCspI2cLineClear();
// Assume we are in bad state, clear line to make sure
///////////////////////////////////////
}

int i2c_write_reg(char slaveAddr, char regAddr, char data) {

    char tx_data[4];
    char rx_data[4];

    int result;

    tx_data[0] = (slaveAddr << 1) | 0; // Device ID and Write
    tx_data[1] = regAddr;
    tx_data[2] = data;
#if 0
    result = eta_csp_i2c_transfer_poll(&tx_data[0], 1 + 2, &rx_data[0], 0, 0);
#else
    result = EtaCspI2cTransferPoll(&tx_data[0], 1 + 2, &rx_data[0], 0, 0);
#endif
    if (result)
    {
        etaPrintf("  Write returned with error. result=%d\r\n", result);
    }
#if 0
    result = eta_csp_i2c_poll_until_done();
#else
    result = EtaCspI2cDonePoll();
#endif
    if (result)
    {
        etaPrintf("  Write returned with error. result=%d\r\n", result);
    }

    return result;
}

int i2c_read_reg(char slaveAddr, char regAddr, char* rx_data, unsigned short int length) 
{
    char tx_data[4];
    int result;

    tx_data[0] = (slaveAddr << 1) | 0; // Device ID and Write
    tx_data[1] = regAddr;
    // restart here, so after 2nd byte
    tx_data[2] = (slaveAddr << 1) | 1; // Device ID and Read

    // VW: I assume that these args are             no. of bytes sent  place holder for receiving data  no. of bytes returned  restart point
#if 0
    result = eta_csp_i2c_transfer_poll(&tx_data[0], 3, rx_data, length, 2);
#else
    result = EtaCspI2cTransferPoll(&tx_data[0], 3, rx_data, length, 2);
#endif

    if (result)
    {
        etaPrintf("  read returned with error. result=%d\r\n", result);
    }
    return result;
}

int i2c_update_reg(char slaveAddr, char regAddr, char mask, char val)
{
    int result;
    char regVal;
    result = i2c_read_reg(slaveAddr, regAddr, &regVal, 1);
    if (result)
    {
        etaPrintf("i2c read returned with error. result=%d\r\n", result);
        return -1;
    }
    regVal = (regVal) & (~mask);
    regVal = regVal | val;
    result = i2c_write_reg(slaveAddr, regAddr, regVal);
    if (result)
    {
        etaPrintf("i2c write returned with error. result=%d\r\n", result);
        return -1;
    }
    return 0;
}


#if 0
/////////////////////////////////////////////////////////////
int write_reg(char address, char data) {

    char tx_data[4];
    char rx_data[4];

    int result;

    tx_data[0] = CODEC_I2C_WR_ADDR; // Device ID and Write
    tx_data[1] = address;
    tx_data[2] = data;

#if 0
    result = eta_csp_i2c_transfer_poll(&tx_data[0], 1 + 2, &rx_data[0], 0, 0);
#else
    result = EtaCspI2cTransferPoll(&tx_data[0], 1 + 2, &rx_data[0], 0, 0);
#endif

    if (result)
    {
        etaPrintf("  Write returned with error. result=%d\r\n", result);
    }

#if 0
    result = eta_csp_i2c_poll_until_done();
#else
    result = EtaCspI2cDonePoll();
#endif

    if (result)
    {
        etaPrintf("  Write returned with error. result=%d\r\n", result);
    }

    return result;
}

/////////////////////////////////////////////////////////////
int read_reg(char address, char* rx_data) {

    char tx_data[4];

    int result;

    tx_data[0] = CODEC_I2C_WR_ADDR; // Device ID and Write
    tx_data[1] = address;
    // restart here, so after 2nd byte
    tx_data[2] = CODEC_I2C_RD_ADDR; // Device ID and Read


    // VW: I assume that these args are             no. of bytes sent  place holder for receiving data  no. of bytes returned  restart point
#if 0
    result = eta_csp_i2c_transfer_poll(&tx_data[0], 3, rx_data, 1, 2);
#else
    result = EtaCspI2cTransferPoll(&tx_data[0], 3, rx_data, 1, 2);
#endif
    if (result)
    {
        etaPrintf("  Read returned with error. result=%d\r\n", result);
    }

    return result;
}

#endif
