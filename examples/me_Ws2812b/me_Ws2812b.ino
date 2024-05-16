// Test of [me_Ws2812b] library.

/*
  Board: Arduino Uno

  Author: Martin Eden
  Development: 2024-02/2024-05
  Last mod.: 2024-05-16
*/

#include <me_Types.h> // TUint's
#include <me_UartSpeeds.h> // serial speed
#include <me_InstallStandardStreams.h> // printf() to serial
#include <me_Math.h> // deg to rad

#include <me_Ws2812b.h>

// --

const TUint_1 LedStripePin = A0;
const TUint_1 NumPixels = 60;

// Forwards
void Test_ObserveBitsTiming(TUint_1 OutputPin);
void Test_WhiteSine(TUint_2 NumPixels, TUint_1 OutputPin);
void Test_ColorSmoothing(TUint_2 NumPixels, TUint_1 OutputPin);

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  delay(500);

  InstallStandardStreams();

  printf("[me_Ws2812b]\n");
  delay(500);
}

void loop()
{
  // Choose one of the tests:

  // for (TUint_1 OutputPin = A0; OutputPin <= A4; ++OutputPin)
  //   Test_ObserveBitsTiming(OutputPin);

  Test_WhiteSine(NumPixels, LedStripePin);

  // Test_ColorSmoothing(NumPixels, LedStripePin);
}

// --

using namespace me_Ws2812b;

// --

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
  TPixel TestPixels[] =
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

  SendPixels(TestPixels, sizeof(TestPixels) / sizeof(TPixel), OutputPin);

  delay(500);
}

// --

/*
  Send rolling white sine wave.
*/
void Test_WhiteSine(TUint_2 NumPixels, TUint_1 OutputPin)
{
  TPixel Pixels[NumPixels];

  static TUint_2 BaseAngle_Deg = 0;
  const TUint_1 BaseAngleShift_Deg = 1;
  const TUint_1 AngleIncrement_Deg = 6;

  TUint_2 Angle_Deg = BaseAngle_Deg;

  for (TUint_1 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
  {
    const TUint_1
      ByteFloor = 0,
      ByteCeil = 120;

    // Map sine range [-1, 1] to byte range [0, 255]:
    TUint_1 ByteSine = (sin(DegToRad(Angle_Deg)) + 1) / 2 * 255;

    // Map byte to floor-ceiling range:
    TUint_1 EtherValue = map(ByteSine, 0, 255, ByteFloor, ByteCeil);

    /*
    printf(
      "Angle: %3u, ByteSine: %3u, Ether: %3u\n",
      Angle_Deg,
      ByteSine,
      EtherValue
    );
    */

    Pixels[PixelIdx] =
      {
        .Green = EtherValue,
        .Red = EtherValue,
        .Blue = EtherValue,
      };

    Angle_Deg = (Angle_Deg + AngleIncrement_Deg) % 360;
  }

  BaseAngle_Deg = (BaseAngle_Deg + BaseAngleShift_Deg) % 360;

  SendPixels(Pixels, NumPixels, OutputPin);

  delay(20);
}

// --
void PrintPixels(TPixel Pixels[], TUint_2 NumPixels);

/*
  Send smooth transition from <StartColor> to <EndColor>.
*/
void Test_ColorSmoothing(TUint_2 NumPixels, TUint_1 OutputPin)
{
  const TPixel StartColor = { .Green = 255, .Red = 96, .Blue = 0, };
  const TPixel EndColor = { .Green = 32, .Red = 64, .Blue = 64, };

  TPixel Pixels[NumPixels];

  for (TUint_1 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
    Pixels[PixelIdx] = MapColor(PixelIdx, NumPixels, StartColor, EndColor);

  // PrintPixels(Pixels, NumPixels);

  SendPixels(Pixels, NumPixels, OutputPin);

  delay(5000);
}

me_Ws2812b::TPixel MapColor(
  TUint_1 PixelIdx,
  TUint_2 NumPixels,
  me_Ws2812b::TPixel StartColor,
  me_Ws2812b::TPixel EndColor
)
{
  me_Ws2812b::TPixel Result;

  Result.Red =
    MapColorComponent(PixelIdx, NumPixels, StartColor.Red, EndColor.Red);
  Result.Green =
    MapColorComponent(PixelIdx, NumPixels, StartColor.Green, EndColor.Green);
  Result.Blue =
    MapColorComponent(PixelIdx, NumPixels, StartColor.Blue, EndColor.Blue);

  return Result;
}

TUint_1 MapColorComponent(
  TUint_1 PixelIdx,
  TUint_2 NumPixels,
  TUint_1 StartColorValue,
  TUint_1 EndColorValue
)
{
  return
    map(
      PixelIdx,
      0,
      NumPixels - 1,
      StartColorValue,
      EndColorValue
    );
}

// --

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
