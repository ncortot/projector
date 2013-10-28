#include <Tlc5940.h>

#include "display.h"


int getPower(int value)
{
    return constrain(map(value, 0, 255, 0, 4095), 0, 4095);
}


void display::update_all(int red, int green, int blue)
{
    Tlc.clear();

    int red_power = getPower(red);
    int green_power = getPower(green);
    int blue_power = getPower(blue);

    for (int channel = 0; channel < 32; ++channel) {
        Tlc.set(++channel, green_power);
        Tlc.set(++channel, blue_power);
        Tlc.set(++channel, red_power);
    }

    Tlc.update();
}


void display::update_kit(int red, int green, int blue)
{
  static int direction;
  static int channel = 0;
  int power;

  Tlc.clear();

  Tlc.set(channel + 1, getPower(green));
  Tlc.set(channel + 2, getPower(blue));
  Tlc.set(channel + 3, getPower(red));

  if (channel == 0) {
    direction = 4;
  }
  if (channel == NUM_TLCS * 16 - 4) {
    direction = -4;
  }
  channel += direction;

  Tlc.update();
}
