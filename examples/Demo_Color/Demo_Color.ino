// Color output via [me_Ws2812b]

/*
  Board: Arduino Uno

  Author: Martin Eden
  Last mod.: 2024-05-16
*/

#include <me_Ws2812b.h>

#include <me_UartSpeeds.h>
#include <me_InstallStandardStreams.h>
#include <me_Types.h>

const TUint_1 LedStripePin = A0;
const TUint_1 NumPixels = 60;

// Forwards
void Test_ColorSmoothing(TUint_2 NumPixels, TUint_1 OutputPin);

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  delay(500);

  InstallStandardStreams();

  printf("[me_Ws2812b.Demo_Color]\n");
  delay(500);

  Test_ColorSmoothing(NumPixels, LedStripePin);
}

void loop()
{
}

using namespace me_Ws2812b;

// --

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

  SendPixels(Pixels, NumPixels, OutputPin);
}

/*
  Interpolate color between <StartColor> and <EndColor> for
  <NumPixels> datapoints.
*/
me_Ws2812b::TPixel MapColor(
  TUint_1 PixelIdx,
  TUint_2 NumPixels,
  me_Ws2812b::TPixel StartColor,
  me_Ws2812b::TPixel EndColor
)
{
  me_Ws2812b::TPixel Result;

  Result.Red =
    MapIndex(PixelIdx, NumPixels, StartColor.Red, EndColor.Red);
  Result.Green =
    MapIndex(PixelIdx, NumPixels, StartColor.Green, EndColor.Green);
  Result.Blue =
    MapIndex(PixelIdx, NumPixels, StartColor.Blue, EndColor.Blue);

  return Result;
}

/*
  Linearly interpolate index between 0 and (<NumValues> - 1)
  to range between <StartValue> and <EndValue>.
*/
TUint_1 MapIndex(
  TUint_1 Index,
  TUint_2 NumValues,
  TUint_1 StartValue,
  TUint_1 EndValue
)
{
  return map(Index, 0, NumValues - 1, StartValue, EndValue);
}

/*
  2024-03
  2024-04
  2024-05
*/
