/*
 * Rotary encoder library for Arduino.
 */

#ifndef _ENCODER_H_
#define	_ENCODER_H_

#include "Arduino.h"

// Enable this to emit codes twice per step.
//#define HALF_STEP

// Enable weak pullups
#define	ENABLE_PULLUPS

// Values returned by 'process'
// No complete step yet.
#define	DIR_NONE	0x0
// Clockwise step.
#define	DIR_CW		0x10
// Anti-clockwise step.
#define	DIR_CCW		0x20

class Encoder
{
public:
  Encoder(uint8_t, uint8_t);
  int32_t read();
  int32_t readAndReset();
  void write(int32_t);
  void update();

private:
  uint32_t position;
  uint8_t state;
  uint8_t pin1;
  uint8_t pin2;
};

#endif
