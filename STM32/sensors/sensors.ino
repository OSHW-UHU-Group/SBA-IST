#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_CCS811.h>

#define oneWirePin PA0

#define DHT22PIN PA1
#define DHT22TYPE DHT22

#define BMP280_CS   PB12
#define BMP280_SCK  PB13
#define BMP280_MISO PB14
#define BMP280_MOSI PB15

#define SDPIN PA4

OneWire oneWireBus(oneWirePin);
DallasTemperature ds18b20(&oneWireBus);

DHT dht22(DHT22PIN, DHT22TYPE);

Adafruit_BMP280 bmp280(BMP280_CS, BMP280_MOSI, BMP280_MISO,  BMP280_SCK);

Adafruit_CCS811 ccs811;

BH1750 bh1750;

File logFile;
bool microsd = false;

HardwareSerial Serial2(USART2); // Wifi Module
HardwareSerial Serial3(USART3); // Bluetooth Module

bool Led = true;

String ssid = "WIFISSID"; // Change it with your Wi-Fi SSID
String pass = "WIFIPASS"; // Change it with your Wi-Fi Password

void setup() {
  Serial.begin(115200);

  ds18b20.begin();

  dht22.begin();

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

  while (!ccs811.begin()) {
    Serial.println("Failed to start sensor CCS811! Please check your wiring.");
    delay(100);
  }
  while (!ccs811.available());

  Wire.begin();
  bh1750.begin();

  int icd = 0;
  while (!microsd && icd < 5) {
    if (SD.begin(SDPIN)) {
      microsd = true;
      Serial.println(F("Save data to microSD"));
    } else {
      microsd = false;
      Serial.println(F("Error: Impossible to save data to microSD"));
    }
    icd++;
    delay(100);
  }

  Serial2.begin(115200);
  wifi_begin();

  Serial3.begin(9600);
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, LOW);

}

void wifi_begin() {

  bool ipAddress = false;
  bool wifi_ok;
  String stringConnect;
  stringConnect = "AT+CWJAP=\"";
  stringConnect += ssid;
  stringConnect += "\",\"";
  stringConnect += pass;
  stringConnect += "\"";

  wifi_AT("AT", 100);
  wifi_AT("AT+CWMODE=3", 100);
  wifi_AT("AT+CWQAP", 100);
  wifi_AT("AT+RST", 5000);
  delay(1000);
  while (Serial2.available() > 0) {
    if (Serial2.find("WIFI GOT IP")) {
      ipAddress = true;
    }
  }
  if (!ipAddress) {
    Serial.println("Connecting Wifi....");
    wifi_ok = wifi_AT(stringConnect, 5000);

  }
  if (wifi_ok) {
    Serial.println("Wifi Connected");
    get_ip();
    wifi_AT("AT+CIPMUX=1", 100); // Multiple client connections
    wifi_AT("AT+CIPSERVER=1,80", 100); // Server at port 80

  } else {
    Serial.println("Error: Wifi not Connected");
  }

}

// This function sends AT commands to ESP module
bool wifi_AT(String cmd, int t) {
  int i = 0;
  bool ok = false;
  while (!ok && i < 5)
  {
    Serial.println(cmd);  // sends to serial monitor
    Serial2.println(cmd); // sends to ESP module via serial communication
    while (Serial2.available())
    {
      if (Serial2.find("OK")) ok = true;
    }
    delay(t);
    i++;
  }
  return ok;
}

// Thie function gets IP address
void get_ip() {

  String IP = "";
  char ch = 0;
  int i = 0;
  while (i < 10)
  {
    Serial2.println("AT+CIFSR");
    while (Serial2.available() > 0) {

      if (Serial2.find("STAIP,")) {
        delay(1000);
        Serial.print("IP Address:");
        while (Serial2.available() > 0) {
          ch = Serial2.read();
          if (ch == '+') break;
          IP += ch;
        }
      }
      if (ch == '+') break;
    }
    if (ch == '+') break;
    i++;
    delay(1000);
  }
  Serial.print(IP); // send IP address to serial monitor
  Serial.print("Port:");
  Serial.println(80);
}

