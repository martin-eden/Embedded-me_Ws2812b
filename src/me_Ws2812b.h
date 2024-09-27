// Send data to WS2812B LED stripe

/*
  Author: Martin Eden
  Last mod.: 2024-09-27
*/

#pragma once

#include <me_BaseTypes.h>

namespace me_Ws2812b
{
  /*
    LED stripe pixel: Green, Red, Blue

    Fields order G-R-B is important. Device expects color bytes
    in that order.
  */
  struct TPixel
  {
    TUint_1 Green;
    TUint_1 Red;
    TUint_1 Blue;
  };

  /*
    LED stripe state:

      * Pointer to pixels data
      * Number of pixels
      * Output pin
  */
  struct TLedStripeState
  {
    TPixel * Pixels;
    TUint_2 Length;
    TUint_1 Pin;
  };

  // Apply state to stripe
  TBool SetLedStripeState(TLedStripeState State);
}

/*
  2024-03 Core
  2023-05 Cleanup
  2023-05-18 TLedStripeState instead of three constant arguments
*/
