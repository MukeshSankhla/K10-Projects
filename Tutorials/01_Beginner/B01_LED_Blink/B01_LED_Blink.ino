#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    // Initialize K10 hardware and power management
    k10.begin();
    // Set brightness to a comfortable level (scale 0-9)
    k10.rgb->brightness(5);
}

void loop() {
    // Turn all 3 LEDs (-1 index) Red (0xFF0000)
    k10.rgb->write(-1, 0xFF0000);
    delay(1000);

    // Turn all 3 LEDs off (0x000000)
    k10.rgb->write(-1, 0x000000);
    delay(1000);
}
