/*
 * Rotary encoder library for Arduino.
 */

#ifndef _T12ENCODER_H_
#define	_T12ENCODER_H_

#include "Arduino.h"

class T12Encoder
{
public:
  T12Encoder();
  int32_t read();
  int32_t readAndReset();
  void write(int32_t);

private:
  int32_t zero_position;
};

#endif
