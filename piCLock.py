#!/usr/bin/python3
#

# usermod -aG audio pi
# sudo pip3 install pulsectl

import RPi.GPIO as GPIO
import time
import datetime
import smbus
import pulsectl



# Start i2c
bus = smbus.SMBus(1)  

# Slave adress
DEVICE_ADDRESS = 0x12

DEVICE_REGISTER_BRIGHT = ord("B")
DEVICE_BRIGHT_PLUS = ord("+")
DEVICE_BRIGHT_MINUS = ord("-")

DEVICE_REGISTER_TEXT = ord("T")

DEVICE_SECONDARY_MODE = ord("P")

DEVICE_RTC = ord("R")

# get enum
def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    return type('Enum', (), enums)
	
# Convert Text to byte array
def txtToByte(text):
    return [ord(i) for i in list(text)]

# Start GPIO

GPIO.setmode(GPIO.BOARD)

# 15 Tel
# 19 push = -
# 11 push = +
# 13 Snooze

nbMode=2
mode=1

# Touch buttons
def touchstart(channel):	

	global mode 
	global nbMode 

	button = channel
	push = GPIO.input(channel)==0
	response = "Button: " + str(button) + " Action: "
	response += "Push" if push else "Release"
	print(response)
	
	
	if button == 15 and push:
		mode = 1 if mode+1>nbMode else mode+1
		bus.write_i2c_block_data(DEVICE_ADDRESS, DEVICE_REGISTER_TEXT, txtToByte("So" if mode==1 else "Lu"))

	if (button == 19 or button == 11) and push:
		newVol=0
		
		if button == 19:
			brightValue = DEVICE_BRIGHT_MINUS
			newVol = newVol - 0.05
		elif button == 11:
			brightValue = DEVICE_BRIGHT_PLUS
			newVol = newVol + 0.05
		
		if mode==1:
			with pulsectl.Pulse('volume-increaser') as pulse:
				for sink in pulse.sink_list():
					# Volume is usually in 0-1.0 range, with >1.0 being soft-boosted
					pulse.volume_change_all_chans(sink, newVol)
					vol = pulse.volume_get_all_chans(sink)
					bus.write_i2c_block_data(DEVICE_ADDRESS, DEVICE_REGISTER_TEXT, txtToByte(str(int(round(vol*100)))))
		elif mode==2:
			bus.write_byte_data(DEVICE_ADDRESS, DEVICE_REGISTER_BRIGHT, brightValue)
		
	if button == 13 and push:
		bus.write_byte_data(DEVICE_ADDRESS, DEVICE_SECONDARY_MODE, 0x00)
		
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