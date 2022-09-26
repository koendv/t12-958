#ifndef _CLOCK_H_
#define	_CLOCK_H_

/*
 * DS1302Z real-time clock
 *
 * - uses external circuit to charge battery; set trickle charger to 0.
 * - ds1302 clone needs ce toggle between commands
 *
 */

#include <Arduino.h>
#include <DS1302.h>
#include "pins.h"

namespace clock {
extern DS1302 rtc;
extern void setup();
extern void setTime(int hr, int min, int sec, int day, int month, int yr);
extern void getTime(int& hr, int& min, int& sec, int& day, int& month, int& yr);
extern void print();
}
#endif
