// Timings code for [me_Ws2812b] library. Use with oscilloscope.

/*
  Author: Martin Eden
  Last mod.: 2024-12-20
*/

/*
  This code was used for observing actual signal with oscilloscope.

  Not much demo potential. It emits same pattern to all sensible pins.
  So once you connect your stripe it will lit and nothing will visibly
  changed.

  -- Martin, 2024-12-12
*/

#include <me_Ws2812b.h>

#include <me_BaseTypes.h>
#include <me_Uart.h>
#include <me_Console.h>

// Print pixel values to serial
void PrintState(
  me_Ws2812b::TLedStripeState State
)
{
  Console.Write("Output pin:");
  Console.Print(State.Pin);
  Console.EndLine();

  for (TUint_2 PixelIdx = 0; PixelIdx < State.Length; ++PixelIdx)
  {
    Console.Print(PixelIdx);
    Console.Print(State.Pixels[PixelIdx].Green);
    Console.Print(State.Pixels[PixelIdx].Red);
    Console.Print(State.Pixels[PixelIdx].Blue);
    Console.EndLine();
  }
}

/*
  Send several specific values to check timing margins with oscilloscope.
*/
void Test_ObserveBitsTiming(
  TUint_1 OutputPin
)
{
  using
    me_Ws2812b::TPixel,
    me_Ws2812b::TLedStripeState,
    me_Ws2812b::SetLedStripeState;

  /*
    I want to see timings between bits. And between bytes.

    So I need bit transitions 00, 01, 11, 10 (Gray codes huh).
    "00110" is okay.

    And same transitions when bits are between bytes.
  */
  TPixel Pixels[] =
    {
      {
        // Byte bit timings
        .Green = B00110000,
        .Red = 0,
        // Interbyte bit timings
        .Blue = 0x00,
      },
      {
        .Green = 0xFF,
        .Red = 0xFF,
        .Blue = 0x00,
      },
    };

  TLedStripeState StripeState;

  StripeState.Pixels = Pixels;
  StripeState.Length = sizeof(Pixels) / sizeof(TPixel);
  StripeState.Pin = OutputPin;

  SetLedStripeState(StripeState);

  PrintState(StripeState);
}

// --

void setup()
{
  me_Uart::Init(me_Uart::Speed_115k_Bps);

  Console.Print("[me_Ws2812b.Debug]");
}

void loop()
{
  for (TUint_1 OutputPin = 2; OutputPin <= A5; ++OutputPin)
  {
    Test_ObserveBitsTiming(OutputPin);
    delay(500);
  }
}

/*
  2024-03
  2024-04
  2024-05
  2024-12-12
*/
