/*
 * Copyright Chris Petrich
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
 *   File:    discover_fake_DS18B20.ino
 *   Author:  Chris Petrich
 *   Version: 22 Oct 2019 
 *   
 *   Source:  https://github.com/cpetrich/counterfeit_DS18B20/
 *   Documentation:  https://github.com/cpetrich/counterfeit_DS18B20/
 *   
 * Arduino library:
 *
 *   Author:  Koen De Vleeschauwer
 *   Source:  https://github.com/koendv/CheckDS18B20
 *
 * This demonstration script performs authenticity tests on DS18B20 sensors by
 * evaluating ROM code and Scratchpad Register. It uses documented ROM commands 
 * F0h and 55h and Function Commands 44h, 4Eh and BEh, only.
 * It does not test the power-up state and it does not write to or test the EEPROM.
 * It is INTENDED for EDUCATIONAL PURPOSES, only.
 * There may be circumstances under which the sketch permanently damages one-wire 
 * sensors in obvious or non-obvious ways.
 * (I don't think it does that to authentic Maxim sensors, but I won't guarantee
 * anything. See licence text for details.)
 * 
 * 
 * This script is written for Arduino. Wiring:
 * 
 * CPU Vcc         -------o------ DS18B20 Vcc
 *                        |
 *                       [R]  <- choose resistor appropriate for supply voltage and current that the microcontroller is able to sink.
 *                        |
 * CPU pin_onewire -------o------ DS18B20 data
 * 
 * CPU GND         -------------- DS18B20 GND
 * 
 */

#include "DiscoverFakeDS18B20.h"

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

