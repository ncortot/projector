#include "audio.h"
#include "display.h"

#include "startup.h"


void audio_splash()
{
  Audio::play(100);
  delay(200);
  Audio::play(500);
  delay(600);
  Audio::play(1000);
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
