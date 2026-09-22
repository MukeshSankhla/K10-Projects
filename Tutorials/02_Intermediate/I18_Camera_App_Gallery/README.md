## What this tutorial is??

This tutorial demonstrates an advanced **Digital Camera Application with Live Viewfinder and Gallery Photo Browser** on the UNIHIKER K10.

Features:
- **Live Viewfinder Mode**: Continuous hardware background video streaming with a sleek camera viewfinder HUD, focus reticle crosshairs, and live status banners.
- **Snapshot Shutter**: Pressing **Button [A]** in camera mode triggers an optical white LED flash and saves the image to the MicroSD card (`S:/photo.bmp`).
- **Photo Gallery Mode**: Pressing **Button [B]** pauses the live stream and opens the photo gallery to view saved pictures.
- **Gallery Navigation**: While in the gallery, **Button [B]** cycles to the next photo in storage, and **Button [A]** exits back to the live camera preview.

---

## How it works

1. **Dual-Engine Display Architecture**:
   - Live Mode: `k10.setBgCamerImage(true)` streams camera frames directly to the display background layer, with UI overlays rendered on the canvas buffer.
   - Gallery Mode: `k10.setBgCamerImage(false)` disables the live stream so bitmap files loaded from the MicroSD card via `k10.canvas->canvasDrawImage()` remain visible without being overwritten.
2. **Dynamic Flash Shutter**:
   - Setting all RGB LED channels to full intensity (`0xFFFFFF`) creates a 200 ms flash strobe simulating a physical camera flash.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.initBgCamerImage()` | None | `void` | Initializes CMOS camera sensor hardware. |
| `k10.setBgCamerImage(bool)` | State | `void` | Enables or pauses background video streaming. |
| `k10.photoSaveToTFCard(path)` | File path | `void` | Captures active camera frame to BMP file on SD. |
| `k10.canvas->canvasDrawImage(...)` | Coords & path | `void` | Renders a saved BMP image to canvas. |
| `k10.buttonA->isPressed()` | None | `bool` | Shutter snap (Live) / Return to camera (Gallery). |
| `k10.buttonB->isPressed()` | None | `bool` | Open gallery (Live) / Next photo (Gallery). |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

bool isGalleryMode = false;
int galleryIndex = 0;
const char* galleryFiles[] = {
    "S:/photo.bmp",
    "S:/photo1.bmp",
    "S:/photo2.bmp"
};
const int totalGallery = sizeof(galleryFiles) / sizeof(galleryFiles[0]);

// Render Camera Viewfinder HUD overlay
void drawCameraHUD(String statusMessage, uint32_t statusColor) {
    k10.canvas->canvasClear();

    // 1. Top HUD Bar
    k10.canvas->canvasRectangle(0, 0, 240, 46, 0x000000, 0x000000, true);
    k10.canvas->canvasText("CAM & GALLERY", 42, 12, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);

    // 2. Status Banner
    k10.canvas->canvasRectangle(12, 50, 216, 24, 0x1E293B, 0x1E293B, true);
    k10.canvas->canvasText(statusMessage, 20, 54, statusColor,
                           k10.canvas->eCNAndENFont16, 26, false);

    // 3. Viewfinder Reticle Brackets
    k10.canvas->canvasLine(100, 160, 140, 160, 0x00E5FF);
    k10.canvas->canvasLine(120, 140, 120, 180, 0x00E5FF);
    k10.canvas->canvasRectangle(90, 130, 60, 60, 0x00E5FF, 0x000000, false);

    // 4. Bottom Controls Bar
    k10.canvas->canvasRectangle(0, 258, 240, 62, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Shutter Snap", 20, 268, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 20, false);
    k10.canvas->canvasText("[B] Open Gallery", 20, 292, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 20, false);

    k10.canvas->updateCanvas();
}

// Render Gallery Photo Viewer
void drawGalleryView(int index) {
    k10.setBgCamerImage(false); // Stop live camera background
    k10.canvas->canvasClear();

    // 1. Render photo from SD card
    k10.canvas->canvasDrawImage(0, 0, galleryFiles[index]);

    // 2. Top Header Overlay
    k10.canvas->canvasRectangle(0, 0, 240, 34, 0x000000, 0x000000, true);
    String headerStr = "Gallery: " + String(index + 1) + "/" + String(totalGallery);
    k10.canvas->canvasText(headerStr, 15, 8, 0xFEE715,
                           k10.canvas->eCNAndENFont16, 20, false);
    k10.canvas->canvasText(galleryFiles[index], 120, 8, 0x94A3B8,
                           k10.canvas->eCNAndENFont16, 16, false);

    // 3. Bottom Controls Overlay
    k10.canvas->canvasRectangle(0, 282, 240, 38, 0x000000, 0x000000, true);
    k10.canvas->canvasText("[A] Back to Cam", 15, 292, 0x00FF88,
                           k10.canvas->eCNAndENFont16, 16, false);
    k10.canvas->canvasText("[B] Next Image", 130, 292, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 16, false);

    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.initSDFile();          // Mount SD Card
    k10.initBgCamerImage();    // Initialize CMOS Camera
    k10.creatCanvas();

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x000000);

    // Start live background video streaming
    k10.setBgCamerImage(true);
    drawCameraHUD("Live Viewfinder Active", 0x00FF88);
}

void loop() {
    // ==========================================
    // Button A Action:
    // - In Live Mode: Snap photo to SD Card
    // - In Gallery Mode: Return to Live Camera
    // ==========================================
    if (k10.buttonA->isPressed()) {
        if (isGalleryMode) {
            isGalleryMode = false;
            k10.setBgCamerImage(true);
            drawCameraHUD("Live Viewfinder Active", 0x00FF88);
        } else {
            // Optical flash & snapshot
            k10.rgb->write(-1, 0xFFFFFF); // Flash on
            drawCameraHUD("Capturing frame...", 0xFF4444);

            k10.photoSaveToTFCard(galleryFiles[galleryIndex]);

            delay(200);
            k10.rgb->write(-1, 0x000000); // Flash off
            String savedMsg = "Saved: " + String(galleryFiles[galleryIndex]);
            drawCameraHUD(savedMsg, 0x00E5FF);
        }

        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    // ==========================================
    // Button B Action:
    // - In Live Mode: Open Gallery
    // - In Gallery Mode: Next Photo
    // ==========================================
    else if (k10.buttonB->isPressed()) {
        if (!isGalleryMode) {
            isGalleryMode = true;
            drawGalleryView(galleryIndex);
        } else {
            galleryIndex = (galleryIndex + 1) % totalGallery;
            drawGalleryView(galleryIndex);
        }

        while (k10.buttonB->isPressed()) {
            delay(50);
        }
    }

    delay(20);
}
```
