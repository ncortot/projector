#include <Arduino.h>
#include <Tlc5940.h>

#include "Projector.h"
#include "Matrix32x16.h"

void setup()
{
  // Initialize serial communication
  Serial.begin(9600);
  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  /* Call Tlc.init() to setup the tlc.
   You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  Tlc.init();

  matrixSetup();
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  /*
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
  */
}


void serialUpdate()
{
  int incomingByte;

  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == 'H') {
      digitalWrite(LED_PIN, HIGH);
      Serial.write("OK HIGH\r");
    }
    if (incomingByte == 'L') {
      digitalWrite(LED_PIN, LOW);
      Serial.write("OK LOW\r");
    }
  }
}


int getPower(int value)
{
    return constrain(map(value, 0, 255, 0, 4095), 0, 4095);
}


void displayUpdate(int red, int green, int blue)
{
  static int direction;
  static int channel = 0;
  int power;

  Tlc.clear();

  switch (channel % 4) {
    case 3:
      power = getPower(red);
      break;
    case 1:
      power = getPower(green);
      break;
    case 2:
      power = getPower(blue);
      break;
    default:
      power = 0;
  }

  if (channel == 0) {
    direction = 1;
  }
  Tlc.set(channel, power);
  if (channel == NUM_TLCS * 16 - 1) {
    direction = -1;
  }

  channel += direction;

  Tlc.update();
}


void fullUpdate(int red, int green, int blue)
{
    Tlc.clear();

    int red_power = getPower(red);
    int green_power = getPower(green);
    int blue_power = getPower(blue);

    for (int channel = 0; channel < 32; ++channel) {
        Tlc.set(++channel, red_power);
        Tlc.set(++channel, green_power);
        Tlc.set(++channel, blue_power);
    }

    Tlc.update();
}

void loop()
{
  static int color = -1;

  switch ((++color / 5) % 5) {
    case 0:
      Serial.write("...\r");
      fullUpdate(255, 0, 0);
      break;
    case 1:
      fullUpdate(0, 255, 0);
      break;
    case 2:
      fullUpdate(0, 0, 255);
      break;
    case 3:
      fullUpdate(255, 255, 255);
      break;
    default:
      displayUpdate(255, 255, 255);
  }

  serialUpdate();
  matrixUpdate();
  delay(200);
}

// vi: et :
