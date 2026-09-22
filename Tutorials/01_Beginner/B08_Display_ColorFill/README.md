## What this tutorial is??

This tutorial introduces the **2.8-inch color TFT display** of the UNIHIKER K10 with an interactive **Palette Card Showcase**.

You will learn how to initialize the display controller, establish portrait orientation, allocate a graphics canvas buffer in RAM, flood the display panel with solid 24-bit background color washes, and render a semi-translucent dark HUD card displaying color names, hex badges, and preset metrics.

### Expected Behavior
The display cycles sequentially through 6 curated color presets every 1.5 seconds:
1. **Nordic Teal** (`#008080`)
2. **Warm Amber** (`#F59E0B`)
3. **Royal Purple** (`#7C3AED`)
4. **Cobalt Navy** (`#1D4ED8`)
5. **Emerald Marine** (`#059669`)
6. **Crimson Rose** (`#DC2626`)

Each preset dynamically updates the centered HUD card and synchronizes with the onboard RGB LEDs.

---

## How it works

1. **Screen Controller & Architecture**:
   - The UNIHIKER K10 features an integrated **2.8-inch IPS TFT LCD** driven by the industry-standard **Sitronix ST7789V** display controller.
   - The display has a physical resolution of **240 $\times$ 320 pixels**, providing $240 \times 320 = 76,800\text{ individual RGB color pixels}$.
2. **Screen Orientation (`initScreen`)**:
   - The screen can be mounted or viewed in 4 rotational angles ($90^\circ$ increments):
     - `0`: Normal Landscape ($320 \times 240$)
     - `1`: Normal Portrait ($240 \times 320$)
     - `2`: Inverted Landscape ($320 \times 240$, standard portrait orientation for K10 hand-held use)
     - `3`: Inverted Portrait ($240 \times 320$)
   - Calling `k10.initScreen(2)` sets up the portrait orientation used throughout the beginner tutorial series.
3. **Frame Buffer Allocation (`creatCanvas`)**:
   - Direct pixel-by-pixel rendering across an external SPI bus can cause visible screen tearing.
   - Calling `k10.creatCanvas()` allocates a dedicated double-buffer (canvas memory) in the ESP32-S3's internal high-speed SRAM.
4. **Full-Screen Color Fill**:
   - `k10.setScreenBackground(color)` instructs the ST7789 controller to fill the active memory window with the specified 24-bit Hex RGB color, instantly clearing previous content and setting the ambient theme.

---

## Sensors/actuator detaile

### What is a TFT LCD Display?
A **TFT LCD (Thin-Film Transistor Liquid Crystal Display)** is an active-matrix flat panel screen:
1. **Backlight Unit**: High-efficiency white LEDs illuminate the rear of the panel.
2. **Polarizing Filters**: Two polarizing sheets oriented perpendicular to each other block light from passing through unless rotated.
3. **Liquid Crystal Layer**: Nematic liquid crystal molecules twist when an electric field is applied by microscopic thin-film transistors deposited directly onto the glass substrate. This controls the exact amount of backlight passing through each sub-pixel.
4. **Color Filter Glass**: Microscopic Red, Green, and Blue filter dyes overlay each transistor, creating sub-pixel triplets that blend into vivid full-color imagery.

```
 [ White LED Backlight ]
          │││ (Light rays)
 ┌────────┴┴┴────────┐  Rear Polarizing Glass
 ├───────────────────┤  Thin-Film Transistor (TFT) Array Matrix
 ├───────────────────┤  Nematic Liquid Crystal Molecules (Voltage-Switched)
 ├───────────────────┤  RGB Color Filter Glass (Red, Green, Blue sub-pixels)
 ├───────────────────┤  Front Polarizing Glass
 └───────────────────┘
      (Image Out)
```

### Color Formats: RGB565 vs. RGB888
- **RGB888 (24-bit True Color)**: Uses 8 bits per channel ($2^{24} = 16.7\text{ million colors}$). This is the user-friendly format you supply in Arduino code (`0xRRGGBB`).
- **RGB565 (16-bit High Color)**: Most embedded SPI controllers convert 24-bit color into 16 bits to cut SPI bus bandwidth in half:
  - **Red**: 5 bits ($2^5 = 32$ levels)
  - **Green**: 6 bits ($2^6 = 64$ levels, extra precision for human eye sensitivity)
  - **Blue**: 5 bits ($2^5 = 32$ levels)
