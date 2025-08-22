// Send pixels to LED stripe WS2812B

/*
  Author: Martin Eden
  Last mod.: 2025-08-22
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
#include <me_Delays.h>
#include <me_Pins.h>

#include <avr/common.h> // SREG
#include <avr/interrupt.h> // cli()

using namespace me_Ws2812b;

// Forwards:
TBool EmitBytes(
  me_MemorySegment::TMemorySegment Data,
  me_Pins::TPinLocation PinRef
) __attribute__ ((optimize("O0")));
//

/*
  Set stripe to given state

  State provides pixels and output pin.
*/
TBool me_Ws2812b::SetLedStripeState(
  TLedStripeState State
)
{
  const TUint_2 LatchDuration_Us = 50;
  const TUint_2 MaxPixelsLength = TUint_2_Max / sizeof(TPixel);

  me_Pins::TOutputPin LedPin;
  me_MemorySegment::TMemorySegment DataSeg;
  me_Pins::TPinLocation PinRef;

  // Fail on impossible length
  if (State.Length > MaxPixelsLength)
    return false;

  DataSeg.Addr = (TAddress) State.Pixels;
  DataSeg.Size = State.Length * sizeof(TPixel);

  if (!me_Pins::Freetown::InitPinRecord(&PinRef, State.Pin))
    return false;

  if (!LedPin.Init(State.Pin))
    return false;

  LedPin.Write(0);
  me_Delays::Delay_Us(LatchDuration_Us);

  if (!EmitBytes(DataSeg, PinRef))
    return false;

  LedPin.Write(0);
  me_Delays::Delay_Us(LatchDuration_Us);

  return true;
}

/*
  Meat function for emitting bytes at 800 kBits
*/
TBool EmitBytes(
  me_MemorySegment::TMemorySegment Data,
  me_Pins::TPinLocation PinRef
)
{
  TUint_2 PortAddress;
  TUint_1 PortOrMask;

  TUint_1 DataByte;
  TUint_1 BitCounter;
  TUint_1 PortValue;

  TUint_1 OrigSreg;

  PortAddress = PinRef.BaseAddress;
  PortOrMask = (1 << PinRef.PinOffset);

  // Zero size? Job done!
  if (Data.Size == 0)
    return true;

  /*
    Disable interrupts while sending packet. Or something will happen
    every 1024 us with a duration near 6 us and spoil our signal.

    Interrupt flag is stored among other things in SREG.
  */
  OrigSreg = SREG;

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
      # ldi %[BitCounter], 0xA9
      # ldi %[BitCounter], 0xAA

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
  2025-08-22
*/
