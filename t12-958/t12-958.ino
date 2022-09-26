#include <RTTStream.h>
#include <T12Encoder.h>
#include <U8g2lib.h>
#include <TaskScheduler.h>
#include "clock.h"
#include "buzzer.h"
#include "display.h"
#include "button.h"
#include "measure.h"
#include "main_screen.h"
#include "menu_screen.h"
#include "gui.h"
#include "iron.h"
#include "settings.h"
#include "free_ram.h"
#include "t12-common.h"

// console
RTTStream Serial;

// scheduler
Scheduler ts;

// rotary encoder
T12Encoder encoder;

// SH1106 128x64 oled
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, DISPLAY_CS, DISPLAY_DC, DISPLAY_RST);

Task task_gui(20, TASK_FOREVER, &gui::TaskGUI, &ts);
Task task_adc(100, TASK_FOREVER, &measure::TaskADC, &ts);
Task tBuzzer(1, TASK_ONCE, NULL, &ts);
Task task_iron(1000/PID_HZ, TASK_FOREVER, &iron::TaskIron, &ts);

void setup()
{
  //Serial.blockUpBufferFull();
  Serial.begin(115200);
  Serial.println();
  Serial.println("t12-958");
  recall_settings();
  u8g2.begin();
  display::setup();
  button::setup();
  clock::setup();
  clock::print();
  buzzer::setup();
  menu_screen::setup();
  main_screen::setup();
  gui::setup();
  measure::setup();
  iron::setup();
  ts.enableAll();
  Serial.print(free_ram());
  Serial.println(" bytes free");
  Serial.println("use arrow keys");
  buzzer::short_beep();
}


void loop()
{
  ts.execute();
}
