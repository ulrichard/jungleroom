// This program is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.

// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.

// This program will serve as an interface between the raspberrypi and the easystepper 
// as well as provide an analog input. Communication between the raspi and the attiny will be I2C.
// Created by Richard Ulrich <richi@paraeasy.ch>
// Inspired by : 
// http://www.schmalzhaus.com/EasyDriver/index.html
// http://arduino.cc/playground/Code/USIi2c
// http://auv.co.za/blog/attiny45quadraturedecoder
// http://correll.cs.colorado.edu/?p=1801

// ATMEL ATTINY45 / ATTINY85
//                                    +-\/-+
// PCINT5/!RESET/ADC0/dW        PB5  1|    |8  Vcc
// PCINT3/XTAL1/CLKI/!OC1B/ADC3 PB3  2|    |7  PB2 SCK/USCK/ADC1/T0/INTO/PCINT2
// PCINT4/XTAL2/CLKO/OC1B/ADC2  PB4  3|    |6  PB1 MISO/D0/OC0B/OC1A/PCINT1            pwm1
//                              GND  4|    |5  PB0 MOSI/D1/SDA/AIN0/!OC0A/AREF/PCINT0  pwm0
//                                    +----+
//                       +-\/-+
//                      1|    |8  Vcc
// stepper step      <- 2|    |7 <-  I2C_SCL
//               LDR -> 3|    |6  -> stepper direction
//               GND    4|    |5 <-> I2C_SDA 
//                       +----+

#include <inttypes.h>
#include "usiTwiSlave.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

uint8_t  EEMEM i2cSlaveAddr = 0x10; // Default address
uint16_t EEMEM stepDuration = 50;   // Default step duration

int main(void)
{
	_delay_ms(100); // give the master some time to grab the i2c bus
	sei(); 			// enable interrupts
	if(eeprom_read_byte(&i2cSlaveAddr) < 7 || eeprom_read_byte(&i2cSlaveAddr) > 77)
		eeprom_write_byte(&i2cSlaveAddr, 0x10);
	usiTwiSlaveInit(eeprom_read_byte(&i2cSlaveAddr)); // initialize i2c

	// Set Port B pins 3 and 1 as output
	DDRB = (1 << PB3) | (1 << PB1);

	// Prepare for analog input
	// Configure ADMUX register
	ADMUX = (1 << ADLAR)| // 8bit precision
			(1 << MUX1) | // Use ADC2 or PB4 pin for Vin
			(0 << REFS0)| // set refs0 and 1 to 0 to use Vcc as Vref
			(0 << REFS1);

  	//Configure ADCSRA register
	ADCSRA = (1 << ADEN)| //set ADEN bit to 1 to enable the ADC
     		 (0 << ADSC); //set ADSC to 0 to make sure no conversions are happening
	
	// the main loop
	while(1)
	{
		if(usiTwiDataInReceiveBuffer())
		{
			const uint8_t cmd = usiTwiReceiveByte();

			switch(cmd)
			{
				case 0xA1: // change the i2c address (sends 1 byte)
				{
					uint8_t recv = usiTwiReceiveByte();
					if(recv < 7 || recv > 77)
						continue;
					eeprom_write_byte(&i2cSlaveAddr, recv);
					usiTwiSlaveInit(recv); // initialize i2c
					break;
				}
				case 0xA2: // set the duration of a single step (sends 2 bytes)
				{
					uint16_t dur = usiTwiReceiveByte() << 8;
					dur         += usiTwiReceiveByte();
					eeprom_write_word(&stepDuration, dur);
					break;
				}
				case 0xB1: // move stepper forward   (sends 2 bytes)
					PORTB |= (1 << PB1); // stepper direction forward
				case 0xB2: // move stepper backwards (sends 2 bytes)
				{
					uint16_t steps  = (uint16_t)(usiTwiReceiveByte()) << 8;
					steps          += usiTwiReceiveByte();
					uint16_t dur2   = eeprom_read_word(&stepDuration) / 2;
					for(uint16_t i=0; i<steps; ++i)
					{
						PORTB |= (1 << PB3);
						for(uint16_t j=0; j<dur2; ++j)
							_delay_ms(1);

						PORTB &= ~(1 << PB3);
						for(uint16_t j=0; j<dur2; ++j)
							_delay_ms(1);
					}
					PORTB &= ~(1 << PB1); // stepper direction low
					break;
				}
				case 0xC1: // query analog value of ADC2 with 8 bit (expects 1 byte)
				{
					ADMUX |= (1 << ADLAR); // 8 bit precision
					// set ADSC pin to 1 in order to start reading the AIN value
					ADCSRA |= (1 << ADSC);
					while(((ADCSRA >> ADSC) & 1)) 
						; // do nothing until the ADSC pin returns back to 0;
					const uint8_t analogval = ADCH; //for 8 bit precision we can just read ADCH
					// send the value from the analog input
					usiTwiTransmitByte(analogval);
					break;
				}
				case 0xC2: // query analog value of ADC2 with 10 bit (expects 2 byte)
				{
					ADMUX &= ~(1 << ADLAR); // 10 bit precision
					// set ADSC pin to 1 in order to start reading the AIN value
					ADCSRA |= (1 << ADSC);
					while(((ADCSRA >> ADSC) & 1)) 
						; // do nothing until the ADSC pin returns back to 0;
					uint8_t analogval = ADCL;
					usiTwiTransmitByte(analogval);
					analogval = ADCH;
					usiTwiTransmitByte(analogval);
					break;
				}
			}; // switch
			continue;
		}

		_delay_ms(10);
	}

	return 1;
}

