#include <ArduinoBLE.h>
#include <MaximWire.h>
#include <DHT.h>
#include <BMP280_SPI.h>
#include <Adafruit_CCS811.h>
#include <Wire.h>
#include <BH1750.h>

#include <mbed.h>
using namespace mbed;

#define PIN_BUS D2
#define DHTPIN D4
#define DHTTYPE DHT22

// DS18B20:  if you use only one sensor, you don't need external 4k7 resistor.
MaximWire::Bus bus(PIN_BUS);
MaximWire::DS18B20 ds18b20;

// DHT22
DHT dht(DHTPIN, DHTTYPE);

// BMP280
PinName BMP_CS = P0_27;
PinName BMP_SCK = P0_13;
PinName BMP_MISO = P1_8;
PinName BMP_MOSI = P1_1;
BMP280_SPI bmp(BMP_MOSI, BMP_MISO, BMP_SCK, BMP_CS);

// CCS811
Adafruit_CCS811 ccs;

BH1750 lightMeter;

float temperature = 0;
float humidity = 0;
float pressure = 0;
float co2 = 0;
float tvoc = 0;
float co = 0;
float ambientLight = 0;


// Custom Service Air Quality Sensing 273c51b8-181A-0000-8f98-0c43b9fa3391
BLEService airqualitySensingService("273c51b8-181A-0000-8f98-0c43b9fa3391");

// Characteristic CO2 273c51b8-181A-0004-8f98-0c43b9fa3391
BLEFloatCharacteristic tempCharacteristic("273c51b8-181A-0001-8f98-0c43b9fa3391", BLERead | BLENotify);
BLEFloatCharacteristic humidCharacteristic("273c51b8-181A-0002-8f98-0c43b9fa3391", BLERead | BLENotify);
BLEFloatCharacteristic pressCharacteristic("273c51b8-181A-0003-8f98-0c43b9fa3391", BLERead | BLENotify);
BLEFloatCharacteristic co2Characteristic("273c51b8-181A-0004-8f98-0c43b9fa3391", BLERead | BLENotify);
BLEFloatCharacteristic tvocCharacteristic("273c51b8-181A-0005-8f98-0c43b9fa3391", BLERead | BLENotify);
BLEFloatCharacteristic coCharacteristic("273c51b8-181A-0006-8f98-0c43b9fa3391", BLERead | BLENotify);
BLEFloatCharacteristic luxCharacteristic("273c51b8-181A-0007-8f98-0c43b9fa3391", BLERead | BLENotify);

String ssid = "WIFISSID"; // Change it with your Wi-Fi SSID
String pass = "WIFIPASS"; // Change it with your Wi-Fi Password
void wifi_begin();
bool wifi_AT(String cmd, int t);
void get_ip();
void wifi_send(int connection, String jsondata);

void readSensors();


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Wire.begin();
  lightMeter.begin();

  int i=0;
  while (!ccs.begin() && i<5) {
    delay(1000);
    i++;
  }
  while (!ccs.available()){
    delay(500);
  }

  while (!BLE.begin()) {
    delay(500);
  }
  // Set up Bluetooth Environmental Sensing service.
  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(airqualitySensingService);
  // Add characteristics for barometric pressure, temperature, and humidity.
  airqualitySensingService.addCharacteristic(tempCharacteristic);
  airqualitySensingService.addCharacteristic(humidCharacteristic);
  airqualitySensingService.addCharacteristic(pressCharacteristic);
  airqualitySensingService.addCharacteristic(co2Characteristic);
  airqualitySensingService.addCharacteristic(tvocCharacteristic);
  airqualitySensingService.addCharacteristic(coCharacteristic);
  airqualitySensingService.addCharacteristic(luxCharacteristic);
  // Make the service available.
  BLE.addService(airqualitySensingService);
  BLE.setConnectable(true);
  BLE.advertise();

  // Wi-Fi Connection
  Serial.begin(115200);
  wifi_begin();

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  readSensors();

  BLEDevice central = BLE.central();

  if (central) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("Incoming connection from: ");
    Serial.println(central.address());

    while (central.connected()) {
      // Get readings from sensors and update the charcteristic values.



      // Update Bluetooth characteristics with new values.
      tempCharacteristic.writeValue(temperature);
      humidCharacteristic.writeValue(humidity);
      pressCharacteristic.writeValue(pressure);
      co2Characteristic.writeValue(co2);
      tvocCharacteristic.writeValue(tvoc);
      coCharacteristic.writeValue(co);
      luxCharacteristic.writeValue(ambientLight);

      // Delay between updates. (Don't make too long of connections start to timeout.)
      delay(1000);
    }
    // Turn off LED when connection is dropped.
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Connection terminated.");
  }
  delay(2000);
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
  while (Serial.available() > 0) {
    if (Serial.find("WIFI GOT IP")) {
      ipAddress = true;
    }
  }
  if (!ipAddress) {
    wifi_ok = wifi_AT(stringConnect, 5000);

  }
  if (wifi_ok) {
    get_ip();
    wifi_AT("AT+CIPMUX=1", 100); // Multiple client connections
    wifi_AT("AT+CIPSERVER=1,80", 100); // Server at port 80

  }

}

// This function sends AT commands to ESP module
bool wifi_AT(String cmd, int t) {
  int i = 0;
  bool ok = false;
  while (!ok && i < 5)
  {
    Serial.println(cmd); // sends to ESP module via serial communication
    while (Serial.available())
    {
      if (Serial.find("OK")) ok = true;
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
    Serial.println("AT+CIFSR");
    while (Serial.available() > 0) {

      if (Serial.find("STAIP,")) {
        delay(1000);
        while (Serial.available() > 0) {
          ch = Serial.read();
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

}

void wifi_send(int connection, String jsondata) {
  int l = jsondata.length();
  bool ok = false;

  Serial.print("AT+CIPSEND=0,");

  Serial.println(l);
  delay(100);

  Serial.println(jsondata); // Sends JSON data to serial ESP8266
  delay(100);
  while (Serial.available()) {
    if (Serial.find("OK")) {
      ok = true;
      break;
    }
  }
  delay(100);
  Serial.println("AT+CIPCLOSE=0"); // Close server connection
}

void readSensors() {

  // Write values to serial port for debug.

  if (ds18b20.IsValid()) {
    temperature = ds18b20.GetTemperature<float>(bus);
    if (!isnan(temperature)) {
      ds18b20.Update(bus);
    } else {
      ds18b20.Reset();
    }
  } else {
    if (bus.Discover().FindNextDevice(ds18b20) && ds18b20.GetModelCode() == MaximWire::DS18B20::MODEL_CODE) {
      ds18b20.Update(bus);
    } else {
      ds18b20.Reset();
    }
  }

  float hDHT22 = dht.readHumidity();
  if ( isnan(hDHT22) ) {
    humidity = 0;
  } else {
    humidity = hDHT22;
  }

  pressure = bmp.getPressure();

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
  float Rs=1000*((3.3-voltaje)/voltaje);  //Calculamos Rs con un RL de 1k
  co = 22.94*pow(Rs/5463, -1.497);
  
  ambientLight = lightMeter.readLightLevel();


}
