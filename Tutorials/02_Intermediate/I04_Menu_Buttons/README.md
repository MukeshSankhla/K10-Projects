## What this tutorial is??

This tutorial demonstrates how to add interactive physical pushbutton controls to a graphical menu interface on the UNIHIKER K10.

Features:
- **Button [A] (Next / Scroll)**: Moves the cursor down the list cyclically through all options.
- **Button [B] (Select / Execute)**: Confirms the highlighted option and updates the active selection status box.
- **Debounced Interaction**: Software hold filters prevent double-clicks and debounce chatter.

---

## How it works

1. **State Tracking**:
   - `int activeIndex`: Tracks the 0-indexed item that currently holds user focus.
   - `String selectedMessage`: Stores the confirmed selection string.
2. **Cyclic Navigation via Modulo**:
   - Pressing Button A evaluates:
     $$\text{activeIndex} = (\text{activeIndex} + 1) \pmod{\text{totalItems}}$$
   - If the user scrolls past item 5, the cursor loops smoothly back to item 1.
3. **Execution on Select**:
   - Pressing Button B captures `menuItems[activeIndex]` and triggers the update.
4. **Dynamic Partial Component Refresh**:
   - Header and bottom navigation controls are drawn once in `setup()`.
   - On Button A, only the previously active card and newly active card are redrawn.
   - On Button B, only the status feedback container is erased and redrawn, preventing any full-screen flicker.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.buttonA->isPressed()` | None | `bool` | Detects physical press on Button A. |
| `k10.buttonB->isPressed()` | None | `bool` | Detects physical press on Button B. |
| `k10.canvas->canvasRectangle(...)` | `x, y, w, h, border, fill, isFilled` | `void` | Draws menu cards. |
| `k10.canvas->updateCanvas()` | None | `void` | Refreshes the display screen. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

const char* menuItems[] = {
    "1. Device Info",
    "2. Test Sensors",
    "3. Display Colors",
    "4. Buzzer Melody",
    "5. System Reboot"
};
const int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);
int activeIndex = 0;
String selectedMessage = "None";

const int START_Y = 54;
const int CARD_HEIGHT = 34;
const int CARD_SPACING = 6;

// Non-blocking button edge-detection state trackers
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

// Render individual menu item card component
void drawMenuCard(int i, bool isFocused) {
    int cardY = START_Y + i * (CARD_HEIGHT + CARD_SPACING);

    if (isFocused) {
        // Selected/Focused Card: Glacial Ice cyan highlight
        k10.canvas->canvasRectangle(12, cardY, 216, CARD_HEIGHT, 0x00E5FF, 0x0F2942, true);
        k10.canvas->canvasText(">", 20, cardY + 8, 0x00E5FF,
                               k10.canvas->eCNAndENFont16, 5, false);
        k10.canvas->canvasText(menuItems[i], 36, cardY + 8, 0xE0FBFC,
                               k10.canvas->eCNAndENFont16, 22, false);
    } else {
        // Unfocused Cards
        k10.canvas->canvasRectangle(12, cardY, 216, CARD_HEIGHT, 0x1E3A5F, 0x0B1626, true);
        k10.canvas->canvasText(menuItems[i], 28, cardY + 8, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 22, false);
    }
}

// Render dynamic status feedback component only
void updateStatusBox(const String& message) {
    k10.canvas->canvasRectangle(12, 226, 216, 34, 0x0284C7, 0x0E1A2E, true);
    String statusStr = "Active: " + message;
    k10.canvas->canvasText(statusStr, 22, 234, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 24, false);
}

// Render static chrome elements (header and navigation guide bar) once
void drawStaticChrome() {
    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x0E1A2E, 0x0E1A2E, true);
    k10.canvas->canvasLine(0, 44, 240, 44, 0x0284C7);
    k10.canvas->canvasText("MENU NAVIGATOR", 36, 10, 0x38BDF8,
                           k10.canvas->eCNAndENFont24, 16, false);

    // 2. Navigation Controls Bar (Zero-overflow 2-column layout)
    k10.canvas->canvasLine(12, 272, 228, 272, 0x1E3A5F);
    k10.canvas->canvasRectangle(12, 276, 216, 34, 0x1E3A5F, 0x0E1A2E, true);
    k10.canvas->canvasText("[A] Next", 22, 285, 0x00E5FF,
                           k10.canvas->eCNAndENFont16, 12, false);
    k10.canvas->canvasText("[B] Select", 136, 285, 0x38BDF8,
                           k10.canvas->eCNAndENFont16, 12, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Arctic Night background
    k10.setScreenBackground(0x08101E);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x00E5FF);

    // Initial full paint: static chrome + all cards + status box
    drawStaticChrome();
    for (int i = 0; i < totalItems; i++) {
        drawMenuCard(i, i == activeIndex);
    }
    updateStatusBox(selectedMessage);
    k10.canvas->updateCanvas();
}

void loop() {
    // Dynamic Partial Refresh on Button A: Update ONLY the toggled menu cards
    if (checkButtonAPressed()) {
        int prevIndex = activeIndex;
        activeIndex = (activeIndex + 1) % totalItems;

        drawMenuCard(prevIndex, false); // Invalidate previous active card
        drawMenuCard(activeIndex, true); // Highlight newly active card
        k10.canvas->updateCanvas();
    }

    // Dynamic Partial Refresh on Button B: Update ONLY the status feedback box
    if (checkButtonBPressed()) {
        selectedMessage = String(menuItems[activeIndex]).substring(3); // Strip number prefix
        updateStatusBox(selectedMessage);
        k10.canvas->updateCanvas();
    }

    delay(20); // Responsive loop tick
}
```
