#include "toneAC4.h"

// Clock source frequency
#define PCK_FREQ 64000000UL

// Maximum timer interrupt frequency
// Must be lower than 160kHz for the interrupts to work in time
#define MAX_FREQ 100000UL

// Maximum TOP value
#define RESOLUTION 0xffUL

// Variables used by the timer interrupt handler
volatile uint8_t _phase_accumulator;
volatile uint8_t _phase_increment;
volatile uint8_t _duty;
volatile uint32_t _timeout;

void ToneAC4::init()
{
  PLLFRQ = _BV(PDIV3) | _BV(PDIV1);  // Init the internal PLL at 96MHz
  PLLCSR = _BV(PINDIV) | _BV(PLLE);  // Clock source is at 16MHz
  while(!(PLLCSR & _BV(PLOCK)));
  PLLFRQ |= _BV(PLLUSB) | _BV(PLLTM1);  // Postscalers for USB 48MHz, PCK 64MHz
}

void ToneAC4::tone(uint32_t frequency, uint16_t duration, uint8_t volume, uint8_t background)
{
  // If frequency or volume are 0, turn off sound and return
  if (frequency == 0 || volume == 0) {
    noTone();
    return;
  }

  // Choose the number of samples to keep the timer interrupt frequency under MAX_FREQ
  uint8_t sample_bits = 8;
  uint32_t timer_freq = frequency << 8;
  while (timer_freq > MAX_FREQ) {
    if (--sample_bits < 2) {
       noTone();
       return;
    }
    timer_freq = timer_freq >> 1;
  }
  uint8_t phase_increment = 0x100 >> sample_bits;

  // Compute prescaling and top values
  uint8_t prescaler_mask = 1;
  uint32_t top = (PCK_FREQ >> sample_bits) / frequency;
  while (top > RESOLUTION) {
    if (++prescaler_mask > 15) {
      noTone();
      return;
    }
    top = top >> 1;
  }

  // Set the duty according to top and volume
  uint8_t duty = top * volume / 255;

  // Set when the note should end
  uint32_t timeout;
  if (duration > 0 && background) {
    timeout = ((duration * frequency) << sample_bits) / 1000;
  } else {
    timeout = 1UL << 31;
  }
  
  // Disable the timer interrupt
  TIMSK4 &= ~_BV(TOIE4);

  // Reset the PWM parameters
  TC4H = 0;
  OCR4A = OCR4D = 0;
  OCR4C = top;

  // Update the interrupt variables
  _phase_increment = phase_increment;
  _timeout = timeout;
  _duty = duty;

  // Set up Fast PWM mode on OC4A and OC4D
  TCCR4A = _BV(COM4A1) | _BV(PWM4A);
  TCCR4B = prescaler_mask;
  TCCR4C |= _BV(COM4D1) | _BV(PWM4D);
  TCCR4D = 0;

  // Activate the timer interrupt.
  TIMSK4 |= _BV(TOIE4);

  // Set timer 4 PWM pins OC4A and OC4D to OUTPUT
  DDRC |= _BV(PORTC7);
  DDRD |= _BV(PORTD7);

  // In non-background operation mode, wait for completion
  if (duration > 0 && !background) {
    delay(duration);
    noTone();
  }
}

void ToneAC4::noTone() {
  // Remove the timer interrupt
  TIMSK4 &= ~_BV(TOIE4);

  // Stop the timer
  TCCR4B = 0; 
  OCR4A = OCR4D = 0;

  // Set timer pins to LOW
  PORTC &= ~_BV(PORTC7);
  PORTD &= ~_BV(PORTD7);
}

ISR(TIMER4_OVF_vect) {
  _phase_accumulator += _phase_increment;

  if (_phase_accumulator >> 7) {
    OCR4A = 0;
    OCR4D = _duty;
  } else {
    OCR4A = _duty;
    OCR4D = 0;
  }
  
  if (!--_timeout) {
    ToneAC4::noTone();
  }
}
