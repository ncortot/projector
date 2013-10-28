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

  int p_red = getPower(red);
  int p_green = getPower(green);
  int p_blue = getPower(blue);

  Tlc.clear();

  if (channel == 0) {
    direction = 4;
  } else {
    Tlc.set(channel - 3, p_green / 4);
    Tlc.set(channel - 2, p_blue / 4);
    Tlc.set(channel - 1, p_red / 4);
  }

  Tlc.set(channel + 1, p_green);
  Tlc.set(channel + 2, p_blue);
  Tlc.set(channel + 3, p_red);

  if (channel == NUM_TLCS * 16 - 4) {
    direction = -4;
  } else {
    Tlc.set(channel + 5, p_green / 4);
    Tlc.set(channel + 6, p_blue / 4);
    Tlc.set(channel + 7, p_red / 4);
  }

  channel += direction;

  Tlc.update();
}