- The UNIHIKER K10 driver automatically converts 24-bit hex colors (`0x008080`) into high-performance 16-bit commands under the hood.

---

## Step by step function wise code break down

### 1. Library Inclusion & Global Setup
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
```
- Loads board drivers and creates board manager `k10`.

### 2. Display Hardware Initialization in `setup()`
```cpp
void setup() {
    k10.begin();
    k10.initScreen(2); // Portrait orientation
    k10.creatCanvas();
}
```
- `k10.begin()`: Initializes system power and SPI display clocks.
- `k10.initScreen(2)`: Configures the ST7789 controller in Portrait mode ($240 \times 320$).
- `k10.creatCanvas()`: Instantiates the 2D graphics canvas drawing buffer in ESP32-S3 RAM.

### 3. Background Color Cycling in `loop()`
```cpp
void loop() {
    k10.setScreenBackground(0x008080); // Teal
    delay(1500);

    k10.setScreenBackground(0xFFBF00); // Amber
    delay(1500);

    k10.setScreenBackground(0x800080); // Purple
    delay(1500);

    k10.setScreenBackground(0x000080); // Navy
    delay(1500);
}
```
- Each call to `k10.setScreenBackground(color)` repaints the entire screen area with a rich 24-bit color.
- `delay(1500)` maintains each color wash for 1.5 seconds.

---

## API and Functions detailes

| API / Method | Parameters | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Powers on display backlight and peripheral buses. |
| `k10.initScreen(dir)` | `int dir` (`0` to `3`) | `void` | Initializes ST7789 display controller in the desired rotational orientation (`2` = Portrait). |
| `k10.creatCanvas()` | None | `void` | Allocates internal graphical frame buffer in microcontroller RAM. |
| `k10.setScreenBackground(color)` | `uint32_t color` (Hex RGB) | `void` | Floods the full display surface with the specified 24-bit color. |
| `delay(ms)` | `unsigned long ms` | `void` | Pauses execution to establish transition timing. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

struct ColorPreset {
    const char* name;
    uint32_t hexValue;
    const char* hexStr;
};

const ColorPreset presets[] = {
    {"Nordic Teal",    0x008080, "#008080"},
    {"Warm Amber",     0xF59E0B, "#F59E0B"},
    {"Royal Purple",   0x7C3AED, "#7C3AED"},
    {"Cobalt Navy",    0x1D4ED8, "#1D4ED8"},
    {"Emerald Marine", 0x059669, "#059669"},
    {"Crimson Rose",   0xDC2626, "#DC2626"}
};
const int totalPresets = sizeof(presets) / sizeof(presets[0]);

void displayColorCard(int index) {
    k10.setScreenBackground(presets[index].hexValue);
    k10.canvas->canvasClear();

    // 1. Semi-translucent dark HUD card centered in the screen
    k10.canvas->canvasRectangle(24, 110, 192, 100, 0xFFFFFF, 0x0F172A, true);

    // Color Name (Centered)
    String nameStr = String(presets[index].name);
    int nameX = 120 - (int)(nameStr.length() * 4);
    k10.canvas->canvasText(nameStr, nameX, 126, 0xF8FAFC,
                           k10.canvas->eCNAndENFont16, 20, false);

    // Hex Code Badge
    String hexStr = String(presets[index].hexStr);
    int hexX = 120 - (int)(hexStr.length() * 4);
    k10.canvas->canvasText(hexStr, hexX, 154, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);

    // Index Counter
    String countStr = String(index + 1) + " of " + String(totalPresets);
    int countX = 120 - (int)(countStr.length() * 4);
    k10.canvas->canvasText(countStr, countX, 180, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();

    // Mirror on RGB LED
    k10.rgb->write(-1, presets[index].hexValue);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();

    k10.rgb->brightness(5);
}

void loop() {
    for (int i = 0; i < totalPresets; i++) {
        displayColorCard(i);
        delay(1500);
    }
}
```
