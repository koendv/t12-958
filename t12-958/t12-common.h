#ifndef _T12_COMMON_H
#define _T12_COMMON_H

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
