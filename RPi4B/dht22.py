import time
import board
import adafruit_dht
 
dht = adafruit_dht.DHT22(board.D26, use_pulseio=False)

def humidity():
    global dht
    i = 0
    while i<10:
        try:
            temperature_c = dht.temperature
            humidity = dht.humidity
            return humidity, temperature_c
        except RuntimeError as error:
             # Reading doesn't always work! (try again max. 10)
            i = i + 1
            time.sleep(1.0)
            continue

#
#while 1:
#   h, t= humidity()
#   print(h)
#   print(t)
#   time.sleep(1)