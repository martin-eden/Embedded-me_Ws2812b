// Color output via [me_Ws2812b]

/*
  Author: Martin Eden
  Last mod.: 2025-08-22
*/

#include <me_Ws2812b.h>

#include <me_BaseTypes.h>
#include <me_Console.h>
#include <me_Delays.h>

// Output pin
const TUint_1 LedStripePin = 2;

// Number of pixels in stripe
const TUint_1 NumPixels = 60;

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
  Blank LED stripe

  Lights off all LEDs. Used for testing.
*/
void BlankLedStripe()
{
  const me_Ws2812b::TPixel BlankColor = { 0, 0, 0 };

  me_Ws2812b::TPixel Pixels[NumPixels];

  for (TUint_1 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
    Pixels[PixelIdx] = BlankColor;

  me_Ws2812b::TLedStripeState StripeState;

  StripeState.Pixels = Pixels;
  StripeState.Length = NumPixels;
  StripeState.Pin = LedStripePin;

  me_Ws2812b::SetLedStripeState(StripeState);
}

/*
  Send smooth transition from <StartColor> to <EndColor>.
*/
void Test_ColorSmoothing()
{
  using
    me_Ws2812b::TPixel,
    me_Ws2812b::TLedStripeState,
    me_Ws2812b::SetLedStripeState;

  const TPixel StartColor = { .Green = 255, .Red = 96, .Blue = 0, };
  const TPixel EndColor = { .Green = 32, .Red = 64, .Blue = 64, };

  TPixel Pixels[NumPixels];

  for (TUint_1 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
    Pixels[PixelIdx] = MapColor(PixelIdx, NumPixels, StartColor, EndColor);

  TLedStripeState State;

  State.Pixels = Pixels;
  State.Length = NumPixels;
  State.Pin = LedStripePin;

  SetLedStripeState(State);
}

//

void setup()
{
  Console.Init();

  Console.Print("[me_Ws2812b.Demo_Color] Color smoothing test");

  BlankLedStripe();

  me_Delays::Delay_S(1);

  Test_ColorSmoothing();

  Console.Print("Done");
}

void loop()
{
}

/*
  2024 # # # #
  2025-08-22
*/
