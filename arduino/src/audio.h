#ifndef toneAC4_h
#define toneAC4_h

#include <Arduino.h>

namespace Audio {
  extern void init();
  extern void play(uint32_t duration = 500);
  extern void stop();
};

#endif
