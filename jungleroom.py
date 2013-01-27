#! /usr/bin/python
# the main script for the jungleroom project that runs on the raspberrypi
# inspirations drawn from the following sources:
# http://www.elinux.org/RPi_Low-level_peripherals
# http://www.acmesystems.it/i2c
# http://quick2wire.com/articles/i2c-and-spi/
# http://stackoverflow.com/questions/307305/play-a-sound-with-python
# retired : 
# http://www.raspberrypi-spy.co.uk/2012/08/reading-analogue-sensors-with-one-gpio-pin/#more-520
# http://www.doctormonk.com/2012/07/raspberry-pi-gpio-driving-servo.html

#                             +---+
#              3.3v power     |* *| 5v power
#          GPIO  0 (SDA)      |* *| 5v power
#          GPIO  1 (SCL)      |* *| Ground
# servo <- GPIO  4 (GPCLK0)   |* *| GPIO 14 (TXD)
#                  Ground     |* *| GPIO 15 (RXD)
#   PIR -> GPIO 17            |* *| GPIO 18 (PCM CLK) <- BTN1 (toggle)
#          GPIO 21 (PCM DOUT) |* *| Ground
#          GPIO 22            |* *| GPIO 23           <- BTN2 
#              3.3v power     |* *| GPIO 24           <- BTN3
#   Btn -> GPIO 10 (MOSI)     |* *| Ground
#          GPIO  9 (MISO)     |* *| GPIO 25  
#          GPIO 11 (SCLK)     |* *| GPIO  8 (CEO) 
#              Ground         |* *| GPIO  7 (CE1)
#                             +---+

import RPi.GPIO as GPIO, time, smbus, os, pyglet
from AttinyStepper import AttinyStepper

GPIO.setmode(GPIO.BCM)
# Set up the GPIO channels - one input and one output
GPIO.setup( 4, GPIO.OUT)  # Servo to open/close the door
GPIO.setup(17, GPIO.IN)   # PIR
GPIO.setup(18, GPIO.IN)   # BTN1 (toggle) open and close the door with a servo
GPIO.setup(23, GPIO.IN)   # BTN2 run the stepper motor
GPIO.setup(24, GPIO.IN)   # BTN3 play the sound of an animal

i2c = smbus.SMBus(1)

tinyStep = AttinyStepper(0x10)

doorOpen = GPIO.input(18) == GPIO.HIGH


# make the blinkm dark
#i2c.write_byte(0x09, 0x6F)
#i2c.write_byte(0x09, 0x6E)
#i2c.write_byte(0x09, 0x00)
#i2c.write_byte(0x09, 0x00)
#i2c.write_byte(0x09, 0x00)

# Main program loop
while True:
	pirval   = GPIO.input(17) == GPIO.HIGH
	btn1val  = GPIO.input(18) == GPIO.HIGH
	btn2val  = GPIO.input(23) == GPIO.HIGH
	btn3val  = GPIO.input(24) == GPIO.HIGH
	lightval = 0

	if btn3val:
#		music = pyglet.resource.media('sounds/ele.wav')
#		music.play()
#		pyglet.app.run()
		print 'btn 3 pressed. playing elephant'
		os.system('aplay sounds/ele.wav')

	if btn2val:
		try:
			# let the stepper motor advance some steps
			tinyStep.stepsForward(25)
			time.sleep(3)
			tinyStep.stepsBackward(25)
			time.sleep(3)
			
		except Exception as ex:
			print 'i2c error with the stepper: %s' % str(ex)

	if doorOpen != btn1val:
		if btn1val:
			print 'open the door'
		else:
			print 'close the door'
		doorOpen = btn1val			
		# todo: talk to the servo

	try:		 
		# get the light value analog reading from the attiny
		lightval = tinyStep.readAnalog8()
	except Exception as ex:
		print 'i2c error with the light sensor : %s' % str(ex)

#	try:
		# read the barometric pressure sensor
#		i2cAddrBaro = 0x77  # 0x77 or 0x76
#		i2c.write_byte(i2cAddrBaro, 0x1E)  # send reset command
#		time.sleep(0.03)
		# read coefficients
#		coefficients = []
#		for i in range(6):
#			i2c.write_byte(i2cAddrBaro, 0xA0 + i * 2)  
#			coef = i2c.read_byte(i2cAddrBaro)
#			coef = i2c.read_byte(i2cAddrBaro) + coef * 256
#			coefficients.append(coef)
		# read pressure
#	except Exception as ex:
#		print 'i2c error baro : %s' % str(ex)


	print "%i  %i  %i  %i  %i" % (btn1val, btn2val, btn3val, pirval, lightval)  

	time.sleep(0.2)

