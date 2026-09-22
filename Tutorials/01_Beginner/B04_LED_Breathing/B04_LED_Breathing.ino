#include "unihiker_k10.h"

UNIHIKER_K10 k10;

void setup() {
    k10.begin();
}

void loop() {
    uint32_t cyanColor = 0x00FFFF;

    // Fade IN (Brightness 0 -> 9)
    for (int b = 0; b <= 9; b++) {
        k10.rgb->brightness(b);
        k10.rgb->write(-1, cyanColor);
        delay(100);
    }

    // Fade OUT (Brightness 9 -> 0)
    for (int b = 9; b >= 0; b--) {
        k10.rgb->brightness(b);
        k10.rgb->write(-1, cyanColor);
        delay(100);
    }

    // Brief pause at rest
    delay(400);
}
