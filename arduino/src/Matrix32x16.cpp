#include <Arduino.h>

#include "HT1632.h"
#include <font_5x4.h>
#include <font_8x6.h>
#include <images.h>

#define LDR_PIN A0

int wd;
int i = 0;

void matrixSetup () {
#ifdef BICOLOR_MATRIX
    HT1632.begin(5, 18, 19, 6);
#else
    HT1632.begin(16, 18, 19);
#endif
    HT1632.clear();

    wd = HT1632.getTextWidth("Hello, how are you?", FONT_5X4_WIDTH, FONT_5X4_HEIGHT);
}

void matrixUpdate () {
    String ambient = "";
    //ambient += analogRead(LDR_PIN);
    ambient += (millis() / 1000);
    char text[40];
    ambient.toCharArray(text, 40);

    HT1632.drawTarget(BUFFER_BOARD(1));
    HT1632.clear();
    HT1632.drawText(text, 4, 4, FONT_8X6, FONT_8X6_WIDTH, FONT_8X6_HEIGHT, FONT_8X6_STEP_GLYPH);
    HT1632.render();
    return;

    // Select board 1 as the target of subsequent drawing/rendering operations.
    HT1632.drawTarget(BUFFER_BOARD(1));
    HT1632.clear();

    HT1632.drawText("Hello, how are you?", 2*OUT_SIZE - i, 2,
        FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);

    HT1632.render(); // Board 1's contents is updated.

    // Select board 2 as the target of subsequent drawing/rendering operations.
    HT1632.drawTarget(BUFFER_BOARD(2));
    HT1632.clear();

    HT1632.drawText("Hello, how are you?", OUT_SIZE - i, 10,
        FONT_5X4, FONT_5X4_WIDTH, FONT_5X4_HEIGHT, FONT_5X4_STEP_GLYPH);

    HT1632.render(); // Board 2's contents is updated.

    i = (i+1)%(wd + OUT_SIZE * 2); // Make it repeating.
}
