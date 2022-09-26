/********************
 * DS18B20 in ArduinoMenu
 *
 * Needs the following libraries:
 * - Arduino-Temperature-Control-Library
 * - ArduinoMenu_library
 * - CheckDS18B20
 *
 * output: Serial
 * input: Serial
 *
 * This example uses serial; but lcd and oled works, too.
 */

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <CheckDS18B20.h>
#include <DallasTemperature.h>

// define sensor pin
#define	PIN_ONEWIRE    PB6

static OneWire onewire(PIN_ONEWIRE);
static DallasTemperature sensors(&onewire);

static DeviceAddress addr;

float temp = 0;

const char *constMEM alphaNum MEMMODE = " 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,\\|!\"#$%&/()=?~*^+-{}[]€";
const char *constMEM alphaNumMask[] MEMMODE = { alphaNum };

using namespace Menu;

/* print DS18B20 family */
char ds18b20_fname[20] = "?";

result onDS18B20Family()
{
  #ifdef __AVR__
  strlcpy_P(ds18b20_fname, (const char *)CheckDS18B20::ds18b20_name(&onewire, 0), sizeof(ds18b20_fname));
  #else
  strlcpy(ds18b20_fname, CheckDS18B20::ds18b20_name(&onewire, 0), sizeof(ds18b20_fname));
  #endif
  return (proceed);
}


MENU(ds18b20Menu, "Check DS18B20", onDS18B20Family, enterEvent, noStyle,
 EDIT("Family", ds18b20_fname, alphaNumMask, doNothing, noEvent, noStyle),
 EXIT("<Back")
 );


MENU(mainMenu, "Main menu", doNothing, noEvent, noStyle
 , FIELD(temp, "Temp", "°C", 0, 100, 10, 1, doNothing, noEvent, noStyle)
 , SUBMENU(ds18b20Menu)
 , EXIT("<Back")
 );

#define	MAX_DEPTH    2

MENU_OUTPUTS(out, MAX_DEPTH
 , SERIAL_OUT(Serial)
 , NONE//must have 2 items at least
 );

serialIn serial(Serial);

NAVROOT(nav, mainMenu, MAX_DEPTH, serial, out);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Serial.println("Dallas test");

  sensors.begin();
  if (!sensors.getAddress(addr, 0)) {
    Serial.println("ds18b20 not found. stop.");
    while (1)
    {
    }
  }
  sensors.setResolution(addr, 12);

  nav.canExit = false;

  // DS18B20 family is read-only
  ds18b20Menu[0].disable();
  // ambient temperature is read-only
  mainMenu[0].disable();
}

/* average temperature and round to 0.1 degree */
float smooth_temp;
bool first = true;

float smooth(float new_temp)
{
  if (first) {
    smooth_temp = new_temp * 16;
    first = false;
  }
  smooth_temp = smooth_temp - smooth_temp/16 + new_temp;
  float y = (int)(smooth_temp*10/16)/10.0;
  return (y);
}


void loop()
{
  nav.poll();
  sensors.requestTemperatures();
  temp = smooth(sensors.getTempC(addr));
  delay(100);//simulate a delay when other tasks are done
}
