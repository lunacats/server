#!/usr/bin/env python
import socket
# functions to interface with arduino
def killSwitch():
	print('kill switch pressed')

def rlAuger():
	print('raise/lower auger pressed')
		
def mine():
	print('mine pressed')
		
def conveyer():
	print('conveyer pressed')
		
def startAuto():
	print('start autonomous pressed')
		
# byte assignments for commands TODO
commands = {
	b'\x00': killSwitch,
	b'\xA5': rlAuger,
	b'\xB5': mine,
	b'\xC5': conveyer,
	b'\xFF': startAuto
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
