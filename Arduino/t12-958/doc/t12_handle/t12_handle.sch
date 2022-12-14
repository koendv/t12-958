EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:Thermistor_NTC TH1
U 1 1 6345A98B
P 3050 1850
F 0 "TH1" V 2760 1850 50  0000 C CNN
F 1 "Thermistor_NTC" V 2851 1850 50  0000 C CNN
F 2 "" H 3050 1900 50  0001 C CNN
F 3 "~" H 3050 1900 50  0001 C CNN
	1    3050 1850
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_SPST SW1
U 1 1 6345C81B
P 3050 1350
F 0 "SW1" H 3050 1585 50  0000 C CNN
F 1 "SW_SPST" H 3050 1494 50  0000 C CNN
F 2 "" H 3050 1350 50  0001 C CNN
F 3 "~" H 3050 1350 50  0001 C CNN
	1    3050 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 6345CC44
P -156600 -194800
F 0 "R1" V -156807 -194800 50  0000 C CNN
F 1 "R" V -156716 -194800 50  0000 C CNN
F 2 "" V -156670 -194800 50  0001 C CNN
F 3 "~" H -156600 -194800 50  0001 C CNN
	1    -156600 -194800
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 63460C69
P 3150 800
F 0 "R2" V 2943 800 50  0000 C CNN
F 1 "R" V 3034 800 50  0000 C CNN
F 2 "" V 3080 800 50  0001 C CNN
F 3 "~" H 3150 800 50  0001 C CNN
	1    3150 800 
	0    1    1    0   
$EndComp
Wire Wire Line
	1800 800  3000 800 
Wire Wire Line
	3300 800  3350 800 
Wire Wire Line
	3350 800  3350 900 
Wire Wire Line
	1800 1350 2850 1350
Wire Wire Line
	3250 1350 3350 1350
Wire Wire Line
	3350 1850 3200 1850
Wire Wire Line
	3350 1000 3350 1350
Connection ~ 3350 1350
$Comp
L power:GND #PWR01
U 1 1 63460303
P 3350 1950
F 0 "#PWR01" H 3350 1700 50  0001 C CNN
F 1 "GND" H 3355 1777 50  0000 C CNN
F 2 "" H 3350 1950 50  0001 C CNN
F 3 "" H 3350 1950 50  0001 C CNN
	1    3350 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 1350 3350 1850
Wire Wire Line
	3350 1950 3350 1850
Connection ~ 3350 1850
Text Label 1900 800  0    50   ~ 0
RED_HEATER
Text Label 1900 900  0    50   ~ 0
BLACK_HEATER
Text Label 1900 1000 0    50   ~ 0
GREEN_GROUND
Text Label 1900 1350 0    50   ~ 0
BLUE_TILT_SWITCH
Text Label 1900 1850 0    50   ~ 0
WHITE_TEMP
$Comp
L Switch:SW_SPST SW2
U 1 1 63462ADD
P 6350 1350
F 0 "SW2" H 6350 1585 50  0000 C CNN
F 1 "SW_SPST" H 6350 1494 50  0000 C CNN
F 2 "" H 6350 1350 50  0001 C CNN
F 3 "~" H 6350 1350 50  0001 C CNN
	1    6350 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 63462AE7
P 6450 800
F 0 "R4" V 6243 800 50  0000 C CNN
F 1 "R" V 6334 800 50  0000 C CNN
F 2 "" V 6380 800 50  0001 C CNN
F 3 "~" H 6450 800 50  0001 C CNN
	1    6450 800 
	0    1    1    0   
$EndComp
Wire Wire Line
	5100 800  6300 800 
Wire Wire Line
	6600 800  6650 800 
Wire Wire Line
	6650 800  6650 900 
Wire Wire Line
	5100 1350 6150 1350
Wire Wire Line
	6550 1350 6650 1350
Wire Wire Line
	6650 1850 6500 1850
Wire Wire Line
	6650 1000 6650 1350
Connection ~ 6650 1350
$Comp
L power:GND #PWR03
U 1 1 63462AFC
P 6650 1950
F 0 "#PWR03" H 6650 1700 50  0001 C CNN
F 1 "GND" H 6655 1777 50  0000 C CNN
F 2 "" H 6650 1950 50  0001 C CNN
F 3 "" H 6650 1950 50  0001 C CNN
	1    6650 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 1350 6650 1850
Wire Wire Line
	6650 1950 6650 1850
Connection ~ 6650 1850
Text Label 5200 800  0    50   ~ 0
RED_HEATER
Text Label 5200 900  0    50   ~ 0
BLACK_HEATER
Text Label 5200 1000 0    50   ~ 0
GREEN_GROUND
Text Label 5200 1350 0    50   ~ 0
BLUE_TILT_SWITCH
Text Label 5200 1850 0    50   ~ 0
WHITE_TEMP
$Comp
L Sensor_Temperature:DS18B20 U1
U 1 1 63463EDF
P 6200 1850
F 0 "U1" H 6650 2200 50  0000 R CNN
F 1 "DS18B20" H 6650 2100 50  0000 R CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5200 1600 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 6050 2100 50  0001 C CNN
	1    6200 1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6200 1550 6500 1550
Wire Wire Line
	6500 1550 6500 1850
Wire Wire Line
	6200 2150 6500 2150
Wire Wire Line
	6500 2150 6500 1850
Connection ~ 6500 1850
$Comp
L Device:R R3
U 1 1 63470595
P 4850 1850
F 0 "R3" V 4643 1850 50  0000 C CNN
F 1 "R" V 4734 1850 50  0000 C CNN
F 2 "" V 4780 1850 50  0001 C CNN
F 3 "~" H 4850 1850 50  0001 C CNN
	1    4850 1850
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR02
U 1 1 634709DE
P 4600 1850
F 0 "#PWR02" H 4600 1700 50  0001 C CNN
F 1 "+3.3V" H 4615 2023 50  0000 C CNN
F 2 "" H 4600 1850 50  0001 C CNN
F 3 "" H 4600 1850 50  0001 C CNN
	1    4600 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 1850 4700 1850
Wire Wire Line
	5000 1850 5050 1850
Wire Notes Line
	5100 700  5100 2250
Text Notes 2150 2350 0    50   ~ 0
HANDLE WITH NTC
Text Notes 5450 2350 0    50   ~ 0
HANDLE WITH DS18B20
Wire Notes Line
	1800 700  1800 2250
Wire Wire Line
	1750 900  1750 1000
Wire Wire Line
	1750 900  3350 900 
Wire Wire Line
	1750 1000 3350 1000
Wire Wire Line
	5050 900  5050 1000
Wire Wire Line
	5050 900  6650 900 
Wire Wire Line
	5050 1000 6650 1000
Text Notes 900  1050 0    50   ~ 0
SOLDER BLOB ON\nGX12-5 CONNECTOR
Wire Wire Line
	1250 1850 2900 1850
Text Notes 1250 1850 0    50   ~ 0
PAD "NTC"
Wire Wire Line
	4600 1500 5050 1500
Wire Wire Line
	5050 1500 5050 1850
Connection ~ 5050 1850
Wire Wire Line
	5050 1850 5900 1850
Text Notes 4600 1500 0    50   ~ 0
PAD "F"
$EndSCHEMATC
