#include <RTTStream.h>
#include <T12Encoder.h>
#include <U8g2lib.h>
#include "clock.h"
#include "buzzer.h"
#include "display.h"
#include "button.h"
#include "measure.h"
#include "main_screen.h"
#include "menu_screen.h"
#include "settings.h"
#include "t12-common.h"
#include "gui.h"

/*
 * Two graphical interfaces are used:
 * main_screen uses u8g2
 * menu_screen uses ArduinoMenu
 *
 * This state machine stops main_screen when ArduinoMenu is in use,
 * and stops menu_screen when main_screen is in use.
 */

namespace gui {
enum state_enum {
  STATE_MENU, STATE_TO_GUI, STATE_GUI, STATE_TO_MENU
};
static state_enum state = STATE_GUI;

void setup()
{
  state = STATE_GUI;
}


// clear screen
void cls()
{
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}


void TaskGUI()
{
  switch (state)
  {
  case STATE_GUI:
    // displaying temperature and setpoint in gui
    main_screen::loop();
    display::loop();
    if (Serial.available()) {
      menu_running = true;
    }
    if (menu_running) {
      cls();
      menu_running = true;
      state = STATE_TO_MENU;
    }
    break;

  case STATE_TO_MENU:
    // wait until button released
    if (digitalRead(ENC_SW)) {
      menu_screen::force_redraw();
      state = STATE_MENU;
    }
    break;

  case STATE_MENU:
    // in menu system
    menu_screen::loop();
    if (!menu_running) {
      cls();
      state = STATE_TO_GUI;
    }
    break;

  case STATE_TO_GUI:
    // wait until button released
    if (digitalRead(ENC_SW)) {
      encoder.readAndReset();
      display::force_redraw = true;
      state = STATE_GUI;
    }
    break;

  default:
    state = STATE_GUI;
    break;
  }
}
}
