import time
import board
import busio
import adafruit_ccs811
i2c_bus = busio.I2C(board.SCL, board.SDA)
ccs811 = adafruit_ccs811.CCS811(i2c_bus)


def eCO2_TVOC():
    global ccs811
    while True:
        try:
            eco2 = ccs811.eco2
            if eco2 == None:
                eco2 = 0
            tvoc = ccs811.tvoc
            if tvoc == None:
                tvoc = 0
            return eco2, tvoc
        except:
            pass


#
#loop = True
#while loop:
#    try:
#        co2, tvoc = eCO2_TVOC()
#        print("CO2: %1.0f PPM" % co2)
#        print("TVOC: %1.0f PPM" % tvoc)
#        time.sleep(3)
#    except KeyboardInterrupt:
#        loop = False
#        print("App closed.")