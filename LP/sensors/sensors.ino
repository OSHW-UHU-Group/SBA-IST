#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_CCS811.h>

#define oneWirePin 5
#define DHT22PIN 12
#define DHT22TYPE DHT22

OneWire oneWireBus(oneWirePin);
DallasTemperature ds18b20(&oneWireBus);

DHT dht22(DHT22PIN, DHT22TYPE);

Adafruit_BMP280 bmp280(7);

Adafruit_CCS811 ccs811;

BH1750 bh1750;


void setup() {
  Serial.begin(115200);
  
  ds18b20.begin(); 

  dht22.begin();
  
  Wire.begin();
  bh1750.begin();

  while (!bmp280.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    delay(100);
  }
  /* Default settings from datasheet. */
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  while(!ccs811.begin()){
    Serial.println("Failed to start sensor CCS811! Please check your wiring.");
    delay(100);
  }
  while(!ccs811.available()) {
    Serial.println("Waiting for sensor CCS811!");
    delay(100);
  }

}

void loop() {

  float temperature;
  float humidity;
  float pressure;
  float co2;
  float tvoc;
  float co;
  float ambientLight;
  
  ds18b20.requestTemperatures();
  temperature = ds18b20.getTempCByIndex(0);

  float tDHT22 = dht22.readTemperature();
  humidity = dht22.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if ( isnan(tDHT22)|| isnan(humidity) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  pressure = bmp280.readPressure()/100;
 
  if(ccs811.available()){
    if(!ccs811.readData()){
      co2 = ccs811.geteCO2();
      tvoc = ccs811.getTVOC();
    } else {
      co2=400;
      tvoc=0;
    }
  }

  int adc_MQ = analogRead(A0); 
  float voltaje = adc_MQ * (3.3 / 1024.0); 
  float Rs=1000.0*((3.3-voltaje)/voltaje);  
  co = 22.94*pow(Rs/8030, -0.671);

  ambientLight = bh1750.readLightLevel();

  if (Serial.available()){
    char op = Serial.read();
    switch(op) {
      case '1':
        Serial.println(temperature);
        break;
      case '2':
        Serial.println(humidity);
        break;
      case '3':
        Serial.println(pressure);
        break;
      case '4':
        Serial.println(co2);
        break;
      case '5':
        Serial.println(tvoc);
        break;
      case '6':
        Serial.println(co);
        break;
      case '7':
        Serial.println(ambientLight);
        break;
    }
  }

  delay(100);

}
