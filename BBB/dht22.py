import time
import board
import adafruit_dht
 
dht = adafruit_dht.DHT22(board.P9_23, use_pulseio=False)

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
#loop = True
#while loop:
#    try:
#        h, t= humidity()
#        print("HUmidity: ", h)
#        time.sleep(3)
#    except KeyboardInterrupt:
#        loop = False
#        print("App closed.")

