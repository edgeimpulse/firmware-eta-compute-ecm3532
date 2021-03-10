#ifndef ETA_CSP_COMMON_ADC_H
#define ETA_CSP_COMMON_ADC_H


//
//
//! ADC Config Struct
//
typedef union {
  struct
  {
      uint32_t       enable:1           ; // Set to 1 to enable the interface
      uint32_t       edge_trig:1        ; // If 0, level interrupts. If 1 triggers gratuitous edge on every interrupt / source ready
  } fields;
  uint32_t packed_config;
}
tAdcConfig;


//
//
//! ADC Config Return Value
//
typedef enum
{
    //
    //! Configuration is Good
    //
    eAdcConfigGood = 0,

    //
    //! Configuration is invalid
    //
    eAdcConfigInvalidConfig = 1,
}
tAdcConfigRetVal;



#endif // ETA_CSP_COMMON_ADC_H

