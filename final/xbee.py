import serial, time

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

while True:
    char = s.read(16)
    print(char.decode())
    time.sleep(1)