// This board is placed inside a globe with buttons on each continent.
// Based on the button that's pressed, an IR code will be sent to the RaspberryPi.


// pin 13 connects a status LED showing when the device is not at sleep 

#include "Arduino-IRremote/IRremote.h"
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

// pin assignments
static uint8_t PIN_IntTrigger 	=  2; // interrupt trigger for the buttons through diodes.
static uint8_t PIN_IR_LED     	=  3; // infrared transmitter LED.
static uint8_t PIN_Europe 		=  4;
static uint8_t PIN_Africa 		=  5;
static uint8_t PIN_Asia   		=  6;
static uint8_t PIN_NorthAmerica =  7;
static uint8_t PIN_SouthAmerica =  8;
static uint8_t PIN_Australia 	=  9;
static uint8_t PIN_Status_LED   = 13;
#define JRG_IR_TRANSMIT_ENABLED
#define JRG_SLEEP_ENABLED

#ifdef JRG_IR_TRANSMIT_ENABLED
IRsend irsend; // uses pin 3
#endif

void setup()
{
	// power saving by not having active ouputs or floating inputs
	DDRD &= B00001111;  // set Arduino pins 4 to 7 as inputs, leaves 0 to 3 as is
	DDRB =  B00100000;  // set pins 8 to 12 as inputs
	PORTD |= B11110100; // enable pullups on pins 2 and 4 to 7, leave pins 0,1,3 alone
	PORTB |= B00011111; // enable pullups on pins 8 to 12

	pinMode(PIN_IR_LED,     OUTPUT);
	pinMode(PIN_Status_LED, OUTPUT);

	digitalWrite(PIN_Status_LED, HIGH);
}

void loop()
{
#ifdef JRG_IR_TRANSMIT_ENABLED
	const unsigned long irCode = getIrCodeFromButton();

	if(0 != irCode)
	{
		for(int i=0; i<3; ++i)
		{
			irsend.sendSony(0xa90, 12); // Sony TV power code
			delay(100);
		}
	}
#else
	for(uint8_t i=0; i<6; ++i)
	{
		digitalWrite(PIN_Status_LED, LOW);
		delay(200);
		digitalWrite(PIN_Status_LED, HIGH);
		delay(200);
	}
#endif

	// Stay awake for 1 second, then sleep.
	// LED turns off when sleeping, then back on upon wake.
	delay(1000);

#ifdef JRG_SLEEP_ENABLED
	sleepNow();
#endif
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
