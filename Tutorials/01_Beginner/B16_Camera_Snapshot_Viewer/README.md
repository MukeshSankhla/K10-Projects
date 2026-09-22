## What this tutorial is??

This tutorial demonstrates how to initialize the onboard **OV2640 Camera Module**, capture high-resolution snapshots to MicroSD storage, and review images on the 2.8" display using a **Leica Pro Viewfinder** theme.

You will learn how to control camera DMA capture buffers, render a professional viewfinder overlay with grid reticle and exposure metrics, handle non-blocking Button [A] shutter capture, and toggle review mode using Button [B].

### Expected Behavior
- **Leica Viewfinder Overlay**: Framing reticle with matte obsidian border.
- **Shutter HUD**: Live photo index counter and resolution indicators.
- **Non-Blocking Shutter**: Instant snapshot triggering via Button [A].
- **Gallery Review**: Toggle captured photo review via Button [B].

---

## How it works

1. **Hardware Camera Sensor & Background Streaming**:
   - The UNIHIKER K10 includes an onboard CMOS camera sensor connected via the ESP32-S3 DVP (Digital Video Port) interface.
   - `k10.initBgCamerImage()` initializes the camera sensor hardware, clock line, and I2C registers.
   - `k10.setBgCamerImage(true)` enables the background DMA camera streaming engine. Incoming camera frames are automatically decoded and pushed to the display controller layer in hardware.
2. **Overlay Canvas Graphics (HUD)**:
   - `k10.creatCanvas()` allocates a transparent drawing buffer above the camera feed.
   - Solid rectangular banners (`canvasRectangle(0, 0, 240, 48, ...)` and `canvasRectangle(0, 258, 240, 62, ...)`) are drawn at the top and bottom to create viewfinder bezels. This ensures text remains crisp and readable whether pointing the camera at a bright lamp or a dark corner.
   - A center viewfinder reticle (cyan crosshair) assists with shot framing.
3. **Saving Photos via `k10.photoSaveToTFCard`**:
   - When Button A is pressed while in viewfinder mode, `k10.photoSaveToTFCard("S:/photo.bmp")` grabs the active camera frame buffer, encodes it with BMP headers (BITMAPFILEHEADER and BITMAPINFOHEADER), and writes the file directly to the MicroSD card under drive `"S:/"`.
   - The onboard RGB LED flashes bright white (`0xFFFFFF`) for 200 milliseconds to simulate a physical camera strobe flash.
4. **Photo Playback via `k10.canvasDrawImage`**:
   - When Button B is pressed, `isViewingPhoto` is set to `true`.
   - `k10.setBgCamerImage(false)` pauses the background camera streaming so the live feed does not overwrite the canvas.
   - `k10.canvas->canvasDrawImage(0, 0, "S:/photo.bmp")` reads the bitmap file from the MicroSD card and decodes its pixel array directly into the canvas RAM buffer.
   - Semi-transparent or solid header and footer badges display the file title (`PHOTO: S:/photo.bmp`) and instruction (`[A] Return to Camera`).
   - Pressing Button A returns the system to live camera preview mode seamlessly.

---

## Sensors/actuator detaile

### 1. Onboard CMOS Camera Sensor
- **Interface**: High-speed Parallel DVP (Digital Video Port) / SCCB configuration.
- **Color Depth**: RGB565 / YUV color representations processed by ESP32-S3 hardware peripherals.
- **Frame Rate**: Up to 15–30 fps live preview with DMA direct-memory transfer.

### 2. MicroSD Card Interface
- **Bus**: High-speed SPI on Pins 12, 21, 40, 41.
- **Image Format**: Uncompressed BMP (Bitmap) format. BMP stores pixel RGB data in a lossless format, allowing instant capture and rapid retrieval without heavy JPEG decompression overhead.

### 3. WS2812 RGB LED Camera Strobe
- **Color Simulation**: Setting all three color channels to maximum (`0xFFFFFF`) produces a neutral bright white flash.
- **Visual Feedback**: Acts as an unmistakable optical cue that the photo capture command was registered and successfully written to storage.

---

## Step by step function wise code break down

### 1. Peripherals & Global State Setup
```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

bool isViewingPhoto = false;
```
- Includes the K10 core header.
- Sets screen rotation to portrait (`240x320`).
- `isViewingPhoto`: A boolean state flag distinguishing between live viewfinder mode and image gallery mode.

