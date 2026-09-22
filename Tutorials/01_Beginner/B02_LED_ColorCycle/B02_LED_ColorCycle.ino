#include "unihiker_k10.h"

UNIHIKER_K10 k10;

// Color table: Red, Green, Blue, Yellow, Cyan, Magenta, White
const uint32_t colors[] = {
    0xFF0000, // Red
    0x00FF00, // Green
    0x0000FF, // Blue
    0xFFFF00, // Yellow (Red + Green)
    0x00FFFF, // Cyan (Green + Blue)
    0xFF00FF, // Magenta (Red + Blue)
    0xFFFFFF  // White (Red + Green + Blue)
};
const int numColors = sizeof(colors) / sizeof(colors[0]);

void setup() {
    k10.begin();
    k10.rgb->brightness(6);
}

void loop() {
    for (int i = 0; i < numColors; i++) {
        k10.rgb->write(-1, colors[i]);
        delay(800);
    }
}
