import serial, time
import sys,tty

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

a = 0
b = 0
c = 0

while True:
    char = s.read(16)
    print(char.decode())
    while char.decode() == "label 1 finish\r\n" and a == 0:
        v1 = input("Enter:v")
        string1 = "/Modify/run " + v1 + "\r"
        s.write(string1.encode())
        if v1 == "3":  a = 1
    while char.decode() == "Turn finish   \r\n" and b == 0:
        v1 = input("Enter:v")
        string1 = "/Modify/run " + v1 + "\r"
        s.write(string1.encode())
        if v1 == "3":  b = 1
    while char.decode() == "Modify Again  \r\n" and c == 0:
        v1 = input("Enter:v")
        string1 = "/Modify/run " + v1 + "\r"
        s.write(string1.encode())
        if v1 == "3":  c = 1
    time.sleep(1)