# Conversion of NTC voltage to temperature

The ambient temperature sensor is an NTC thermistor, 10K at 25C, and a 4.7K pull-up resistor. An adc measures the voltage over the NTC resistor.

The relationship between NTC voltage and temperature is given by the [Steinhart-Hart equation](https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation).
Unfortunately, this equation requires floating point.

For better performance, curve fitting has been used to map the  manufacturers' data on two fourth degree polynomials - one polynomial for temperatures below 25C, and one polynomial for temperatures above 25C, 25C being the inflection point in the relation voltage - temperature. This way the calculations can be done using 32-bit integers.
 
