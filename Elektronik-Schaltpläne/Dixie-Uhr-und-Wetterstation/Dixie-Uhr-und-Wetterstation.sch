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
L ESP8266:NodeMCU_1.0_(ESP-12E) U?
U 1 1 60C01402
P 2050 1900
F 0 "U?" H 2050 2987 60  0000 C CNN
F 1 "NodeMCU_1.0_(ESP-12E)" H 2050 2881 60  0000 C CNN
F 2 "" H 1450 1050 60  0000 C CNN
F 3 "" H 1450 1050 60  0000 C CNN
	1    2050 1900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Male J?
U 1 1 60BFECCD
P 4000 4100
F 0 "J?" H 4108 4581 50  0000 C CNN
F 1 "Dixie_01" H 4108 4490 50  0000 C CNN
F 2 "" H 4000 4100 50  0001 C CNN
F 3 "~" H 4000 4100 50  0001 C CNN
	1    4000 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Male J?
U 1 1 60C010E8
P 4950 4100
F 0 "J?" H 5058 4581 50  0000 C CNN
F 1 "Dixie_02" H 5058 4490 50  0000 C CNN
F 2 "" H 4950 4100 50  0001 C CNN
F 3 "~" H 4950 4100 50  0001 C CNN
	1    4950 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Male J?
U 1 1 60C024BF
P 5750 4100
F 0 "J?" H 5858 4581 50  0000 C CNN
F 1 "Dixie_03" H 5858 4490 50  0000 C CNN
F 2 "" H 5750 4100 50  0001 C CNN
F 3 "~" H 5750 4100 50  0001 C CNN
	1    5750 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Male J?
U 1 1 60C040CA
P 6600 4100
F 0 "J?" H 6708 4581 50  0000 C CNN
F 1 "Dixie_04" H 6708 4490 50  0000 C CNN
F 2 "" H 6600 4100 50  0001 C CNN
F 3 "~" H 6600 4100 50  0001 C CNN
	1    6600 4100
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Male J?
U 1 1 60C050DA
P 7500 4100
F 0 "J?" H 7608 4581 50  0000 C CNN
F 1 "Dixie_05" H 7608 4490 50  0000 C CNN
F 2 "" H 7500 4100 50  0001 C CNN
F 3 "~" H 7500 4100 50  0001 C CNN
	1    7500 4100
	1    0    0    -1  
$EndComp
$Comp
L Sensor:BME280 U?
U 1 1 60C06F11
P 7750 2100
F 0 "U?" H 7321 2146 50  0000 R CNN
F 1 "BME280" H 7321 2055 50  0000 R CNN
F 2 "Package_LGA:Bosch_LGA-8_2.5x2.5mm_P0.65mm_ClockwisePinNumbering" H 9250 1650 50  0001 C CNN
F 3 "https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf" H 7750 1900 50  0001 C CNN
	1    7750 2100
	1    0    0    -1  
$EndComp
$EndSCHEMATC
