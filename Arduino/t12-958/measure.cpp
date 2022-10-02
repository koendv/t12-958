#include <Arduino.h>
#include <DallasTemperature.h>

#include "t12-common.h"
#include "pins.h"
#include "measure.h"
#include "ds18b20_temp.h"
#include "settings.h"
#include "smooth.h"
#include "adcdma.h"

/*
 * adc interface
 * dma1_channel1_irq() gets called when new adc values are available in adc_value[]
 * dma1_channel1_irq() copies adc_value[] to analog_read[]
 * see adcdma_init()
 */

#define	ADC_NTC		0
#define	ADC_VIN		1
#define	ADC_TIP		2
#define	ADC_ATEMP	3

int32_t analog_read[ADC_CHANNELS]; // averaged adc readings
bool adc_flag = false;

extern "C" {
void dma1_channel1_irq(void)
{
  // Serial.print(']');
  // digitalToggle(UART_TX);
  for (int i = 0; i < 4; i++)
  {
    analog_read[i] = adc_value[i];
  }
  adc_flag = true;
}
}

/*
 * note:
 * fast_map() is same as arduino map(), but defined as an inline function
 * so the compiler can replace run-time math by compile-time optimizations
 */
static inline long fast_map(long x, long in_min, long in_max, long out_min, long out_max)
{
  if (in_max == in_min) {
    return (x);
  }
  return ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}


// print comparison of fixed point and floating point versions of NTCVoltageToTemp
// #define PRINT_NTC_COMPARISON

namespace measure {
const int32_t cold_junction_temp_max_x10 = 999;         // 99.9 degree C max
const int32_t cold_junction_temp_min_x10 = -200;        // -20 degree C min
const int32_t tip_adc_no_iron = 4000;                   // thermocouple adc values above 4000 indicate no t12 tip present

static smooth smooth_vsupply_x10;
static smooth smooth_ambient_temp_x10;
static smooth smooth_tip_temp_x10;
static smooth smooth_ntc_adc;

// fixed point arithmetic, scale factor 4096
// curve fitting to manufacturers' data
// difference between polynomial approximation and manufacturers' data
// is less than 1 C for temperature between -30.0 C .. 110.0 C
// see directory doc/ntc

bool pull_up = true;


// thermistor MF58 10K 3950B with 4.7K pullup
// input: adc reading 0..4095
// output: degree C * 10
// specified for temperature between -30.0 C .. 110.0 C

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
  temp_x10 = constrain(temp_x10, -300, 1100); // manufacturer specified temperature range
  return (temp_x10);
}


#ifdef PRINT_NTC_COMPARISON
// floating point arithmetic
// Steinhart-Hart equation

#include <math.h>

float NTCVoltageToTemp_float(int32_t adc)
{
  const float pull_res = 4700;
  const float NTC_res = 10000;
  const float NTC_Beta = 3950;

  if (!pull_up) {
    adc = 4095-adc;             // pull-down
  }
  float temp = (1/((logf(((pull_res * adc) / (4095.0 - adc))/NTC_res)/NTC_Beta) + (1 / (273.15 + 25.000)))) - 273.15;

  return (temp);
}


// prints comparison of floating point and fixed point versions of NTCVoltageToTemp()

void CompareNTCCalculations()
{
  Serial.blockUpBufferFull();
  Serial.println("adc\tfloat\tfixed");
  for (int i = 1; i < 4095; i++)
  {
    Serial.print(i);
    Serial.print('\t');
    Serial.print(NTCVoltageToTemp_float(i));
    Serial.print('\t');
    Serial.println(NTCVoltageToTemp_x10(i));
  }
  Serial.println("---");
  return;
}
#endif


int32_t NTCTemp_x10()
{
  int32_t adc, adc_adjusted;
  int32_t temp_x10;

  adc = analog_read[ADC_NTC];
  ntc_adc = smooth_ntc_adc.data(adc);
  adc_adjusted =
   fast_map(adc, settings.cal_ntc_in1, settings.cal_ntc_in2, settings.cal_ntc_out1, settings.cal_ntc_out2);
  adc_adjusted = constrain(adc_adjusted, 0, 4095);
  temp_x10 = NTCVoltageToTemp_x10(adc_adjusted);

  return (temp_x10);
}


