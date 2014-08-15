#include <Arduino.h>

#include "audio.h"
#include "sample.h"

// Clock source frequency
#define PCK_FREQ 48000000UL

// Audio sampling frequency
#define AUDIO_FREQ 48000UL

// Timer4 TOP
#define TOP (PCK_FREQ / AUDIO_FREQ)

// Variables used by the timer interrupt handler
const uint16_t *_min = audio_sample;
const uint16_t *_max = audio_sample + SAMPLE_SIZE;
volatile const uint16_t *_sample = _min;
volatile uint32_t _timeout;

void Audio::init()
{
    // Init the internal PLL at 96MHz
    PLLFRQ = _BV(PDIV3) | _BV(PDIV1);  // Init the internal PLL at 96MHz
    PLLCSR = _BV(PINDIV) | _BV(PLLE);  // Clock source is at 16MHz
    while(!(PLLCSR & _BV(PLOCK)));
    // Postscaler for USB 48MHz, PCK 48MHz
    PLLFRQ |= _BV(PLLUSB) | _BV(PLLTM1) | _BV(PLLTM0);
}

void Audio::play(uint32_t duration)
{
    // Play the sample for duration ms
    _timeout = duration * (AUDIO_FREQ / 1000UL);

    // Disable the timer interrupt
    TIMSK4 &= ~_BV(TOIE4);

    // Reset the PWM parameters
    TC4H = 0;
    OCR4A = OCR4D = 0;
    TC4H = TOP >> 8;
    OCR4C = TOP & 0xff;

    // Set up Fast PWM mode on OC4A and OC4D, no prescaler
    TCCR4A = _BV(COM4A1) | _BV(PWM4A);
    TCCR4B = _BV(CS40);
    TCCR4C |= _BV(COM4D1) | _BV(PWM4D);
    TCCR4D = 0;
    TCCR4E = _BV(ENHC4);  // Enhanced Compare/PWM Mode

    // Activate the timer interrupt
    TIMSK4 |= _BV(TOIE4);

    // Set timer 4 PWM pins OC4A and OC4D to OUTPUT
    DDRC |= _BV(PORTC7);
    DDRD |= _BV(PORTD7);
}

void Audio::stop()
{
    // Remove the timer interrupts
    TIMSK4 &= ~_BV(TOIE4);

    // Stop the timer
    TCCR4B = 0;
    TC4H = 0;
    OCR4A = OCR4D = 0;

    // Set timer pins to LOW
    PORTC &= ~_BV(PORTC7);
    PORTD &= ~_BV(PORTD7);
}

ISR(TIMER4_OVF_vect)
{
    // Sample down the signal to 12 bits:
    //   - bit 11: output select, 1 = PC7 (Pin 13), 0 = PD7 (Pin 6);
    //   - bits 10-1: PWM signal generation;
    //   - bit 0: Enhanced PWM Mode bit.
    // Since TOP = 1000 < 0x7fff, extreme values will be trimmed.
    if (*_sample & 0x8000) {
        const uint16_t h = (*_sample & 0x7fff) >> 4;
        TC4H = h >> 8;
        OCR4A = h & 0xff;
        TC4H = 0;
        OCR4D = 0;
    } else {
        const uint16_t l = (0x7fff - *_sample) >> 4;
        TC4H = 0;
        OCR4A = 0;
        TC4H = l >> 8;
        OCR4D = l & 0xff;
    }

    if (!--_timeout) {
        Audio::stop();
    } else if (++_sample > _max) {
        _sample = _min;
    }
}
