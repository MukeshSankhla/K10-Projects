# Beginner 07: Buzzer Preset Melody Loop

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Plays built-in musical compositions stored in the UNIHIKER K10 firmware ROM in a repeating loop with visual LED accompaniment.

### Expected Behavior
- Plays Beethoven's *Ode to Joy* (`ODE`) through the onboard speaker.
- All onboard RGB LEDs glow **Cyan** (`0x00FFFF`) during playback.
- Pauses for 1 second with LEDs off, then repeats the melody loop continuously.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard audio speaker, 3x WS2812 RGB LEDs.
- **Connection**: USB Type-C cable for power and programming.

---

## Detailed Code Explanation

### 1. The `playMusic` API
- `music.playMusic(melody, options)`: Decodes and synthesizes multi-note melodies stored in flash.
- Available preset melodies in `Melodies`:
  - `ODE` (Ode to Joy)
  - `BIRTHDAY` (Happy Birthday)
  - `ENTERTAINER` (The Entertainer)
  - `PRELUDE`
  - `NYAN`
  - `RINGTONE`
  - `DADADADUM` (Beethoven's 5th)

---

## Complete Sketch Source Code

```cpp
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
```
