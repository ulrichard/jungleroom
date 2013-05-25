/*

 Interrupt driven S*ny SIRCS protocol receiver.
 
 Created by Charlie Robson November 2007.
 
 Released into the public domain.
 
 See header IR.h for contact details, etc.
 
 */

#include <Arduino.h>
#include "IR.h"


// Hi-res timer
//
extern volatile unsigned long timer0_overflow_count;


// Used to wrap the queue head & tail indices. Using logical AND
// to trim & wrap the value is likely faster than the MOD operator
// which incurs a division. QUEUE_LENGTH MUST BE POWER OF 2!
//
#define QUEUE_MASK (QUEUE_LENGTH-1)


// The command code queue.
//
static volatile short irQueueHead = 0;
static volatile short irQueueTail = 0;
static IR_COMMAND_TYPE irQueue[QUEUE_LENGTH];


#define FINALBIT (1<<(IR_BITS-1))


// Clear out the command queue
//
void IR::queueFlush(void)
{
  irQueueHead = 0;
  irQueueTail = 0;
}


// Returns true when a code has been read.
//
bool IR::queueRead(IR_COMMAND_TYPE& code)
{
  if (irQueueHead == irQueueTail)
  {
    // Queue is empty.
    //
    return false;
  }

  // Pop the command and return true to indicate a code was read.
  //
  code = irQueue[irQueueTail];

  // There is a slight chance that we get interrupted while
  // incrementing this. The ISR could in theory bump the tail
  // pointer if we have just started emptying a full queue. This
  // means we would lose an entry because we'd increment twice.
  // But seeing as the queue is full, then we're likely to be
  // losing data anyhoo.
  //
  irQueueTail = (irQueueTail + 1) & QUEUE_MASK;

  return true;
}


// Enquire as to whether the buffer contains any codes.
//
bool IR::queueIsEmpty()
{
  return irQueueHead == irQueueTail;
}


// The interrupt service routine.
//
// ** Is there too much being done here?? **
// I tried another method which only looked at falling edges.
// This was neater, but it did mean that it wasn't possible to
// decode the entire stream as the last bit is lost.
//
static void noteIR(void)
{
  static long start;

  // Initialise bit to 0 to prevent false starts.
  //
  static IR_COMMAND_TYPE code, bit = 0;

  if(digitalRead(2) == 0)
  {
    // Start timing on falling edge. The storing of bits may start
    // happening before a start pulse is read, but that doesn't
	// matter as long as bit is 0.
    //
    start = (timer0_overflow_count << 8) + TCNT0;
  }
  else
  {
    // This is a rising edge. Calculate low-pulse length.
    //
    long time = ((timer0_overflow_count << 8) + TCNT0) - start;

    // We see values of ~600 (~2.4ms) for the start pulse
    //
    if(time > 550 && time < 650)
    {
      // This is the start pulse. The command code arrives next,
	  // LSBit first.
      //
      // I only want to collect the command - the 1st 7 bits. The
	  // device code follows, which is either 5, 8 or 9 extra bits.
      //
      bit = 1;
      code = 0;
    }
    else if (bit)
    {
      // We see values of ~300 (~1.2ms) for '1' pulses. Half of that
	  // for '0'.
      //
      if (time > 250)
      {
        code |= bit;
      }
      if (bit & FINALBIT)
      {
        // The last bit has been received.
        //
        // I know there's a lot being done here, but we've finished
		// reading now and there's some free time before the next
		// frame arrives.
        //
        // Remote controls generally transmit the same pulse train a
		// number of times. Queue up the command code if it is the
		// second time we've received it. If the queue is full,
		// discard the head entry.
        //
        // There is no locking on the shared queue. I don't really
		// regard this as too much of an issue as the ISR only ever
		// writes to the queue, and the userland code only ever
		// reads from it.
        //
        IR_COMMAND_TYPE lastCode = irQueue[irQueueHead];
        irQueue[irQueueHead] = code;
        if (lastCode == code)
        {
          irQueueHead = (irQueueHead + 1) & QUEUE_MASK;
          if(irQueueHead == irQueueTail)
          {
            irQueueTail = (irQueueTail + 1) & QUEUE_MASK;
          }
        }
      }

      // get ready for next bit.
      //
      bit *= 2;
    }
  }
}



void IR::initialise(int pinNum)
{
  attachInterrupt(pinNum, noteIR, CHANGE);
  queueFlush();
}


void IR::uninitialise(int pinNum)
{
  detachInterrupt(pinNum);
}

