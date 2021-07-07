
import serial, time

leonardo = serial.Serial('COM5', 115200, timeout=0)

def send(sensor):
    data = bytes(sensor, 'UTF8')
    leonardo.write(data)

def receive():
    data = leonardo.readline()
    return data.decode("UTF8")

def temperature():
    temp = send(1)
    time.sleep(0.1)
    temp = receive()
    return temp

def humidity():
    hum = send(2)
    time.sleep(0.1)
    hum = receive()
    return hum

def pressure():
    pres = send(3)
    time.sleep(0.1)
    pres = receive()
    return pres


def co2():
    co2 = send(4)
    time.sleep(0.1)
    co2 = receive()
    return co2

def tvoc():
    tvoc = send(5)
    time.sleep(0.1)
    tvoc = receive()
    return tvoc

def co():
    co = send(6)
    time.sleep(0.1)
    co = receive()
    return co

def lux():
    lux = send(7)
    time.sleep(0.1)
    lux = receive()


