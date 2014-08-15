#ifndef toneAC4_h
#define toneAC4_h

#include <Arduino.h>

namespace ToneAC4 {
  extern void init();
  extern void tone(uint32_t frequency = 0, uint16_t duration = 500, uint8_t volume = 255, uint8_t background = false);
  extern void noTone();
};

#endif
