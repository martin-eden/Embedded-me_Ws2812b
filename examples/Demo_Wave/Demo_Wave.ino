// Demo of animation via [me_Ws2812b]

/*
  Author: Martin Eden
  Last mod.: 2025-08-22
*/

#include <me_Ws2812b.h>

#include <me_BaseTypes.h>
#include <me_Console.h>
#include <me_ConvertUnits_Angle.h>
#include <me_Delays.h>

const TUint_1 LedStripePin = 2;
const TUint_1 NumPixels = 60;

/*
  Send rolling white sine wave.
*/
void Test_WhiteSine()
{
  using
    me_ConvertUnits_Angle::DegToRad,
    me_ConvertUnits_Angle::NormalizeDeg,
    me_Ws2812b::TPixel,
    me_Ws2812b::TLedStripeState,
    me_Ws2812b::SetLedStripeState;

  TPixel Pixels[NumPixels];

  static TUint_2 BaseAngle_Deg = 0;

  TUint_2 Angle_Deg = BaseAngle_Deg;

  for (TUint_1 PixelIdx = 0; PixelIdx < NumPixels; ++PixelIdx)
  {
    const TUint_1
      ByteFloor = 0,
      ByteCeil = 120,
      AngleIncrement_Deg = 12;

    // Map sine range [-1, 1] to byte range [0, 255]:
    TUint_1 ByteSine = (sin(DegToRad(Angle_Deg)) + 1) / 2 * 255;

    // Map byte to floor-ceiling range:
    TUint_1 EtherValue = map(ByteSine, 0, 255, ByteFloor, ByteCeil);

    Pixels[PixelIdx] =
      {
        .Green = EtherValue,
        .Red = EtherValue,
        .Blue = EtherValue,
      };

    Angle_Deg += AngleIncrement_Deg;
    Angle_Deg = NormalizeDeg(Angle_Deg);
  }

  const TUint_1 BaseAngleShift_Deg = 1;

  BaseAngle_Deg += BaseAngleShift_Deg;
  BaseAngle_Deg = NormalizeDeg(BaseAngle_Deg);

  TLedStripeState State;

  State.Pixels = Pixels;
  State.Length = NumPixels;
  State.Pin = LedStripePin;

  SetLedStripeState(State);
}

void setup()
{
  Console.Init();

  Console.Print("[me_Ws2812b.Demo_Wave] Infinite demo/test for RGB LED");
}

void loop()
{
  Test_WhiteSine();

  me_Delays::Delay_Ms(20);
}

/*
  2024 # # # #
  2025-08-22
*/
