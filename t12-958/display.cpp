#include "t12-common.h"
#include "main_screen.h"
#include "menu_screen.h"
#include "display.h"
#include "icons.h"
#include "fonts.h"
#include "settings.h"

using namespace main_screen;

namespace display {
const char *degrees = "\260C";

uint32_t displayHeight;
uint32_t displayWidth;
uint32_t ambTempXpos;
uint32_t setPointXpos;
uint32_t powerBarYpos;
uint32_t charHeight;
uint32_t tempXPos;
uint32_t tempYPos;
const uint32_t shakeXPos = 49;
const uint32_t warningXPos = 38;
uint32_t ironXPos;
uint32_t ironYPos;
uint32_t x_markXPos;
uint32_t x_markYPos;

/* saved variables to detect display change */
bool force_redraw;
gui_enum saved_gui_state;
uint32_t saved_temp_setpoint;
uint32_t saved_vsupply_x10;
uint32_t saved_ambient_temp_x10;
uint32_t saved_tip_temp_x10;
bool saved_flag_no_iron;
bool saved_flag_shake;
bool saved_flag_warning;
uint32_t saved_power_bar;
bool brightness_changed = false;

static void draw_icons()
{
  u8g2.setFont(u8g2_font_small);
  u8g2.setDrawColor(WHITE);

  /* power supply voltage */
  u8g2.drawXBMP(0, 0, voltXBM[0], voltXBM[1], &voltXBM[2]);
  u8g2.setCursor(voltXBM[0]+2, charHeight-1);
  u8g2.print(vsupply_x10/10);
  u8g2.print('.');
  u8g2.print(vsupply_x10%10);
  u8g2.print('V');

  /* ambient temperature */
  u8g2.drawXBMP(ambTempXpos, 0, tempXBM[0], tempXBM[1], &tempXBM[2]);
  u8g2.setCursor(ambTempXpos+tempXBM[0]+2, charHeight-1);
  u8g2.print(ambient_temp_x10/10);
  u8g2.print('.');
  u8g2.print(ambient_temp_x10%10);
  u8g2.print(degrees);

#if 0
  /* future use */
  u8g2.setCursor(0, displayHeight-1);
  u8g2.print("XXX");
#endif

  /* setpoint */
  u8g2.setCursor(setPointXpos, charHeight-1);
  switch (gui_state)
  {
  case STATE_BOOST:
    u8g2.print("BOOST");
    break;

  case STATE_STANDBY:
    u8g2.print("STDBY");
    break;

  case STATE_SLEEP:
    u8g2.print("SLEEP");
    break;

  default:
    u8g2.print(temp_setpoint);
    u8g2.print(degrees);
    break;
  }

  /* shake icon */
  if (flag_shake) {
    u8g2.drawXBMP(shakeXPos, displayHeight-shakeXBM[1], shakeXBM[0], shakeXBM[1], &shakeXBM[2]);
  }

  /* warning icon */
  if (flag_warning) {
    u8g2.drawXBMP(warningXPos, displayHeight-warningXBM[1], warningXBM[0], warningXBM[1], &warningXBM[2]);
  }

  /* power bar */
  u8g2.drawRFrame(displayWidth-power_bar-4, displayHeight - 9, power_bar+4, 9, 2);

  return;
}


void display_temp()
{
  u8g2.clearBuffer();
  draw_icons();

  /* iron temperature */
  u8g2.setFont(u8g2_font_numbers);
  u8g2.setCursor(tempXPos, tempYPos);
  u8g2.print(tip_temp_x10/10);
  u8g2.print(degrees);
  u8g2.setFont(u8g2_font_small);

  u8g2.sendBuffer();
}


void display_setpoint()
{
  u8g2.clearBuffer();
  draw_icons();

  /* setpoint */
  u8g2.setFont(u8g2_font_numbers);
  u8g2.drawRBox(0, charHeight+2, displayWidth, u8g2.getMaxCharHeight()+2, 8);
  u8g2.setCursor(tempXPos, tempYPos);
  u8g2.setDrawColor(BLACK);
  u8g2.print(temp_setpoint);
  u8g2.print(degrees);
  u8g2.setDrawColor(WHITE);
  u8g2.setFont(u8g2_font_small);

  u8g2.sendBuffer();
}


void display_no_iron()
{
  u8g2.clearBuffer();
  draw_icons();
  u8g2.drawXBM(ironXPos, ironYPos, ironXBM[0], ironXBM[1], &ironXBM[2]);                // no iron detected
  u8g2.drawXBM(x_markXPos, x_markYPos, x_markXBM[0], x_markXBM[1], &x_markXBM[2]);
  u8g2.sendBuffer();
}


void display_sleep()
{
  /* to avoid skin burns, display temperature until iron has cooled down */
  if (!brightness_changed && (flag_no_iron || (tip_temp_x10 < 450))) {
    /* dim screen */
    u8g2.setContrast(0);
    brightness_changed = true;
  }
  if (flag_no_iron) {
    display_no_iron();
  } else{
    display_temp();
  }
}


void setup()
{
  displayHeight = u8g2.getHeight();
  displayWidth = u8g2.getWidth();

  // we have booted - print smiley
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_emoticons21_tr);
  u8g2.setCursor((displayWidth-u8g2.getMaxCharWidth())/2, (displayHeight+u8g2.getMaxCharHeight())/2);
  u8g2.print((char)0x21); // smiley
  u8g2.sendBuffer();

