#!/bin/bash

# Install dependencies
arduino-cli \
  lib install \
    --git-url \
      https://github.com/martin-eden/Embedded-me_BaseTypes \
      https://github.com/martin-eden/Embedded-me_MemoryPoint \
      https://github.com/martin-eden/Embedded-me_MemorySegment \
      https://github.com/martin-eden/Embedded-me_UnoAddresses \
      https://github.com/martin-eden/Embedded-me_ConvertUnits_Angle \
      https://github.com/martin-eden/Embedded-me_InstallStandardStreams \
      https://github.com/martin-eden/Embedded-me_UartSpeeds

# Install library
arduino-cli \
  lib install \
    --git-url \
      https://github.com/martin-eden/Embedded-me_Ws2812b
