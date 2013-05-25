// Extension board for the raspberry pi to drive a servo and receive infrared signals
// Using the IR library from http://playground.arduino.cc/Main/IRLib

#include "IR.h"
#include <Arduino.h>

void setup()
{
  Serial.begin(115200);

  IR::initialise(0); // IR receiver hardware is on pin2.
  
  Serial.println("listening for IR signals");
}

void loop()
{
  if(!IR::queueIsEmpty())
  {
    Serial.println("____");


    IR_COMMAND_TYPE code;
    while(IR::queueRead(code))
    {
      Serial.println(code, DEC);
    }
  }
}
