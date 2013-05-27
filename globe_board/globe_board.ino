// This board is placed inside a globe with buttons on each continent.
// Based on the button that's pressed, an IR code will be sent to the RaspberryPi.
// An IR LED must be connected to Arduino PWM pin 3.

#include "Arduino-IRremote/IRremote.h"
#include <avr/power.h>

IRsend irsend;

void setup()
{
	// power save
	power_adc_disable();
	power_twi_disable();
}

void loop()
{
	for(int i=0; i<3; ++i)
	{
      irsend.sendSony(0xa90, 12); // Sony TV power code
      delay(100);
    }

	delay(2000);
}

