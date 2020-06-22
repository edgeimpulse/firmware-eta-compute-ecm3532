#ifndef ETA_CSP_COMMON_I2S_H
#define ETA_CSP_COMMON_I2S_H

//
//
//! I2S Sample Frequency
//
typedef enum
{
    //
    //! 8K Samples / Sec
    //
    eI2sSampleFreq8Ksps = 0,

    //
    //! 16K Samples / Sec
    //
    eI2sSampleFreq16Ksps = 1,

    //
    //! 32K Samples / Sec
    //
    eI2sSampleFreq32Ksps = 2,
}
tI2sSampleFreq;

//
//
//! I2S Upscale Value
//
typedef enum
{
    //
    //! Shift by 0
    //
    eI2sUpscaleShiftBy0 = 0,

    //
    //! Shift by 2
    //
    eI2sUpscaleShiftBy2 = 1,

    //
    //! Shift by 4
    //
    eI2sUpscaleShiftBy4 = 2,

    //
    //! Shift by 6
    //
    eI2sUpscaleShiftBy6 = 3,
}
tI2sUpscale;

//
//
//! I2S Channel Select
//
typedef enum
{
    //
    //! Both Channels
    //
    eI2sChanConfigBoth = 0,

    //
    //! Reserved
    //
    eI2sChanConfigRsvd = 1,

    //
    //! Left Channel Only
    //
    eI2sChanConfigLeftOnly = 2,

    //
    //! Right Channel Only
    //
    eI2sChanConfigRightOnly = 3,
}
tI2sChanConfig;

//
//
//! I2S Interrupt Mode
//
typedef enum
{
    //
    //! No Processor Interrupt
    //
    eI2sIntModeNone = 0,

    //
    //! Interrupt when RX FIFO has data
    //
    eI2sIntModeRX = 1,

    //
    //! Interrupt when TX FIFO has space or RX FIFO has data
    //
    eI2sIntModeBoth = 2,

    //
    //! Interrupt when TX FIFO has space
    //
    eI2sIntModeTX = 3,
}
tI2sIntMode;

//
//
//! I2S Clock Source
//
typedef enum
{
    //
    //! Clock Source is 2MHz derived from attached 16MHz oscillator
    //
    eI2sClkSrc2MhzXtal = 0,

    //
    //! Clock Source is 2MHz from onboard HFO
    //
    eI2sClkSrc2MhzHfo = 1,

    //
    //! Clock Source is 1MHz from onboard HFO
    //
    eI2sClkSrc1MhzHfo = 2,

    //
    //! Clock is sourced from GPIO12
    //
    eI2sClkSrcFromPad = 3,
}
tI2sClkSrc;


//
//! I2S Config Struct
//
typedef union {
  struct
  {
      uint32_t       enable_rx:1        ; // Set to 1 to enable the interface
      uint32_t       enable_tx:1        ; // Set to 1 to enable the interface
      uint32_t       swap_chan_tx:1     ; // Set to 1 to swap channel on TX
      uint32_t       swap_chan_rx:1     ; // Set to 1 to swap channel on RX
      uint32_t       mode_64bit:1       ; // Set to 0 to enabled 16 bit samples per channel. Set to 1 to enable 32 bit samples per channel.
      uint32_t       edge_tx_trig:1     ; // If 0, level interrupts. If 1 triggers gratuitous edge on every interrupt / source ready / dest ready
      uint32_t       edge_rx_trig:1     ; // If 0, level interrupts. If 1 triggers gratuitous edge on every interrupt / source ready / dest ready
      tI2sClkSrc     clock_src:2        ;
      tI2sSampleFreq sample_freq:2      ;
      tI2sChanConfig chan_tx_config:2   ;
      tI2sChanConfig chan_rx_config:2   ;
      tI2sUpscale    chan_tx_upscale:2  ;
      tI2sUpscale    chan_rx_upscale:2  ;
      tI2sIntMode    int_mode:2         ;
      //uint16_t       rsvd0:11         ;
  } fields;
  uint32_t packed_config;
}
tI2sConfig;


//
//
//! I2S Config Return Value
//
typedef enum
{
    //
    //! Configuration is Good
    //
    eI2sConfigGood = 0,

    //FIXME, not sure this is valid anymore, but only a "good" return seems odd too
    //
    //! Invalid or Unsupported Clock Source
    //
    eI2sConfigInvalidClkSource = 1,
}
tI2sConfigRetVal;



#endif // ETA_CSP_COMMON_I2S_H

