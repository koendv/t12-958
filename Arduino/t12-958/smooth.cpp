#include "smooth.h"

// for easy calculation, choose beta a power of two.
static const uint32_t beta = 8;

int32_t smooth::data(int32_t x)
{
  if (count < beta) {
    // average
    y += x;
    count++;
    return (y / count);
  }
  // smooth
  y = y + x - y / beta;
  return (y / beta);
}


void smooth::reset()
{
  count = 0;
  y = 0;
}
