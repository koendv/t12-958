#include "FastPID.h"
#include "settings.h"
#include "iron.h"
#include "buzzer.h"
#include "pins.h"
#include "t12-common.h"

using namespace main_screen;

namespace iron {
FastPID fast_pid;

static const uint32_t pwm_max = MAX_PWM - 1;
static const uint32_t hysteresis_x10 = 15;
static bool over_temp = false;
static bool full_power = false;

void setup()
{
  fast_pid.setCoefficients(settings.Kp_x10, settings.Ki_x10, settings.Kd_x10, PID_HZ*10);
  fast_pid.setOutputConfig(15, false);
  // need 20ms idle time to do tip temperature measurement
  fast_pid.setOutputRange(0, pwm_max);
  analogWriteResolution(15);
}


void TaskIron()
{
  int32_t setpoint, setpoint_x10;
  int32_t threshold_x10;

  if (flag_no_iron) {
    iron_pwm = 0;
    power_bar = 0;
    analogWrite(PWM, 0);
    return;
  }

  // over temp warning
  if ((tip_temp_x10 > max_temp * 10) && !over_temp) {
    Serial.println("over temp");
    buzzer::alarm_on();
    over_temp = true;
  } else if (over_temp) {
    buzzer::alarm_off();
    over_temp = false;
  }

  if ((gui_state == STATE_SLEEP) || (gui_state == STATE_INIT)) {
    iron_pwm = 0;
  } else {
    switch (gui_state)
    {
    case STATE_RUNNING:
    case STATE_SETPOINT:
      setpoint = temp_setpoint;
      break;

    case STATE_BOOST:
      setpoint = temp_setpoint + settings.boost_temp;
      break;

    case STATE_STANDBY:
      setpoint = settings.standby_temp;
      break;

    default:
      setpoint = min_temp;
      break;
    }
    setpoint = constrain(setpoint, min_temp, max_temp);
    setpoint_x10 = setpoint * 10;

    // to avoid PID wind-up, start PID controller at 10% below setpoint
    threshold_x10 = setpoint_x10 - setpoint;
    // add a bit of hysteresis, to avoid bang-bang control
    if (unsmoothed_tip_temp_x10 < threshold_x10 - hysteresis_x10) {
      full_power = true;
    }else if (unsmoothed_tip_temp_x10 > threshold_x10 + hysteresis_x10) {
      full_power = false;
    }

    if (full_power) {
      iron_pwm = pwm_max;
      fast_pid.clear();
    } else {
      // use unsmoothed tip temperature as PID feedback.
      // smoothed tip temperature is delayed by the number of samples averaged
      // and too much delay causes PID instability
      iron_pwm = fast_pid.step(setpoint_x10, unsmoothed_tip_temp_x10);
    }
  }

  // safety check
  if (iron_pwm > pwm_max) {
    Serial.println("pwm idle too short");
    iron_pwm = pwm_max;
  }

  // set display power bar
  power_bar = iron_pwm * MAX_POWER_BAR / (MAX_PWM + 1);

  // set pwm output
  analogWrite(PWM, iron_pwm);
}
}
