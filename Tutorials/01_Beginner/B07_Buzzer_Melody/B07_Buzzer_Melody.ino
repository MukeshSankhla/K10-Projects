#include "unihiker_k10.h"

UNIHIKER_K10 k10;
Music music;

void setup() {
    k10.begin();
    k10.rgb->brightness(5);
}

void loop() {
    // Play built-in melody in a loop
    k10.rgb->write(-1, 0x00FFFF); // Cyan LED during playback
    music.playMusic(ODE, Once);    // Plays Beethoven's Ode to Joy
    k10.rgb->write(-1, 0x000000); // Turn off LEDs

    // 1-second interval before repeating melody loop
    delay(1000);
}
