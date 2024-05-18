// Send data to WS2812B LED stripe

/*
  Author: Martin Eden
  Last mod.: 2024-05-18
*/

/*
  SetLedStripeState(State)

  State:

    Pixels[]
    Length: ui2
    Pin: ui1

  Pixel:

    Green: ui1
    Red: ui1
    Blue: ui1
*/

#pragma once

#include <me_BaseTypes.h>

namespace me_Ws2812b
{
  using namespace me_BaseTypes;

  struct TPixel
  {
    TUint_1 Green;
    TUint_1 Red;
    TUint_1 Blue;
  };

  struct TLedStripeState
  {
    TPixel * Pixels;
    TUint_2 Length;
    TUint_1 Pin;
  };

  TBool SetLedStripeState(TLedStripeState State);
}

/*
  2024-03 Core
  2023-05 Cleanup
  2023-05-18 TLedStripeState instead of three constant arguments
*/
