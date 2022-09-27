#include "clock.h"
#include "t12-common.h"


namespace clock {
/* DS1302Z real-time clock */
DS1302 rtc(RTC_CE, RTC_SDA, RTC_SCL);

void setup()
{
  rtc.writeProtect(false);
  rtc.halt(false);
  rtc.writeRegister(0x08, 0xa0); // trickle charger off - uses external circuit to charge battery
  Time t = rtc.time();
  // set time if unreasonable
  if ((t.yr < 2022) || (t.yr > 2040)) {
    Time t_new(2025, 1, 1, 0, 0, 0, Time::kWednesday);
    rtc.time(t_new);
  }
}


int weekday(int year, int month, int day)
/* Calculate day of week in proleptic Gregorian calendar. Sunday == 0. */
{
  int adjustment, mm, yy;

  if (year < 2000) {
    year += 2000;
  }
  adjustment = (14 - month) / 12;
  mm = month + 12 * adjustment - 2;
  yy = year - adjustment;
  return ((day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7);
}


void setTime(int hr, int min, int sec, int day, int month, int yr)
{
  Time::Day dayweek = (Time::Day)(weekday(yr, month, day) + 1);
  Time t_new(yr, month, day, hr, min, sec, dayweek);

  rtc.time(t_new);
}


void getTime(int& hr, int& min, int& sec, int& day, int& month, int& yr)
{
  Time t = rtc.time();

  hr = t.hr;
  min = t.min;
  sec = t.sec;
  day = t.day;
  month = t.mon;
  yr = t.yr;
}


static const char *dayofweek[] = { "Sun", "Mon", "Tue", "Wednes", "Thurs", "Fri", "Satur" };
void print()
{
  Time t = rtc.time();

  Serial.print(dayofweek[(t.day-1)%7]);
  Serial.print("day ");
  Serial.print(t.yr);
  Serial.print('/');
  Serial.print(t.mon);
  Serial.print('/');
  Serial.print(t.date);
  Serial.print(' ');
  if (t.hr < 10) {
    Serial.print('0');
  }
  Serial.print(t.hr);
  Serial.print(':');
  if (t.min < 10) {
    Serial.print('0');
  }
  Serial.print(t.min);
  Serial.print(':');
  if (t.sec < 10) {
    Serial.print('0');
  }
  Serial.println(t.sec);
}
}
