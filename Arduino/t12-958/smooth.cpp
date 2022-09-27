#include "smooth.h"

static const uint32_t beta = 8;

int32_t smooth::data(int32_t x)
{
  // y is sum of beta samples
  if (first) {
    y = x * beta;
    first = false;
    return (x);
  }
  y = y + x - y / beta;
  return (y / beta);
}


void smooth::reset()
{
  first = true;
}
