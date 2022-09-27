#include "settings.h"
#include <cassert>
#include <FlashStorage.h>
#include <HWCRC.h>
#include <RTTStream.h>
extern RTTStream Serial;


/*
 * settings
 *
 * Settings are stored in two places:
 * - real-time clock nvram. small (31 bytes) but fast, and can be written as many times as desired.
 * - internal flash. big (128 kbyte) but writing is slow, and can only be written 100.000 times.
 *
 * tip temperature is a settings that changes often.
 * settings that change often are written to rtc nvram.
 * store_setpoint(); recall_setpoint();
 *
 * settings that change infrequently are written to flash.
 * tip calibration data is an example of settings that do not change often.
 * store_settings(); recall_settings();
 *
 */

#include "clock.h"
using clock::rtc;

FlashStorage(saved_settings, settings_struct);
HWCRC hwcrc;

bool menu_running = false;

uint32_t temp_setpoint = 0;
uint32_t vsupply_x10 = 0;
int32_t ambient_temp_x10 = 0;
int32_t unsmoothed_tip_temp_x10 = 0;
int32_t tip_temp_x10 = 0;
int32_t thermocouple_temp_x10 = 0;
uint32_t ntc_adc = 0;   // expose ntc adc count for ntc calibration
uint32_t tip_adc = 0;   // expose tip adc count for tip calibration
uint32_t iron_pwm = 0;
bool flag_no_iron = false;
bool flag_shake = false;
bool flag_warning = false;
uint32_t power_bar = 0;

// packed struct for rtc nvram
typedef struct __attribute__((__packed__)) {
  uint16_t temp_setpoint;
  uint8_t checksum;       // checksum is remainder after dividing by 251; 251 is the biggest prime that fits in a byte.
}
saved_setpoint_struct;

static const settings_struct default_settings =
{ .brightness		=  255,
  .long_press_millis	=  500,
  .standby_time_minutes =   15,
  .standby_temp		=  200,
  .sleep_time_minutes	=   30,
  .boost_time_minutes	=    5,
  .boost_temp		=   50,
  .buzzer_enabled	= false,
  .temp_step		=    5,
  .Kp_x10		=  400,
  .Ki_x10		=  550,
  .Kd_x10		=   70,
  .cal_ntc_in1		=  120,
  .cal_ntc_out1		=    0,
  .cal_ntc_in2		= 1600,
  .cal_ntc_out2		= 2700,
  .cal_adc_at_0		=    0,
  .cal_adc_at_250	= 1900,
  .cal_adc_at_400	= 2800,
  .tempsensor		= CPU_TEMPSENSOR,
  .poweron		= POWERON_SLEEP,
};

settings_struct settings = default_settings;

void reset_settings()
{
  flag_no_iron = false;
  flag_shake = false;
  flag_warning = false;
  power_bar = 0;
  settings = default_settings;
  temp_setpoint = min_temp;
}


void store_setpoint()
{
  saved_setpoint_struct saved_setpoint;

  saved_setpoint.temp_setpoint = temp_setpoint;
  saved_setpoint.checksum = saved_setpoint.temp_setpoint % 251;
  rtc.writeProtect(false);
  rtc.writeRamBulk((uint8_t *)&saved_setpoint, sizeof(saved_setpoint));
  if (DS1302::kRamSize < sizeof(saved_setpoint)) {
    Serial.print("settings truncated");
  }
}


void recall_setpoint()
{
  saved_setpoint_struct saved_setpoint;
  uint8_t calculated_checksum;

  rtc.readRamBulk((uint8_t *)&saved_setpoint, sizeof(saved_setpoint));
  calculated_checksum = saved_setpoint.temp_setpoint % 251;
  if (calculated_checksum == saved_setpoint.checksum) {
    temp_setpoint = saved_setpoint.temp_setpoint;
  }
}


void store_settings()
{
  store_setpoint();
  /* calculate crc */
  if (((uint32_t)&settings % 4 == 0) && (sizeof(settings) % 4 == 0)) {  // needed for hardware crc
    settings.crc = 0;
    hwcrc.reset();
    hwcrc.add((uint32_t *)&settings, sizeof(settings)/sizeof(uint32_t));
    settings.crc = hwcrc.get();
  }else {
    settings.crc = 0xdeadbeef;
  }
  /* save settings to flash */
  saved_settings.write(settings);
}


void recall_settings()
{
  uint32_t saved_crc, calculated_crc;

  /* restore settings from flash */
  settings = saved_settings.read();
  /* check crc */
  if (((uint32_t)&settings % 4 == 0) && (sizeof(settings) % 4 == 0)) {  // needed for hardware crc
    saved_crc = settings.crc;
    settings.crc = 0;
    hwcrc.reset();
    hwcrc.add((uint32_t *)&settings, sizeof(settings)/sizeof(uint32_t));
    calculated_crc = hwcrc.get();
  } else {
    Serial.println("no crc?");
    calculated_crc = 0xdeadbeef;
  }
  recall_setpoint();
  temp_setpoint = constrain(temp_setpoint, min_temp, max_temp);
  if (calculated_crc != saved_crc) {
    Serial.println("first run");
    reset_settings();
    store_settings();
  }
}


static void print_item(char *str, int32_t i, bool last = false)
{
  Serial.print("\"");
  Serial.print(str);
  Serial.print("\":");
  Serial.print(i);
  if (last) {
    Serial.println();
  } else{
    Serial.println(',');
  }
  delay(2);
}


static void print_item(char *str, char *i)
{
  Serial.print("\"");
  Serial.print(str);
  Serial.print("\":\"");
  Serial.print(i);
  Serial.println("\",");
  delay(2);
}


/* print settings on serial as json */
void print_settings()
{
  //Serial.blockUpBufferFull(); // might hang if no rtt reader present
  Serial.println();
  Serial.println('{');
  print_item("temp_setpoint", temp_setpoint);
  print_item("brightness", settings.brightness);
  print_item("long_press_millis", settings.long_press_millis);
  print_item("standby_time_minutes", settings.standby_time_minutes);
  print_item("standby_temp", settings.standby_temp);
  print_item("sleep_time_minutes", settings.sleep_time_minutes);
  print_item("boost_time_minutes", settings.boost_time_minutes);
  print_item("boost_temp", settings.boost_temp);
  print_item("buzzer_enabled", settings.buzzer_enabled);
  print_item("temp_step", settings.temp_step);
  print_item("Kp_x10", settings.Kp_x10);
  print_item("Ki_x10", settings.Ki_x10);
  print_item("Kd_x10", settings.Kd_x10);
  print_item("cal_ntc_in1 ", settings.cal_ntc_in1);
  print_item("cal_ntc_out1 ", settings.cal_ntc_out1);
  print_item("cal_ntc_in2 ", settings.cal_ntc_in2);
  print_item("cal_ntc_out2 ", settings.cal_ntc_out2);
  print_item("cal_adc_at_0", settings.cal_adc_at_0);
  print_item("cal_adc_at_250", settings.cal_adc_at_250);
  print_item("cal_adc_at_400", settings.cal_adc_at_400);
  print_item("tempsensor", settings.tempsensor);
  print_item("poweron", settings.poweron, true);
  Serial.println("}");
}
