
The Quecoo t12-958 / KSGER v2.1S is a T12 soldering station using the MM32SPIN27 Arm processor. These are developers' notes. See README.md for users' notes.


## Console

The system console is on RTT.


```
t12-958
Thursday 2022/9/22 06:36:37
first run
3256 bytes free
use arrow keys
[1]>Iron
[2] Cold end
[3] System
[4] Calibration
[5] Controller
[6] Date and Time
[7] System info
[8] Settings
[9] Overview
[-] Back
```
The menu on the console and the menu on the oled screen are the same.

RTT speed depends upon buffer size:

|BUFFER_UP|RTT speed|
|---|---|
|bytes|char/s|
|1024|800000|
|512|450000|
|256|250000|
|128|125000|

(Speed measured with RTTStream SpeedTest.)
To save memory, a buffer size of 256 bytes was used. 

If you wish to have system console on the UART, edit t12-common.h, comment out  RTTStream.h and uncomment Serial.h.

## Menu system

The menu system is [ArduinoMenu](https://github.com/neu-rah/ArduinoMenu), with the following tweaks:

- use interrupt-driven rotary encoder as input (file t12-958/encoderIn.h)
- accept arrow keys as input on the serial console (file ArduinoMenu_library/src/nav.cpp)
- custom field to print fixed point numbers (class FPx10 in t12-958/menu_screen.cpp )

## Graphics

Graphics are [U8G2](https://github.com/olikraus/u8g2) on a SPI 128x64 OLED. A choice was made to keep the graphics from the STM32 project. The processor has changed from STMicroelectronics STM32F103 to MindMotion MM32SPIN27; the SDK has changed from STM32Cube MX to Arduino; but the display looks the same.

## PID controller

The proportional–integral–derivative controller is [FastPID](https://github.com/mike-matera/FastPID), patched to use fixed point Kp, Ki, Kd instead of floating point.

## NTC Temperature sensor

The pcb contains a voltage divider is made with an NTC and a pull-up resistor. The voltage across the NTC resistor is measured with an ADC, and used to calculate temperature.

The [Steinhart-Hart equation](https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation)  gives temperature as a function of NTC resistance. The Steinhart-Hart equation uses logarithm and floating point division. The MM32SPIN27 does not have hardware floating-point FPU, so using the Steinhart-Hart equation is slow and impractical for this processor.

Instead, curve fitting has been used to map the adc reading to a temperature. The function ``NTCVoltageToTemp_x10`` in file ``t12-958/measure.cpp``.

The NTC datasheet shows an inflection point at 25C temperature, 10K resistance. It is convenient to use two polynomials, one polynomial for temperatures above 25C, another polynomial for temperatures below 25C.

The polynomial is translated so 25 degrees C is zero on the y axis, and the adc reading at 25 degrees C is zero on the x axis. This keeps numbers small, so everything fits in a 32-bit register. See ``doc/ntc`` directory for details how the polynomial was derived.

The resulting polynomial converts an adc reading to temperature with four 32-bit integer multiplications, and four 32-bit integer additions.

For best accuracy, when measuring the NTC voltage, set the ADC to maximum input impedance. In ``analogSampleTime()`` set the slowest sample rate.

Open-circuit voltage and input impedance at the NTC pads vary considerably between boards. Perhaps putting a voltage follower opamp between NTC and ADC would help. (MM32SPIN27 has four built-in opamps available.) 


Instead of manually calibrating an analogue temperature sensor it is much more convenient to use factory-calibrated digital sensors.

## Rotary Encoder

The rotary encoder is decoded in interrupt-driven software.

The MM32SPIN27 timers have an encoder mode, to decode rotary encoders in hardware. Unfortunately, the pcb connects the two lines from the rotary encoder to processor pins that belong to _different_ timers, so this pcb can not be used to decode a rotary encoder in hardware.

## Debugger

- Segger JLink does not explicitly support MM32SPIN27 but MM32L072XX works.

- OpenOCD has an old patch to support the similar [MM32L062](https://sourceforge.net/p/openocd/mailman/message/37388746/)

- [H7-Tool](https://www.armfly.com/product/H7-TOOL/H7-TOOL.shtml) supports many "Made in Asia" processors, including MM32. Includes an RTT Viewer. H7-Tool is an [open-source](https://github.com/armfly/H7-TOOL_STM32H7_App) commercial product.

- The MM32 also supports firmware download via serial port, but on the T12-958 pcb the BOOT0 pin has been soldered to ground.

_not truncated_



