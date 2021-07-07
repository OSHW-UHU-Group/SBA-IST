#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_CCS811.h>
#include <BH1750.h>

#define ONE_WIRE_BUS 0
#define DHTPIN 2
#define DHTTYPE DHT22
#define BMP_CS 16

#ifndef STASSID
#define STASSID "WIFISSID" // Change it with your Wi-Fi SSID
#define STAPSK  "WIFIPASS" // Change it with your Wi-Fi Password
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// DS18B20: Pin 0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// DHT22: PIN 2
DHT dht(DHTPIN, DHTTYPE);

// BMP280 - SPI --> SDO-MISO:12  CSB-CSS:16  SDA-MOSI:13  SCL-SCK:14
Adafruit_BMP280 bmp(BMP_CS);

// CCS811 - I2C --> SCL:5  SDA:4  WAK:GND
Adafruit_CCS811 ccs;

// BH1750 - I2C -->  SCL:5  SDA:4
BH1750 lightMeter;

// Server at port 80
WiFiServer server(80);


void setup() {

  ds18b20.begin();

  dht.begin();

  while (!bmp.begin()) {
    delay(500);
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  // Init CCS811
  while (!ccs.begin()) {
    delay(500);
  }

  Wire.begin();
  lightMeter.begin();

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  // Start the server
  server.begin();
  // Print the IP address
  //WiFi.localIP();

  // Serial to BLE module
  Serial.begin(9600);

}

void loop() {

  float temperature, humidity, pressure, co2, tvoc, co, lux;

  ds18b20.requestTemperatures();
  temperature = ds18b20.getTempCByIndex(0);

  float tDHT = dht.readTemperature();
  float hDHT = dht.readHumidity();
  if ( isnan(tDHT) || isnan(hDHT) ) {
    tDHT = 0;
    hDHT = 0;
  }
  humidity = hDHT;

  pressure = bmp.readPressure() / 100.0;

  if (ccs.available()) {
    if (!ccs.readData()) {
      co2 = ccs.geteCO2();
      tvoc = ccs.getTVOC();
    } else {
      co2 = 400;
      tvoc = 0;
    }
  }

  int adc_MQ = analogRead(A0); //Lemos la salida analógica  del MQ
  float voltaje = adc_MQ * (3.3 / 4096.0); //Convertimos la lectura en un valor de voltaje
  float Rs = 1000 * ((3.3 - voltaje) / voltaje); //Calculamos Rs con un RL de 1k
  co = 22.94 * pow(Rs / 5463, -1.497);

  lux = lightMeter.readLightLevel();


  // Send Sensor data to BLE
  if (Serial.available() > 0) {
    char inputData = Serial.read();
    switch (inputData) {
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
        Serial.println(lux);
        break;
    }
  }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  client.setTimeout(5000); 
  String req = client.readStringUntil('\r');
  while (client.available()) {
    client.read();
  }

  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
  client.print("{ \"temperature\": ");
  client.print(temperature);
  client.print(", \"humidity\": ");
  client.print(humidity);
  client.print(", \"pressure\": ");
  client.print(pressure);
  client.print(", \"co2\": ");
  client.print(co2);
  client.print(", \"tvoc\": ");
  client.print(tvoc);
  client.print(", \"co\": ");
  client.print(co);
  client.print(", \"lux\": ");
  client.print(lux);    
  client.print("}");


  delay(2000);
}
