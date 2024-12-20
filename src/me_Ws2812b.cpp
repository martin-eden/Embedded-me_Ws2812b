// Send pixels to LED stripe WS2812B

/*
  Author: Martin Eden
  Last mod.: 2024-12-20
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
    It should be less than 500 ns. Lengths of LOW tails can
    be quite longer. So some additional inter-bit and inter-byte
    time overhead is tolerable because it occurs at LOW times.
*/

#include <me_Ws2812b.h>

#include <me_BaseTypes.h>
#include <me_MemorySegment.h>
#include <me_UnoAddresses.h> // GetPinAddress_Bits()

#include <Arduino.h> // delayMicroseconds()

using namespace me_Ws2812b;

using
  me_MemorySegment::TMemorySegment;

// Forwards:
TBool EmitBytes(TMemorySegment Data, TUint_1 Pin)
  __attribute__ ((optimize("O0")));
//

/*
  Set stripe to given state.

  Number of pixels, their colors and output pin -
  all described in state.
*/
TBool me_Ws2812b::SetLedStripeState(
  TLedStripeState State
)
{
  TUint_2 PixMemSize; // length of memory segment with pixels data

  // Set <PixMemSize>
  {
    const TUint_2 MaxPixelsLength = 0xFFFF / sizeof(TPixel);

    if (State.Length > MaxPixelsLength)
      return false;

    PixMemSize = State.Length * sizeof(TPixel);
  }

  // Prepare for transmission
  pinMode(State.Pin, OUTPUT);
  digitalWrite(State.Pin, LOW);

  // Transmission
  TMemorySegment DataSeg;

  DataSeg.Addr = (TUint_2) State.Pixels;
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
TBool EmitBytes(
  TMemorySegment Data,
  TUint_1 Pin
)
{
  // Populate <PortAddress> and <PortOrMask> from <Pin>
  TUint_2 PortAddress;
  TUint_1 PortOrMask;
  {
    TUint_2 PinAddress;
    TUint_1 PinBit;

    TBool IsOk =
      me_UnoAddresses::GetPinAddress(&PinAddress, &PinBit, Pin);

    if (!IsOk)
      return false;

    PortAddress = PinAddress;
    PortOrMask = (1 << PinBit);
  }

  // Zero size? Job done!
  if (Data.Size == 0)
    return true;

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

          We have port address and bit number. We are creating bit mask
          with that bit set to 1. OR-ing with it will set pin to HIGH.
          Next XOR-ing will set pin to LOW.
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
    [Bytes] "x" (Data.Addr)
  );

  SREG = OrigSreg;

  return true;
}

/*
  2024-03 Core
  2024-04 Cleanup
  2024-05 Core change to support variable pins
  2024-12-20
*/