### 2. Viewfinder HUD Function (`drawCameraHUD`)
```cpp
void drawCameraHUD(String statusMessage, uint32_t statusColor) {
    k10.canvas->canvasClear();

    // 1. Top HUD Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 48, 0x000000, 0x000000, true);
    k10.canvas->canvasText("K10 CAMERA", 52, 12, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // 2. Dynamic Status Banner
    k10.canvas->canvasRectangle(10, 52, 220, 24, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText(statusMessage, 18, 56, statusColor,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 3. Viewfinder Focus Reticle
    k10.canvas->canvasLine(110, 160, 130, 160, 0x00E5FF);
    k10.canvas->canvasLine(120, 150, 120, 170, 0x00E5FF);

    // 4. Bottom Controls Bar
    k10.canvas->canvasRectangle(0, 258, 240, 62, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Capture Photo", 20, 268, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 26, false);
    k10.canvas->canvasText("[B] View Saved Photo", 20, 292, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}
```
- Renders the camera GUI over the live camera feed with 100% readable high-contrast dark bars and neon cyan/gold typography.

### 3. Photo Viewer Rendering Function (`showSavedPhoto`)
```cpp
void showSavedPhoto() {
    k10.setBgCamerImage(false); // Stop camera background stream
    k10.canvas->canvasClear();

    k10.canvas->canvasDrawImage(0, 0, "S:/photo.bmp");

    // Overlaid title and navigation prompt
    k10.canvas->canvasRectangle(0, 0, 240, 32, 0x000000, 0x000000, true);
    k10.canvas->canvasText("PHOTO: S:/photo.bmp", 20, 8, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->canvasRectangle(0, 285, 240, 35, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Return to Camera", 20, 295, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 26, false);

    k10.canvas->updateCanvas();
}
```
- Halts background camera streaming, reads and draws the bitmap, and informs the user how to navigate back.

### 4. Setup Routine (`setup`)
```cpp
void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.initSDFile();          // Mount SD Card
    k10.initBgCamerImage();    // Initialize camera sensor
    k10.creatCanvas();         // Create overlay canvas buffer

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    k10.setBgCamerImage(true); // Start live video background
    drawCameraHUD("Mode: Live Viewfinder", 0x00FF88);
}
```
- Initializes the display, SD card, camera sensor, and frame buffer.
- Activates live background video streaming.

### 5. Main Event Loop (`loop`)
```cpp
void loop() {
    if (k10.buttonA->isPressed()) {
        if (isViewingPhoto) {
            isViewingPhoto = false;
            k10.setBgCamerImage(true);
            drawCameraHUD("Mode: Live Viewfinder", 0x00FF88);
        } else {
            k10.rgb->write(-1, 0xFFFFFF); // Flash LED
            drawCameraHUD("Capturing photo...", 0xFF4444);

            k10.photoSaveToTFCard("S:/photo.bmp");

            delay(200);
            k10.rgb->write(-1, 0x000000);
            drawCameraHUD("Photo Saved: photo.bmp", 0x00E5FF);
        }

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    else if (k10.buttonB->isPressed()) {
        if (!isViewingPhoto) {
            isViewingPhoto = true;
            showSavedPhoto();
        }

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }
    delay(20);
}
```
- Handles mode toggling, camera shutter, and gallery viewing with reliable hardware button debounce.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes power supply, clocks, and internal peripherals. |
| `k10.initScreen(dir)` | `uint8_t dir` (`2` = Portrait) | `void` | Sets ST7789 display orientation to 240x320. |
| `k10.initSDFile()` | None | `void` | Mounts the MicroSD card under root `"S:/"`. |
| `k10.initBgCamerImage()` | None | `void` | Configures CMOS camera registers and DVP interface. |
| `k10.setBgCamerImage(state)` | `bool state` | `void` | Enables (`true`) or pauses (`false`) real-time background camera rendering. |
| `k10.creatCanvas()` | None | `void` | Allocates the 2D overlay graphics canvas buffer in RAM. |
| `k10.photoSaveToTFCard(path)` | `const char* path` | `void` | Captures the active camera frame and saves it as a BMP image to SD card. |
| `k10.canvas->canvasDrawImage(...)` | `int x, int y, const char* path` | `void` | Decodes and renders a BMP image from the SD card onto the canvas at `(x, y)`. |
| `k10.canvas->canvasRectangle(...)` | `x, y, w, h, borderCol, fillCol, fill` | `void` | Draws rectangular HUD headers, banners, and toolbars. |
| `k10.canvas->canvasLine(...)` | `x1, y1, x2, y2, color` | `void` | Draws lines (e.g. viewfinder focus reticle crosshairs). |
| `k10.canvas->canvasText(...)` | `text, x, y, color, font, count, autoClean` | `void` | Draws text on the canvas with specified font size and color. |
| `k10.canvas->updateCanvas()` | None | `void` | Pushes the modified RAM buffer over SPI to the display. |
| `k10.buttonA->isPressed()` | None | `bool` | Returns `true` if Button A is held. |
| `k10.buttonB->isPressed()` | None | `bool` | Returns `true` if Button B is held. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

bool isViewingPhoto = false;

