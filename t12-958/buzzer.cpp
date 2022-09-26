#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include "t12-common.h"
#include "buzzer.h"
#include "settings.h"
#include "pins.h"

extern Task tBuzzer;

#define	SHORT_BEEP	10
#define	LONG_BEEP	30
#define	FATAL_BEEP	2000
#define	ALARM_PERIOD	2000
#define	ALARM_HIGH	10
#define	ALARM_LOW	(ALARM_PERIOD-ALARM_HIGH)

namespace buzzer {
void setup()
{
  tBuzzer.disable();
  pinMode(BUZ0, OUTPUT);
  digitalWrite(BUZ0, 1);
}


static inline void buzzer_off()
{
  digitalWrite(BUZ0, 1);
}


static inline void buzzer_on()
{
  if (settings.buzzer_enabled) {
    digitalWrite(BUZ0, 0);
  }
}


void callback_buzzer_off()
{
  buzzer_off();
  tBuzzer.disable();
  return;
}


void beep(uint32_t duration)
{
  buzzer_on();
  tBuzzer.setIterations(TASK_ONCE);
  tBuzzer.setCallback(&callback_buzzer_off);
  tBuzzer.restartDelayed(duration * TASK_MILLISECOND);
}


void short_beep()
{
  beep(SHORT_BEEP);
}


void long_beep()
{
  beep(LONG_BEEP);
}


void fatal_beep()
{
  beep(FATAL_BEEP);
}


void callback_alarm()
{
  if (tBuzzer.getRunCounter() & 1) {
    buzzer_on();
    tBuzzer.setInterval(ALARM_HIGH * TASK_MILLISECOND);
  } else {
    buzzer_off();
    tBuzzer.setInterval(ALARM_LOW * TASK_MILLISECOND);
  }
  return;
}


void alarm_on()
{
  tBuzzer.setIterations(TASK_FOREVER);
  tBuzzer.setCallback(&callback_alarm);
  tBuzzer.restartDelayed(TASK_MILLISECOND);
}


void alarm_off()
{
  buzzer_off();
  tBuzzer.disable();
}
}
