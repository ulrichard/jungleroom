#! /usr/bin/python
# see : http://docs.python.org/2/library/thread.html

import time, thread, threading, RPi.GPIO as GPIO

class ServoPi(threading.Thread):
	def __init__(self, gpioPin, leftDuration = 0.001, rightDuration = 0.002, cycleDuration = 0.02):
		threading.Thread.__init__(self)
		self.terminate = False
		self.active = False
		self.gpioPin = gpioPin
		self.leftDuration  = leftDuration
		self.rightDuration = rightDuration
		self.cycleDuration = cycleDuration
		self.currentDuration = leftDuration
		self.argLock  = thread.allocate_lock()

		GPIO.setup(gpioPin, GPIO.OUT)

		self.start()
	
	def stop(self):
		with self.argLock:
			self.terminate = True
		self.join()

	def enable(self, val):
		with self.argLock:
			self.active = val

	def goTo(self, val): # fast transition -> val shall be a float between 0.0 and 1.0
		print 'ServoPi.goTo(%f)' % val
		interv = self.rightDuration - self.leftDuration
		with self.argLock:
			self.currentDuration = self.leftDuration + val * interv
			self.active = True

	def move(self, val, dur): # slow transition -> val shall be a float between 0.0 and 1.0
		self.goTo(val)
		
	def run(self):
		print 'entering ServoPi.run()'
		gpioPin = self.gpioPin
		leftDuration  = self.leftDuration
		rightDuration = self.rightDuration
		while True:
			with self.argLock:
				if self.terminate:
					break
				if not self.active:
					time.sleep(100)
					continue
				
				cycleDuration = self.cycleDuration
				currentDuration = self.leftDuration

			GPIO.output(gpioPin, False)
			time.sleep(currentDuration)
			GPIO.output(gpioPin, True)
			time.sleep(cycleDuration)
				
		
# test code
if __name__ == "__main__":
	GPIO.setmode(GPIO.BCM)
	servo = ServoPi(4, 0.002, 0.001, 0.02)
	servo.move(0, 1)
	time.sleep(1)
	servo.move(0.5, 1)
	time.sleep(1)
	servo.move(1, 1)
	time.sleep(1)
	servo.move(0, 1)
	

