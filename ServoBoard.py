#! /usr/bin/python

import smbus, time

class ServoBoard:
	def __init__(self, i2cSlaveAddr = 0x11, i2cBusNbr = 1):
		self.i2cSlaveAddr = i2cSlaveAddr
		self.i2c = smbus.SMBus(i2cBusNbr)

	def getInfrared(self):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA1)
		time.sleep(0.02)
		irCode = self.i2c.read_byte(self.i2cSlaveAddr)
		return irCode

	def servoPos(self, val):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA2)
		self.i2c.write_byte(self.i2cSlaveAddr, val)

	def speakerPower(self, val):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA3)
		self.i2c.write_byte(self.i2cSlaveAddr, val)

	def __repr__(self):
		print "AtMega8 interfacing to the ir receiver, servo and speaker power, at i2c address %d" % self.i2cSlaveAddr



