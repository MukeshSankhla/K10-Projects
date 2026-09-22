## What this tutorial is??

This tutorial demonstrates how to build an interactive **Electronic Photo Album** on the UNIHIKER K10. The application cycles through a collection of bitmap images stored on the onboard FAT32 MicroSD card with real-time navigation controls and metadata display.

Features:
- **Button [A]**: Navigates to the previous photograph in the album.
- **Button [B]**: Navigates to the next photograph in the album.
- **Card Framing**: Each image is displayed within an offset picture frame container alongside an album header, photo index counter (`Photo X of Y`), and file path caption.
- **Ambient Lighting**: Onboard RGB LEDs provide soft cyan backlighting.

---

## How it works

1. **MicroSD Storage Array**:
   - Photo file paths (`"S:/photo.bmp"`, `"S:/photo1.bmp"`, `"S:/photo2.bmp"`) are structured in a string array.
   - `k10.initSDFile()` initializes the SPI bus and mounts the MicroSD card to the system virtual drive `"S:/"`.
2. **Image Rendering via `canvasDrawImage`**:
   - `k10.canvas->canvasDrawImage(x, y, path)` loads and decodes the BMP image from the MicroSD card into the RAM canvas buffer at `(x, y)`.
   - The sketch renders an index banner below the frame and updates the display using `k10.canvas->updateCanvas()`.
3. **Circular Array Navigation & Debounce**:
   - When Button A or B is pressed, `currentPhotoIndex` is decremented or incremented cyclically using modulo arithmetic.
   - A `while (k10.button->isPressed())` polling loop prevents unintended rapid page skips.

---

## Sensors/actuator detaile

### 1. MicroSD (TF Card) Storage
- **Protocol**: 4-wire SPI (Pins 12, 21, 40, 41).
- **Filesystem**: Standard FAT32 partition.
- **Format**: 24-bit uncompressed RGB BMP images.

### 2. Dual Tactile Pushbuttons (A & B)
- **Inputs**: Active-low physical momentary buttons wired to dedicated ESP32-S3 GPIOs.
- **Debounce**: Polled in `loop()` with software hold guards.

---

## Step by step function wise code break down

### 1. Photo Array Setup
```cpp
const char* photoAlbum[] = {
    "S:/photo.bmp",
    "S:/photo1.bmp",
    "S:/photo2.bmp"
};
const int totalPhotos = sizeof(photoAlbum) / sizeof(photoAlbum[0]);
int currentPhotoIndex = 0;
```
- Defines the album playlist and tracks the active image index.

### 2. Rendering the Frame (`displayAlbumPhoto`)
```cpp
void displayAlbumPhoto(int index) {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("PHOTO ALBUM", 48, 10, 0xFEE715, k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasRectangle(10, 50, 220, 200, 0x334155, 0x000000, true);

    k10.canvas->canvasDrawImage(10, 50, photoAlbum[index]);

    String indexStr = "Photo " + String(index + 1) + " of " + String(totalPhotos);
    k10.canvas->canvasText(indexStr, 20, 225, 0x00E5FF, k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}
```

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes power supply, clocks, and internal buses. |
| `k10.initScreen(dir)` | `uint8_t dir` (`2` = Portrait) | `void` | Initializes 2.8" ST7789 display (240x320). |
| `k10.initSDFile()` | None | `void` | Mounts the MicroSD card under root `"S:/"`. |
| `k10.canvas->canvasDrawImage(...)` | `int x, int y, const char* path` | `void` | Decodes and renders a BMP image from the SD card. |
| `k10.canvas->canvasRectangle(...)` | `x, y, w, h, borderCol, fillCol, fill` | `void` | Draws rectangular photo borders and banners. |
| `k10.canvas->updateCanvas()` | None | `void` | Transmits the canvas frame buffer to the physical screen. |
| `k10.buttonA->isPressed()` | None | `bool` | Detects physical press on Button A. |
| `k10.buttonB->isPressed()` | None | `bool` | Detects physical press on Button B. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Array of photo filenames on the MicroSD card
const char* photoAlbum[] = {
    "S:/photo.bmp",
    "S:/photo1.bmp",
    "S:/photo2.bmp"
};
const int totalPhotos = sizeof(photoAlbum) / sizeof(photoAlbum[0]);
int currentPhotoIndex = 0;

// Render modern photo frame HUD with image title & control legend
void displayAlbumPhoto(int index) {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x0F172A);

    // 1. Top Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText("PHOTO ALBUM", 48, 10, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // 2. Photo Display Area (Centered in 220x200 frame)
    k10.canvas->canvasRectangle(10, 50, 220, 200, 0x334155, 0x000000, true);

    // Draw the BMP image from SD Card
    k10.canvas->canvasDrawImage(10, 50, photoAlbum[index]);

    // 3. Photo Meta & Index Badge
    String indexStr = "Photo " + String(index + 1) + " of " + String(totalPhotos);
    k10.canvas->canvasText(indexStr, 20, 225, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 4. Bottom Controls Bar
    k10.canvas->canvasLine(10, 260, 230, 260, 0x334155);
    k10.canvas->canvasText("[A] Prev Photo", 20, 272, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("[B] Next Photo", 130, 272, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText(photoAlbum[index], 20, 298, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    k10.initSDFile(); // Mount MicroSD card

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF); // Soft cyan ambient glow

    // Display initial photo
    displayAlbumPhoto(currentPhotoIndex);
}

void loop() {
    // Button A: Previous Photo
    if (k10.buttonA->isPressed()) {
        currentPhotoIndex--;
        if (currentPhotoIndex < 0) {
            currentPhotoIndex = totalPhotos - 1;
        }
        displayAlbumPhoto(currentPhotoIndex);

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // Button B: Next Photo
    else if (k10.buttonB->isPressed()) {
        currentPhotoIndex++;
        if (currentPhotoIndex >= totalPhotos) {
            currentPhotoIndex = 0;
        }
        displayAlbumPhoto(currentPhotoIndex);

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(30);
}
```
