#ifndef _ADCDMA_H_
#define	_ADCDMA_H_

#include <stdint.h>

// number of adc channels
#define	ADC_CHANNELS	4
// number of samples per channel
#define	ADC_SAMPLES	1

// array with sample values
extern uint16_t adc_value[ADC_CHANNELS*ADC_SAMPLES];
// start background adc conversion
extern void adcdma_init();
// irq handler - gets called when adc_value[] full
extern void dma1_channel1_irq(void);

#endif
