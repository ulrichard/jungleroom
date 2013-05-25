// Extension board for the raspberry pi to drive a servo and receive infrared signals

#include "IR.h"

void setup()
{
  Serial.begin(38400);

  // IR receiver hardware is on pin2.
  IR::initialise(0);
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