  force_redraw = true;
  /* width in pixels of fields */
  const uint32_t fieldWidth = 41;
  /* space between top row fields */
  uint32_t space;

  space = (displayWidth - 3*fieldWidth)/2;

  u8g2.setFont(u8g2_font_small);
  charHeight = u8g2.getMaxCharHeight();

  ambTempXpos = displayWidth - fieldWidth;
  setPointXpos = fieldWidth + space;

  u8g2.setFont(u8g2_font_numbers);
  tempXPos = (displayWidth - u8g2.getStrWidth("888") - u8g2.getStrWidth(degrees))/2;
  tempYPos = charHeight + u8g2.getMaxCharHeight();
  u8g2.setFont(u8g2_font_small);

  ironXPos = (displayWidth-ironXBM[0]-x_markXBM[0]-5)/2;
  ironYPos = (displayHeight-ironXBM[1])/2;

  x_markXPos = ironXPos + ironXBM[0]+5;
  x_markYPos = (displayHeight-x_markXBM[1])/2;

  u8g2.setContrast(settings.brightness);
}


// XXX this could be improved by drawing only that which has changed.

void loop()
{
  bool display_changed =
   (saved_gui_state != gui_state) ||
   (saved_temp_setpoint != temp_setpoint) ||
   (saved_vsupply_x10 != vsupply_x10) ||
   (saved_ambient_temp_x10 != ambient_temp_x10) ||
   (saved_tip_temp_x10 != tip_temp_x10) ||
   (saved_flag_no_iron != flag_no_iron) ||
   (saved_flag_shake != flag_shake) ||
   (saved_flag_warning != flag_warning) ||
   (saved_power_bar != power_bar);


  if (display_changed || force_redraw) {
    switch (gui_state)
    {
    case STATE_RUNNING:
    case STATE_BOOST:
    case STATE_STANDBY:
      if (brightness_changed) {
        u8g2.setContrast(settings.brightness);
        brightness_changed = false;
      }
      if (flag_no_iron) {
        display_no_iron();
      } else{
        display_temp();
      }
      break;

    case STATE_SETPOINT:
      if (brightness_changed) {
        u8g2.setContrast(settings.brightness);
        brightness_changed = false;
      }
      display_setpoint();
      break;

    case STATE_SLEEP:
      display_sleep();
      break;

    default:
      break;
    }
    force_redraw = false;
    saved_gui_state = gui_state;
    saved_temp_setpoint = temp_setpoint;
    saved_vsupply_x10 = vsupply_x10;
    saved_ambient_temp_x10 = ambient_temp_x10;
    saved_tip_temp_x10 = tip_temp_x10;
    saved_flag_no_iron = flag_no_iron;
    saved_flag_shake = flag_shake;
    saved_flag_warning = flag_warning;
    saved_power_bar = power_bar;
  }
}
}
