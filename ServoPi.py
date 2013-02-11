#! /usr/bin/python
# see : http://docs.python.org/2/library/thread.html

import time, thread, RPi.GPIO as GPIO

class ServoPi:
	def __init__(self, gpioPin, leftDuration = 0.001, rightDuration = 0.002, cycleDuration = 0.02):
		self.terminate = False
		self.active = False
		self.gpioPin = gpioPin
		self.leftDuration  = leftDuration
		self.rightDuration = rightDuration
		self.cycleDuration = cycleDuration
		self.currentDuration = leftDuration
		self.threadId = thread.start_new_thread(threadFunc)
		self.argLock  = thread.allocate_lock()
	
	def enable(val):
		with self.argLock:
			self.active = val

	def goTo(val): # fast transition -> val shall be a float between 0.0 and 1.0
		interv = self.rightDuration - self.leftDuration
		with self.argLock:
			self.currentDuration = self.leftDuration + val * interv
			self.active = True

	def move(val, dur): # slow transition -> val shall be a float between 0.0 and 1.0
		goTo(val)
		
	def threadFunc():
		gpioPin = self.gpioPin
		leftDuration  = self.leftDuration
		rightDuration = self.rightDuration
		while True:
			with self.argLock:
				if !self.terminate:
					break
				if !self.active:
					time.sleep(100)
					continue
				
				cycleDuration = self.cycleDuration
				currentDuration = self.leftDuration

				GPIO.output(gpioPin, False)
				time.sleep(currentDuration)
				GPIO.output(gpioPin, True)
				time.sleep(cycleDuration)
				
		
