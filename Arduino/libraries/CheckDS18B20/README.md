# Check DS18B20 Temperature Sensor

The DS18B20 is a one-wire temperature sensor. The original DS18B20 is manufactured by Analog Devices, Maxim or Dallas Semi. Other companies manufacture functionally similar pin-compatible temperature sensors. Each of these chips has its own price point, merits and inconveniences.

When you buy a DS18B20 it is not always clear who the manufacturer is. _CheckDS18B20_ is an arduino library that attempts to answer the following questions:

- Is the DS18B20 in my arduino project a genuine Maxim DS18B20?
- Who is the manufacturer of the DS18B20 in my project?

## Is the DS18B20 in my arduino project genuine?

The library provides three methods: _discover_fake_ds18b20()_, _ds18b20_family()_ and _ds18b20_name()_.

The method _discover_fake_ds18b20()_ classifies a DS18B20 into three classes: genuine, fake, and "don't know". The method _discover_fake_ds18b20()_ uses documented function codes.

The method _ds18b20_family()_ classifies a DS18B20 into a dozen different families.  The characteristics of each of these families can be looked up at Chris Petrich's git [counterfeit DS18B20](https://github.com/cpetrich/counterfeit_DS18B20/#how-do-i-know-if-i-am-affected).

The method _ds18b20_name()_ returns a string, making printing diagnostics easy.

 The methods _ds18b20_family()_ and _ds18b20_name()_ use undocumented function codes.

## Usage case

There are arguments in favor and against using this library.

- Having a DS18B20 temperature sensor in your project, but not knowing whether the measured temperature can be trusted is a problem.

- Using this library may give you more confidence in sensor output. But as this library sends undocumented function codes to components of uncertain origin, the result can not be guaranteed. If sensor calibration data are overwritten, the sensor may become useless.

Use at your own risk. No guarantees given or implied.

## Acknowledgment

This library is based upon [counterfeit_DS18B20](https://github.com/cpetrich/counterfeit_DS18B20) by Chris Petrich.

## License

This software is licensed under the Apache license.
