/*
 * This demonstration script attempts to classify DS18B20 clones
 * assuming the chip belongs to one of the Families detailed on
 * https://github.com/cpetrich/counterfeit_DS18B20/
 * MAX31820 sensors cannot be distinguished from DS18B20 in software.
 *
 * This script evaluates the response to UNDOCUMENTED FUNCTION CODES.
 * If this script is used on DS18B20 clones that do not belong to one
 * of the Families listed on https://github.com/cpetrich/counterfeit_DS18B20/
 * then write operations could be triggered inadvertently that may
 * mess up calibration constants and render the part useless -- or
 * produce permanent damage in other ways.
 *
 * Disclaimer: This code sends undocumented function codes to 1-wire
 * chips and may therefore permanently damage the part attached.
 * USE AT YOUR OWN RISK AND FOR YOUR OWN SAKE DO NOT USE THIS SCRIPT
 * ON CHIPS INTENDED FOR DEPLOYMENT OR SALE!
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
 * Always attach the power pin of the DS18B20 to either GND or Vcc unless it is a DS18B20-PAR.
 */

#include <CheckDS18B20.h>

// define sensor pin
#define	PIN_ONEWIRE    PB6

OneWire oneWire(PIN_ONEWIRE);

using namespace CheckDS18B20;

void setup()
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println();
  Serial.print("Family ");
  Serial.println(ds18b20_name(&oneWire, 0));
}

void loop()
{
}
