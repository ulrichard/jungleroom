/*
 Interrupt driven S*ny SIRCS protocol receiver.
 
 Created by Charlie Robson November 2007.
 
 Released into the public domain.
 
 
 Outline
 -------
 Maintains a circular queue of received IR codes. Interrupt driven
 to remove the need to poll, freeing up processor time and reducing
 constraints on the structure of the host code.
 
 SIRCS
 -----
 This protocol is well known and can be researched on many interweb
 sites. A very brief overview follows. A bitstream is introduced
 with a 2.4ms low pulse. This is then followed by 12 to 16 bits. '0'
 is coded as 0.6ms high, 0.6ms low. '1' is coded by 0.6 ms high,
 1.2ms low. The stream is 7 bits of command followed by 5 to 13 bits
 of device id code.
 
 Limitations
 -----------
 I'm only decoding the command portion of the bitstream, which only
 requires chars to transport. This is sufficient for most apps.
 Change IR_COMMAND_TYPE if you increase the bitcount in IR_BITS.
 
 Contact
 -------
 Electric letters can be posted to me here: charlie robson at
 hotmail dot com. No stamp required.
 
 All constructive comments and improvement suggestions welcomed.
 
 */

#ifndef IR_h
#define IR_h

// There are 12, 15 & 20 bit streams in existence. All stream
// lengths have 7 bit command codes.
//
#define IR_BITS 7

// 7 bits only require char type.
//
typedef short IR_COMMAND_TYPE;

// This needs to be a power of two. Can be 8,16,32,64 or more. Up to
// you :) The queue length will depend on how often you want/can
// afford to poll. In practice I've never filled a queue of 8 codes.
//
#define QUEUE_LENGTH 8

namespace IR
{
  // Attaches interrupt and clears queue
  //
  void initialise(int irPin);

  // unlikely ever to be used, supplied for convenience!
  //
  void uninitialise(int irPin);

  // Queue of received codes.
  //
  void queueFlush(void);
  bool queueRead(IR_COMMAND_TYPE& code);
  bool queueIsEmpty(void);
};

#endif // IR_h
