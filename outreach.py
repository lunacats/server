#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  outreach.py
#
# the plan for this file is to be the client and server functionality in one. it should run at
# startup and close itself if no controller is connected to the bot. if a controller is connected, then
# it will send controller commands straight to the arduino
#
# functionality is mostly there but won't work as committed on 3/12/2018
#
# should be installed to autorun on pi for the bot once completed
#



import serial
import socket
import pygame

ser = serial.Serial('/dev/tty4', 9600)

pygame.init()  # init pygame
pygame.joystick.init()  # init pygame joysticks
joysticks = pygame.joystick.Joystick(0)

joycount = pygame.joystick.get_count()
print( joycount )
for x in (0, joycount):
    print(joysticks)  # print list of connected controllers

# The steelseries gamepad used to set this up has the following characteristics
# When running the controller test program, the axes are as follows:
#
# 0: Left stick's left-to-right. Left is -1, right is 1
# 1: Left stick's up-to-down. Up is -1, down is 1
# **When the mode light on the controller is red
# 2: Right stick's left-to-right. Left is -1, right is 1
# 3: Right stick's up-to-down. Up is -1, down is 1
#
# The buttons are straightforward. Each button has a number on it, n, and
# and number that represents in in the code, m.
# m = n - 1
# If the button says 1 on it, then the code has it as button 0
# There are 12 buttons in this manner
#
#
# This code will be compatible with other controllers, but the behavior is not guaranteed,
# don't go to competition without verifying the controller works the way it is expected to
# in the hardware it is running in
#


def handlebutton(number, value):
    """
    :param number: the number of the button, as represented in the code (m)
    :param value: pressed or released
    :return:
    """
    if value == 0:  # button has been pressed. most methods will activate on this condition
        if number == 0:   # on the steelseries gamepad, this in the top button on the right
            print("1 pressed")
            senddata(b'\xB1')
        elif number == 1: # on the steelseries gamepad, this in the right button on the right
            print("2 pressed")
            senddata(b'\xB2')
        elif number == 2: # on the steelseries gamepad, this in the bottom button on the right
            print("3 pressed")
            senddata(b'\xB3')
        elif number == 3: # on the steelseries gamepad, this in the left button on the right
            print("4 pressed")
            senddata(b'\xB4')
        elif number == 4: # on the steelseries gamepad, this in the trigger button on the left
            print("5 pressed")
            senddata(b'\xB5')
        elif number == 5: # on the steelseries gamepad, this in the trigger button on the right
            print("6 pressed")
            senddata(b'\xB6')
        elif number == 6: # on the steelseries gamepad, this in the bumper button on the left
            print("7 pressed")
            senddata(b'\xB7')
        elif number == 7: # on the steelseries gamepad, this in the bumper button on the right
            print("8 pressed")
            senddata(b'\xB8')
        elif number == 8: # on the steelseries gamepad, this is the left middle button
            print("9 pressed")
            senddata(b'\xB9')
        elif number == 9: # on the steelseries gamepad, this is the right middle button
            print("10 prssed")
            senddata(b'\xB0')
        elif number == 10: # on the steelseries gamepad, this is pressing the left control stick in
            print("11 pressed")
            senddata(b'\xBA')
        elif number == 11: # on the steelseries gamepad, this is pressing the right control stick in
            print("12 pressed")
            senddata(b'\xBB')
        else:
            print("This isn't a supported button!")
            print("This shouldn't do anything")


def handleaxis(number, value):
    if number == 0: # left stick's left to right
        if value < -0.5:
            print("Left stick to the left")
            senddata( b'\xD9')
        elif value > 0.5:
            print("Left stick to the right")
            senddata( b'\xD7')
        #else:
        #   senddata( b'\xD1')
        #   print("Left stick LR neutral")
    elif number == 1: # left stick's up to down
        if value < -0.5:
            print("Left stick forward")
            senddata( b'\xD6')
        elif value > 0.5:
            print("Left stick backward")
            senddata( b'\xD8')
        #else:
        #    senddata( b'\xD5')
        #    print("Left stick UD neutral")
        elif number == 2: # right stick's left to right
            if value < -0.5:
                print("Right stick left")
                senddata( b'\xDE')
            elif value > 0.5:
                print("Right stick right")
                senddata( b'\xDC')
        #else:
        #    senddata( b'\xDA')
        #    print("Right stick LR neutral")
    elif number == 3: # right stick's up to down
        if value < -0.5:
            senddata( b'\xDB')
            print("Right stick up")
        elif value > 0.5:
            senddata( b'\xDD')
            print("Right stick down")
        #else:
        #senddata( b'\xDA')
        #    print("Right stick UD neutral") '''


def handlehat(number, value):

    if number == 0: # left stick's left to right
        if value[0] == 0 and value[1] == 1:
            print("DPad up")
            senddata(b'\xD1')
        elif value[0] == 1 and value[1] == 0:
            print("DPad right")
            senddata(b'\xD2')
        elif value[0] == 0 and value[1] == -1:
            print("DPad down")
            senddata(b'\xD3')
        elif value[0] == -1 and value[1] == 0:
            print("DPad left")
            senddata(b'\xD4')


def handlecontrol( number, value, conttype = "button" ):
    """

    :param number: the ID number of the button, axis, or other control
    :param value:   the value inputted. the meaning will depend on control type.
                    button values are down and up for pressed and released
                    respectively
    :param conttype: default is "button". there is also "axis"
    :return:
    """
    if conttype == "button":
        handlebutton(number, value)
    elif conttype == "axis":
        handleaxis(number, value)
    elif conttype == "hat":
        handlehat(number, value)


def sendToArduino(command):
    print(COMMAND_BYTE_PAIR.append(command))
    ser.write(COMMAND_BYTE_PAIR.append(command))


def senddata( message ):
    """
    :param message: the data that gets sent to the arduino, to be handled and control the motors
    :return: no return
    """
    sendToArduino(b'\xFF')
    sendToArduino(b'\xFE')
    sendToArduino(message)


# This while loop is like the main for client.py. Python files don't necessarily need mains when used like this.


while True:  # during control loop
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    # command handling. events should be sent to server
    for event in pygame.event.get():
        if event.type == pygame.JOYAXISMOTION:
            handlecontrol(event.axis,event.value,"axis")
        if event.type == pygame.JOYBUTTONDOWN:
            handlecontrol(event.button,0,"button")
        if event.type == pygame.JOYBUTTONUP:
            handlecontrol(event.button,1,"button")
        if event.type == pygame.JOYHATMOTION:
            handlecontrol(event.hat,event.value,"hat")

#
