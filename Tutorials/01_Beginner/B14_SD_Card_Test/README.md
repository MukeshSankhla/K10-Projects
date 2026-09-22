## What this tutorial is??

This tutorial demonstrates how to interface with the UNIHIKER K10 onboard MicroSD (TF) card slot using SPI communication and the FAT32 filesystem with compact graphical typography.

The sketch tests for the physical presence of a MicroSD card and inspects its contents:
- If **no card is inserted** (or initialization fails), the screen displays `"No SD card found"`.
- If the card is **connected but empty**, the screen displays `"SD card connected but no files"` and reports the card format type (SDHC, SDSC, MMC).
- If files or folders exist on the card, the screen displays a green success status, reports the card format, and lists up to 10 file and directory names in a neat, compact 16px font.
- It supports interactive re-scanning via **Button A** so you can insert or swap cards without rebooting the board.

---

## How it works

1. **Hardware Communication**:
   - The MicroSD card slot on the UNIHIKER K10 is connected to the ESP32-S3 microcontroller via a dedicated 4-wire Serial Peripheral Interface (SPI) bus:
     - **MOSI (Master Out Slave In)**: Pin 21
     - **MISO (Master In Slave Out)**: Pin 41
     - **SCK (Serial Clock)**: Pin 12
     - **CS / SS (Chip Select)**: Pin 40
2. **Mounting & Detection**:
   - When `SD.begin()` is invoked, the ESP32-S3 asserts the Chip Select (CS) line low and sends SPI initialization commands (`CMD0`, `CMD8`, `ACMD41`) to the SD controller inside the card.
   - If the card acknowledges these initialization handshakes, `SD.begin()` returns `true` and mounts the FAT32/exFAT filesystem partition into the system root (`/`).
   - If the card is absent or unformatted, the handshake fails, returning `false` and reporting `SD.cardType() == CARD_NONE`.
3. **Small-Font Text Rendering**:
   - Unlike row-based text methods which enforce a large 24px font, this sketch utilizes the coordinate-based API:
     `k10.canvas->canvasText(text, x, y, color, k10.canvas->eCNAndENFont16, count, false)`
   - The 16-pixel font allows up to 26 characters per line and tightly spaced 19px vertical intervals, enabling filenames, directory tags (`[DIR]`), and multi-line guidance messages to fit comfortably on the 240x320 screen.
4. **Directory Traversal**:
   - Once mounted, the sketch opens the root directory (`/`) using `SD.open("/")`.
   - The loop iteratively calls `root.openNextFile()` to read directory entries sequentially.
   - For each entry, it checks if it is a directory (`file.isDirectory()`) or a regular file, extracts the filename (`file.name()`), formats it cleanly, and renders it in cyan (`0x00E5FF`).
5. **Interactive Scanning**:
   - In `loop()`, the sketch monitors onboard `Button A`. Pressing Button A triggers an immediate re-scan, clearing the display buffer and re-evaluating the card status.

---

## Sensors/actuator detaile

### MicroSD (TransFlash / TF) Card Technology
A MicroSD card is a non-volatile flash storage medium based on NAND flash memory technology:

1. **Flash Memory Architecture**:
   - Unlike volatile RAM (which loses data when powered off), NAND flash stores electrons in microscopic floating-gate transistors or charge-trap cells.
   - Even without electrical power, the trapped charge remains intact for years, preserving images, logs, audio files, and model weights.
2. **Onboard Controller & SPI Protocol**:
   - Every MicroSD card contains both raw flash memory and an integrated microcontroller (SD controller).
   - While modern PCs use 4-bit high-speed SDIO, microcontrollers like the ESP32 commonly communicate via **SPI mode** (Serial Peripheral Interface), a synchronous serial protocol utilizing a master clock line (`SCK`), data input (`MISO`), data output (`MOSI`), and an active-low chip select line (`CS`).
3. **Card Formats & Capacities**:
   - **SDSC (Standard Capacity)**: Up to 2 GB (FAT16)
   - **SDHC (High Capacity)**: 4 GB to 32 GB (FAT32) — *Recommended for UNIHIKER K10*
   - **SDXC (Extended Capacity)**: 64 GB to 2 TB (exFAT)
4. **Pinout & Terminals**:
   - MicroSD cards have 8 gold contact fingers:
     - **Pin 1 (DAT2 / NC)**: Data Line 2 (Unused in SPI)
     - **Pin 2 (CD/DAT3 / CS)**: Chip Select / Slave Select (Active LOW)
     - **Pin 3 (CMD / MOSI)**: Command / Master Out Slave In
     - **Pin 4 (VDD)**: 3.3V Power Supply
     - **Pin 5 (CLK / SCK)**: Serial Clock
     - **Pin 6 (VSS)**: Ground (0V)
     - **Pin 7 (DAT0 / MISO)**: Data Out / Master In Slave Out
     - **Pin 8 (DAT1 / NC)**: Data Line 1 (Unused in SPI)
