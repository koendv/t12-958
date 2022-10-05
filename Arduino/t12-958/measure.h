#ifndef _MEASURE_H_
#define	_MEASURE_H_
namespace measure {
void setup();
int32_t NTCTemp_x10();
int32_t CPUTemp_x10();
int32_t SupplyVoltage_mV();
int32_t TipTemperature_x10();

/* task for task scheduler */
void TaskColdJunction();
void TaskADC();
}
#endif
