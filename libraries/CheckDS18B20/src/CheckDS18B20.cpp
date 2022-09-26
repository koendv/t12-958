/*
 * Copyright 2020 Chris Petrich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 *   
 *   File:    classify_fake_DS18B20.ino
 *   Author:  Chris Petrich
 *   Version: 25 May 2020
 *   
 *   Source:  https://github.com/cpetrich/counterfeit_DS18B20/
 *   Documentation:  https://github.com/cpetrich/counterfeit_DS18B20/
 *
 * Arduino library:
 *
 *   Author:  Koen De Vleeschauwer
 *   Source:  https://github.com/koendv/CheckDS18B20
 *   
 * This demonstration script attempts to classify DS18B20 clones 
 * assuming the chip belongs to one of the Families detailed on
 * https://github.com/cpetrich/counterfeit_DS18B20/
 * MAX31820 sensors cannot be distinguished from DS18B20 in software.
 * 
 * This script evaluates the response to UNDOCUMENTED FUNCTION CODES.
 * If this script is used on DS18B20 clones that do not belong to one
 * of the Families listed on
 * https://github.com/cpetrich/counterfeit_DS18B20/
 * then write operations could be triggered inadvertently that may
 * mess up calibration constants and render the part useless -- or
 * produce permanent damage in other ways.
 * 
 * Disclaimer: This code sends undocumented function codes to 1-wire
 * chips and may therefore permanently damage the part attached.
 * USE AT YOUR OWN RISK AND FOR YOUR OWN SAKE DO NOT USE THIS SCRIPT 
 * ON CHIPS INTENDED FOR DEPLOYMENT OR SALE!
 * 
 * 
 * This script is written for Arduino. Wiring:
 * 
 * CPU Vcc or GND  -------------- DS18B20 Vcc
 * 
 * CPU Vcc         -------\
 *                        |
 *                       [R]  <- choose resistor appropriate for supply voltage and current that the microcontroller is able to sink.
 *                        |
 * CPU pin_onewire -------o------ DS18B20 data
 * 
 * CPU GND         -------------- DS18B20 GND
 * 
 * 
 * Note: Always attach the power pin of the DS18B20 to either GND or Vcc unless it is a DS18B20-PAR.
 * 
 */

// output via serial, or no output

#if 0
#define PRINT(...) Serial.print(__VA_ARGS__)
#define PRINTLN(...) Serial.println(__VA_ARGS__)
#endif

#ifndef PRINT
#define PRINT(...)
#define PRINTLN(...)
#endif


// Tested with OneWire Version 2.3
// https://github.com/PaulStoffregen/OneWire
#include "OneWire.h"

#include "CheckDS18B20.h"

