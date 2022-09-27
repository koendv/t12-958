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

#ifndef _DiscoverFakeDS18B20_H_
#define _DiscoverFakeDS18B20_H_

#include <OneWire.h>

namespace DiscoverFakeDS18B20 {

typedef enum {IS_GENUINE, IS_FAKE, DONT_KNOW} discover_ds18b20_enum;

/* first sensor on bus has sensor_number 0 */
discover_ds18b20_enum discover_fake_ds18b20(OneWire* _OneWire, int sensor_number);

}

#endif