namespace DiscoverFakeDS18B20 {

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

discover_ds18b20_enum discover_fake_ds18b20(OneWire* _OneWire, int sensor_number) {
  ds = _OneWire;
  if (ds == NULL) return DONT_KNOW;
  int sensor_count = 0;

  // ROM address of current sensor
  uint8_t addr[8];  
  // buffers for scratchpad register
  uint8_t buffer0[9];
  uint8_t buffer1[9];
  uint8_t buffer2[9];
  uint8_t buffer3[9];
  // flag to indicate if validation
  //  should be repeated at a different
  //  sensor temperature
  bool t_ok;

  ds->reset_search();
  while (ds->search(addr)) {
    sensor_count ++;
    if (sensor_count != sensor_number + 1) continue;

    int fake_flags = 0;
    
    print_array(addr, 8, '-');
    if (0 != OneWire::crc8(addr, 8)) {
      // some fake sensors can have their ROM overwritten with
      // arbitrary nonsense, so we don't expect anything good
      // if the ROM doesn't check out
      fake_flags += 1;
      PRINT(F(" (CRC Error -> Error.)"));
    }

    if ((addr[6] != 0) || (addr[5] != 0) || (addr[0] != 0x28)) {
      fake_flags += 1;
      PRINT(F(": ROM does not follow expected pattern 28-xx-xx-xx-xx-00-00-crc. Error."));
    } else {
      PRINT(F(": ROM ok."));
    }    
    PRINTLN();
    
    if (!read_scratchpad(addr, buffer0)) read_scratchpad(addr, buffer0);
    
    PRINT(F("  Scratchpad Register: "));
    print_array(buffer0, 9, '/');
    if (0 != OneWire::crc8(buffer0, 9)) {
      // Unlikely that a sensor will mess up the CRC of the scratchpad.
      // --> Assume we're dealing with a bad connection rather than a bad 
      //     sensor, dump data, and move on to next sensor.
      PRINTLN(F("  CRC Error. Check connections or replace sensor."));
      continue;      
    }
    PRINTLN();

    // Check content of user EEPROM. Since the EEPROM may have been programmed by the user earlier
    // we do not use this as a test. Rather, we dump this as info.
    PRINT(F("  Info only: Scratchpad bytes 2,3,4 ("));
    print_array(buffer0+2,3,'/');
    PRINT(F("): "));
    if ((buffer0[2] != 0x4b) || (buffer0[3] != 0x46) || (buffer0[4] != 0x7f))
      PRINTLN(F(" not Maxim default values 4B/46/7F."));
    else
      PRINTLN(F(" Maxim default values."));


    PRINT(F("  Scratchpad byte 5 (0x"));
    print_hex(buffer0[5]);
    PRINT(F("): "));
    if (buffer0[5] != 0xff) {
      fake_flags += 1;
      PRINTLN(F(" should have been 0xFF according to datasheet. Error."));
    } else {
      PRINTLN(F(" ok."));
    }

    PRINT(F("  Scratchpad byte 6 (0x"));
    print_hex(buffer0[6]);
    PRINT(F("): "));
    if ( ((buffer0[6] == 0x00) || (buffer0[6] > 0x10)) || // totall wrong value
         ( ((buffer0[0] != 0x50) || (buffer0[1] != 0x05)) && ((buffer0[0] != 0xff) || (buffer0[1] != 0x07)) && // check for valid conversion...
           (((buffer0[0] + buffer0[6]) & 0x0f) != 0x00) ) ) { //...before assessing DS18S20 compatibility.
      fake_flags += 1;
      PRINTLN(" unexpected value. Error.");
    } else
      PRINTLN(" ok.");
    
    PRINT(F("  Scratchpad byte 7 (0x"));
    print_hex(buffer0[7]);
    PRINT(F("): "));
    if (buffer0[7] != 0x10) {
      fake_flags += 1;
      PRINTLN(F(" should have been 0x10 according to datasheet. Error."));
    } else {
      PRINTLN(F(" ok."));
    }

    // set the resolution to 10 bit and modify alarm registers    
    ds->reset();
    ds->select(addr);
    ds->write(0x4E); // write scratchpad. MUST be followed by 3 bytes as per datasheet.
    ds->write(buffer0[2] ^ 0xff);
    ds->write(buffer0[3] ^ 0xff);
    ds->write(0x3F);
    ds->reset();

    if (!read_scratchpad(addr, buffer1)) read_scratchpad(addr, buffer1);
    
    PRINT(F("  0x4E modifies alarm registers: "));
    if ((buffer1[2] != (buffer0[2] ^ 0xff)) || (buffer1[3] != (buffer0[3] ^ 0xff))) {
      fake_flags += 1;
      PRINT(F(" cannot modify content as expected (want: "));
      print_hex(buffer0[2] ^ 0xff);
      PRINT('/');
      print_hex(buffer0[3] ^ 0xff);
      PRINT(F(", got: "));
      print_array(buffer1+2, 2, '/');
      PRINTLN(F("). Error."));      
    } else
      PRINTLN(F(" ok."));

    PRINT(F("  0x4E accepts 10 bit resolution: "));
    if (buffer1[4] != 0x3f) {
      fake_flags += 1;
      PRINT(F(" rejected (want: 0x3F, got: "));
      print_hex(buffer1[4]);
      PRINTLN(F("). Error."));
    } else
      PRINTLN(F(" ok."));

    PRINT(F("  0x4E preserves reserved bytes: "));
    if ((buffer1[5] != buffer0[5]) || (buffer1[6] != buffer0[6]) || (buffer1[7] != buffer0[7])) {
      fake_flags += 1;
      PRINT(F(" no, got: "));
      print_array(buffer1+5, 3, '/');
      PRINTLN(F(". Error."));
    } else
      PRINTLN(F(" ok."));    

    // set the resolution to 12 bit
    ds->reset();
    ds->select(addr);
    ds->write(0x4E); // write scratchpad. MUST be followed by 3 bytes as per datasheet.
    ds->write(buffer0[2]);
    ds->write(buffer0[3]);
    ds->write(0x7f);
    ds->reset();

    if (!read_scratchpad(addr, buffer2)) read_scratchpad(addr, buffer2);
    
    PRINT(F("  0x4E accepts 12 bit resolution: "));
    if (buffer2[4] != 0x7f) {
      fake_flags += 1;
      PRINT(F(" rejected (expected: 0x7F, got: "));
      print_hex(buffer2[4]);
      PRINTLN(F("). Error."));
    } else
      PRINTLN(F(" ok."));

    PRINT(F("  0x4E preserves reserved bytes: "));
    if ((buffer2[5] != buffer1[5]) || (buffer2[6] != buffer1[6]) || (buffer2[7] != buffer1[7])) {
      fake_flags += 1;
      PRINT(F(" no, got: "));
      print_array(buffer2+5, 3, '/');
      PRINTLN(F(". Error."));
    } else
      PRINTLN(F(" ok."));

    PRINT("  Checking byte 6 upon temperature change: ");
    if (( ((buffer2[0] == 0x50) && (buffer2[1] == 0x05)) || ((buffer2[0] == 0xff) && (buffer2[1] == 0x07)) ||
         ((buffer2[6] == 0x0c) && (((buffer2[0] + buffer2[6]) & 0x0f) == 0x00)) ) &&
         ((buffer2[6] >= 0x00) && (buffer2[6] <= 0x10)) ){
      // byte 6 checked out as correct in the initial test but the test ambiguous.
      //   we need to check if byte 6 is consistent after temperature conversion
            
      // We'll do a few temperature conversions in a row.
      // Usually, the temperature rises slightly if we do back-to-back
      //   conversions.
      int count = 5;
      do {
        count -- ;
        if (count < 0)
          break;
        // perform temperature conversion
        ds->reset();
        ds->select(addr);
        ds->write(0x44);
        delay(750);
        
        if (!read_scratchpad(addr, buffer3)) read_scratchpad(addr, buffer3);
        
      } while ( ((buffer3[0] == 0x50) && (buffer3[1] == 0x05)) || ((buffer3[0] == 0xff) && (buffer3[1] == 0x07)) ||
                ((buffer3[6] == 0x0c) && (((buffer3[0] + buffer3[6]) & 0x0f) == 0x00)) );
      if (count < 0) {
        PRINTLN(F(" Inconclusive. Please change sensor temperature and repeat."));
        t_ok = false;
      } else {
        t_ok = true;
        if ((buffer3[6] != 0x0c) && (((buffer3[0] + buffer3[6]) & 0x0f) == 0x00)) {
          PRINTLN(F(" ok."));
        } else {
          fake_flags += 1;
          PRINT(F(" Temperature LSB = 0x"));
          print_hex(buffer3[0]);
          PRINT(F(" but byte 6 = 0x"));
          print_hex(buffer3[6]);
          PRINTLN(F(". Error."));
        }
      }
    } else {
      PRINTLN(F("not necessary. Skipped."));
      t_ok = true;
    }

    PRINT(F("  --> "));
    if (!t_ok) {
      PRINT(F("DS18S20 counterfeit test not completed, otherwise sensor"));
    } else 
      PRINT(F("Sensor"));
      
    if (fake_flags == 0) {
      PRINTLN(F(" responded like a genuine Maxim."));
      PRINTLN(F("      Not tested: EEPROM, Parasite Power, and undocumented commands."));
    } else {
      PRINT(F(" appears to be counterfeit based on "));
      PRINT(fake_flags, DEC);
      PRINTLN(F(" deviations."));
      if (fake_flags == 1) {
        PRINTLN(F("  The number of deviations is unexpectedly small."));
        PRINTLN(F("  Please see https://github.com/cpetrich/counterfeit_DS18B20/"));
        PRINTLN(F("  to help interpret the result."));
      }
    }
    PRINTLN();
    if (!t_ok) return DONT_KNOW;
    if (fake_flags == 0) return IS_GENUINE;
    else return IS_FAKE;
  } // done with all sensors
  PRINTLN(F("------------------------------------------------"));
  return DONT_KNOW;
}

}

