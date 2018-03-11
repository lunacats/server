#!/usr/bin/env python
import socket
import serial
# replace [serial_item] with output of ls dev/tty*
# https://oscarliang.com/connect-raspberry-pi-and-arduino-usb-cable/
# second argument is baud rate
ser = serial.Serial('/dev/tty4', 9600)
# to avoid interpreting noise, these are the first two bytes of every command
COMMAND_BYTE_PAIR = bytearray(b'\xFF\xEE')

def sendToArduio(command):
	print(COMMAND_BYTE_PAIR.append(command))
	ser.write(COMMAND_BYTE_PAIR.append(command))
# functions to interface with arduino
def button1():
	sendToArduio(bytearray(b'\xB1'))
	print("Button 1 command recieved")
def button2():
	print("Button 2 command recieved")
	sendToArduio(bytearray(b'\xB2'))
def button3():
	print("Button 3 command recieved")
	sendToArduio(bytearray(b'\xB3'))
def button4():
	print("Button 4 command recieved")
	sendToArduio(bytearray(b'\xB4'))
def button5():
	print("Button 5 command recieved")
	sendToArduio(bytearray(b'\xB5'))
def button6():
	print("Button 6 command recieved")
	sendToArduio(bytearray(b'\xB6'))
def button7():
	sendToArduio(bytearray(b'\xB7'))
	print("Button 7 command recieved")
def button8():
	print("Button 8 command recieved")
	sendToArduio(bytearray(b'\xB8'))
def button9():
	print("Button 9 command recieved")
	sendToArduio(bytearray(b'\xB9'))
def button10():
	print("Button 10 command recieved")
	sendToArduio(bytearray(b'\xB0'))
def button11():
	print("Button 11 command recieved")
	sendToArduio(bytearray(b'\xBA'))
def button12():
	print("Button 12 command recieved")
	sendToArduio(bytearray(b'\xBB'))
	
# byte assignments for commands TODO
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
}

def startServer():
	# start server
	UDP_IP = "127.0.0.1"
	UDP_PORT = 8000
	print("Starting connection with ip %s on port %d" %(UDP_IP, UDP_PORT))
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.bind((UDP_IP, UDP_PORT))
	while True:
		data, addr = sock.recvfrom(1024)
		commands[data]()
		# send response
		sock.sendto(data, addr)
			
	
startServer()
