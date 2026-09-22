## What this tutorial is??

This tutorial demonstrates how to design and render a modern graphical **Menu Interface Layout** on the UNIHIKER K10 2.8" color screen.

Features:
- **Card-Based UI**: Individual menu options are encapsulated in rounded rectangles with dynamic borders.
- **Active Focus Cursor**: Distinguishes the currently focused item using vibrant cyan outlines and indicator arrow symbols (`>`).
- **Scalable Layout**: Calculates vertical spacing dynamically based on array length and line counts.

---

## How it works

1. **Menu Item Array**:
   - Menu choices are defined in a simple string array:
     ```cpp
     const char* menuItems[] = { ... };
     ```
2. **Calculated Card Offsets**:
   - For each item index $i$, the vertical coordinate is calculated as:
     $$\text{cardY} = \text{startY} + i \cdot (\text{cardHeight} + \text{spacing})$$
3. **Modular Component Architecture**:
   - Instead of clearing the whole canvas, UI chrome (header, divider, footer) is isolated from dynamic menu item cards (`drawMenuCard()`).
   - Cards use dark backgrounds (`0x12121E`) with slate text (`0x94A3B8`) for inactive states, and vibrant violet frames (`0x818CF8`) with warm gold indicators (`0xF59E0B`) for active states.

---

## API and Functions detailes

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `k10.canvas->canvasRectangle(...)` | `x, y, w, h, border, fill, isFilled` | `void` | Draws menu item container cards. |
| `k10.canvas->canvasText(...)` | Text, coordinates, color, font | `void` | Renders typography. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Full Code

```cpp
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
uint8_t screen_dir = 2; // Portrait orientation (240x320)

// Menu structure definition
const char* menuItems[] = {
    "1. System Status",
    "2. Sensor Monitor",
    "3. RGB Light Show",
    "4. Audio Settings",
    "5. SD File Browser"
};
const int totalItems = sizeof(menuItems) / sizeof(menuItems[0]);
int highlightedIndex = 1; // Example active item (Sensor Monitor)

const int START_Y = 56;
const int CARD_HEIGHT = 36;
const int CARD_SPACING = 8;

// Render individual menu item card component
void drawMenuCard(int i, bool isActive) {
    int cardY = START_Y + i * (CARD_HEIGHT + CARD_SPACING);

    if (isActive) {
        // Highlighted Active Card: Royal violet border with warm gold indicator
        k10.canvas->canvasRectangle(12, cardY, 216, CARD_HEIGHT, 0x818CF8, 0x1E1B4B, true);
        k10.canvas->canvasText(">", 20, cardY + 9, 0xF59E0B,
                               k10.canvas->eCNAndENFont16, 5, false);
        k10.canvas->canvasText(menuItems[i], 36, cardY + 9, 0xE0E7FF,
                               k10.canvas->eCNAndENFont16, 22, false);
    } else {
        // Inactive Cards: Muted borders and slate text
        k10.canvas->canvasRectangle(12, cardY, 216, CARD_HEIGHT, 0x27273A, 0x12121E, true);
        k10.canvas->canvasText(menuItems[i], 28, cardY + 9, 0x94A3B8,
                               k10.canvas->eCNAndENFont16, 22, false);
    }
}

// Render static chrome (Header banner and footer guide) once
void drawStaticChrome() {
    // 1. Header Banner
    k10.canvas->canvasRectangle(0, 0, 240, 44, 0x141424, 0x141424, true);
    k10.canvas->canvasLine(0, 44, 240, 44, 0x312E81); // Royal indigo divider
    k10.canvas->canvasText("MAIN MENU", 62, 10, 0xF59E0B,
                           k10.canvas->eCNAndENFont24, 15, false);

    // 2. Footer Navigation Guide
    k10.canvas->canvasLine(15, 282, 225, 282, 0x312E81);
    k10.canvas->canvasText("Static Menu Preview", 46, 294, 0x818CF8,
                           k10.canvas->eCNAndENFont16, 22, false);
}

void setup() {
    k10.begin();
    k10.initScreen(screen_dir);
    k10.creatCanvas();
    // Executive Obsidian theme background
    k10.setScreenBackground(0x0A0A14);

    k10.rgb->brightness(5);
    k10.rgb->write(-1, 0x818CF8); // Indigo glow

    // Render chrome and components without full screen clears
    drawStaticChrome();
    for (int i = 0; i < totalItems; i++) {
        drawMenuCard(i, i == highlightedIndex);
    }
    k10.canvas->updateCanvas();
}

void loop() {
    // Static layout demonstration
    delay(100);
}
```
