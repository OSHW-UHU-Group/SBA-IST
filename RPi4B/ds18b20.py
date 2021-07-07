# Temperature Sensor DS18B20
from w1thermsensor import W1ThermSensor
import time

# 1-wired
# PIN: RBPi --> default Pin GPIO4    BBB --> default Pin 9_12

def temperature():
    i=0
    while i<10:
        try:
            sensor = W1ThermSensor()
            temperature = sensor.get_temperature()
            return temperature
        except Exception as error:
            # Reading doesn't always work! (try again max. 10)
            i = i + 1
            time.sleep(0.5)
            continue




#loop = True
#while loop:
#    try:
#        print("Temperatue: ", temperature())
#    except KeyboardInterrupt:
#        loop = False
#        print("App closed.")