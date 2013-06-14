#! /bin/bash
# from http://raspberrypi.io/

# Always best to make sure your upto date
sudo apt-get update
 
# Install i2c-tools
sudo apt-get install i2c-tools python-smbus
 
#############################################
# Module Lines added to '/etc/modules' file
# Add the following lines
 
i2c-bcm2708
i2c-dev
 
#############################################
 
# Give pi user group access to the i2c ports
sudo usermod -a -G i2c pi
 
# reboot
# check to see the ports are working
sudo i2cdetect -l
 
# reboot
sudo wget http://raspberrypi.io/raspberrypi/w1-test
sudo bash w1-test

