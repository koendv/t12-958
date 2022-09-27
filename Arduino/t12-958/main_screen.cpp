#include "t12-common.h"
#include "display.h"
#include "pins.h"
#include "settings.h"
#include "main_screen.h"
#include "menu_screen.h"
#include "button.h"
#include "motion_sensor.h"

#define	MINUTES_TO_MILLIS    60000

MotionSensor motion_sensor;

using namespace button;

namespace main_screen {
int32_t encoder_change;
uint32_t encoder_change_millis;
uint32_t last_active_millis;
static bool setpoint_changed = false;
static uint32_t setpoint_changed_millis = 0;

gui_enum gui_state;

using namespace main_screen;

void update_encoder()
{
  encoder_change = encoder.readAndReset();
  if (encoder_change) {
    encoder_change_millis = millis();
  }
}


void update_setpoint(int32_t change)
{
  temp_setpoint += change;
  if (temp_setpoint > max_temp) {
    temp_setpoint = max_temp;
  } else if (temp_setpoint < min_temp) {
    temp_setpoint = min_temp;
  }
  setpoint_changed = true;
  setpoint_changed_millis = millis();
}


#if 0
/* used to make pictures for the documentation */
void screendump()
{
  Serial.blockUpBufferFull();
  u8g2.writeBufferPBM(Serial);
}
#endif


void setup()
{
  encoder_change = 0;
  encoder_change_millis = 0;
  last_active_millis = millis();
  gui_state = STATE_INIT;
}


void loop()
{
  update_encoder();

  /* save current setpoint when user is finished adjusting */
  if (setpoint_changed && (millis() - setpoint_changed_millis > 30000)) {
    store_setpoint();
    setpoint_changed = false;
  }

  switch (gui_state)
  {
  case STATE_INIT:
    if (!button::pressed()) { // wait until button quiet
      if (settings.poweron == POWERON_STANDBY) {
        gui_state = STATE_STANDBY;
      } else if (settings.poweron == POWERON_SLEEP) {
        gui_state = STATE_SLEEP;
      } else {
        gui_state = STATE_RUNNING;
      }
    }
    break;

  case STATE_RUNNING:
    if (encoder_change) {
      last_active_millis = millis();
      gui_state = STATE_SETPOINT;
    }else if (button::click()) {
      last_active_millis = millis();
      gui_state = STATE_BOOST;
    }else if (button::longpress()) {
      last_active_millis = millis();
      menu_running = true;
      menu_screen::force_redraw();
    }else if (motion_sensor.moved()) {
      last_active_millis = millis();
    }else if (millis() - last_active_millis > settings.sleep_time_minutes * MINUTES_TO_MILLIS) {
      gui_state = STATE_SLEEP;
    }else if (millis() - last_active_millis > settings.standby_time_minutes * MINUTES_TO_MILLIS) {
      gui_state = STATE_STANDBY;
    }
    break;

  case STATE_SETPOINT:
    if (encoder_change) {
      last_active_millis = millis();
      if (button::pressed()) {
        update_setpoint(encoder_change);
      } else{
        update_setpoint(encoder_change * settings.temp_step);
      }
    }else if ((millis() - encoder_change_millis > settings.long_press_millis) && !button::pressed()) {
      /* clear button events before switching screen */
      button::click();
      button::longpress();
      gui_state = STATE_RUNNING; // timeout, return to displaying temperature
    }else if (motion_sensor.moved()) {
      last_active_millis = millis();
    }
    break;

  case STATE_BOOST:
    if (encoder_change) {
      last_active_millis = millis();
      gui_state = STATE_SETPOINT;
    }else if (button::click()) {
      last_active_millis = millis();
      gui_state = STATE_STANDBY;
    }else if (button::longpress()) {
      last_active_millis = millis();
      menu_running = true;
      menu_screen::force_redraw();
    }else if (motion_sensor.moved()) {
      last_active_millis = millis();
      gui_state = STATE_RUNNING;
    }else if (millis() - last_active_millis > settings.sleep_time_minutes * MINUTES_TO_MILLIS) {
      gui_state = STATE_SLEEP;
    }else if (millis() - last_active_millis > settings.standby_time_minutes * MINUTES_TO_MILLIS) {
      gui_state = STATE_STANDBY;
    }
    break;

  case STATE_STANDBY:
    if (encoder_change) {
      last_active_millis = millis();
      gui_state = STATE_SETPOINT;
    }else if (button::click()) {
      last_active_millis = millis();
      gui_state = STATE_SLEEP;
    }else if (button::longpress()) {
      last_active_millis = millis();
      menu_running = true;
      menu_screen::force_redraw();
    }else if (motion_sensor.moved()) {
      last_active_millis = millis();
      gui_state = STATE_RUNNING;
    }else if (millis() - last_active_millis > settings.sleep_time_minutes * MINUTES_TO_MILLIS) {
      gui_state = STATE_SLEEP;
    }
    break;

  case STATE_SLEEP:
    if (encoder_change) {
      last_active_millis = millis();
      gui_state = STATE_SETPOINT;
    }else if (button::click()) {
      last_active_millis = millis();
      gui_state = STATE_RUNNING;
    }else if (button::longpress()) {
      last_active_millis = millis();
      menu_running = true;
      menu_screen::force_redraw();
    }else if (motion_sensor.moved()) {
      last_active_millis = millis();
      gui_state = STATE_RUNNING;
    }
    break;
  }
}
}
