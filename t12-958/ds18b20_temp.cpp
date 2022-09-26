#include <OneWire.h>
#include <DallasTemperature.h>
#include "t12-common.h"
#include "pins.h"
#include "ds18b20_temp.h"
#include "buzzer.h"
#include "settings.h"

#define	MAX_TIMEOUT    5

OneWire onewire(ONEWIRE);
static DallasTemperature sensor(&onewire);
static DeviceAddress addr;
static uint32_t ds18b20_timeout = 0;
static bool first_run = true;
static bool alarm_on = false;

static void DS18B20Reset()
{
  sensor.begin();
  if (!sensor.getAddress(addr, 0)) {
    Serial.println("ds18b20 not found");
    if (!alarm_on) {
      buzzer::alarm_on();
      alarm_on = true;
    }
    return;
  }
  if (alarm_on) {
    buzzer::alarm_off();
    alarm_on = false;
  }
  sensor.setResolution(addr, 12);
  sensor.setWaitForConversion(false);
  sensor.requestTemperatures();
  ds18b20_timeout = 0;
}


// temperature in 1/10 degree C

int32_t DS18B20Temp_x10()
{
  int32_t t;

  if (first_run) {
    DS18B20Reset();
    first_run = false;
    return (ERROR_TEMP);
  }
  if (ds18b20_timeout >= MAX_TIMEOUT) {
    Serial.println("\nds18b20 reset");
    DS18B20Reset();
    return (ERROR_TEMP);
  }
  if (!sensor.isConversionComplete()) {
    ds18b20_timeout++;
    return (ERROR_TEMP);
  }
  t = sensor.getTemp(addr); // temperature in 1/128 degree C
  sensor.requestTemperatures();
  if (t == DEVICE_DISCONNECTED_RAW) {
    Serial.println("\nds18b20 disconnected");
    ds18b20_timeout++;
    return (ERROR_TEMP);
  }else if (t == DEVICE_FAULT_OPEN_RAW) {
    Serial.println("\nds18b20 fault open");
    ds18b20_timeout++;
    return (ERROR_TEMP);
  }else if (t == DEVICE_FAULT_SHORTVDD_RAW) {
    Serial.println("\nds18b20 fault shortvdd");
    ds18b20_timeout++;
    return (ERROR_TEMP);
  }
  t *= 5;
  t >>= 6; // temperature in 0.1 degree C
  return (t);
}
