// This board is placed inside a globe with buttons on each continent.
// Based on the button that's pressed, an IR code will be sent to the RaspberryPi.

#include "IRremote.h"
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

// pin assignments
static uint8_t PIN_IntTrigger 	=  2; // INT0 -> interrupt trigger for the buttons through diodes.
static uint8_t PIN_IR_LED     	=  9; // OC1A -> infrared transmitter LED.
static uint8_t PIN_Europe 		=  3;
static uint8_t PIN_Africa 		=  4;
static uint8_t PIN_Asia   		=  5;
static uint8_t PIN_NorthAmerica =  6;
static uint8_t PIN_SouthAmerica =  7;
static uint8_t PIN_Australia 	=  8;
static uint8_t PIN_Status_LED   = 13;

void setup()
{
	// Apply power saving by not having active ouputs or floating inputs.
	DDRD  &= B00000011; // set Arduino pins 2 to 7 as inputs, leaves 0 and 1 as is
	DDRB   = B00100010; // set pins 8 and 10 to 12 as inputs, leaves 9 as is
	PORTD |= B11111100; // enable pullups on pins 2 to 7, leave pins 0 and 1 alone
	PORTB |= B00011101; // enable pullups on pins 8 and 10 to 12 as inputs, leaves 9 alone

	pinMode(PIN_IR_LED,     OUTPUT);
	pinMode(PIN_Status_LED, OUTPUT);
	digitalWrite(PIN_Status_LED, HIGH);
}

void loop()
{
	const unsigned long irCode = 0xa90; //getIrCodeFromButton();

	if(0 != irCode)
	{
		IRsend irsend; // uses pin 9

		for(int i=0; i<3; ++i)
		{
			irsend.sendSony(irCode, 12); // Sony TV power code
			delay(100);
		}
	}


	// Stay awake for 1 second, then sleep.
	// LED turns off when sleeping, then back on upon wake.
	delay(1000);

//	sleepNow();
}

const unsigned long getIrCodeFromButton()
{
	if(LOW == digitalRead(PIN_Europe))
		return 0xa90;
	if(LOW == digitalRead(PIN_Asia))
		return 0xa91;
	if(LOW == digitalRead(PIN_Africa))
		return 0xa92;
	if(LOW == digitalRead(PIN_NorthAmerica))
		return 0xa93;
	if(LOW == digitalRead(PIN_SouthAmerica))
		return 0xa94;
	if(LOW == digitalRead(PIN_Australia))
		return 0xa95;

	return 0;
}

void sleepNow()
{
	digitalWrite(PIN_Status_LED, LOW);  // turn LED off to indicate sleep
	attachInterrupt(0, wakeUpNow, LOW); // Set pin 2 as interrupt and attach handler:
	delay(100);

	// The 5 different modes are:
	// SLEEP_MODE_IDLE         -the least power savings 
	// SLEEP_MODE_ADC
	// SLEEP_MODE_PWR_SAVE
	// SLEEP_MODE_STANDBY
	// SLEEP_MODE_PWR_DOWN     -the most power savings
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // Choose our preferred sleep mode:
	sleep_enable();         // Set sleep enable (SE) bit:
	sleep_mode();           // Put the device to sleep:
	
	sleep_disable();        // Upon waking up, sketch continues from this point.

	digitalWrite(PIN_Status_LED, HIGH); // turn LED on to indicate awake
}

void wakeUpNow(void)
{
    detachInterrupt(0); // so that it doesn't fire over and over again
}
