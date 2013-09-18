#! /usr/bin/python

import smbus, time

class AttinyStepper:
	def __init__(self, i2cSlaveAddr = 0x10, stepDuration = 50, i2cBusNbr = 1):
		self.i2cSlaveAddr = i2cSlaveAddr
		self.stepDuration = stepDuration
		self.i2c = smbus.SMBus(i2cBusNbr)
		# set the step duration
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA2)
		self.i2c.write_byte(self.i2cSlaveAddr, self.stepDuration >> 8)
		self.i2c.write_byte(self.i2cSlaveAddr, self.stepDuration & 0xFF)

	def changeSlaveAddress(self, newAddr):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA1)
		self.i2c.write_byte(self.i2cSlaveAddr, newAddr)

	def stepsForward(self, numSteps):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xB1)
		self.i2c.write_byte(self.i2cSlaveAddr, numSteps >> 8)
		self.i2c.write_byte(self.i2cSlaveAddr, numSteps & 0xFF)

	def stepsBackward(self, numSteps):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xB2)
		self.i2c.write_byte(self.i2cSlaveAddr, numSteps >> 8)
		self.i2c.write_byte(self.i2cSlaveAddr, numSteps & 0xFF)

	def readAnalog8(self):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xC1)
		time.sleep(0.02)
		lightval = self.i2c.read_byte(self.i2cSlaveAddr)
		return lightval

	def readAnalog10(self):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xC2)
		time.sleep(0.02)
		tmp = self.i2c.read_byte(self.i2cSlaveAddr)
		lightval = (self.i2c.read_byte(self.i2cSlaveAddr) << 8) + tmp
		return lightval

	def __repr__(self):
		print "attiny interfacing to the easy stepper driver at i2c address %d" % self.i2cSlaveAddr

# test code
if __name__ == "__main__":
	ts = AttinyStepper(0x10, 1) # bus is 0 on the alix, and 1 on the raspbe
	lightVal = ts.readAnalog8()
	print lightVal

