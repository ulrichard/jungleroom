// This board is placed inside a globe with buttons on each continent.
// Based on the button that's pressed, an IR code will be sent to the RaspberryPi.
// pin  2 serves as interrupt trigger for the buttons.
// pin  3 connects an infrared transmitter LED.
// pin 13 connects a status LED showing when the device is not at sleep 

#include "Arduino-IRremote/IRremote.h"
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>


IRsend irsend; // uses pin 3

void setup()
{
	// power save
	DDRD &= B00001111;  // set Arduino pins 4 to 7 as inputs, leaves 0 to 3 as is
	DDRB =  B00000000;  // set pins 8 to 13 as inputs
	PORTD |= B11110100; // enable pullups on pins 2 and 4 to 7, leave pins 0,1,3 alone
	PORTB |= B11111111; // enable pullups on pins 8 to 13
	
#ifdef NOT_MEGA8
	power_adc_disable();
	power_twi_disable();
#endif

	pinMode(3,  OUTPUT);    // set pin 3 as an output for the IR LED
	pinMode(13, OUTPUT);    // set pin 3 as an output for status LED

	digitalWrite(13, HIGH); // turn indicator LED on
}

void loop()
{
	// ToDo : send different codes based on the button that was pressed
	for(int i=0; i<3; ++i)
	{
		irsend.sendSony(0xa90, 12); // Sony TV power code
		delay(100);
	}

	// Stay awake for 1 second, then sleep.
	// LED turns off when sleeping, then back on upon wake.
	delay(1000);
	sleepNow();
}

void sleepNow()
{
	attachInterrupt(0, pinInterrupt, LOW); // Set pin 2 as interrupt and attach handler:
	delay(100);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // Choose our preferred sleep mode:
	
	sleep_enable();         // Set sleep enable (SE) bit:

	digitalWrite(13, LOW);  // turn LED off to indicate sleep
	
	sleep_mode();           // Put the device to sleep:
	
	sleep_disable();        // Upon waking up, sketch continues from this point.

	digitalWrite(13, HIGH); // turn LED on to indicate awake
}

void pinInterrupt(void)
{
    detachInterrupt(0);
}
