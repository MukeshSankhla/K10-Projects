# Beginner 06: Buzzer Beep & Audio Synthesizer

**Category**: Beginner - Fundamentals  
**Target Board**: UNIHIKER K10  
**Difficulty**: Beginner (Level 1)

---

## What This Example Is Of

Demonstrates generating clean acoustic audio tones using the UNIHIKER K10 onboard I2S audio amplifier and speaker.

### Expected Behavior
- Plays a single **1500 Hz confirmation beep** (2000 samples / 250ms) accompanied by a green LED flash.
- Pauses for 1.5 seconds.
- Plays a rapid **double alert beep at 2000 Hz** (two 125ms pulses) accompanied by an orange LED flash.
- Repeats indefinitely.

---

## Hardware Requirements

- **Development Board**: UNIHIKER K10 (ESP32-S3)
- **Peripherals Needed**: UNIHIKER K10 onboard speaker / magnetic buzzer.
- **Connection**: USB Type-C cable for power and programming.

---

## Detailed Code Explanation

### 1. The `playTone(frequency, beat)` Engine
- `freq`: Tone frequency in Hertz (e.g. 1500 Hz, 2000 Hz).
- `beat`: Audio sample count processed by the hardware I2S DMA pipeline at an **8000 Hz sample rate**:
  - `8000` samples = 1.0 second
  - `4000` samples = 500 ms
  - `2000` samples = 250 ms
  - `1000` samples = 125 ms

---

## Complete Sketch Source Code

```cpp
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
```
