#ifndef _CHECK_DS18B20_H_
#define _CHECK_DS18B20_H_

#include <OneWire.h>

namespace CheckDS18B20 {
  typedef enum {FAMILY_A1, FAMILY_A2, FAMILY_A_UNKNOWN, FAMILY_B1, FAMILY_B2, FAMILY_B_UNKNOWN, FAMILY_C, FAMILY_D1_NO_PARASITIC, FAMILY_D1, FAMILY_D2, FAMILY_D_UNKNOWN, FAMILY_E, FAMILY_UNKNOWN, NO_SENSOR} DS18B20_family_enum;

  /* the first sensor has sensor_number 0 */
  DS18B20_family_enum ds18b20_family(OneWire* _OneWire, int sensor_number);
#ifdef __AVR__
  const __FlashStringHelper* ds18b20_name(OneWire* _OneWire, int sensor_number);
#else
  const char* ds18b20_name(OneWire* _OneWire, int sensor_number);
#endif
};
#endif
