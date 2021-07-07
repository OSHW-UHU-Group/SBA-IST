import time
import board
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn
import RPi.GPIO as GPIO


def co():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(6, GPIO.IN)

    i2c = busio.I2C(board.SCL, board.SDA)
    ads = ADS.ADS1115(i2c)
    chan0 = AnalogIn(ads, ADS.P0)

   #if GPIO.input(6) == GPIO.LOW:
   #    print("Warning: Limit exceeded!!!")
    voltage = chan0.voltage
    Rs=1000.0*((3.3-voltage)/voltage)
    co=22.94*pow(Rs/1030, -0.671)
    #GPIO.cleanup()
    return co


#loop = True
#while loop:
#    try:
#        print("CO: ", co())
#        time.sleep(4)
#    except KeyboardInterrupt:
#        loop = False
#        print("App closed.")