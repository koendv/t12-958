#include "pins.h"
#include "adcdma.h"

/*
 * PWM, ADC, DMA, IRQ
 *
 * ADC1 samples when triggered by TIMER3, CHANNEL4 (pin PC9 falling)
 * set sample moment with e.g. analogWrite(PC9, 32000, PWM_TIM_3);
 * ADC1 samples 4 channels:
 * - NTC
 * - VIN 24V power supply
 * - TIP soldering iron thermocouple
 * - cpu temperature sensor
 * DMA1, CHANNEL1 writes the adc samples to array adc_value[]
 * When adc samples are ready, an interrupt is sent
 * dma1_channel1_irq() is called
 */

#include "hal_adc.h"
#include "hal_dma.h"
#include "hal_rcc.h"
#include "hal_gpio.h"
#include "mm32_reg.h"
#include "isr.h"
#include "SEGGER_RTT.h"

// array for adc results.
// in order: NTC, VIN, TIP, ATEMP, AVREF
uint16_t adc_value[ADC_CHANNELS*ADC_SAMPLES];

#if 0
void dma1_channel1_irq(void)
{
  SEGGER_RTT_WriteString(0, "]");
  gpio_toggle(B7);
}
#endif

static void t12_dma_init(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStruct;

  //SEGGER_RTT_WriteString(0, "dma init\n");
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel1);
  //DMA transfer peripheral address
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->ADDATA);
  //DMA transfer memory address
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_value;
  //DMA transfer direction from peripheral to memory
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  //DMA cache size
  DMA_InitStructure.DMA_BufferSize = sizeof(adc_value)/sizeof(adc_value[0]);
  //After receiving the data, the peripheral address is forbidden to move backward
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  //After receiving the data, the memory address is shifted backward
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  //Define the peripheral data width to 16 bits
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  //Define the memory data width to 16 bits
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  //Cycle conversion mode
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  //DMA priority is high
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);

  //DMA interrupt initialization
  dmaisr_ptr[0] = &dma1_channel1_irq;
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
  NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}


static void t12_adc_init()
{
  //SEGGER_RTT_WriteString(0, "adc init\n");
  ADC_InitTypeDef ADC_InitStructure;
  ADC_StructInit(&ADC_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_16;
  ADC_InitStructure.ADC_Mode = ADC_Mode_Single_Period;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC1_ExternalTrigConv_T3_CC4;                        // XXX
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 0, ADC_SampleTime_239_5Cycles);                 // PA5, NTC
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 0, ADC_SampleTime_239_5Cycles);                 // PB0, VIN
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 0, ADC_SampleTime_239_5Cycles);                 // PB1, TIP
  ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 0, ADC_SampleTime_239_5Cycles);        // temp sensor
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 0, ADC_SampleTime_239_5Cycles);           // Internal reference voltage
  ADC_TempSensorCmd(ENABLE);
  ADC_VrefintCmd(ENABLE);
  ADC_DMACmd(ADC1, ENABLE);
  ADC_ExternalTrigConvCmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
}


void adcdma_init()
{
  t12_dma_init();
  t12_adc_init();
}
