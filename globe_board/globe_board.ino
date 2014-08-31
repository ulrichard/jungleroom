// This board is placed inside a globe with buttons on each continent.
// Based on the button that's pressed, an IR code will be sent to the RaspberryPi.

// Either use sound directly or send infrared commands
#define USE_SOUND

#ifdef USE_SOUND
  #include <SD.h>  // library for reading and writing to SD cards
  #include "TMRpcm/TMRpcm.h"  // library for playing audio wav files
#else
  #include "IRremote.h"
#endif
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>


// pin assignments
static const uint8_t PIN_IntTrigger   =  2; // INT0 -> interrupt trigger for the buttons through diodes.
#ifdef USE_SOUND
static const uint8_t PIN_SD_ChipSel   = 10; //using digital pin 4 on arduino nano 328
static const uint8_t PIN_SPEAKER      =  9; // OC1A -> speaker or piezo
#else
static const uint8_t PIN_IR_LED       =  9; // OC1A -> infrared transmitter LED.
#endif
static const uint8_t PIN_Europe 	  =  3;
static const uint8_t PIN_Africa 	  =  4;
static const uint8_t PIN_Asia   	  =  5;
static const uint8_t PIN_NorthAmerica =  6;
static const uint8_t PIN_SouthAmerica =  7;
static const uint8_t PIN_Australia 	  =  8;
static const uint8_t PIN_Status_LED   = 13;

#ifdef USE_SOUND
TMRpcm tmrpcm;
#endif

void setup()
{
	// Apply power saving by not having active ouputs or floating inputs.
	DDRD  &= B00000011; // set Arduino pins 2 to 7 as inputs, leaves 0 and 1 as is
	DDRB   = B00100010; // set pins 8 and 10 to 12 as inputs, leaves 9 as is
	PORTD |= B11111100; // enable pullups on pins 2 to 7, leave pins 0 and 1 alone
	PORTB |= B00011101; // enable pullups on pins 8 and 10 to 12 as inputs, leaves 9 alone

#ifdef USE_SOUND
    if(!SD.begin(PIN_SD_ChipSel))
        return; // early exit
    tmrpcm.play("hello.wav");
    
    tmrpcm.speakerPin = PIN_SPEAKER;
#else
	pinMode(PIN_IR_LED,     OUTPUT);
	pinMode(PIN_Status_LED, OUTPUT);
	digitalWrite(PIN_Status_LED, HIGH);
#endif
}

void loop()
{
#ifdef USE_SOUND
	if(LOW == digitalRead(PIN_Europe))
        tmrpcm.play("europe.wav");
	else if(LOW == digitalRead(PIN_Asia))
        tmrpcm.play("asia.wav");
	else if(LOW == digitalRead(PIN_Africa))
        tmrpcm.play("africa.wav");
	else if(LOW == digitalRead(PIN_NorthAmerica))
        tmrpcm.play("northamerica.wav");
	else if(LOW == digitalRead(PIN_SouthAmerica))
        tmrpcm.play("southamerica.wav");
	else if(LOW == digitalRead(PIN_Australia))
        tmrpcm.play("australia.wav");
#else
	const unsigned long irCode = getIrCodeFromButton();

	if(0 != irCode)
	{
		IRsend irsend; // uses pin 9

		for(int i=0; i<6; ++i)
		{
			irsend.sendSony(irCode, 12);
//			delay(100); // doesn't work
			for(uint16_t i=0; i<10; ++i)
				for(uint16_t j=0; j<1000; ++j)
					digitalWrite(PIN_Status_LED, HIGH);
		}
	}
#endif

	if(HIGH == digitalRead(PIN_IntTrigger)) // if no button is pressed
	{
		// Stay awake for 1/2 second, then sleep.
		for(uint16_t i=0; i<100; ++i)
			for(uint16_t j=0; j<1000; ++j)
				digitalWrite(PIN_Status_LED, HIGH);

		if(HIGH == digitalRead(PIN_IntTrigger)) // if no button is pressed
			sleepNow();
	}
}


#ifndef USE_SOUND
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
#endif

void sleepNow()
{
	digitalWrite(PIN_Status_LED, LOW);   // turn LED off to indicate sleep
	attachInterrupt(0, wakeUpNow, LOW);  // Set pin 2 as interrupt and attach handler:
//	delay(100); // does not work
	for(uint16_t i=0; i<10; ++i)
		for(uint16_t j=0; j<1000; ++j)
			digitalWrite(PIN_Status_LED, LOW);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Choose our preferred sleep mode
	sleep_enable();         			 // Set sleep enable (SE) bit
	sleep_mode();           			 // Put the device to sleep
	
	sleep_disable();        			 // Upon waking up, sketch continues from this point.

	digitalWrite(PIN_Status_LED, HIGH);  // turn LED on to indicate awake
}

void wakeUpNow(void)
{
    detachInterrupt(0); // so that it doesn't fire over and over again
}


