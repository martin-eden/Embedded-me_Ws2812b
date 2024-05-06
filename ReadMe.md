# What

Arduino library to control LED stripe.

# How

```C++
#include <me_Ws2812b.h>
...
using namespace me_Ws2812b;

TPixel TestPixels[] =
  {
    { .Green = 0xFF, .Red = 0, .Blue = 0 },
    { .Green = 0, .Red = 0xFF, .Blue = 0 },
    { .Green = 0, .Red = 0, .Blue = 0xFF },
  };

SendPixels(
  TestPixels,
  sizeof(TestPixels) / sizeof(TPixel),
  A0
);
```

See [example](examples/me_Ws2812b/me_Ws2812b.ino) for real code.

# Keywords

Arduino. AVR. ATmega328/P. 16 MHz. RGB LED stripe. WS2812B. Function.
C. Assembler. Standalone implementation. GPL3.

# See also

* (Additional documentation)(https://github.com/martin-eden/EmbeddedCpp-me_Ws2812b-Docs)
* (My other repositories)(https://github.com/martin-eden/contents)
