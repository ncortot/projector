#ifndef _projector_display_h
#define _projector_display_h

#include <Arduino.h>

namespace display {
    void update_all(int red, int green, int blue);
    void update_kit(int red, int green, int blue);
}

#endif
