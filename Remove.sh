#!/bin/bash

# Uninstall library
arduino-cli lib uninstall me_Ws2812b

# Uninstall dependencies
arduino-cli lib uninstall \
  me_UartSpeeds \
  me_InstallStandardStreams \
  me_MemorySegment \
  me_MemoryPoint \
  me_UnoAddresses \
  me_BaseTypes
