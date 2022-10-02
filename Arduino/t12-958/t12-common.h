#ifndef _T12_COMMON_H
#define _T12_COMMON_H

// number of times to run pid algorithm per second
#define PID_HZ 5

/* console on RTT */
#include <RTTStream.h>
extern RTTStream Serial;

#if 0
/* console on UART */
#include <Serial.h>
HWSerial Serial;
#endif

#include <T12Encoder.h>
extern T12Encoder encoder;

#endif
