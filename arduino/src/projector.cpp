#include <Arduino.h>
#include <Console.h>
#include <Tlc5940.h>

#include "audio.h"
#include "display.h"
#include "startup.h"

#include "projector.h"

void setup()
{
  Tlc.init();
  Audio::init();
  pinMode(CORE_LDR, INPUT);

  Bridge.begin();
  Console.begin();

  audio_splash();
  display_splash();
}


bool parse_arg_string(String &input, int &start_index, int &end_index, String &value)
{
  if (start_index < 0)
    return false;

  end_index = input.indexOf(' ', start_index);
  if (end_index > 0) {
    value = input.substring(start_index, end_index);
    start_index = end_index + 1;
  } else {
    value = input.substring(start_index);
    start_index = -1;
  }
  return true;
}

bool parse_arg_int(String &input, int &start_index, int &end_index, int &value)
{
  String arg;
  if (parse_arg_string(input, start_index, end_index, arg)) {
    value = arg.toInt();
    return true;
  } else {
    return false;
  }
}


void loop()
{
  static uint32_t kit_hold = 0;

  if (millis() > kit_hold) {
    display::update_kit(0, 0, 0x20);
  }

  while (Console.available() > 0) {
    String input = Console.readStringUntil('\n');
    input.trim();
    if (!input.length()) {
      continue;
    }

    String command;
    int start_index = 0;
    int end_index = -1;
    parse_arg_string(input, start_index, end_index, command);
    command.toUpperCase();

    if (command == "BEEP") {
      int duration = 500;
      parse_arg_int(input, start_index, end_index, duration);
      Audio::play(duration);
    } else if (command == "DISPLAY") {
      int red = 0;
      int green = 0;
      int blue = 0;
      int duration = 1000;

      parse_arg_int(input, start_index, end_index, red);
      parse_arg_int(input, start_index, end_index, green);
      parse_arg_int(input, start_index, end_index, blue);
      parse_arg_int(input, start_index, end_index, duration);

      display::update_all(red, green, blue);
      if (duration) {
        kit_hold = millis() + duration;
      } else {
        kit_hold = 1UL << 31;
      }
    } else if (command == "HELP") {
      Console.print(F(
        "Available commands: \n"
        "BEEP [duration]\n"
        "  duration: interger, in ms - default 500\n"
        "DISPLAY <R> <G> <B> [duration]\n"
        "  R, G, B: color values, 0-255 - default 255, 255, 255\n"
        "  duration: integer, in ms - default 1000\n"
      ));
    } else {
      Console.print(F("Unknown command: "));
      Console.print(input);
      Console.println();
    }
  }

  delay(200);
}

// vi: et :
