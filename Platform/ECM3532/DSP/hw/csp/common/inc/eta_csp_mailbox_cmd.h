#ifndef ETA_CSP_MAILBOX_CMD_H
#define ETA_CSP_MAILBOX_CMD_H

//////////////////////////////////////////////////////////////
// High Level Mailbox Commands
// Note, these commands have similar structure in 32 bit M3 and 16 bit DSP.
// First 16 bit mailbox word is 0x??CC where CC is the command and the extended 32
// bit field is the data, but the user can parse as they see fit.

// DSP To M3 Mailbox Commands
#define MAILBOX_DSP2M3CMD_PRINTVIAM3  0x01 // Print Via M3 Mailbox
#define MAILBOX_DSP2M3CMD_CODEWORD    0x02 // Send application specific mailbox.
#define MAILBOX_DSP2M3CMD_I2S_CFG     0x03 // Send I2S Configuration Request
#define MAILBOX_DSP2M3CMD_PDM_CFG     0x04 // Send PDM Configuration Request
#define MAILBOX_DSP2M3CMD_RSVD        0xFF // rsvd

// M3 To DSP Mailbox Commands
#define MAILBOX_M32DSPCMD_RETURNCRED  MAILBOX_DSP2M3CMD_PRINTVIAM3
#define MAILBOX_M32DSPCMD_CODEWORD    MAILBOX_DSP2M3CMD_CODEWORD
#define MAILBOX_M32DSPCMD_ADC_CFG     0x05 // Send ADC Enable Request
#define MAILBOX_M32DSPCMD_RSVD        MAILBOX_DSP2M3CMD_RSVD


// This is not really a command, but somethign burried in the mailbox stream
#define MAILBOX_DSP2M3CMD_PWRAP   0x11

// High Level Mailbox Commands
//////////////////////////////////////////////////////////////

#endif // ETA_CSP_MAILBOX_CMD_H

