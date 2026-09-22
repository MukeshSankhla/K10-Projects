## What this tutorial is??

This tutorial introduces **acoustic feedback and audio synthesis** on the UNIHIKER K10. 

You will learn how the microcontroller generates distinct audible tones and notification patterns using the onboard audio subsystem, synchronizing sound pulses with visual LED indicators.

### Expected Behavior
The board alternates between two distinct acoustic alerts:
1. **Single Confirmation Beep**: A 1500 Hz tone for ~250 ms accompanied by a Green LED flash on LED 0, followed by a 1.5-second pause.
2. **Double Alert Beep**: Two rapid 2000 Hz pulses (~125 ms each) accompanied by an Orange LED flash on LED 2, followed by a 2-second pause.

---

## How it works

1. **Audio Synthesis Architecture**:
   - The UNIHIKER K10 uses an integrated I2S (Inter-IC Sound) digital audio amplifier driving an onboard miniature speaker/transducer.
   - The `Music` driver class generates mathematical square/sine waveforms in real time by pumping digital audio samples into the I2S DMA (Direct Memory Access) buffer.
2. **Frequency & Pitch (Hertz)**:
   - Sound is created by mechanical pressure waves vibrating in air. The frequency of vibration determines the **pitch**:
     - Higher frequency (e.g., 2000 Hz) $\rightarrow$ higher pitch (sharp warning alert).
     - Lower frequency (e.g., 1500 Hz or 440 Hz) $\rightarrow$ lower pitch (mellow confirmation tone).
3. **Tone Duration & Sample Counting**:
   - In `music.playTone(freq, beat)`, the second parameter represents the number of audio samples output at an 8000 Hz sampling rate:
     $$\text{Duration (seconds)} = \frac{\text{samples}}{8000\text{ samples/sec}}$$
     - A parameter of `2000` samples produces: $2000 / 8000 = 0.25\text{ seconds}$ (250 ms).
     - A parameter of `1000` samples produces: $1000 / 8000 = 0.125\text{ seconds}$ (125 ms).
4. **Multimodal Feedback**:
   - Triggering visual cues (LEDs) simultaneously with acoustic cues (buzzer tones) significantly improves user feedback clarity in noisy or visually distracting environments.

---

## Sensors/actuator detaile

### What is a Buzzer / Piezoelectric Transducer?
A **buzzer** is an electroacoustic transducer that converts electrical energy into audible sound waves.

### The Inverse Piezoelectric Effect
- The core element inside most miniature electronic buzzers is a thin disc of **piezoelectric ceramic** (such as Lead Zirconate Titanate, PZT) bonded to a thin brass or nickel-alloy diaphragm.
- **The Physics**: Piezoelectric materials have an asymmetrical crystalline lattice. When an alternating electric voltage is applied across the crystal faces, the lattice physically expands and contracts at that exact electrical frequency (**Inverse Piezoelectric Effect**).
- This rapid mechanical flexing vibrates the attached metal diaphragm, which compresses and decompresses the surrounding air molecules to create audible sound waves.

```
   [+ Voltage Applied]    --> Diaphragm bows UPWARD   --> Air compressed (Sound Peak)
   [- Voltage Applied]    --> Diaphragm bows DOWNWARD --> Air rarified   (Sound Trough)
```

### Active vs. Passive Buzzers
In electronics, buzzers fall into two distinct categories:
1. **Active Buzzer**:
   - Contains an internal built-in transistor oscillator circuit.
   - **Operation**: Simply applying DC voltage (e.g., 3.3V) produces a continuous tone at a single fixed frequency (typically 2.5 kHz). You cannot change its pitch or play melodies.
2. **Passive Buzzer (Speaker/Transducer)**:
   - Does *not* have an internal oscillator; it is merely a raw piezo disc or electromagnetic coil.
   - **Operation**: Requires an external AC audio signal or microcontroller PWM waveform. The frequency of the signal directly controls the pitch of the sound, enabling music, beeps, and sirens.
   - The UNIHIKER K10 features a fully programmable passive transducer driven through high-fidelity digital I2S audio circuitry.

---

## Step by step function wise code break down

### 1. Library Inclusion & Driver Declaration
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
Music music;
```
- `#include "unihiker_k10.h"`: Loads board APIs.
- `UNIHIKER_K10 k10;`: Board management object.
- `Music music;`: Instantiates the audio synthesizer engine responsible for I2S tone generation.

### 2. Hardware Setup in `setup()`
```cpp
void setup() {
    k10.begin();
    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000); // Turn off LEDs
}
```
- Powers on peripheral buses, sets LED brightness to 5, and turns off all LEDs.

### 3. Single Confirmation Beep Pattern
```cpp
void loop() {
    // 1500 Hz tone for ~250ms (2000 samples / 8000 Hz)
    k10.rgb->write(0, 0x00FF00); // Green indicator on LED 0
    music.playTone(1500, 2000);
    k10.rgb->write(0, 0x000000);
    delay(1500);
```
- Turns LED 0 Green.
- `music.playTone(1500, 2000)` blocks execution while outputting 1500 Hz audio for 250 ms.
- Turns LED 0 off.
- Pauses for 1.5 seconds.

### 4. Double Alert Beep Pattern
```cpp
    // Double 2000 Hz pulse (~125ms each, 1000 samples)
    k10.rgb->write(2, 0xFF9900); // Orange indicator on LED 2
    music.playTone(2000, 1000);
    delay(100);
    music.playTone(2000, 1000);
    k10.rgb->write(2, 0x000000);
    delay(2000);
}
```
- Turns LED 2 Orange (`0xFF9900`).
- Plays a 2000 Hz pulse for 125 ms, pauses 100 ms, and plays a second 2000 Hz pulse.
- Extinguishes LED 2 and pauses 2 seconds before looping.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes power circuits and peripheral bus clocks. |
| `music.playTone(freq, beat)` | `int freq` (Hz), `int beat` (samples) | `void` | Synthesizes a tone at frequency `freq` for `beat` samples (8000 samples/sec). |
| `k10.rgb->write(index, color)` | `int8_t index`, `uint32_t color` | `void` | Sets LED color for visual status indication. |
| `delay(ms)` | `unsigned long ms` | `void` | Pauses program flow for the specified time in milliseconds. |

---

## Full Code

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
