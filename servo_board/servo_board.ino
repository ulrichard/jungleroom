// Extension board for the raspberry pi to drive a servo and receive infrared signals
// Using the IR library from http://playground.arduino.cc/Main/IRLib

#include "IR.h"
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

static const uint8_t PIN_IrReceiver   =  2; // fix by the IR library
static const uint8_t PIN_Servo        =  3;
static const uint8_t PIN_SpeakerPower =  4;
static const uint8_t PIN_IrStatus_LED = 13;

uint8_t recvBuffer[16]; // i2c receive buffer
uint8_t recvPos;        // position in the recvBuffer
unsigned long recvLast; // time of last received i2c data
int8_t  lastIrCode;     // our codes are small enough to use a type with atomic access, negative means no code
Servo myservo;			// top open the door of the raspberry

void setup()
{
    recvPos = 0;
    recvLast = millis();

	Wire.begin(0x11); 			// join i2c bus with address #0x11
    Wire.onReceive(receiveI2C); // register event

//	Serial.begin(115200);

	myservo.attach(PIN_Servo);
	myservo.write(10);

	lastIrCode = -1;
	IR::initialise(0); // IR receiver hardware is on pin2.
//	Serial.println("listening for IR signals");
	logToNokiaDisplay("listening for IR signals");

	pinMode(PIN_SpeakerPower, OUTPUT);
	digitalWrite(PIN_SpeakerPower, LOW);

	pinMode(PIN_IrStatus_LED, OUTPUT);
	digitalWrite(PIN_IrStatus_LED, HIGH);
	delay(500);
	digitalWrite(PIN_IrStatus_LED, LOW);
	delay(500);
	digitalWrite(PIN_IrStatus_LED, HIGH);
	delay(500);
	digitalWrite(PIN_IrStatus_LED, LOW);
}

void loop()
{
	HandleI2cCommands();


	if(!IR::queueIsEmpty())
	{
		digitalWrite(PIN_IrStatus_LED, HIGH);
//		Serial.println("____");

		IR_COMMAND_TYPE irCode;
		while(IR::queueRead(irCode))
		{
			lastIrCode = irCode;

			char tmp[32];
			sprintf(tmp, "%d", irCode);
//			Serial.println(lastIrCode, DEC);
//			Serial.println(tmp);
			logToNokiaDisplay(tmp);
		}
		digitalWrite(PIN_IrStatus_LED, LOW);
	}
}

void HandleI2cCommands()
{
    if(recvPos < 1)
        return;        
    if(recvLast + 3000 < millis())
	{
        recvPos = 0;  // reset if we didn't receive anything for more than three seconds
		digitalWrite(PIN_IrStatus_LED, HIGH);
        delay(50);
        digitalWrite(PIN_IrStatus_LED, LOW);
	}
        
    switch(recvBuffer[0])
    {
        case 0xA1: // get last IR code
			Wire.write(lastIrCode);
			lastIrCode = -1;
            break;

        case 0xA2: // move servo to position
			myservo.write(recvBuffer[1]);
            break;

        case 0xA3: // speaker power
			digitalWrite(PIN_SpeakerPower, 0 == recvBuffer[1] ? LOW : HIGH);
            break;

        default:
            // invalid command. just reset below          
            digitalWrite(PIN_IrStatus_LED, HIGH);
            delay(500);
            digitalWrite(PIN_IrStatus_LED, LOW);
    }
   
    // if we get here, assume that the command was executed
    // so we can reset the receive buffer
    recvPos = 0;
}

void receiveI2C(int howMany)
{
    for(int i=0; i<howMany; ++i)
    {
        const uint8_t val = Wire.read();
        
        if(recvPos + 1 < sizeof(recvBuffer))
            recvBuffer[recvPos++] = val;
    }
    recvLast = millis();
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
