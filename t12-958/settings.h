#ifndef _SETTINGS_H_
#define	_SETTINGS_H_

#include <Arduino.h>
#include "main_screen.h"

// power_bar displays a bar graph of power applied
// MAX_POWER_BAR is value of power_bar that corresponds to 100% power
#define	MAX_POWER_BAR    64

// iron_pwm is soldering iron power
// value of iron_pwm that corresponds to 100% power
#define	MAX_PWM    32767

// MIN_PWM_IDLE_MS is minimum idle time of pwm, in milliseconds
// adc measurement of tip temperature is done during pwm idle time
// allow 20 ms for thermocouple opamp to settle.
#define	MIN_PWM_IDLE_MS    20

typedef enum {
  NO_TEMPSENSOR, NTC_TEMPSENSOR, CPU_TEMPSENSOR, DS18B20_TEMPSENSOR
} sensor_enum;

typedef enum {
  POWERON_RUNNING, POWERON_STANDBY, POWERON_SLEEP
} poweron_enum;

/* global variables */
extern bool menu_running;
extern bool flag_no_iron;
extern bool flag_shake;
extern bool flag_warning;
extern uint32_t power_bar;
extern uint32_t vsupply_x10;
extern int32_t ambient_temp_x10;
extern int32_t tip_temp_x10;
extern int32_t thermocouple_temp_x10;
extern int32_t unsmoothed_tip_temp_x10;
extern uint32_t ntc_adc;
extern uint32_t tip_adc;
extern uint32_t iron_pwm;

/* constants, temperature range */
static const int32_t min_temp = 180;
static const int32_t max_temp = 480;

/* frequently changed settings get stored in nvram or eeprom */
extern uint32_t temp_setpoint;

/* less frequently changed settings get stored in flash */
typedef struct {
  uint32_t	crc;
  uint32_t	brightness;
  uint32_t	long_press_millis;
  uint32_t	standby_time_minutes;
  uint32_t	standby_temp;
  uint32_t	sleep_time_minutes;
  uint32_t	boost_time_minutes;
  uint32_t	boost_temp;
  bool		buzzer_enabled;
  int32_t	temp_step; // change in setpoint for one click of rotary encoder
  int32_t	Kp_x10;
  int32_t	Ki_x10;
  int32_t	Kd_x10;
  int32_t	cal_ntc_in1;
  int32_t	cal_ntc_out1;
  int32_t	cal_ntc_in2;
  int32_t	cal_ntc_out2;
  uint32_t	cal_adc_at_0;
  uint32_t	cal_adc_at_250;
  uint32_t	cal_adc_at_400;
  sensor_enum	tempsensor;     /* cold junction temperature sensor: NTC, CPU or DS18B20 */
  poweron_enum	poweron;        /* state after power on */
} settings_struct;

extern settings_struct settings;

void reset_settings();
void store_setpoint();
void recall_setpoint();
void store_settings();
void recall_settings();
void print_settings();

#endif