5. **Proper Formatting for Embedded Use**:
   - MicroSD cards should be formatted as **FAT32** with 32KB allocation unit size for maximum compatibility with Arduino ESP32 libraries.

---

## Step by step function wise code break down

### 1. Header Inclusions & Global Setup
```cpp
#include "unihiker_k10.h"
#include "FS.h"
#include "SD.h"

UNIHIKER_K10 k10;
```
- `#include "unihiker_k10.h"`: Provides core access to screen, canvas, buttons, and onboard peripherals.
- `#include "FS.h"` & `#include "SD.h"`: Standard Arduino ESP32 filesystem libraries that manage directory structures, file handles, and SPI communication.
- `UNIHIKER_K10 k10;`: Instantiates the master board object.

### 2. Card Type Helper Function
```cpp
String getCardType() {
    uint8_t type = SD.cardType();
    switch (type) {
        case CARD_MMC:  return "MMC";
        case CARD_SD:   return "SDSC";
        case CARD_SDHC: return "SDHC";
        default:        return "Unknown";
    }
}
```
- Queries the SD controller and maps hardware card types to human-readable strings.

### 3. The `checkSDCard()` Diagnostic Function
```cpp
void checkSDCard() {
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);

    k10.canvas->canvasText("SD CARD TEST", 45, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    k10.canvas->canvasLine(15, 42, 225, 42, 0x334155);
```
- Wipes the frame buffer and draws the 24px bold title and subtle divider line.

```cpp
    bool isConnected = SD.begin();

    if (!isConnected || SD.cardType() == CARD_NONE) {
        k10.canvas->canvasText("Status: Not Connected", 15, 55, 0xFF5555,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("No SD card found", 15, 80, 0xFF5555,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("Please insert a FAT32", 15, 120, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("MicroSD card and", 15, 140, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("press Button A to scan.", 15, 160, 0x00E5FF,
                               k10.canvas->eCNAndENFont16, 26, false);
    }
```
- When no card is detected, prints detailed troubleshooting steps in the small 16px font (`eCNAndENFont16`).

```cpp
    else {
        String cardType = getCardType();
        File root = SD.open("/");
        ...
        while (file) {
            fileCount++;
            if (fileCount <= maxDisplayFiles) {
                ...
                k10.canvas->canvasText(fileName, 15, yPos, 0x00E5FF,
                                       k10.canvas->eCNAndENFont16, 26, false);
                yPos += 19;
            }
            file = root.openNextFile();
        }
```
- When a card is detected, lists up to 10 files with 19px spacing in the compact 16px font.

### 4. `setup()` & `loop()` Functions
```cpp
void setup() {
    k10.begin();
    k10.initScreen(2);
    k10.creatCanvas();
    k10.setScreenBackground(0x000000);
    checkSDCard();
}

void loop() {
    if (k10.buttonA->isPressed()) {
        k10.canvas->canvasText("Scanning SD card...", 15, 300, 0xFFFF00,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->updateCanvas();
        delay(300);
        checkSDCard();
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }
    delay(50);
}
```
- Handles boot-time initialization and provides responsive button polling with debounce.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.begin()` | None | `void` | Initializes power supply, clocks, and I2C/SPI buses on UNIHIKER K10. |
| `k10.initScreen(dir)` | `int dir` (`2` = Portrait) | `void` | Initializes the 2.8" ST7789 TFT screen in portrait mode (240x320). |
| `k10.creatCanvas()` | None | `void` | Allocates the internal graphics drawing buffer in RAM. |
| `k10.setScreenBackground(color)` | `uint32_t color` (Hex RGB) | `void` | Fills the background frame buffer with a 24-bit RGB color. |
| `k10.canvas->canvasClear()` | None | `void` | Clears all graphics and properties on the canvas buffer. |
| `k10.canvas->canvasText(...)` | `text, x, y, color, font, count, autoClean` | `void` | Renders text with pixel coordinate precision and font selection (`eCNAndENFont16` or `eCNAndENFont24`). |
| `k10.canvas->canvasLine(x1, y1, x2, y2, color)` | Line coordinates & color | `void` | Draws a 1-pixel horizontal, vertical, or diagonal line. |
| `k10.canvas->updateCanvas()` | None | `void` | Sends the updated RAM canvas buffer over SPI to the ST7789 display. |
| `SD.begin()` | None (uses default SS pin 40) | `bool` | Initializes SPI communication with the MicroSD card; returns `true` on success. |
| `SD.cardType()` | None | `sdcard_type_t` | Returns card category (`CARD_NONE`, `CARD_MMC`, `CARD_SD`, `CARD_SDHC`). |
| `SD.open(path)` | `const char* path` | `File` | Opens a file or directory handle from the root filesystem. |
| `file.isDirectory()` | None | `bool` | Returns `true` if the current file handle is a subdirectory. |
| `file.name()` | None | `const char*` | Returns the ASCII name of the file or directory. |
| `root.openNextFile()` | None | `File` | Returns the file handle of the next sibling item in the directory. |
| `k10.buttonA->isPressed()` | None | `bool` | Polls the physical contact state of Button A (`true` when held down). |

---

## Full Code

```cpp
#include "unihiker_k10.h"
#include "FS.h"
#include "SD.h"

