#include <mm32_reg.h>
#include "t12-common.h"
#include "pins.h"
#include "button.h"
#include "settings.h"

namespace button {
bool button_pressed = false;
bool button_click = false;
bool button_longpress = false;
uint32_t button_press_millis = 0;
uint32_t button_release_millis = 0;

extern "C" {
void irq_button()
{
  uint8_t pin_state = (GPIOA->IDR >> 15) & 0x1;

  if (pin_state) {
    button_pressed = false;
    button_release_millis = millis();
    if (button_release_millis - button_press_millis > settings.long_press_millis) {
      button_longpress = true;
    }else {
      button_click = true;
    }
  } else {
    button_pressed = true;
    button_press_millis = millis();
  }
}
}

void setup()
{
  button_pressed = false;
  button_click = false;
  button_longpress = false;
  button_press_millis = 0;
  button_release_millis = 0;
  pinMode(ENC_SW, INPUT);
  attachInterrupt(ENC_SW, irq_button, CHANGE);
}
}
