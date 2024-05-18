// Send pixels to LED stripe WS2812B

/*
  Author: Martin Eden
  Last mod.: 2024-05-18
*/

/*
  Protocol summary

    Packet
      Sequence of colors
      LOW >= 50 us after sending sequence

    Color
      Three bytes
      Order is Green-Red-Blue

    Bits
      Sent from highest to lowest

      SendBit
      ~~~~~~~
        | HIGH
        | Wait_ns(0: 350, 1: 900)
        | LOW
        | Wait_ns(0: 900, 1: 350)

      Bit rate is 800 kBits (1250 ns per bit)
*/

/*
  Real world

    What really critical is the length of HIGH pulse for bit 0.
    It should be less than 500 ns. Lengths of LOW strides can
    be quite longer. So code execution overhead for data extraction
    is not critical.
*/

#include "me_Ws2812b.h"

#include <stdio.h> // printf()
#include <Arduino.h> // delayMicroseconds()

#include <me_UnoAddresses.h> // GetPinAddress_Bits()
#include <me_MemorySegment.h>
#include <me_MemoryPoint.h>
#include <me_BaseTypes.h>

using namespace me_Ws2812b;
using namespace me_MemorySegment;
using namespace me_MemoryPoint;
using namespace me_BaseTypes;

// Forwards:
TBool EmitBytes(TMemorySegment Data, TUint_1 Pin)
  __attribute__ ((optimize("O0")));
//

/*
  Set stripe to given state.

  Number of pixels, their colors and output pin -
  all described in state.
*/
TBool me_Ws2812b::SetLedStripeState(TLedStripeState State)
{
  TUint_2 PixMemSize; // length of memory segment with pixels data

  // Set <PixMemSize>
  {
    const TUint_2 MaxPixelsLength = 0xFFFF / sizeof(TPixel);

    if (State.Length > MaxPixelsLength)
    {
      printf(
        "Send(): <.Length> is %u and is too long. Max value is %u.\n",
        State.Length,
        MaxPixelsLength
      );
      return false;
    }

    PixMemSize = State.Length * sizeof(TPixel);
  }

  // Prepare for transmission
  pinMode(State.Pin, OUTPUT);
  digitalWrite(State.Pin, LOW);

  // Transmission
  TMemorySegment DataSeg;

  DataSeg.Start = (TMemoryPoint) State.Pixels;
  DataSeg.Size = PixMemSize;

  TBool Result = EmitBytes(DataSeg, State.Pin);

  // End transmission: send reset - keep LOW for 50+ us
  const TUint_2 LatchDuration_us = 50;
  delayMicroseconds(LatchDuration_us);

  return Result;
}

/*
  Meat function for emitting bytes at 800 kBits
*/
TBool EmitBytes(TMemorySegment Data, TUint_1 Pin)
{
  // Populate <PortAddress> and <PortOrMask> from <Pin>
  TUint_2 PortAddress;
  TUint_1 PortOrMask;
  {
    TMemoryPoint_Bits PinAddress;

    TBool IsOk =
      me_UnoAddresses::GetPinAddress_Bits(&PinAddress, Pin);

    if (!IsOk)
    {
      printf("me_Ws2812b: Can't figure out port address for pin %d.\n", Pin);
      return false;
    }

    PortAddress = PinAddress.Base;
    PortOrMask = (1 << PinAddress.BitOffs);
  }

  TUint_1 DataByte;
  TUint_1 BitCounter;
  TUint_1 PortValue;

  /*
    Disable interrupts while sending packet. Or something will happen
    every 1024 us with a duration near 6 us and spoil our signal.

    Interrupt flag is stored among other things in SREG.
  */
  TUint_1 OrigSreg = SREG;
  cli();

  /*
    Double "for" in GNU asm.

    Implementation details

      * Data bytes counter and bits counter are decremented till zero.
        Cleaner assembly code.

      * Bits counter is decremented inside "if"s. We have time slots
        there.

      * There are no instructions to get/set bit by variable index.

        Get:

          We need bits from highest to lowest in data byte. So we can
          AND with 0x80 and shift left. But there is better option.

          "lsl" (Logical Shift Left) stores high bit in carry flag and
          shifts left. (Actually it's translated to "add <x>, <x>".)

        Set:

          We need output to specific pin. It means we need to write
          some bit at some byte address.

          We have port address and bit number. We are creating bit masks
          for OR and AND. Reading byte from port at beginning, then
          OR-ing and writing back to set HIGH. AND-ing to set LOW.
  */
  asm volatile
  (
    R"(

    # Init

      # Weird instructions to locate this place in disassembly
      ldi %[BitCounter], 0xA9
      ldi %[BitCounter], 0xAA

      ld %[PortValue], %a[PortAddress]

    DataLoop_Start:

      ld %[DataByte], %a[Bytes]+

      # Eight bits in byte
      ldi %[BitCounter], 8

    BitLoop_Start:

      # Output HIGH
      or %[PortValue], %[PortOrMask]
      st %a[PortAddress], %[PortValue]

      # Extract next data bit
      lsl %[DataByte]

      brcs IsOne

    IsZero:

      # Flip to LOW
      eor %[PortValue], %[PortOrMask]
      st %a[PortAddress], %[PortValue]

      nop
      nop
      nop
      nop

      nop
      nop
      nop
      nop

      dec %[BitCounter]
      breq BitLoop_End
      rjmp BitLoop_Start

    IsOne:

      nop
      nop
      nop
      nop

      nop
      nop
      nop

      # Flip to LOW
      eor %[PortValue], %[PortOrMask]
      st %a[PortAddress], %[PortValue]

      dec %[BitCounter]
      breq BitLoop_End
      rjmp BitLoop_Start

    BitLoop_End:

    DataLoop_Next:

      sbiw %[RemainedLength], 1
      brne DataLoop_Start

    )"
    :
    // temporary memory
    [RemainedLength] "+w" (Data.Size),
    [DataByte] "=la" (DataByte),
    [PortValue] "=a" (PortValue),
    [BitCounter] "=a" (BitCounter)
    :
    // Pointer to port address
    [PortAddress] "z" (PortAddress),
    [PortOrMask] "a" (PortOrMask),
    // Pointer to byte array in some auto-incremented register
    [Bytes] "x" (Data.Start)
  );

  SREG = OrigSreg;

  return true;
}

/*
  2024-03 Core
  2024-04 Cleanup
  2024-05 Core change to support variable pins
*/
