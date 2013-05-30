// Extension board for the raspberry pi to drive a servo and receive infrared signals
// Using the IR library from http://playground.arduino.cc/Main/IRLib

#include "IR.h"
#include <Arduino.h>
#include <Wire.h>

void setup()
{
	Wire.begin();
	Serial.begin(115200);

	IR::initialise(0); // IR receiver hardware is on pin2.

	Serial.println("listening for IR signals");
	logToHitachiDisplay("listening for IR signals");
	logToNokiaDisplay("listening for IR signals");
}

void loop()
{
	if(!IR::queueIsEmpty())
	{
		Serial.println("____");

		IR_COMMAND_TYPE code;
		while(IR::queueRead(code))
		{
			char tmp[32];
			sprintf(tmp, "%d", code);
			Serial.println(code, DEC);
			Serial.println(tmp);
			logToHitachiDisplay(tmp);
			logToNokiaDisplay(tmp);
		}
	}
}

void logToHitachiDisplay(const char* msg)
{
	Wire.beginTransmission(0x21);
	if(Wire.endTransmission() == 0)
	{
		Wire.beginTransmission(0x21);
		Wire.write(0xB0); // command clear
		Wire.endTransmission();
                delay(50);
		Wire.beginTransmission(0x21);
		Wire.write(0xB1); // set cursor
		Wire.write(1); 
		Wire.write(1);
		Wire.endTransmission();
                delay(50);
		Wire.beginTransmission(0x21);
                uint8_t tmp[32];
                tmp[0] = 0xB0; // command print
                tmp[1] = min(30, strlen(msg));
                memcpy(tmp + 2, msg, tmp[1]);
		Wire.write(tmp, tmp[1] + 2); 
		Wire.endTransmission();
	}
}

void logToNokiaDisplay(const char* msg)
{
	Wire.beginTransmission(0x19);
	if(Wire.endTransmission() == 0)
	{
		Wire.beginTransmission(0x19);
		Wire.write(0xC1); // backlight on
		Wire.endTransmission();
                delay(50);
		Wire.beginTransmission(0x19);
		Wire.write(0xB0); // command clear
		Wire.endTransmission();
                delay(50);
		Wire.beginTransmission(0x19);
                uint8_t tmp[32];
                tmp[0] = 0xB3; // command print at pos
                tmp[1] = 1;    // X pos
                tmp[2] = 1;    // X pos
                tmp[3] = 0;    // big font
                tmp[4] = min(27, strlen(msg));
                memcpy(tmp + 5, msg, tmp[4]);
		Wire.write(tmp, tmp[4] + 5); 
		Wire.endTransmission();
                delay(50);
		Wire.beginTransmission(0x19);
		Wire.write(0xB1); // command update
		Wire.endTransmission();
                delay(50);
		Wire.beginTransmission(0x19);
		Wire.write(0xC2); // backlight off
		Wire.endTransmission();
	}
}
