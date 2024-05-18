// Timings code for [me_Ws2812b] library. Use with oscilloscope.

/*
  Author: Martin Eden
  Last mod.: 2024-05-18
*/

#include <me_Ws2812b.h>

#include <me_UartSpeeds.h>
#include <me_InstallStandardStreams.h>
#include <me_BaseTypes.h>

using namespace me_Ws2812b;
using namespace me_BaseTypes;

// Forwards
void Test_ObserveBitsTiming(TUint_1 OutputPin);
void PrintPixels(TLedStripeState State);

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  delay(500);

  InstallStandardStreams();

  printf("[me_Ws2812b.Debug]\n");
  delay(500);
}

void loop()
{
  for (TUint_1 OutputPin = A0; OutputPin <= A4; ++OutputPin)
  {
    Test_ObserveBitsTiming(OutputPin);
    delay(500);
  }
}

/*
  Send several specific values to check timing margins with oscilloscope.
*/
void Test_ObserveBitsTiming(TUint_1 OutputPin)
{
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

// Print pixel values to serial
void PrintState(TLedStripeState State)
{
  printf("Output pin: %u\n", State.Pin);
  for (TUint_2 PixelIdx = 0; PixelIdx < State.Length; ++PixelIdx)
  {
    printf(
      "[%2u] (%02X %02X %02X)\n",
      PixelIdx,
      State.Pixels[PixelIdx].Green,
      State.Pixels[PixelIdx].Red,
      State.Pixels[PixelIdx].Blue
    );
  }
}

/*
  2024-03
  2024-04
  2024-05
*/
