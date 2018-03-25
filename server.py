#!/usr/bin/env python
import socket
import serial

# replace [serial_item] with output of ls dev/tty*
# https://oscarliang.com/connect-raspberry-pi-and-arduino-usb-cable/
# second argument is baud rate
ser = serial.Serial('/dev/tty4', 9600)
# to avoid interpreting noise, these are the first two bytes of every command
COMMAND_BYTE_PAIR = bytearray(b'\xFF\xEE')


def sendToArduino(command):
    data = COMMAND_BYTE_PAIR
    ser.write(command)
    print(ser.read())


# functions to interface with arduino
def button1():
    print("Button 1 command recieved")
    x = "\xB1"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button2():
    print("Button 2 command recieved")
    x = "\xB2"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button3():
    print("Button 3 command recieved")
    x = "\xB3"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button4():
    print("Button 4 command recieved")
    x = "\xB4"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button5():
    print("Button 5 command recieved")
    x = "\xB5"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button6():
    print("Button 6 command recieved")
    x = "\xB6"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button7():
    print("Button 7 command recieved")
    x = "\xB7"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button8():
    print("Button 8 command recieved")
    x = "\xB8"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button9():
    print("Button 9 command recieved")
    x = "\xB9"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button10():
    print("Button 10 command recieved")
    x = "\xB0"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button11():
    print("Button 11 command recieved")
    x = "\xBA"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def button12():
    print("Button 12 command recieved")
    x = "\xBB"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def dpadup():
    print("dpad up command recieved")
    x = "\xD1"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def dpadright():
    print("dpad right command recieved")
    x = "\xD2"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def dpaddown():
    print("dpad down command recieved")
    x = "\xD3"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def dpadleft():
    print("dpad left command recieved")
    x = "\xD4"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def lstickup():
    print("left stick up command recieved")
    x = "\xD6"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def lstickright():
    print("left stick up command recieved")
    x = "\xD7"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def lstickdown():
    print("left stick up command recieved")
    x = "\xD8"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def lstickleft():
    print("left stick up command recieved")
    x = "\xD9"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def rstickup():
    print("left stick up command recieved")
    x = "\xDB"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def rstickright():
    print("left stick up command recieved")
    x = "\xDC"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def rstickdown():
    print("left stick up command recieved")
    x = "\xDD"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)


def rstickleft():
    print("left stick up command recieved")
    x = "\xDE"
    print("Sending command %s to arduino" %x)
    sendToArduino(x)

# byte assignments for commands TODO
#
# Directional command reference
#
#   DPad    Left Stick  Right Stick
#   +-+         +-+         +-+
#   |1|         |6|         |B|
# +-----+     +-----+     +-----+
# |4| |2|     |9| |7|     |E| |C|
# +-----+     +-----+     +-----+
#   |3|         |8|         |D|
#   +-+         +-+         +-+
#
#
#

commands = {
    b'\xB1': button1,
    b'\xB2': button2,
    b'\xB3': button3,
    b'\xB4': button4,
    b'\xB5': button5,
    b'\xB6': button6,
    b'\xB7': button7,
    b'\xB8': button8,
    b'\xB9': button9,
    b'\xB0': button10,
    b'\xBA': button11,
    b'\xBB': button12,
    b'\xD1': dpadup,
    b'\xD2': dpadright,
    b'\xD3': dpaddown,
    b'\xD4': dpadleft,
    b'\xD6': lstickup,
    b'\xD7': lstickright,
    b'\xD8': lstickdown,
    b'\xD9': lstickleft,
    b'\xDB': rstickup,
    b'\xDC': rstickright,
    b'\xDD': rstickdown,
    b'\xDE': rstickleft,
}

def default():
    print("Invalid joystick input")

def startServer():
    # start server
    UDP_IP = "127.0.0.1"
    UDP_PORT = 8000
    print("Starting connection with ip %s on port %d" % (UDP_IP, UDP_PORT))
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))
    while True:
        data, addr = sock.recvfrom(1024)
        try:
            commands[data]()
        except KeyError:
            default()
        # send response
        sock.sendto(data, addr)


startServer()

#
