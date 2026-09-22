## What this tutorial is??

This tutorial demonstrates how to play **polyphonic musical compositions and classic tunes** using the UNIHIKER K10 onboard audio engine. 

Instead of manually calculating individual tone frequencies and durations in loops, you will use the built-in melody sequencing system to play recognizable tunes (such as Beethoven's *Ode to Joy*) with synchronized visual stage lighting.

### Expected Behavior
- While the melody plays: All 3 onboard LEDs light up in a brilliant Cyan (`0x00FFFF`).
- The buzzer plays Beethoven's classical masterpiece *Ode to Joy* (*An die Freude*).
- When the song concludes: The LEDs turn OFF, the board pauses quietly for 1000 milliseconds, and then repeats the performance.

---

## How it works

1. **Musical Scale Representation in Microcontrollers**:
   - In Western music, the 12-tone equal temperament scale divides each octave into 12 semitones.
   - The reference pitch is standard concert pitch **A4 = 440 Hz**. The frequency of any subsequent note $n$ semitones away is calculated mathematically:
     $$f_n = 440 \times 2^{n / 12}\text{ Hz}$$
   - For example:
     - **Middle C (C4)**: ~261.63 Hz
     - **D4**: ~293.66 Hz
     - **E4**: ~329.63 Hz
     - **F4**: ~349.23 Hz
     - **G4**: ~392.00 Hz
2. **Melody Strings & Sequencer Engine**:
   - The K10 library contains an internal string-based musical sequencer inspired by the RTTTL (Ring Tone Text Transfer Language) standard.
   - Melodies are encoded as strings of musical notes, octaves, and note durations (e.g., `E4:4|E4:4|F4:4|G4:4`).
3. **The `playMusic` API**:
   - Calling `music.playMusic(ODE, Once)` passes the predefined melody enum `ODE` (Beethoven's 9th Symphony) and sets the execution mode to `Once`.
   - The sequencer parses each note, plays its corresponding frequency for the exact fraction of a beat determined by the tempo (Beats Per Minute), and terminates cleanly when finished.

---

## Sensors/actuator detaile

### Musical Tempo, Beats & Durations
In musical synthesis, every song is defined by two fundamental parameters:
1. **Pitch (Frequency)**: Which musical note is vibrating (measured in Hertz).
2. **Duration (Rhythm)**: How long each note is held:
   - **Tempo (BPM)**: The speed of the musical pulse (e.g., 120 Beats Per Minute means 1 beat occurs every 0.5 seconds).
   - **Whole Note**: 4 beats
   - **Half Note**: 2 beats
   - **Quarter Note**: 1 beat
   - **Eighth Note**: 0.5 beat
   - **Sixteenth Note**: 0.25 beat

### Built-in Melodies in the UNIHIKER K10 SDK
The UNIHIKER K10 library includes several classic built-in tunes accessible via the `Melodies` enumeration:
- `ODE`: Beethoven's *Ode to Joy*
- `DADADADUM`: Beethoven's 5th Symphony Opening Motif
- `BIRTHDAY`: *Happy Birthday to You*
- `WEDDING`: Wagner's *Bridal Chorus*
- `FUNERAL`: Chopin's *Funeral March*
- `ENTERTAINER`: Scott Joplin's *The Entertainer*
- `PRELUDE`: J.S. Bach's *Prelude in C Major*

### Playback Options (`MelodyOptions`)
- `Once`: Plays the complete song from start to finish as a blocking function, returning only when the last note ends.
- `Forever`: Automatically loops the melody continuously.
- `OnceInBackground`: Spawns a dedicated FreeRTOS background task so your sketch can perform other duties while music plays.

---

## Step by step function wise code break down

### 1. Library Inclusion & Declarations
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
Music music;
```
- Includes K10 core APIs and creates the board object `k10` and audio engine `music`.

### 2. Hardware Initialization in `setup()`
```cpp
void setup() {
    k10.begin();
    k10.rgb->brightness(5);
}
```
- Sets system power rails and establishes medium LED brightness `5`.

### 3. Melody Execution Loop in `loop()`
```cpp
void loop() {
    k10.rgb->write(-1, 0x00FFFF); // Cyan LED during playback
    music.playMusic(ODE, Once);    // Plays Beethoven's Ode to Joy
    k10.rgb->write(-1, 0x000000); // Turn off LEDs

    delay(1000);
}
```
- `k10.rgb->write(-1, 0x00FFFF)`: Illuminates all 3 onboard LEDs in bright Cyan, serving as an active visual playback indicator.
- `music.playMusic(ODE, Once)`: Synthesizes the *Ode to Joy* melody note-by-note through the I2S speaker subsystem.
- `k10.rgb->write(-1, 0x000000)`: Turns off the LEDs as soon as the melody concludes.
- `delay(1000)`: Waits 1 second in silence before repeating the song.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes hardware, clocks, and audio buses. |
| `music.playMusic(melody, option)` | `Melodies melody`, `MelodyOptions option` | `void` | Plays a built-in melody tune (`ODE`, `BIRTHDAY`, etc.) using mode `Once` or `Forever`. |
| `k10.rgb->write(index, color)` | `int8_t index`, `uint32_t color` | `void` | Sets LED color for performance visualization. |
| `delay(ms)` | `unsigned long ms` | `void` | Resting pause between musical playback loops. |

---

## Full Code

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
