#ifndef _PINS_H
#define _PINS_H

// RTC DS1302Z real time clock
#define RTC_CE          PC15
#define RTC_SDA         PA4
#define RTC_SCL         PA3

// OLED SH1106 128x64 spi oled
#define DISPLAY_CS      PA10
#define DISPLAY_DC      PA9
#define DISPLAY_RST     PA8
#define DISPLAY_SCL     PB13
#define DISPLAY_SDA     PB15

// rotary encoder with switch
#define ENC_SW          PA15
#define ENC_R           PB4
#define ENC_L           PB3

// uart on pins PB6, PIN_B7.
// these are the pins marked D and F, next to the SWD header.
#define UART_TX         PB6
#define UART_RX         PB7

// PB6, PB7 are also I2C
#define I2C1_SCL        PB6
#define I2C1_SDA        PB7

// DS18B20 temperature sensor
#define ONEWIRE         PB7

// swd on pins PA13, PIN_A14.

// motion sensor in soldering iron
#define WAKE            PA0

// piezo buzzer, digital output
#define BUZ0            PA1
#define BUZ1            PA2

// NTC temperature compensation from soldering iron, analogue input
#define NTC             PA5

// PWM soldering iron heating element, pwm output
#define PWM             PA6

// TIP soldering iron thermocouple, analogue input
#define TIP             PB1

// VIN power supply voltage 1/10, analogue input
#define VIN             PB0

// adc samples when pwm pin PC9 falling
#define PIN_SAMPLE	PC9

#endif
