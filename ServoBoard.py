#! /usr/bin/python

import smbus, time

class ServoBoard:
	def __init__(self, i2cSlaveAddr = 0x11, i2cBusNbr = 1):
		self.i2cSlaveAddr = i2cSlaveAddr
		time.sleep(0.01)
		self.i2c = smbus.SMBus(i2cBusNbr)

	def getInfrared(self):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA1)
		time.sleep(0.05)
		irCode = self.i2c.read_byte(self.i2cSlaveAddr)
		return irCode

	def servoPos(self, val):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA2)
		time.sleep(0.01)
		self.i2c.write_byte(self.i2cSlaveAddr, val)
		time.sleep(0.02)

	def speakerPower(self, val):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA3)
		time.sleep(0.01)
		self.i2c.write_byte(self.i2cSlaveAddr, val)
		time.sleep(0.02)

	def clearInfrared(self):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA4)
		time.sleep(0.01)

	def __repr__(self):
		print "AtMega8 interfacing to the ir receiver, servo and speaker power, at i2c address %d" % self.i2cSlaveAddr


# test code
if __name__ == "__main__":
	sb = ServoBoard(0x11, 1) # bus is 0 on the alix, and 1 on the raspbe

	if True:
		print "speaker power"
		sb.speakerPower(1)
		time.sleep(2.0)
		sb.speakerPower(0)

	if False:
		print "servo test"
		for i in range(10):
			sb.servoPos(i * 10)
			time.sleep(1.0)

	if False:
		print "getting infrared codes"
		print "hit ctrl+c to stop"
		while True:
			irCode = sb.getInfrared()
			if irCode > 0:
				print irCode
				sb.clearInfrared()
			time.sleep(1.0)

	print 'bye'
		
		

