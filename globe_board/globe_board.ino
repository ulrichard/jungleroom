// This board is placed inside a globe with buttons on each continent.
// Based on the button that's pressed, an IR code will be sent to the RaspberryPi.
// The code for IR transmission is from http://playground.arduino.cc/Main/MultiIR


#define TOPBIT 0x80000000
int timertx=0; 
int resettx=0; 
int bitstx=0; 
float elapsed=0; 
unsigned long datatx=0; 
boolean spacetx = false;

void setup()
{

  //set PWM: FastPWM, OC2A as top, OC2B sets at bottom, clears on compare
  //COM2B1=1 sets PWM active on pin3, COM2B1=0 disables it
  //Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS21);
  OCR2A = 49;
  OCR2B = 24;
  delay(500);

}

void loop()
{
  delay(2500);
  elapsed = micros();
  SonyIR(0xa90, 12);
  elapsed = 45 - ((micros() - elapsed) / 1000);
  delay(elapsed);
  SonyIR(0xa90, 12);
  delay(elapsed);
  SonyIR(0xa90, 12);
}

boolean SonyIR(unsigned long data, int nbits)
{
  if(bitstx == 0) 
  { //if IDLE then transmit 
    timertx=0; //reset timer
    TIMSK2 |= _BV(TOIE2); //activate interrupt on overflow (TOV flag, triggers at OCR2A)
    resettx=96; //initial header pulse is 2400us long. 2400/25us ticks = 96
    spacetx = false;  
    datatx=data << (32 - (nbits + 1)); //unsigned long is 32 bits. data gets   shifted so that the leftmost (MSB) will be the first of the 32.
    TCCR2A |= _BV(COM2B1); // Enable pin 3 PWM output for transmission of header
    bitstx=nbits + 1; //bits left to transmit, including header
    return true;
  }
  else
  {
    return false;
  }

}

ISR(TIMER2_OVF_vect, ISR_NOBLOCK)
{
  //RESET_TIMER2;

  //TRANSMISSION

  if(bitstx != 0)
  {  //if we have got something to transmit

    timertx++; 
    if(timertx >= resettx)
    {  //if reset value is reached

      timertx=0;
      if(spacetx)
      {  //it was a space that has just been sent thus a total "set" (bit + space) so..  
        spacetx = false; //we are not going to send a space now  
        bitstx = bitstx - 1;  //we got 1 less bit to send
        datatx <<= 1;  //shift it so MSB becomes 1st digit
        TCCR2A |= _BV(COM2B1);  //activate pin 3 PWM (ONE)
          if((datatx & TOPBIT)&&(bitstx != 0))
          {  
            resettx = 48;
          }
          else if(!(datatx & TOPBIT)&&(bitstx != 0))
          {
            resettx = 24;
          }
          else
          {
            TCCR2A &= ~(_BV(COM2B1));  //deactivate pin 3 PWM (end of transmission, no more bits to send)
            TIMSK2 &= ~(_BV(TOIE2));   //deactivate interrupts on overflow
          }
      }
      else
      {  //we sent the bit, now we have to "send" the space
        spacetx = true;  //we are sending a space
        resettx = 24; //600us/25us = 24
        TCCR2A &= ~(_BV(COM2B1));
      }
    }
  }
} 
