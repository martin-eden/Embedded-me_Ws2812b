// Demo of animation via [me_Ws2812b]

/*
  Board: Arduino Uno

  Author: Martin Eden
  Last mod.: 2024-05-16
*/

#include <me_Ws2812b.h>

#include <me_UartSpeeds.h>
#include <me_InstallStandardStreams.h>

#include <me_Math.h> // deg to rad

#include <me_Types.h>

// --

const TUint_1 LedStripePin = A0;
const TUint_1 NumPixels = 60;

// Forwards
void Test_WhiteSine(TUint_2 NumPixels, TUint_1 OutputPin);

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  delay(500);

  InstallStandardStreams();

  printf("[me_Ws2812b.Demo_Wave]\n");
  delay(500);
}

void loop()
{
  Test_WhiteSine(NumPixels, LedStripePin);

  delay(20);
}

// --

using namespace me_Ws2812b;

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
}

/*
  2024-03
  2024-04
  2024-05
*/
