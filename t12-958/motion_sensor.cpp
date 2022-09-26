#include <stdint.h>
#include "motion_sensor.h"

/*
 * the soldering iron handle contains a vibration sensor.
 * when the soldering iron moves, pin PA0 toggles.
 * put a hardware counter on pin PA0 to detect movement.
 */


extern "C" {
#include "zf_tim.h"
}

MotionSensor::MotionSensor()
{
  // put counter on pin PA0
  tim_counter_init(TIM_2, TIM_2_ENC1_A00);
}


bool MotionSensor::moved()
{
  uint16_t new_count = tim_counter_get_count(TIM_2);
  bool ret_val = new_count != count;

  count = new_count;
  return (ret_val);
}
