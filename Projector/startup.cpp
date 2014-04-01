#include "display.h"
#include "toneAC4.h"

#include "startup.h"


void audio_splash()
{
  ToneAC4::tone(440, 100, 128, false);
  delay(100);
  ToneAC4::tone(880, 100, 128, false);
  delay(100);
  ToneAC4::tone(1760, 100, 128, false);
}


void display_splash()
{
  display::update_all(0xff, 0x00, 0x00);
  delay(1000);
  display::update_all(0x00, 0xff, 0x00);
  delay(1000);
  display::update_all(0x00, 0x00, 0xff);
  delay(1000);
  display::update_all(0xff, 0xff, 0xff);
  delay(1000);
  display::update_all(0x00, 0x00, 0x00);
}
