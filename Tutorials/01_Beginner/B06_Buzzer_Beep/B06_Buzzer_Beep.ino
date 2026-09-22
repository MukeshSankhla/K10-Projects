#include "unihiker_k10.h"

UNIHIKER_K10 k10;
Music music;

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Turn off LEDs
}

void loop() {
    // Single confirmation beep: 1500 Hz for ~250ms (2000 samples at 8000 Hz sample rate)
    k10.rgb->write(0, 0x00FF00); // Green LED indicator
    music.playTone(1500, 2000);
    k10.rgb->write(0, 0x000000);
    delay(1500);

    // Double alert beep: two 2000 Hz pulses (~125ms each, 1000 samples)
    k10.rgb->write(2, 0xFF9900); // Orange LED indicator
    music.playTone(2000, 1000);
    delay(100);
    music.playTone(2000, 1000);
    k10.rgb->write(2, 0x000000);
    delay(2000);
}