UNIHIKER_K10 k10;

// Helper to get descriptive SD card format type
String getCardType() {
    uint8_t type = SD.cardType();
    switch (type) {
        case CARD_MMC:  return "MMC";
        case CARD_SD:   return "SDSC";
        case CARD_SDHC: return "SDHC";
        default:        return "Unknown";
    }
}

// Function to scan and display SD card status & file listings using small font
void checkSDCard() {
    // Clear canvas and reset background
    k10.canvas->canvasClear();
    k10.setScreenBackground(0x000000);

    // Header Title (24px bold)
    k10.canvas->canvasText("SD CARD TEST", 45, 14, 0xFEE715,
                           k10.canvas->eCNAndENFont24, 20, false);
    // Decorative separator rule
    k10.canvas->canvasLine(15, 42, 225, 42, 0x334155);

    // Initialize/detect SD card
    bool isConnected = SD.begin();

    if (!isConnected || SD.cardType() == CARD_NONE) {
        // SD card not detected or cannot be initialized (small 16px font)
        k10.canvas->canvasText("Status: Not Connected", 15, 55, 0xFF5555,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("No SD card found", 15, 80, 0xFF5555,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("Please insert a FAT32", 15, 120, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("MicroSD card and", 15, 140, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->canvasText("press Button A to scan.", 15, 160, 0x00E5FF,
                               k10.canvas->eCNAndENFont16, 26, false);
    } else {
        // SD card detected successfully!
        String cardType = getCardType();
        File root = SD.open("/");

        if (!root || !root.isDirectory()) {
            k10.canvas->canvasText("Status: Read Error", 15, 55, 0xFF5555,
                                   k10.canvas->eCNAndENFont16, 26, false);
            k10.canvas->canvasText("Cannot open root dir", 15, 80, 0xFF5555,
                                   k10.canvas->eCNAndENFont16, 26, false);
        } else {
            int fileCount = 0;
            int yPos = 95;
            const int maxDisplayFiles = 10;
            File file = root.openNextFile();

            while (file) {
                fileCount++;
                if (fileCount <= maxDisplayFiles) {
                    String fileName = String(file.name());
                    if (fileName.startsWith("/")) {
                        fileName = fileName.substring(1);
                    }
                    if (file.isDirectory()) {
                        fileName = "[DIR] " + fileName;
                    }
                    // Display each file in small 16px font
                    k10.canvas->canvasText(fileName, 15, yPos, 0x00E5FF,
                                           k10.canvas->eCNAndENFont16, 26, false);
                    yPos += 19;
                }
                file = root.openNextFile();
            }

            if (fileCount == 0) {
                // Connected, but empty filesystem (small 16px font)
                k10.canvas->canvasText("Status: Connected (" + cardType + ")", 15, 55, 0x00FF00,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("SD card connected", 15, 85, 0x00FF00,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("but no files", 15, 105, 0xFEE715,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("Add files to card &", 15, 140, 0x94A3B8,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("press Button A to re-scan.", 15, 160, 0x00E5FF,
                                       k10.canvas->eCNAndENFont16, 26, false);
            } else {
                // Connected and files found (small 16px font)
                k10.canvas->canvasText("Status: Connected (" + cardType + ")", 15, 52, 0x00FF00,
                                       k10.canvas->eCNAndENFont16, 26, false);
                k10.canvas->canvasText("Files found (" + String(fileCount) + "):", 15, 72, 0xFEE715,
                                       k10.canvas->eCNAndENFont16, 26, false);
                if (fileCount > maxDisplayFiles) {
                    int remaining = fileCount - maxDisplayFiles;
                    k10.canvas->canvasText("...and " + String(remaining) + " more", 15, yPos, 0x94A3B8,
                                           k10.canvas->eCNAndENFont16, 26, false);
                }
            }
        }
    }

    // Bottom helper prompt in compact 16px font
    k10.canvas->canvasText("Press Button A: Re-scan", 15, 300, 0x64748B,
                           k10.canvas->eCNAndENFont16, 26, false);

    // Push canvas buffer to physical screen
    k10.canvas->updateCanvas();
}

void setup() {
    k10.begin();
    k10.initScreen(2);                 // Portrait orientation (240x320)
    k10.creatCanvas();                 // Create 2D canvas buffer
    k10.setScreenBackground(0x000000);   // Deep black background

    // Initial check on boot
    checkSDCard();
}

void loop() {
    // Press Button A anytime to re-scan the SD card
    if (k10.buttonA->isPressed()) {
        k10.canvas->canvasText("Scanning SD card...", 15, 300, 0xFFFF00,
                               k10.canvas->eCNAndENFont16, 26, false);
        k10.canvas->updateCanvas();
        delay(300);

        checkSDCard();

        // Wait for button release
        while (k10.buttonA->isPressed()) {
            delay(50);
        }
    }

    delay(50);
}
```
