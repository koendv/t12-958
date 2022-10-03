# Schematic

During development, I've used the schematics [STM32 T12 V3.0](Station_STM32_T12_V3.pdf) 

Differences between MM32 and STM32 version:

- MM32SPIN27PF instead of STM32F103C8Tx
- DS1302Z real-time clock instead of AT24C08N EEPROM. DS1302Z connections:
	- CE PC15
	- SDA PA4
	- SCL PA3
- 32768Hz crystal moved from processor to DS1302Z
- NTC  is mounted on pcb. Soldering iron NTC does not seem to be connected?
- VBAT becomes GPIO pin PD7
- BAT connector connects to +3.3V
- UART and SWD connector joined

The MM32 reset pin is connected to capacitor C4.