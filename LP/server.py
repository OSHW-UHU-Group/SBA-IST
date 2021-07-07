from http.server import HTTPServer, BaseHTTPRequestHandler
from http import HTTPStatus
import multiprocessing as mp
import json
import time
import csv
import urllib3 

import datetime

import threading
import logging


import arduino


logging.basicConfig(
    level=logging.DEBUG,
    format='(%(threadName)-10s) %(message)s',
)


# Data Sensors
temperature = arduino.temperature()
humidity = arduino.humidity()
pressure = arduino.pressure()
co2 = arduino.co2()
tvoc = arduino.tvoc()
co = arduino.co()
ambLight = arduino.lux()

####################################################################
## API Web
##########
class _RequestHandler(BaseHTTPRequestHandler): 

    def _set_headers(self):
        self.send_response(HTTPStatus.OK.value)
        self.send_header("Content-type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()

    def do_GET(self):
        self._set_headers()
        data = read_data()
        response = json.dumps(data)
        response = bytes(response, "utf-8")
        self.wfile.write(response)

## Run Web Server
#################
def run_server():
    server_address = ("", 8001)
    httpd = HTTPServer(server_address, _RequestHandler)
    print("Start Sensoring Server at %s:%d" % server_address)
    print("Ctrl+C to end.")
    httpd.serve_forever()

## Return Sensor Data
###################
def read_data():
    global temperature
    global humidity
    global pressure
    global co2
    global tvoc
    global co
    global ambLight

    data = {
        "Temperature": temperature,
        "Humidity": humidity,
        "Pressure": pressure,
        "CO2": co2,
        "TVOC": tvoc,
        "CO": co,
        "AmbientLight": ambLight
    }

    return data


#################################################################
## Read Sensors Data Thread
###########################
def read_sensors():
    global temperature
    global humidity
    global pressure
    global co2
    global tvoc
    global co
    global ambLight
    
    global alive

    global SAVEFILE
    global THINGSPEAK

    x = datetime.datetime.now()
    fileName = x.strftime("%Y") + x.strftime("%m") +  x.strftime("%d") + x.strftime("%H") + x.strftime("%M") + ".csv"


    logging.debug("Reading sensors.")

    while alive:
        temperature = arduino.temperature()
        humidity = arduino.humidity()
        pressure = arduino.pressure()
        co2 = arduino.co2()
        tvoc = arduino.tvoc()
        co = arduino.co()
        ambLight = arduino.lux()

        f = datetime.datetime.now()
        logging.debug('- - [ %s ] Temperature = %s  - -   Ambient Light = %s', f, temperature, ambLight)
        logging.debug('- -  \t\t\t\t Humidity = %s  - -   Pressure = %s', humidity, pressure)
        logging.debug('- -  \t\t\t\t eCO2 = %s  - -   TVOC = %s', co2, tvoc)
        logging.debug('- -  \t\t\t\t CO = %s  ',  co)

        if SAVEFILE:
            csv_save(fileName, temperature, humidity, pressure, co2, tvoc, co, ambLight)

        if THINGSPEAK:
            thingSpeak(temperature, humidity, pressure, co2, tvoc, co, ambLight)

        time.sleep(5)


#################################################################
## Save Sensors Data to file
############################
SAVEFILE = False # False or True

def csv_save(fileName, temperature, humidity, pressure, CO2,TVOC, CO, lux):
  d = datetime.datetime.now()
  with open(fileName, 'a', newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=' ')
    writer.writerow([d] + [lux] + [temperature] + [humidity] + [pressure] + [CO2] + [TVOC] + [CO])


#################################################################
## Send Sensors Data to THINGSPEAK
##################################
THINGSPEAK = False # False or your 'APIKEY'

def thingSpeak(temp, hum, press, co2, tvoc, co, lux):
    print('Sending to ThingSpeak API...')
    url = "https://api.thingspeak.com/update?api_key="
    url += THINGSPEAK
    url += "&field1="
    url += str(temp)
    url += "&field2="
    url += str(hum)
    url += "&field3="
    url += str(press)
    url += "&field4="
    url += str(co2)
    url += "&field5="
    url += str(tvoc)
    url += "&field6="
    url += str(co)
    url += "&field7="    
    url += str(lux)
    #print url
    try: 
      http = urllib3.PoolManager()
      response = http.request('GET', url)
      if response.status != 200:
        print("Error: Not valid Request")
    except:
      print("Error HTTP Request")
      return False
    print('Done')



#################################################################################
## main
#######
alive = True

if __name__ == "__main__":
    try:
        t = threading.Thread(name="ReadSensors", target=read_sensors)
        t.start()

        run_server()

    except KeyboardInterrupt:
        print("losing Application.....")
        print("---- > Server closed.")
        alive = False
        t.join()
        print("---- > Monitorization closed.")
        