void wifi_send(int connection, String jsondata) {
  int l = jsondata.length();
  bool ok = false;
  Serial.print("AT+CIPSEND=0,");
  Serial2.print("AT+CIPSEND=0,");
  Serial.println(l);
  Serial2.println(l);
  delay(100);
  Serial.println(jsondata); // Sends JSON data to serial monitor
  Serial2.println(jsondata); // Sends JSON data to serial ESP8266
  delay(100);
  while (Serial2.available()) {
    if (Serial2.find("OK")) {
      ok = true;
      Serial.println("OK");
      break;
    }
  }
  delay(100);
  Serial.println("AT+CIPCLOSE=0");
  Serial2.println("AT+CIPCLOSE=0"); // Close server connection
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
  if ( isnan(tDHT22) || isnan(humidity) ) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  pressure = bmp280.readPressure() / 100;

  if (ccs811.available()) {
    if (!ccs811.readData()) {
      co2 = ccs811.geteCO2();
      tvoc = ccs811.getTVOC();
    } else {
      co2 = 400;
      tvoc = 0;
    }
  }

  int adc_MQ = analogRead(PB1);
  float voltaje = adc_MQ * (3.3 / 4095.0);
  float Rs = 1000.0 * ((3.3 - voltaje) / voltaje);
  co = 22.94 * pow(Rs / 8030, -0.671);

  ambientLight = bh1750.readLightLevel();

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print(" ºC");
  Serial.print("   -o-   Humidity: ");
  Serial.print(humidity);
  Serial.print("% ");
  Serial.print("   -o-   Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");
  Serial.print("CO2: ");
  Serial.print(co2);
  Serial.print("ppm   -o-   TVOC: ");
  Serial.print(tvoc);
  Serial.print("ppm   -o-   CO:");
  Serial.print(co);
  Serial.println("ppm");
  Serial.print("Light: ");
  Serial.print(ambientLight);
  Serial.println(" lx");

  if (microsd) {
    logFile = SD.open("datalog.txt", FILE_WRITE);
    if (logFile) {
      logFile.print(temperature);
      logFile.print(", ");
      logFile.print(humidity);
      logFile.print(", ");
      logFile.print(pressure);
      logFile.print(", ");
      logFile.print(co2);
      logFile.print(", ");
      logFile.print(tvoc);
      logFile.print(", ");
      logFile.print(co);
      logFile.print(", ");
      logFile.println(ambientLight);
      logFile.close();
    }
    else {
      Serial.println("Error: Failed to save data");
    }
  }

  String jsondata = "{ \"temperature\": " + (String)temperature + ", ";
  jsondata += " \"humidity\": " + (String)humidity + ", ";
  jsondata += " \"pressure\": " + (String)pressure + ", ";
  jsondata += " \"co2\": " + (String)co2 + ", ";
  jsondata += " \"tvoc\": " + (String)tvoc + ", ";
  jsondata += " \"co\": " + (String)co + ", ";
  jsondata += " \"ambientLight\": " + (String)ambientLight + " } ";

  // Send JSON data to Web Client
  while (Serial2.available())
  {
    if (Serial2.find("0,CONNECT"))
    {
      Serial.println("Start Printing");

      wifi_send(0, jsondata);
      Serial.println("Done Printing");
      //delay(1000);

    }
  }

  // Send Sensor data to BLE
  if (Serial3.available() > 0) {
    char inputData = Serial3.read();
    switch(inputData) {
      case '1':
        Serial3.println(temperature);
        break;
      case '2':
        Serial3.println(humidity);
        break;
      case '3':
        Serial3.println(pressure);
        break;
      case '4':
        Serial3.println(co2);
        break;
      case '5':
        Serial3.println(tvoc);
        break;
      case '6':
        Serial3.println(co);
        break;
      case '7':
        Serial3.println(ambientLight);
        break;
    }
  }

  delay(100);
}
