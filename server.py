#!/usr/bin/env python
import socket
import serial

# replace [serial_item] with output of ls dev/tty*
# https://oscarliang.com/connect-raspberry-pi-and-arduino-usb-cable/
# second argument is baud rate
ser = serial.Serial('/dev/ttyACM0', 9600)
# to avoid interpreting noise, these are the first two bytes of every command
COMMAND_BYTE_PAIR = bytearray(b'\xFF\xEE')


def sendToArduino(command):
    data = COMMAND_BYTE_PAIR
    ser.write(command.encode())
    print(ser.read())


# functions to interface with arduino
def button1():
    print("Button 1 command [E_STOP] recieved")
    x = "\xB1"
    sendToArduino(x)


def button2():
    print("Button 2 [AUTO_MINE] command recieved")
    x = "\xB2"
    sendToArduino(x)


def button3():
    print("Button 3 [AUG_ON] command recieved")
    x = "\xB3"
    sendToArduino(x)


def button4():
    print("Button 4 [AUG_REV] command recieved")
    x = "\xB4"
    sendToArduino(x)


def button5():
    print("Button 5 [AUG_DOWN] command recieved")
    x = "\xB5"
    sendToArduino(x)


def button6():
    print("Button 6 [FRAME_DOWN] command recieved")
    x = "\xB6"
    sendToArduino(x)


def button7():
    print("Button 7 [AUG_UP] command recieved")
    x = "\xB7"
    sendToArduino(x)


def button8():
    print("Button 8 [FRAME_UP] command recieved")
    x = "\xB8"
    sendToArduino(x)


def button9():
    print("Button 9 command recieved")
    x = "\xB9"
    sendToArduino(x)


def button10():
    print("Button 10 command recieved")
    x = "\xB0"
    sendToArduino(x)


def button11():
    print("Button 11 command recieved")
    x = "\xBA"
    sendToArduino(x)


def button12():
    print("Button 12 command recieved")
    x = "\xBB"
    sendToArduino(x)


def dpadup():
    print("dpad up command recieved")
    x = "\xD1"
    sendToArduino(x)


def dpadright():
    print("dpad right command recieved")
    x = "\xD2"
    sendToArduino(x)


def dpaddown():
    print("dpad down command recieved")
    x = "\xD3"
    sendToArduino(x)


def dpadleft():
    print("dpad left command recieved")
    x = "\xD4"
    sendToArduino(x)


def lstickup():
    print("left stick up command recieved")
    x = "\xD6"
    sendToArduino(x)

def lstickdown():
    print("left stick down command recieved")
    x = "\xD8"
    sendToArduino(x)

def lstickneutral():
    print("left stick neutral command recieved")
    x = "\xD0"
    sendToArduino(x)

def rstickup():
    print("right stick up command recieved")
    x = "\xDB"
    sendToArduino(x)

def rstickdown():
    print("right stick down command recieved")
    x = "\xDD"
    sendToArduino(x)

def rstickneutral():
    print("right stick neutral command recieved")
    x = "\xD5"
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
    b'\xD8': lstickdown,
    b'\xDB': rstickup,
    b'\xDD': rstickdown,
    b'\xD5': rstickneutral,
    b'\xD0': lstickneutral,
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
