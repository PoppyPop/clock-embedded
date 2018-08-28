#!/usr/bin/python3
#

import RPi.GPIO as GPIO
import time
import datetime
import smbus


# Start i2c
bus = smbus.SMBus(1)  

# Slave adress
DEVICE_ADDRESS = 0x12

DEVICE_REGISTER_BRIGHT = 0x42
DEVICE_BRIGHT_PLUS = 0x2B
DEVICE_BRIGHT_MINUS = 0x2D






# get enum
def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    return type('Enum', (), enums)

# Start GPIO

GPIO.setmode(GPIO.BOARD)

# 19 push = -
# 11 push = +

# Touch buttons
def touchstart(channel):	

	button = channel
	push = GPIO.input(channel)==0
	response = "Button: " + str(button) + " Action: "
	response += "Push" if push else "Release"
	print(response)
	
	if button == 19 and push:
		bus.write_byte_data(DEVICE_ADDRESS, DEVICE_REGISTER_BRIGHT, DEVICE_BRIGHT_MINUS)
	elif button == 11 and push:
		bus.write_byte_data(DEVICE_ADDRESS, DEVICE_REGISTER_BRIGHT, DEVICE_BRIGHT_PLUS)
	
	return;
    
    
GPIO.setup(11, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.remove_event_detect(11)
GPIO.add_event_detect(11, GPIO.BOTH, callback=touchstart, bouncetime=75) 

GPIO.setup(13, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.remove_event_detect(13)
GPIO.add_event_detect(13, GPIO.BOTH, callback=touchstart, bouncetime=75) 

GPIO.setup(15, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.remove_event_detect(15)
GPIO.add_event_detect(15, GPIO.BOTH, callback=touchstart, bouncetime=75) 

GPIO.setup(19, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.remove_event_detect(19)
GPIO.add_event_detect(19, GPIO.BOTH, callback=touchstart, bouncetime=75) 

try:

	while True:	
		time.sleep(1)
	
except KeyboardInterrupt:

    GPIO.cleanup()