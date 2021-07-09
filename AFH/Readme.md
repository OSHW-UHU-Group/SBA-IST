# Adafruit Feather HUZZAH ESP8266

Adafruit Feather HUZZAH ESP8266 (AFH) has an ESP8266 WiFi microcontroller, 3.3V logic and can be programming using the Arduino IDE.

## Pinout

<img src="https://cdn-learn.adafruit.com/assets/assets/000/046/249/original/adafruit_products_Huzzah_ESP8266_Pinout_v1.2-1.png?1504885873" alt="Adafruit Feather HUZZAH ESP8266 Pinout">
<p>Source: Adafruit.com</p>


## DS18B20 Sensor

The DS18B20 is a temperature sensor that communicates over 1-Wire protocol.

<img src="../images/ds18b20.jpg" width="10%" alt="DS18B20 Pinout">
<p>Source: DS18B20 DataSheet</p>

VDD - 3.3V

GND - GND

DQ  - AFH IO0 (4k7 resistor - 3.3V)


## DTH22 Sensor

The DHT22 is a temperature and humidity sensor where it outputs calibrated digital signal using exclusive collecting-technique.

<img src="../images/dht22.png" width="20%" alt="DHT22 Pinout">

(+) VDD - 3.3V

(-) GND - GND

Out  - AFH IO2


## BMP280 Sensor

The BMP280 is an environmental sensor of barometric pressure and it can be used in both I2C and SPI buses. Here, we use SPI bus.

<img src="../images/bmp280_1.jpg" width="20%" alt="BMP280 Pinout_1"> <img src="../images/bmp280_2.jpg" width="20%" alt="BMP280 Pinout_2">

VCC - 3.3V

GND - GND

SCL - SCK - AFH IO14

SDA - MOSI - AFH IO13

CSB - CSS - AFH IO16

SDO - MISO - AFH IO12

## CCS811 Sensor

The CCS811 is a gas sensor which detects a wide range of Volatile Organic Compounds (VOCs) for indoor air quality monitoring and it outputs an equivalent CO2 (eCO2) levels and TVOC values. It can be connected via I2C bus using one of its address (0x5A).

<img src="../images/ccs811_1.jpg" width="20%" alt="CCS811 Pinout_1"> <img src="../images/ccs811_2.jpg" width="20%" alt="CCS811 Pinout_2">

VCC - 3.3V

GND - GND

SCL - AFH IO5

SDA - AFH IO4

WAK - GND  --> Addr: 0x5A  

INT - (none)

RST - (none)

ADD - (none)


## MQ7 Sensor
MQ7 is a gas detection sensor, that specifically it is used for detection of carbon monoxide (CO) concentrations in the air. It outputs digital and analog signals.

<img src="../images/mq7_1.jpg" width="20%" alt="MQ7 Pinout_1"> <img src="../images/mq7_2.jpg" width="20%" alt="MQ7 Pinout_2">


VCC - 3.3V

GND - GND

D0 - (none)

A0 - AFH ADC (A0)  -- Warning!: this pin has a 1.0V maximum, a voltage divider 3.3V to 1.0V is need.


## BH1750 Sensor

The BH1750 is a sensor for luminous flux (lux) measurement. It can be connected via I2C bus using one of its address (0x23).

<img src="../images/bh1750_1.jpg" width="15%" alt="BH1750 Pinout_1"> <img src="../images/bh1750_2.jpg" width="15%" alt="BH1750 Pinout_2">


VCC - 3.3V

GND - GND

SCL - AFH IO5

SDA - AFH IO4

ADD - (none)  --> Addr: 0x23


 
## Additional Modules

### HM10 Bluetooth 4.0 (BLE)

The HM-10 is a Bluetooth Low Energy (BLE, Bluetooth 4.0) module based on the CC2541 chip from Texas Instruments. This module provides to SBA the BLE capability via Serial port.


<img src="../images/hm10_1.jpg" width="15%" alt="HM10 Pinout_1"> <img src="../images/hm10_2.jpg" width="15%" alt="HM10 Pinout_2">

VCC - 3.3V

GND - GND

TX - AFH IO3 (RX)

RX - AFH IO1 (TX)

STATE - (none)

BRK - (none)


### Voltage Divider 3.3V to 1.0V

A voltage divider is a circuit with resistors un series that translates an input voltage (Vin) to an output one (Vout). For example, with two resistors in series (R1 and R2) an input voltage (3.3V) can be translate to an output voltage (1.0V) where R1/R2 = 2.3.

```
R1 = 2.3 * R2
```

<img src="../images/voltagedivider.jpg" width="15%" alt="Voltage Divider">
 
 ### microSD Adapter

<img src="../images/microsd_1.jpg" width="15%" alt="HM10 Pinout_1"> <img src="../images/microsd_2.jpg" width="15%" alt="HM10 Pinout_2">

VCC - 3.3V

GND - GND

SCK - AFH IO14

MOSI - AFH IO13

CS - AFH IO15

MISO - AFH IO12