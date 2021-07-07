import time
import io
import fcntl
import board
import busio
import adafruit_ccs811

i2c = board.I2C()

# I2C Address
CCS811_ADDRESS =  (0x5A)
I2C_SLAVE=0x0703
N_I2C_DEVICE=1

init = False
seg = 0
while not init:
    try: 
        CCS811_fr= io.open("/dev/i2c-"+str(N_I2C_DEVICE), "rb", buffering=0)
        CCS811_fw= io.open("/dev/i2c-"+str(N_I2C_DEVICE), "wb", buffering=0)
        fcntl.ioctl(CCS811_fr, I2C_SLAVE, CCS811_ADDRESS)
        fcntl.ioctl(CCS811_fw, I2C_SLAVE, CCS811_ADDRESS)

        # Switch sensor to application mode
        CSS811_APP_START        =  (0xF4)
        s = [CSS811_APP_START]
        s2 = bytearray( s )
        CCS811_fw.write( s2 )
        time.sleep(0.0625)

        CSS811_MEAS_MODE        =  (0x01)
        configuration = 0b110000
        s = [CSS811_MEAS_MODE,configuration,0x00]
        s2 = bytearray( s )
        CCS811_fw.write( s2 )
        time.sleep(0.015)
        
        time.sleep(0.015)
        CSS811_ALG_RESULT_DATA  =  (0x02)
        s = [CSS811_ALG_RESULT_DATA]
        s2 = bytearray( s )
        CCS811_fw.write( s2 )
        time.sleep(0.0625)
        data = CCS811_fr.read(1)

        time.sleep(0.015)
        CSS811_STATUS           =  (0x00)
        s = [CSS811_STATUS]
        s2 = bytearray( s )
        CCS811_fw.write( s2 )
        time.sleep(0.0625)  

        seg += 10

        ccs811 = adafruit_ccs811.CCS811(i2c)
        init = True
    except Exception as e:
        print(e)
        print("Inicializing sensor CC811...(", seg, ")")
        
        time.sleep(10)

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



#while 1:
#    e, t = eCO2_TVOC()
#
#    print("eCO2", e)
#    print("TVOC", t)
#    time.sleep(5)