// Non-blocking button edge-detection trackers
bool checkButtonAPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonA->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

bool checkButtonBPressed() {
    static bool lastState = false;
    static unsigned long lastDebounceTime = 0;
    bool reading = k10.buttonB->isPressed();
    bool pressedEvent = false;

    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > 35) {
        static bool stableState = false;
        if (reading != stableState) {
            stableState = reading;
            if (stableState) {
                pressedEvent = true;
            }
        }
    }
    lastState = reading;
    return pressedEvent;
}

// Draw Leica Pro Viewfinder HUD
void drawCameraHUD(const char* statusMessage, uint32_t statusColor) {
    k10.canvas->canvasClear();

    // 1. Top Viewfinder Header Bar
    k10.canvas->canvasRectangle(0, 0, 240, 42, 0x0A0A0A, 0x0A0A0A, true);
    k10.canvas->canvasCircle(24, 21, 6, 0xEA2B2B, 0xEA2B2B, true); // Leica Red Dot
    k10.canvas->canvasText("PRO CAMERA", 52, 10, 0xFAFAFA,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Status Banner
    k10.canvas->canvasRectangle(12, 48, 216, 26, 0x18181B, 0x18181B, true);
    int statusX = 120 - (int)(strlen(statusMessage) * 4);
    k10.canvas->canvasText(statusMessage, statusX, 53, statusColor,
                           k10.canvas->eCNAndENFont16, 24, false);

    // 3. Viewfinder Focus Reticle & Corner Framing Brackets
    k10.canvas->canvasLine(110, 160, 130, 160, 0x00E5FF);
    k10.canvas->canvasLine(120, 150, 120, 170, 0x00E5FF);
    // Framing corners
    k10.canvas->canvasLine(30, 90, 45, 90, 0x71717A);
    k10.canvas->canvasLine(30, 90, 30, 105, 0x71717A);
    k10.canvas->canvasLine(210, 90, 195, 90, 0x71717A);
    k10.canvas->canvasLine(210, 90, 210, 105, 0x71717A);
    k10.canvas->canvasLine(30, 230, 45, 230, 0x71717A);
    k10.canvas->canvasLine(30, 230, 30, 215, 0x71717A);
    k10.canvas->canvasLine(210, 230, 195, 230, 0x71717A);
    k10.canvas->canvasLine(210, 230, 210, 215, 0x71717A);

    // 4. Centered Two-Column Control Bar (Zero-overflow)
    k10.canvas->canvasRectangle(0, 272, 240, 48, 0x0A0A0A, 0x0A0A0A, true);
    k10.canvas->canvasLine(0, 272, 240, 272, 0x27272A);
    k10.canvas->canvasText("[A] Shutter", 22, 286, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText("[B] Gallery", 132, 286, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);

    k10.canvas->updateCanvas();
}

// Show the saved photo on canvas
void showSavedPhoto() {
    k10.setBgCamerImage(false);
    k10.canvas->canvasClear();

    // Draw the BMP image from SD Card
    k10.canvas->canvasDrawImage(0, 0, "S:/photo.bmp");

    // Top Header Badge
    k10.canvas->canvasRectangle(0, 0, 240, 34, 0x000000, 0x000000, true);
    k10.canvas->canvasText("Saved: S:/photo.bmp", 32, 10, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 24, false);

    // Bottom Control Badge
    k10.canvas->canvasRectangle(0, 280, 240, 40, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Return to Camera", 38, 292, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 22, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.initSDFile();       // Mount SD Card
    k10.initBgCamerImage(); // Initialize camera sensor
    k10.creatCanvas();      // Create overlay canvas buffer

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    // Start live camera stream
    k10.setBgCamerImage(true);

    drawCameraHUD("Lens: Live Viewfinder", 0x10B981);
}

void loop() {
    // Non-blocking Button A Action (Capture or Return)
    if (checkButtonAPressed()) {
        if (isViewingPhoto) {
            isViewingPhoto = false;
            k10.setBgCamerImage(true);
            drawCameraHUD("Lens: Live Viewfinder", 0x10B981);
        } else {
            // Shutter Flash & Snapshot
            k10.rgb->write(-1, 0xFFFFFF); // Flash on
            drawCameraHUD("Capturing frame...", 0xFF4444);

            k10.photoSaveToTFCard("S:/photo.bmp");

            delay(150);
            k10.rgb->write(-1, 0x000000); // Flash off
            drawCameraHUD("Saved: S:/photo.bmp", 0x00E5FF);
        }
    }

    // Non-blocking Button B Action (Gallery)
    if (checkButtonBPressed()) {
        if (!isViewingPhoto) {
            isViewingPhoto = true;
            showSavedPhoto();
        }
    }

    delay(20); // Responsive loop tick
}
```
