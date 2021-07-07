import time
import smbus

bus = smbus.SMBus(2)
addr = 0x23


def lux():
    rawdata = bus.read_i2c_block_data(addr,0x11)
    data = str((rawdata[1] + (256 * rawdata[0])) / 1.2)
    lux = round(float(data),2)
    return lux

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