namespace CheckDS18B20 {

static const char descr_a1[] PROGMEM              = "A1 DS18B20";
static const char descr_a2[] PROGMEM              = "A2";
static const char descr_a_unknown[] PROGMEM       = "A";
static const char descr_b1[] PROGMEM              = "B1 GX18B20";
static const char descr_b2[] PROGMEM              = "B2 QT18B20";
static const char descr_b_unknown[] PROGMEM       = "B";
static const char descr_c[] PROGMEM               = "C CT1820B";
static const char descr_d1_no_parasitic[] PROGMEM = "D1 (no par pwr)";
static const char descr_d1[] PROGMEM              = "D1";
static const char descr_d2[] PROGMEM              = "D2 SE18B20";
static const char descr_d_unknown[] PROGMEM       = "D";
static const char descr_e[] PROGMEM               = "E NS18B20";
static const char descr_unknown[] PROGMEM         = "?";
static const char descr_no_sensor[] PROGMEM       = "No sensor";

static const char *const sensor_descr[] PROGMEM = {
  descr_a1, descr_a2, descr_a_unknown, descr_b1, descr_b2, descr_b_unknown, descr_c,
  descr_d1_no_parasitic, descr_d1, descr_d2, descr_d_unknown, descr_e, descr_unknown,
  descr_no_sensor
};

OneWire *ds = NULL;

static void print_hex(uint8_t value) {
  if (value < 16) PRINT('0');
  PRINT(value, HEX);
}

static void print_array(uint8_t *data, int n, char sep = ',') {  
  int idx;
  for (idx=0; idx<n; idx++) {
    print_hex(data[idx]);
    if (idx != n-1)
      PRINT(sep);
  }
}

static bool read_scratchpad(uint8_t *addr, uint8_t *buff9) {
  ds->reset();
  ds->select(addr);
  ds->write(0xBE); // read scratchpad
  int idx;
  for (idx=0; idx<9; idx++)
    buff9[idx] = ds->read();
  return 0 == OneWire::crc8(buff9, 9);
}

static int lengthFF(uint8_t *buff, int len) {
  // RIGHT-STRIPPING all 0xff, what is the byte length of the buffer? (NB: may be 0)
  while (len>0) {
    if (buff[len-1] != 0xff) break;
    len--;
  }
  return len;
}

static void send_reset(uint8_t *addr) {
  ds->reset();
  ds->select(addr);
  ds->write(0x64);
  delay(10);
}

static uint8_t one_byte_return(uint8_t *addr, uint8_t cmd) {
  ds->reset();
  ds->select(addr);
  ds->write(cmd);
  return ds->read();
}

static uint8_t bit_invert(uint8_t a) {
  uint8_t b = 0;
  int i;
  for (i=0; i<8; i++) {
    b *= 2;
    b += (a & 0x01);
    a /= 2;
  }
  return b;
}

static void param2trim(uint16_t offset_param_11bit, uint8_t curve_param_5bit, uint8_t *param1, uint8_t *param2) {
  *param1 = bit_invert(offset_param_11bit & 0x0ff);
  *param2 = curve_param_5bit * 8 + offset_param_11bit / 256;  
}

static void trim2param(uint8_t param1, uint8_t param2, uint16_t *offset_param_11bit, uint8_t *curve_param_5bit) {
  *offset_param_11bit = bit_invert(param1) + ((uint16_t)(param2 & 0x07)) * 256;
  *curve_param_5bit = param2 / 8;
}

static void get_trim_A(uint8_t *addr, uint8_t *trim1, uint8_t *trim2) {
  ds->reset();
  if (addr) ds->select(addr);
  else ds->write(0xCC);
  ds->write(0x93);
  *trim1 = ds->read();
  ds->reset();
  if (addr) ds->select(addr);
  else ds->write(0xCC);
  ds->write(0x68);
  *trim2 = ds->read();  
}

static void set_trim_A(uint8_t *addr, uint8_t trim1, uint8_t trim2) {
  ds->reset();
  ds->select(addr);
  ds->write(0x95);
  ds->write(trim1);

  ds->reset();
  ds->select(addr);
  ds->write(0x63);
  ds->write(trim2);
  // don't store permanently, don't call reset  
}

static void get_trim_params_A(uint8_t *addr, uint16_t *offset_param_11bit, uint8_t *curve_param_5bit) {
  uint8_t trim1, trim2;
  get_trim_A(addr, &trim1, &trim2);
  trim2param(trim1, trim2, offset_param_11bit, curve_param_5bit);  
}

static void set_trim_params_A(uint8_t *addr, uint16_t offset_param_11bit, uint8_t curve_param_5bit) {
  uint8_t trim1, trim2;
  param2trim(offset_param_11bit, curve_param_5bit, &trim1, &trim2);
  set_trim_A(addr, trim1, trim2);  
}

static bool is_valid_A_scratchpad(uint8_t *buff) {
  if ((buff[4] != 0x7f) && (buff[4] != 0x5f) && (buff[4] != 0x3f) && (buff[4] != 0x1f)) return false;
  if ((buff[0] == 0x50) && (buff[1] == 0x05) && (buff[6] == 0x0C)) return true; // power-up
  if ((buff[0] == 0xff) && (buff[1] == 0x07) && (buff[6] == 0x0C)) return true; // unsuccessful conversion
  return buff[6] == (0x10 - (buff[0] & 0x0f));
}

static bool is_all_00(uint8_t *buff, int N) {
  int i;
  for (i=0; i<N; i++)
    if (buff[i] != 0x00) return false;
  return true;
}

static void trigger_convert(uint8_t *addr, uint8_t conf, uint16_t wait) {
  ds->reset();
  ds->select(addr);
  ds->write(0x4E);
  ds->write(0xaf);
  ds->write(0xfe);
  ds->write(conf);
    
  ds->reset();
  ds->select(addr);
  ds->write(0x44, 1); // start conversion and keep data line high in case we need parasitic power
  delay(wait);
}

static int curve_param_prop(uint8_t *addr) {
  // check if the curve parameter is unsigned (1) signed (2), temperature range inconclusive (-1), doesn't seem to be A (-2), CRC error (-3), or doesn't seem to do anything (0)
  uint8_t buff[9];
  uint16_t off = 0x3ff; // half way

  set_trim_params_A(addr, off, 0x0f);
  trigger_convert(addr, 0x7f, 800);
  if (!read_scratchpad(addr, buff)) read_scratchpad(addr, buff);
  if (0 != OneWire::crc8(buff,9)) return -3;
  if (!is_valid_A_scratchpad(buff)) return -2;
  int16_t r0f = buff[0] + 256*buff[1];
  {
    // check if we can set and read trim parameters
    uint16_t o;
    uint8_t c;
    get_trim_params_A(addr, &o, &c);
    if ((o != off) || (c != 0x0f)) return 0;
  }
  
  set_trim_params_A(addr, off, 0x00);
  trigger_convert(addr, 0x7f, 800);
  if (!read_scratchpad(addr, buff)) read_scratchpad(addr, buff);
  if (0 != OneWire::crc8(buff,9)) return -3;
  if (!is_valid_A_scratchpad(buff)) return -2;
  int16_t r00 = buff[0] + 256*buff[1];
  
  set_trim_params_A(addr, off, 0x1f);
  trigger_convert(addr, 0x7f, 800);
  if (!read_scratchpad(addr, buff)) read_scratchpad(addr, buff);
  if (0 != OneWire::crc8(buff,9)) return -3;
  if (!is_valid_A_scratchpad(buff)) return -2;
  int16_t r1f = buff[0] + 256*buff[1];
    
  set_trim_params_A(addr, off, 0x10);
  trigger_convert(addr, 0x7f, 800);
  if (!read_scratchpad(addr, buff)) read_scratchpad(addr, buff);
  if (0 != OneWire::crc8(buff,9)) return -3;
  if (!is_valid_A_scratchpad(buff)) return -2;
  int16_t r10 = buff[0] + 256*buff[1];
  
  int16_t mini = min(r00, min(r0f, min(r10, r1f)));  
  int16_t maxi = max(r00, max(r0f, max(r10, r1f)));
  bool is_signed = (r0f-r10 > r1f-r00);
  bool is_unsigned = (r0f-r10 < r1f-r00);

  if (is_signed && (maxi-mini > 20*16)) return 2; // A2
  if (is_unsigned && (maxi-mini > 1*16) && (maxi-mini < 6*16)) return 1; // A1

  return -1;
}  

DS18B20_family_enum ds18b20_family(OneWire* _OneWire, int sensor_number)
{
  DS18B20_family_enum sensor_family = NO_SENSOR;

  ds = _OneWire;
  if (ds == NULL) return NO_SENSOR;

  // ROM address of current sensor
  uint8_t addr[8];  

  int sensor_count = 0;
  ds->reset_search();
  while (ds->search(addr)) {
    sensor_count ++;

    if (sensor_count != sensor_number + 1) continue;

    // dump ROM    
    print_array(addr, 8, '-');    
    if (0 != OneWire::crc8(addr, 8)) {
      PRINT(F(" (CRC Error)"));      
    }
    
    PRINT(F(":"));
    
    int identified = 0;

    { // test for family A
      uint8_t r68 = one_byte_return(addr, 0x68);
      uint8_t r93 = one_byte_return(addr, 0x93);
      if (r68 != 0xff) {        
        int cpp = curve_param_prop(addr);
        if (cpp == 1) {
          PRINT(F(" Family A1 (Genuine Maxim).")); // unsigned and 3.8 oC range
          sensor_family = FAMILY_A1;
        }
        else if (cpp == 2) {
          PRINT(F(" Family A2 (Clone).")); // signed and 32 oC range
          sensor_family = FAMILY_A2;
        }
        else if (cpp == -3) PRINT(F(" (Error reading scratchpad register [A].)"));
        else if (cpp == -1) {
          PRINT(F(" Family A, unknown subtype (0x93="));
          print_hex(one_byte_return(addr, 0x93));
          PRINT(F(", 0x68="));
          print_hex(r68);
          PRINT(F(")."));
          sensor_family = FAMILY_A_UNKNOWN;
        }
        // cpp==0 or cpp==-2: these aren't Family A as we know them.
        if ((cpp==1) || (cpp==2) || (cpp==-1)) identified++;
      }
      send_reset(addr);
    }
    
    { // test for family B
      uint8_t r97 = one_byte_return(addr, 0x97);
      if (r97 == 0x22) {
        PRINT(F(" Family B1 (Clone)."));
        sensor_family = FAMILY_B1;
      }
      else if (r97 == 0x31) {
        PRINT(F(" Family B2 (Clone)."));
        sensor_family = FAMILY_B2;
      }
      else if (r97 != 0xFF) {
        PRINT(F(" Family B (Clone), unknown subtype (0x97="));
        print_hex(r97);
        PRINT(F(")."));
        sensor_family = FAMILY_B_UNKNOWN;
      }
      if (r97 != 0xff) identified++;
    }

    { // test for family D
      uint8_t r8B = one_byte_return(addr, 0x8B);
      // mention if parasitic power is known not to work.
      if (r8B == 0x06) {
        PRINT(F(" Family D1 (Clone w/o parasitic power)."));
        sensor_family = FAMILY_D1_NO_PARASITIC;
      }
      else if (r8B == 0x02) {
        PRINT(F(" Family D1 (Clone)."));
        sensor_family = FAMILY_D1;
      }
      else if (r8B == 0x00) {
        PRINT(F(" Family D2 (Clone w/o parasitic power)."));
        sensor_family = FAMILY_D2;
      }
      else if (r8B != 0xFF) {
        PRINT(F(" Family D (Clone), unknown subtype (0x8B="));
        print_hex(r8B);
        PRINT(F(")."));
        sensor_family = FAMILY_D_UNKNOWN;
      }
      if (r8B != 0xff) identified++;
    }

    {
      // this should be Family C by implication.
      // Don't have a test for response to undocumented codes, so check if
      // config register is constant, which is unique property among 2019 Families.
      uint8_t buff[9];
      uint8_t cfg1, cfg2;
      ds->reset();
      ds->select(addr);
      ds->write(0x4E);
      ds->write(0xaa);
      ds->write(0x55);
      ds->write(0x00);
      if (!read_scratchpad(addr, buff)) read_scratchpad(addr, buff);
      if (is_all_00(buff, 9) || (0 != OneWire::crc8(buff,9))) goto err_C;
      cfg1 = buff[4];
      ds->reset();
      ds->select(addr);
      ds->write(0x4E);
      ds->write(0xaa);
      ds->write(0x55);
      ds->write(0xff);
      if (!read_scratchpad(addr, buff)) read_scratchpad(addr, buff);
      if (is_all_00(buff, 9) || (0 != OneWire::crc8(buff,9))) goto err_C;
      cfg2 = buff[4];
      ds->reset();
      ds->select(addr);
      ds->write(0x64);
      delay(10);
      if (cfg1 == cfg2) {
        PRINT(F(" Family C (Clone)."));
        sensor_family = FAMILY_C;
        identified++;
      }
      if (0) {
err_C:
        PRINT(F(" (Error reading scratchpad register [C].)"));
      }
    }

    if (identified == 0) {
      PRINT(F(" (Could not identify Family.)"));
      sensor_family = FAMILY_UNKNOWN;
    }

    if (identified > 1)
      PRINT(F(" (Part may belong to a Family not seen in 2019.)"));

    PRINTLN();
    
  } // end iterate over all sensors

  if (sensor_count == 0) {
    // produce output so the user knows we tried (and failed)
    // to detect a sensor. May have forgotten the pull-up
    // resistor.
    PRINTLN(F("No sensors detected."));
    sensor_family = NO_SENSOR;
  }
  
  PRINTLN(F("-------------")); // indicate end
  return sensor_family;
}


#ifdef __AVR__
const __FlashStringHelper * ds18b20_name(OneWire* _OneWire, int sensor_number)
{
  DS18B20_family_enum i = ds18b20_family(_OneWire, sensor_number);
  return (const __FlashStringHelper *)pgm_read_word(&(sensor_descr[i]));
}
#else
const char * ds18b20_name(OneWire* _OneWire, int sensor_number)
{
  DS18B20_family_enum i = ds18b20_family(_OneWire, sensor_number);
  return (sensor_descr[i]);
}
#endif

}
