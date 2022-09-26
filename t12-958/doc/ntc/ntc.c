#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// thermistor NTC MF58-103-3450B with 4k7 pullup
bool pull_up = true;

// thermistor MF58 10K 3950B with 4.7K pullup
// input: adc reading 0..4095
// output: degree C * 10
// for temperature between -30.0 C .. 110.0 C

// fixed point arithmetic, scale factor 4096

int32_t NTCVoltageToTemp_x10(int32_t adc)
{
  const int32_t adc25 = 2786; // reading at 25 degree C, =10/(4.7+10)*4096
  int32_t temp_x10;

  if (!pull_up) {
    adc = 4095-adc;             // pull-down
  }
  if (adc >= adc25) {
    // temperature below 25 C
    int32_t x = adc-adc25;
    int32_t y = (((134764*x/4096-56037)*x/4096+8711)*x/4096+700)*x;
    temp_x10 = 250-y/4096;
  } else {
    // temperature above 25 C
    int32_t x = adc25-adc;
    int32_t y = (((9504*x/4096-6730)*x/4096+1677)*x/4096+876)*x;
    temp_x10 = 250+y/4096;
  }
  return (temp_x10);
}

int main()
{
  for (int adc = 1; adc < 4096; adc++) {
    uint32_t t_x10 = NTCVoltageToTemp_x10(adc);
    printf("%d %d\n", adc, t_x10);
  }
}

