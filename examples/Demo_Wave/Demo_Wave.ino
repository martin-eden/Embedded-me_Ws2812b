// Demo of animation via [me_Ws2812b]

/*
  Author: Martin Eden
  Last mod.: 2024-09-12
*/

#include <me_Ws2812b.h>

#include <me_ConvertUnits_Angle.h>

#include <me_UartSpeeds.h>
#include <me_InstallStandardStreams.h>
#include <me_BaseTypes.h>

using namespace me_Ws2812b;

const TUint_1 LedStripePin = A0;
const TUint_1 NumPixels = 60;

// Forwards
void Test_WhiteSine();

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  delay(500);

  InstallStandardStreams();

  printf("[me_Ws2812b.Demo_Wave] Hello there!\n");
  delay(500);

  printf("It's infinite demo.\n");
}

void loop()
{
  Test_WhiteSine();

  delay(20);
}

/*
  Send rolling white sine wave.
*/
void Test_WhiteSine()
{
  using namespace me_ConvertUnits_Angle;

  TPixel Pixels[NumPixels];

  static TUint_2 BaseAngle_Deg = 0;

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

    const TUint_1 AngleIncrement_Deg = 12;

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

/*
  2024-03
  2024-04
  2024-05
*/
