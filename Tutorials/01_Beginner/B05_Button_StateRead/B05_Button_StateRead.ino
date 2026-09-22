#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Start with LEDs OFF
}

void loop() {
    // When Button A is held, glow LEDs Green
    if (k10.buttonA->isPressed()) {
        k10.rgb->write(-1, 0x00FF00); // Green
    }
    // When Button B is held, glow LEDs Blue
    else if (k10.buttonB->isPressed()) {
        k10.rgb->write(-1, 0x0000FF); // Blue
    }
    // On release (neither button pressed), turn off LEDs
    else {
        k10.rgb->write(-1, 0x000000); // OFF
    }

    delay(20); // Small debounce delay
}