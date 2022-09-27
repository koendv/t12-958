#include <U8g2lib.h>
#include <SPI.h>
#include <menu.h>
#include <menuIO/keyIn.h>
//#include <menuIO/encoderIn.h>
// use T12 encoder instead of ArduinoMenu encoder
#include "encoderIn.h"
#include <menuIO/serialIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/u8g2Out.h>
#include <menuIO/serialOut.h>
#include <CheckDS18B20.h>
#include "menu_screen.h"
#include "t12-common.h"
#include "pins.h"
#include "buzzer.h"
#include "clock.h"
#include "ds18b20_temp.h"
#include "icons.h"
#include "fonts.h"
#include "free_ram.h"
#include "settings.h"

namespace menu_screen {
#define	fontName	u8g2_font_7x13_mf
#define	fontX		7
#define	fontY		16
#define	offsetX		0
#define	offsetY		0
#define	U8_Width	128
#define	U8_Height	64

// define menu colors
// each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
// this is a monochromatic color table
const colorDef<uint8_t> colors[6] MEMMODE = {
  { { 0, 0 }, { 0, 1, 1 } },    //bgColor
  { { 1, 1 }, { 1, 0, 0 } },    //fgColor
  { { 1, 1 }, { 1, 0, 0 } },    //valColor
  { { 1, 1 }, { 1, 0, 0 } },    //unitColor
  { { 0, 1 }, { 0, 0, 1 } },    //cursorColor
  { { 1, 1 }, { 1, 0, 0 } },    //titleColor
};

static char * const alphaNum =
 " 0123456789.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,\\|!\"#$%&/()=?~*^+-{}[]€";
static char * const alphaNumMask[] = { alphaNum };

// use "showEvent" to trace menu system events like this:
// FIELD(test, "temp", "°C", 0, 100, 10, 1, showEvent, anyEvent, noStyle),

void showPath(navRoot& root)
{
  Serial.print("nav level:");
  Serial.print(root.level);
  Serial.print(" path:[");
  for (int n = 0; n <= root.level; n++)
  {
    Serial.print(n?",":"");
    Serial.print(root.path[n].sel);
  }
  Serial.println("]");
}


result showEvent(eventMask e, navNode& nav, prompt& item)
{
  Serial.println();
  Serial.print("Event for target: 0x");
  Serial.println((long)nav.target, HEX);
  showPath(*nav.root);
  Serial.print(e);
  switch (e)
  {
  case noEvent:  //just ignore all stuff
    Serial.println(" noEvent");
    break;

  case activateEvent:  //this item is about to be active (system event)
    Serial.println(" activateEvent");
    break;

  case enterEvent:  //entering navigation level (this menu is now active)
    Serial.println(" enterEvent");
    break;

  case exitEvent:  //leaving navigation level
    Serial.println(" exitEvent");
    break;

  case returnEvent:  //TODO:entering previous level (return)
    Serial.println(" returnEvent");
    break;

  case focusEvent:  //element just gained focus
    Serial.println(" focusEvent");
    break;

  case blurEvent:  //element about to lose focus
    Serial.println(" blurEvent");
    break;

  case selFocusEvent:  //child just gained focus
    Serial.println(" selFocusEvent");
    break;

  case selBlurEvent:  //child about to lose focus
    Serial.println(" selBlurEvent");
    break;

  case updateEvent:  //Field value has been updated
    Serial.println(" updateEvent");
    break;

  case anyEvent:
    Serial.println(" anyEvent");
    break;
  }
  return (proceed);
}


//custom field print for fixed point numbers

template<typename T>
class FPx10 : public menuField<T> {
public:
  using menuField<T>::menuField;
  Used printTo(navRoot& root, bool sel, menuOut& out, idx_t idx, idx_t len, idx_t panelNr = 0) override
  {
    menuField<T>::reflex = menuField<T>::target();

    prompt::printTo(root, sel, out, idx, len);
    bool ed = this == root.navFocus;

    out.print((root.navFocus == this && sel)?(menuField<T>::tunning?'>':':'):' ');
    out.setColor(valColor, sel, menuField<T>::enabled, ed);
    out.print(menuField<T>::reflex / 10);
    out.print('.');
    if (menuField<T>::reflex > 0) {
      out.print(menuField<T>::reflex % 10);
    }else{
      out.print(-menuField<T>::reflex % 10);
    }
    out.setColor(unitColor, sel, menuField<T>::enabled, ed);
    print_P(out, menuField<T>::units(), len);
    return (len);
  }
};

// Brightness

static result onDisplayBrightness()
{
  u8g2.setContrast(settings.brightness);
  return (proceed);
}


// Buzzer

CHOOSE(settings.buzzer_enabled, choose_buzzer, "Buzzer", doNothing, noEvent, noStyle,
 VALUE("ON", true, doNothing, noEvent),
 VALUE("OFF", false, doNothing, noEvent)
 );

// Iron

MENU(iron_menu, "Iron", doNothing, anyEvent, noStyle,
 FIELD(settings.standby_time_minutes, "Standby time", "min", 0, 30, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.standby_temp, "Standby temp", "°C", min_temp, max_temp, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.sleep_time_minutes, "Sleep delay", "min", 0, 30, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.boost_time_minutes, "Boost time", "min", 0, 30, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.boost_temp, "Boost temp", "°C", min_temp, max_temp, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

// Cold end

CHOOSE(settings.tempsensor, tempsensorMenu, "Sensor", doNothing, noEvent, noStyle,
 VALUE("ONEWIRE", DS18B20_TEMPSENSOR, doNothing, noEvent),
 VALUE("CPU", CPU_TEMPSENSOR, doNothing, noEvent),
 VALUE("NTC", NTC_TEMPSENSOR, doNothing, noEvent),
 VALUE("NONE", NO_TEMPSENSOR, doNothing, noEvent)
 );


/* print DS18B20 family */
static char ds18b20_fname[20] = "?";

static result onDS18B20Family()
{
  delay(100); // allow some time between last read from ds18b20
  strlcpy(ds18b20_fname, CheckDS18B20::ds18b20_name(&onewire, 0), sizeof(ds18b20_fname));
  return (proceed);
}


MENU(ds18b20Menu, "Check ONEWIRE", onDS18B20Family, enterEvent, noStyle,
 EDIT("Family", ds18b20_fname, alphaNumMask, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

MENU(cold_end_menu, "Cold end", doNothing, noEvent, noStyle,
 SUBMENU(tempsensorMenu),
 SUBMENU(ds18b20Menu),
 EXIT("Back")
 );

CHOOSE(settings.poweron, poweronMenu, "Power On", doNothing, noEvent, noStyle,
 VALUE("RUNNING", POWERON_RUNNING, doNothing, noEvent),
 VALUE("STANDBY", POWERON_STANDBY, doNothing, noEvent),
 VALUE("SLEEP", POWERON_SLEEP, doNothing, noEvent)
 );

MENU(system_menu, "System", doNothing, anyEvent, noStyle,
 SUBMENU(poweronMenu),
 SUBMENU(choose_buzzer),
 FIELD(settings.temp_step, "Temp step", "", -50, 50, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.brightness, "Brightness", "", 0, 255, 32, 1, onDisplayBrightness, enterEvent, noStyle),
 FIELD(settings.long_press_millis, "Long press", "ms", 200, 2000, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

// Calibration

MENU(ntcCalibration1Menu, "NTC cal 1", doNothing, anyEvent, noStyle,
 FIELD(ntc_adc, "NTC ADC", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, ambient_temp_x10, "NTC TEMP", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.cal_ntc_in1, "NTC IN1", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.cal_ntc_out1, "NTC ADJUST1", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );


MENU(ntcCalibration2Menu, "NTC cal 2", doNothing, anyEvent, noStyle,
 FIELD(ntc_adc, "NTC ADC", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, ambient_temp_x10, "NTC TEMP", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.cal_ntc_in2, "NTC IN2", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.cal_ntc_out2, "NTC ADJUST2", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );


MENU(tipCalibration1Menu, "Tip cal at 0°C", doNothing, anyEvent, noStyle,
 altFIELD(FPx10, tip_temp_x10, "Tip", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, thermocouple_temp_x10, "TC", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 FIELD(tip_adc, "Tip ADC", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.cal_adc_at_0, "Cal at 0°C", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

MENU(tipCalibration2Menu, "Tip cal at 250°C", doNothing, anyEvent, noStyle,
 altFIELD(FPx10, tip_temp_x10, "Tip", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, thermocouple_temp_x10, "TC", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 FIELD(tip_adc, "Tip ADC", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.cal_adc_at_250, "Cal at 250°C", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

MENU(tipCalibration3Menu, "Tip cal at 400°C", doNothing, anyEvent, noStyle,
 altFIELD(FPx10, tip_temp_x10, "Tip", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, thermocouple_temp_x10, "TC", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 FIELD(tip_adc, "Tip ADC", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 FIELD(settings.cal_adc_at_400, "Cal at 400°C", "", 0, 4095, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

MENU(calibrationMenu, "Calibration", doNothing, anyEvent, noStyle,
 SUBMENU(ntcCalibration1Menu),
 SUBMENU(ntcCalibration2Menu),
 SUBMENU(tipCalibration1Menu),
 SUBMENU(tipCalibration2Menu),
 SUBMENU(tipCalibration3Menu),
 EXIT("Back")
 );

// PID controller


MENU(controllerMenu, "Controller", doNothing, anyEvent, noStyle,
 altFIELD(FPx10, settings.Kp_x10, "Kp", "", 0, 1000, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, settings.Ki_x10, "Ki", "", 0, 1000, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, settings.Kd_x10, "Kd", "", 0, 1000, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

// date and time

static int hour = 0, minutes = 0, seconds = 0, day = 1, month = 1, year = 2025;

static result onEnterDateTime()
{
  clock::getTime(hour, minutes, seconds, day, month, year);
  return (proceed);
}


static result onSetTime()
{
  clock::setTime(hour, minutes, seconds, day, month, year);
  buzzer::short_beep();
  return (proceed);
}


MENU(datetimeMenu, "Date and Time", onEnterDateTime, enterEvent, noStyle,
 FIELD(hour, "Hour", "", 0, 23, 1, 1, doNothing, noEvent, wrapStyle),
 FIELD(minutes, "Minutes", "", 0, 59, 1, 1, doNothing, noEvent, wrapStyle),
 FIELD(seconds, "Seconds", "", 0, 59, 1, 1, doNothing, noEvent, wrapStyle),
 FIELD(day, "Day", "", 0, 59, 1, 1, doNothing, noEvent, wrapStyle),
 FIELD(month, "Month", "", 1, 12, 1, 1, doNothing, noEvent, wrapStyle),
 FIELD(year, "Year", "", 2022, 2040, 1, 1, doNothing, noEvent, noStyle),
 OP("Set time", onSetTime, enterEvent),
 EXIT("Back")
 );


// saving settings in flash

static result onStoreSettings()
{
  store_settings();
  buzzer::short_beep();
  return (proceed);
}


static result onRecallSettings()
{
  recall_settings();
  buzzer::short_beep();
  return (proceed);
}


static result onResetSettings()
{
  reset_settings();
  buzzer::short_beep();
  return (proceed);
}


static result onPrintSettings()
{
  print_settings();
  buzzer::short_beep();
  return (proceed);
}


MENU(resetMenu, "Reset", doNothing, anyEvent, noStyle,
 OP("Confirm", onResetSettings, enterEvent),
 EXIT("Back")
 );


MENU(settingsMenu, "Save and Recall", doNothing, anyEvent, noStyle,
 OP("Save", onStoreSettings, enterEvent),
 OP("Recall", onRecallSettings, enterEvent),
 OP("Print", onPrintSettings, enterEvent),
 SUBMENU(resetMenu),
 EXIT("Back")
 );


// sysinfo menu

static uint32_t ram_free_bytes	= 0;

static result onSysInfo()
{
  ram_free_bytes = free_ram();
  return (proceed);
}


MENU(overview_menu, "Overview", doNothing, anyEvent, noStyle,
 altFIELD(FPx10, tip_temp_x10, "Tip temp", "°C", min_temp, max_temp, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, ambient_temp_x10, "Ambient temp", "°C", -200, 1000, 10, 1, doNothing, noEvent, noStyle),
 altFIELD(FPx10, vsupply_x10, "Power supply", " V", min_temp, max_temp, 10, 1, doNothing, noEvent, noStyle),
 FIELD(iron_pwm, "PWM", "", 0, 32768, 10, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );


MENU(sysinfoMenu, "System info", onSysInfo, enterEvent, noStyle,
 OP("SW: " __DATE__, doNothing, noEvent),
 OP("HW: " BOARD_NAME, doNothing, noEvent),
 FIELD(ram_free_bytes, "RAM", " bytes free", 0, 0xffff, 32, 1, doNothing, noEvent, noStyle),
 EXIT("Back")
 );

// main menu

static result onExitMenu()
{
  menu_running = false;
  buzzer::short_beep();
  return (proceed);
}


MENU(mainMenu, "t12-958", doNothing, noEvent, noStyle,
 SUBMENU(iron_menu),
 SUBMENU(cold_end_menu),
 SUBMENU(system_menu),
 SUBMENU(calibrationMenu),
 SUBMENU(controllerMenu),
 SUBMENU(datetimeMenu),
 SUBMENU(sysinfoMenu),
 SUBMENU(settingsMenu),
 SUBMENU(overview_menu),
 OP("Back", onExitMenu, enterEvent)
 );

#define	MAX_DEPTH    3

// rotary encoder
//encoderIn<ENC_R, ENC_R> encoder;                                // simple quad encoder driver
//encoderInStream<ENC_R, ENC_R> encStream(encoder, 1);            // simple quad encoder fake Stream
encoderInStream encStream(1);                                     // use T12Encoder instead of ArduinoMenu encoderIn

// encoder button is keyboard with only one key
keyMap encBtn_map[]		= { { ENC_SW, defaultNavCodes[enterCmd].ch } };
keyIn<1> encButton(encBtn_map);//1 is the number of keys

// console input
serialIn serial(Serial);

MENU_INPUTS(in, &encStream, &encButton, &serial);

MENU_OUTPUTS(out, MAX_DEPTH,
 SERIAL_OUT(Serial),
 U8G2_OUT(u8g2, colors, fontX, fontY, offsetX, offsetY, { 0, 0, U8_Width/fontX, U8_Height/fontY }));

// menu top level
NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

void force_redraw()
{
  mainMenu.dirty = true;        // force redraw
  nav.idleOff();
  nav.reset();
  encStream.flush();
}


void setup()
{
  u8g2.setDrawColor(WHITE);
  u8g2.setFont(fontName);
  nav.showTitle = false;
  nav.timeOut = 30;
  nav.canExit = false;
  // read only variables (e.g. measured temperature)
  overview_menu[0].disable();
  overview_menu[1].disable();
  overview_menu[2].disable();
  // ds18b20 family name
  ds18b20Menu[0].disable();
  // calibration menu
  ntcCalibration1Menu[0].disable();             // calculated temperature
  ntcCalibration1Menu[1].disable();             // adc count
  ntcCalibration2Menu[0].disable();
  ntcCalibration2Menu[1].disable();
  tipCalibration1Menu[0].disable();
  tipCalibration1Menu[1].disable();
  tipCalibration2Menu[0].disable();
  tipCalibration2Menu[1].disable();
  tipCalibration3Menu[0].disable();
  tipCalibration3Menu[1].disable();
  // sysinfo menu
  sysinfoMenu[2].disable();                     // free ram
}


void loop()
{
  // https://github.com/neu-rah/ArduinoMenu/wiki/Idling
  nav.doInput();
  if (nav.changed(0)) {
    u8g2.firstPage();
    do
    {
      nav.doOutput();
    } while (u8g2.nextPage());
  }
}
}
