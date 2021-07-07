import board
import busio
import digitalio
import time
import adafruit_bmp280
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
cs = digitalio.DigitalInOut(board.P9_17)
bmp280 = adafruit_bmp280.Adafruit_BMP280_SPI(spi, cs)

bmp280.sea_level_pressure = 1013.25  #1014.00

def pressure():
    global bmp280
    pressure = round(bmp280.pressure,2)
    temperature = round(bmp280.temperature,2)
    return pressure, temperature


#
#loop = True
#while loop:
#    try:
#        p, t = pressure()
#        print("Temperatue: ", t)
#        print("Pressure: ", p)
#        time.sleep(3)
#    except KeyboardInterrupt:
#        loop = False
#        print("App closed.")