// cpu temperature in tenths of degrees C
int32_t CPUTemp_x10()
{
  /*
   * from DS_MM32SPIN2x_SC.pdf, 5.3.16 温度传感器特性
   * avg_slope = 4.801 mV/°C
   * V25 = 1.451 V
   * linearity +/- 5°C
   *
   * from UM_MM32SPIN2x_p_SC.pdf, 12.7 温度传感器
   * T(◦C) = (VSENSE - V25) / avg_Slope + 25
   * VSENSE = Value * Vdd / 4096
   *
   * Summary:
   * - adc is 0 at -273.2°C
   * - adc is 1800 at 25.0°C
   */

  int32_t adc = analog_read[ADC_ATEMP]; // value in 0..4095
  int32_t temp_x10 = fast_map(adc, 0, 1800, -2732, 250);

  return (temp_x10);
}


int32_t TipTemperature_x10()
{
  int32_t tip_temp_x10;

  tip_adc = analog_read[ADC_TIP];

  flag_no_iron = tip_adc > tip_adc_no_iron;

  if (tip_adc < settings.cal_adc_at_250) {
    thermocouple_temp_x10 = fast_map(tip_adc, settings.cal_adc_at_0, settings.cal_adc_at_250, 0, 2500);
  }else {
    thermocouple_temp_x10 = fast_map(tip_adc, settings.cal_adc_at_250, settings.cal_adc_at_400, 2500, 4000);
  }
  tip_temp_x10 = thermocouple_temp_x10 + ambient_temp_x10;
  return (tip_temp_x10);
}


int32_t SupplyVoltage_mV()
{
  int32_t adc_val = analog_read[ADC_VIN];
  // 3.3V full scale is 4095
  // input is a R1(10K) / R2(1K) voltage divider, ratio 1/11.
  int32_t vsupply = fast_map(adc_val, 0, 4095, 0, 3300 * 11);

  return (vsupply);
}


void TaskADC()
{
  while (!adc_flag) {
    return;
  }
  /* power supply */
  vsupply_x10 = smooth_vsupply_x10.data(SupplyVoltage_mV() / 100);  // in 0.1V

  /* solder iron tip */
  unsmoothed_tip_temp_x10 = TipTemperature_x10();
  tip_temp_x10 = smooth_tip_temp_x10.data(unsmoothed_tip_temp_x10);

  /* cold junction temperature */
  int32_t temp_x10;
  switch (settings.tempsensor)
  {
  case NTC_TEMPSENSOR:
    temp_x10 = NTCTemp_x10();
    break;

  case CPU_TEMPSENSOR:
    temp_x10 = CPUTemp_x10();
    break;

  case DS18B20_TEMPSENSOR:
    temp_x10 = DS18B20Temp_x10();
    break;

  case NO_TEMPSENSOR:
  default:
    temp_x10 = 350;        // default 35.0ºC if no sensors
    break;
  }
  if (temp_x10 != ERROR_TEMP) {
    if (temp_x10 > cold_junction_temp_max_x10) {
      temp_x10 = cold_junction_temp_max_x10;
    }else if (temp_x10 < cold_junction_temp_min_x10) {
      temp_x10 = cold_junction_temp_min_x10;
    }
    ambient_temp_x10 = smooth_ambient_temp_x10.data(temp_x10);
  }
  return;
}


void setup()
{
#ifdef PRINT_NTC_COMPARISON
  CompareNTCCalculations();
#endif
  pinMode(VIN, INPUT_ANALOG);
  pinMode(NTC, INPUT_ANALOG);
  pinMode(TIP, INPUT_ANALOG);
  pinMode(UART_TX, OUTPUT);

  /* because adc and pwm are synchronized, configure adc and pwm at the same time */

  analogReadResolution(12);
  analogSampleTime(ADC_SAMPLE_TIME_239_5);

  analogWriteResolution(15);
  analogWriteFrequency(PID_HZ);                 // pwm frequency and adc sampling frequency are the same
  analogWrite(PWM, 0, PWM_TIM_3);
  analogWrite(PIN_SAMPLE, 32000, PWM_TIM_3);    // sample 5 ms before the next PWM pulse
  delay(201);
  adcdma_init();
}
}
