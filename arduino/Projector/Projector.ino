#include <Arduino.h>
#include <Console.h>
#include <Tlc5940.h>

#include "display.h"
#include "startup.h"
#include "toneAC4.h"

#include "Projector.h"

void setup()
{
  Tlc.init();
  ToneAC4::init();
  pinMode(CORE_LDR, INPUT);

  Bridge.begin();
  Console.begin();

  audio_splash();
  display_splash();
}


void loop()
{
  static uint32_t kit_hold = 0;

  if (millis() > kit_hold) {
    display::update_kit(0, 0, 0x20);
  }

  while (Console.available() > 0) {
    String command = Console.readStringUntil('\n');
    command.trim();
    if (!command.length()) {
      continue;
    }

    String arg;
    int i_start = 0;
    int i_end = command.indexOf(' ', i_start);
    if (i_end > 0) {
      arg = command.substring(i_start, i_end);
      i_start = i_end + 1;
    } else {
      arg = command.substring(i_start);
    }
    arg.toUpperCase();

    if (arg == "BEEP") {
      uint32_t frequency = 880;
      uint16_t duration = 500;
      uint8_t volume = 128;

      if (i_end > 0) {
        int i_end = command.indexOf(' ', i_start);
        if (i_end > 0) {
          arg = command.substring(i_start, i_end);
          i_start = i_end + 1;
        } else {
          arg = command.substring(i_start);
        }
        frequency = arg.toInt();
      }
 
      if (i_end > 0) {
        int i_end = command.indexOf(' ', i_start);
        if (i_end > 0) {
          arg = command.substring(i_start, i_end);
          i_start = i_end + 1;
        } else {
          arg = command.substring(i_start);
        }
        duration = arg.toInt();
      }
 
      if (i_end > 0) {
        int i_end = command.indexOf(' ', i_start);
        if (i_end > 0) {
          arg = command.substring(i_start, i_end);
          i_start = i_end + 1;
        } else {
          arg = command.substring(i_start);
        }
        volume = arg.toInt();
      }

      ToneAC4::tone(frequency, duration, volume, true);
    } else if (arg == "DISPLAY") {
      uint8_t red = 0;
      uint8_t green = 0;
      uint8_t blue = 0;
      uint16_t duration = 1000;

      if (i_end > 0) {
        int i_end = command.indexOf(' ', i_start);
        if (i_end > 0) {
          arg = command.substring(i_start, i_end);
          i_start = i_end + 1;
        } else {
          arg = command.substring(i_start);
        }
        red = arg.toInt();
      }
 
      if (i_end > 0) {
        int i_end = command.indexOf(' ', i_start);
        if (i_end > 0) {
          arg = command.substring(i_start, i_end);
          i_start = i_end + 1;
        } else {
          arg = command.substring(i_start);
        }
        green = arg.toInt();
      }
 
      if (i_end > 0) {
        int i_end = command.indexOf(' ', i_start);
        if (i_end > 0) {
          arg = command.substring(i_start, i_end);
          i_start = i_end + 1;
        } else {
          arg = command.substring(i_start);
        }
        blue = arg.toInt();
      }
 
      if (i_end > 0) {
        int i_end = command.indexOf(' ', i_start);
        if (i_end > 0) {
          arg = command.substring(i_start, i_end);
          i_start = i_end + 1;
        } else {
          arg = command.substring(i_start);
        }
        duration = arg.toInt();
      }

      display::update_all(red, green, blue);
      if (duration) {
        kit_hold = millis() + duration;
      } else {
        kit_hold = 1UL << 31;
      }
    } else if (arg == "HELP") {
      Console.println("Available commands: ");
      Console.println("BEEP [frequency] [duration] [volume]");
      Console.println("  frequency: integer, in Hz - default 880");
      Console.println("  duration: interger, in ms - default 500");
      Console.println("  volume: integer 0-255 - default 128");
      Console.println("DISPLAY <R> <G> <B> [duration]");
      Console.println("  R, G, B: color values, 0-255 - default 255, 255, 255");
      Console.println("  duration: integer, in ms - default 1000");
    } else {
      Console.print("Unknown command: ");
      Console.print(command);
      Console.println();
    }
  }

  delay(200);
}

// vi: et :
