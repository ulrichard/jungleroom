// This board is placed inside a globe with buttons on each continent.
// Based on the button that's pressed, a sound is played.

#include "IRremote.h"
#include "Mp3Player.h"
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>


// pin assignments
static const uint8_t PIN_IntTrigger   =  2; // INT0 -> interrupt trigger for the buttons through diodes.
static const uint8_t PIN_IR_LED       =  9; // OC1A -> infrared transmitter LED.
static const uint8_t PIN_Europe 	  =  3;
static const uint8_t PIN_Africa 	  =  4;
static const uint8_t PIN_Asia   	  =  5;
static const uint8_t PIN_NorthAmerica =  6;
static const uint8_t PIN_SouthAmerica =  7;
static const uint8_t PIN_Australia 	  =  8;
static const uint8_t PIN_Status_LED   = 13;

Mp3Player player;

void setup()
{
	// Apply power saving by not having active ouputs or floating inputs.
	DDRD  &= B00000011; // set Arduino pins 2 to 7 as inputs, leaves 0 and 1 as is
	DDRB   = B00100010; // set pins 8 and 10 to 12 as inputs, leaves 9 as is
	PORTD |= B11111100; // enable pullups on pins 2 to 7, leave pins 0 and 1 alone
	PORTB |= B00011101; // enable pullups on pins 8 and 10 to 12 as inputs, leaves 9 alone

    player.begin();

	pinMode(PIN_IR_LED,     OUTPUT);
	pinMode(PIN_Status_LED, OUTPUT);
	digitalWrite(PIN_Status_LED, HIGH);
}

void loop()
{
    playSongFromButton();

	const unsigned long irCode = getIrCodeFromButton();

	if(0 != irCode)
	{
		IRsend irsend; // uses pin 9

		for(int i=0; i<6; ++i)
		{
			irsend.sendSony(irCode, 12);
            pause(100);
		}
	}

	if(HIGH == digitalRead(PIN_IntTrigger)) // if no button is pressed
	{
		pause(5000); // Stay awake for 5 second, then sleep.
        
		if(HIGH == digitalRead(PIN_IntTrigger)) // if no button is pressed
			sleepNow();
	}
}

void playSongFromButton()
{
	if(LOW == digitalRead(PIN_Europe))
		player.PlayTitle(1);
	if(LOW == digitalRead(PIN_Asia))
		player.PlayTitle(2);
	if(LOW == digitalRead(PIN_Africa))
		player.PlayTitle(3);
	if(LOW == digitalRead(PIN_NorthAmerica))
		player.PlayTitle(4);
	if(LOW == digitalRead(PIN_SouthAmerica))
		player.PlayTitle(5);
	if(LOW == digitalRead(PIN_Australia))
		player.PlayTitle(6);

}

const unsigned long getIrCodeFromButton()
{
	if(LOW == digitalRead(PIN_Europe))
		return 0B100000001000; // 1
	if(LOW == digitalRead(PIN_Asia))
		return 0B010000001000; // 2
	if(LOW == digitalRead(PIN_Africa))
		return 0B110000001000; // 3
	if(LOW == digitalRead(PIN_NorthAmerica))
		return 0B001000001000; // 4
	if(LOW == digitalRead(PIN_SouthAmerica))
		return 0B101000001000; // 5
	if(LOW == digitalRead(PIN_Australia))
		return 0B011000001000; // 6

	return 0;
}

void pause(const uint32_t duration) // in ms
{
//	delay(duration); // does not work
	for(uint32_t i=0; i<duration; ++i)
		for(uint16_t j=0; j<100; ++j)
        {
            const int state = digitalRead(PIN_Status_LED);
			digitalWrite(PIN_Status_LED, state);
        }
}

void sleepNow()
{
	digitalWrite(PIN_Status_LED, LOW);   // turn LED off to indicate sleep
	attachInterrupt(0, wakeUpNow, LOW);  // Set pin 2 as interrupt and attach handler:
    pause(100);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Choose our preferred sleep mode
	sleep_enable();         			 // Set sleep enable (SE) bit
	sleep_mode();           			 // Put the device to sleep
	
	sleep_disable();        			 // Upon waking up, sketch continues from this point.

	digitalWrite(PIN_Status_LED, HIGH);  // turn LED on to indicate awake
    pause(100);
    player.begin();
}

void wakeUpNow(void)
{
    detachInterrupt(0); // so that it doesn't fire over and over again
}


