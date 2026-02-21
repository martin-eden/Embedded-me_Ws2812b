// Send pixels to LED stripe WS2812B

/*
  Author: Martin Eden
  Last mod.: 2026-02-20
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
#include <me_Delays.h>
#include <me_Pins.h>
#include <me_Bits.h>
#include <me_Interrupts.h>

using namespace me_Ws2812b;

// Forwards:
static void EmitBytes(
  TAddressSegment Data,
  TAddress PortAddress,
  TUint_1 PortMask
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
  TAddressSegment DataSeg;
  me_Bits::TBitLocation PinWriteBitLoc;
  TAddress PortAddress;
  TUint_1 PortMask;

  // Fail on impossible length
  if (State.Length > MaxPixelsLength)
    return false;

  DataSeg.Addr = (TAddress) State.Pixels;
  DataSeg.Size = State.Length * sizeof(TPixel);

  if (!me_Pins::Freetown::CheckPinNumber(State.Pin))
    return false;

  me_Pins::Freetown::GetWritePinBit(&PinWriteBitLoc, State.Pin);

  PortAddress = PinWriteBitLoc.Address;
  PortMask = me_Bits::Freetown::GetBitMask(PinWriteBitLoc.BitOffset);

  if (!LedPin.Init(State.Pin))
    return false;

  LedPin.Write(0);

  me_Delays::Delay_Us(LatchDuration_Us);

  /*
    Disable interrupts while sending packet

    Or stock Arduino's Counter 1 interrupt will happen every 1024 us
    with a duration near 6 us and spoil our signal.
  */
  {
    me_Interrupts::TInterruptsDisabler NoInts;

    EmitBytes(DataSeg, PortAddress, PortMask);
  }

  LedPin.Write(0);

  me_Delays::Delay_Us(LatchDuration_Us);

  return true;
}

/*
  Meat function for emitting bytes at 800 kBits
*/
void EmitBytes(
  TAddressSegment Data,
  TAddress PortAddress,
  TUint_1 PortMask
)
{
  TUint_1 DataByte;
  TUint_1 BitCounter;
  TUint_1 PortValue;

  // Zero size? Job done!
  if (Data.Size == 0)
    return;

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
      or %[PortValue], %[PortMask]
      st %a[PortAddress], %[PortValue]

      # Extract next data bit
      lsl %[DataByte]

      brcs IsOne

    IsZero:

      # Flip to LOW
      eor %[PortValue], %[PortMask]
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
      eor %[PortValue], %[PortMask]
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
    [PortMask] "a" (PortMask),
    // Pointer to byte array in some auto-incremented register
    [Bytes] "x" (Data.Addr)
  );
}

/*
  2024 # # # #
  2025 #
  2026-02-19
*/
