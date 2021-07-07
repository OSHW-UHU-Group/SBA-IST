import time
import Adafruit_BBIO.ADC as ADC
ADC.setup()


def co():
    
    voltage = ADC.read("P9_40")

    Rs=1000.0*((3.3-voltage)/voltage)
    co=22.94*pow(Rs/1030, -0.671)
    
    return co
    
# loop = True
# while loop:
#     try:
#         print("CO: ", co())
#         time.sleep(3)
#     except KeyboardInterrupt:
#         loop = False
#         print("App closed.")