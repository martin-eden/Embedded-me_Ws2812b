// Timings code for [me_Ws2812b] library. Use with oscilloscope.

/*
  Board: Arduino Uno

  Author: Martin Eden
  Last mod.: 2024-05-16
*/

#include <me_Ws2812b.h>

#include <me_UartSpeeds.h>
#include <me_InstallStandardStreams.h>
#include <me_Types.h>

using namespace me_Ws2812b;

// Forwards
void Test_ObserveBitsTiming(TUint_1 OutputPin);
void PrintPixels(TPixel Pixels[], TUint_2 NumPixels);

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

  TUint_2 NumPixels = sizeof(Pixels) / sizeof(TPixel);

  SendPixels(Pixels, NumPixels, OutputPin);

  printf("Output pin: %u\n", OutputPin);
  PrintPixels(Pixels, NumPixels);
}

// Print pixel values to serial
void PrintPixels(TPixel Pixels[], TUint_2 NumPixels)
{
  for (TUint_2 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
  {
    printf(
      "[%2u] (%02X %02X %02X)\n",
      PixelIdx,
      Pixels[PixelIdx].Green,
      Pixels[PixelIdx].Red,
      Pixels[PixelIdx].Blue
    );
  }
}

/*
  2024-03
  2024-04
  2024-05
*/
