/*
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
 */

#include <DiscoverFakeDS18B20.h>

// define sensor pin
#define	PIN_ONEWIRE    PB6

OneWire oneWire(PIN_ONEWIRE);

using namespace DiscoverFakeDS18B20;

void setup()
{
  Serial.begin(115200);
  while(!Serial);
  switch(discover_fake_ds18b20(&oneWire, 0)) {
    case IS_GENUINE: 
      Serial.println("genuine");
      break;
    case IS_FAKE: 
      Serial.println("fake");
      break;
    case DONT_KNOW: 
    default:
      Serial.println("don't know");
      break;
  }
}

void loop()
{
}
