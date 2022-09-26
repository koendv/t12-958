/* Rotary encoder handler for T12 soldering station */

#include "Arduino.h"
#include "WInterrupts.h"
#include "T12Encoder.h"

extern "C" {
#include "t12encoder.h"
}

T12Encoder::T12Encoder()
{
  // Set pins to input.
  pinMode(PB3, INPUT);
  pinMode(PB4, INPUT);
  zero_position = 0;
  // enable interrupts
  //t12enc_init();
  attachInterrupt(digitalPinToInterrupt(PB3), t12enc_update, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PB4), t12enc_update, CHANGE);
}


int32_t T12Encoder::read()
{
  return (t12enc_position-zero_position);
}


int32_t T12Encoder::readAndReset()
{
  int32_t pos = t12enc_position;
  int32_t ret = pos-zero_position;

  zero_position = pos;
  return (ret);
}


void T12Encoder::write(int32_t p)
{
  zero_position = t12enc_position-p;
}
