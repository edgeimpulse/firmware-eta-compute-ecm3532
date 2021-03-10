#ifndef ETA_CSP_COMMON_PDM_H
#define ETA_CSP_COMMON_PDM_H

//
//
//! PDM Sample Frequency
//
typedef enum
{
    //
    //! 8K Samples / Sec
    //
    ePdmSampleFreq8Ksps = 0,

    //
    //! 16K Samples / Sec
    //
    ePdmSampleFreq16Ksps = 1,
}
tPdmSampleFreq;

//
//
//! PDM Channel Select
//
typedef enum
{
    //
    //! Both Channels
    //
    ePdmChanConfigBoth = 0,

    //
    //! Reserved
    //
    ePdmChanConfigRsvd = 1,

    //
    //! Left Channel Only
    //
    ePdmChanConfigLeftOnly = 2,

    //
    //! Right Channel Only
    //
    ePdmChanConfigRightOnly = 3,
}
tPdmChanConfig;

//
//
//! PDM Port / Instance Select
//
typedef enum
{
    //
    //! PDM0 (GPIO 18,17)
    //
    ePdmPort0 = 0,

    //
    //! PDM0 (GPIO 9,8)
    //
    ePdmPort0Alt = 1,

    //
    //! PDM1 (GPIO 15,14)
    //
    ePdmPort1 = 2,
}
tPdmPortSelect;


//
//! PDM Clock Source
//
typedef enum
{
    //
    //! Clock Source is 2MHz derived from attached 16MHz oscillator.
    //
    ePdmClkSrc2MhzXtal = 0,

    //
    //! Clock Source is 2MHz from onboard HFO.
    //
    ePdmClkSrc2MhzHfo = 1,

    //
    //! Clock Source is 1MHz from onboard HFO.
    //
    ePdmClkSrc1MhzHfo = 2,

    //
    //! Clock is sourced from GPIO12.
    //
    ePdmClkSrcFromPad = 3,
}
tPdmClkSrc;

//
//
//! PDM Config Struct
//
typedef union {
  struct
  {
      uint32_t       enable:1           ; // Set to 1 to enable the interface
      uint32_t       swap_chan:1        ; // Set to 1 to swap channel
      uint32_t       edge_trig:1        ; // If 0, level interrupts. If 1 triggers gratuitous edge on every interrupt / source ready
      tPdmClkSrc     clock_src:2        ;
      tPdmSampleFreq sample_freq:1      ;
      tPdmChanConfig chan_config:2      ;
      tPdmPortSelect port_select:2      ;
      uint32_t       pga_left:5         ;
      uint32_t       compiler_bug:1     ; // bug in chess compiler crossing over
                                          // 16-bit boundary
      uint32_t       pga_right:5        ;
//      uint16_t       rsvd0:4            ;
//      uint16_t       rsvd1:16            ;
  } fields;
  uint32_t packed_config;
}
tPdmConfig;


//
//! PDM Config Return Value
//
typedef enum
{
    //
    //! Configuration is Good
    //
    ePdmConfigGood = 0,

    //
    //! Configuration is invalid
    //
    ePdmConfigInvalidConfig = 1,
}
tPdmConfigRetVal;



#endif // ETA_CSP_COMMON_PDM_H

