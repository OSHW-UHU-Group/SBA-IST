import time
import board
import adafruit_bh1750
 
i2c = board.I2C()
 


def lux():
    sensor = adafruit_bh1750.BH1750(i2c)
    return sensor.lux

#
#loop = True
#while loop:
#    try:
#        lux = lux()
#        print("Lux: ", lux)
#        time.sleep(3)
#    except KeyboardInterrupt:
#        loop = False
#        print("App closed.")