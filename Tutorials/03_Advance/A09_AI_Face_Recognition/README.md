## What this tutorial is??

This tutorial demonstrates using the UNIHIKER K10 onboard **Neural Face Recognition & Biometric Enrollment Engine** to learn facial features, authenticate verified users, and flag unknown faces.

Features:
- **Biometric Face Enrollment**: Press Button A to register a new user face into internal flash memory (confirmed by green RGB illumination).
- **Identity Verification**: Press Button B to compare live camera video against enrolled templates:
  - **Enrolled Face (ID >= 0)**: Displays `Face Verified` with green status and illumination.
  - **Unknown Face (ID == -1)**: Displays `Unknown Face` with red alert badge (`COLOR_RED`) and illumination.
- **Minimalist Light Theme**: Pure white cards (`#FFFFFF`), delicate 1px borders (`#E2E8F0`), deep slate typography (`#0F172A`), and cobalt blue brand accent (`#1D4ED8`).
- **Live State Pill**: Dynamic status pill reflecting enrollment, verification, and unknown face results without full-screen flicker.

---

## How it works

1. **Enrollment & Recognition Commands**:
   - `ai.sendFaceCmd(ENROLL)`: Captures face landmarks and generates a feature embedding stored in local non-volatile memory.
   - `ai.sendFaceCmd(RECOGNIZE)`: Matches the current camera frame against enrolled embeddings.
   - `ai.isRecognized()` & `ai.getRecognitionID()`: Extracts the recognition result. If the face is not recognized, `ai.getRecognitionID()` returns `"-1"`.
2. **Recognition Logic**:
   - When `recognizedID == "-1"` or `< 0`: flags as `Unknown Face` with red badge and red LED.
   - When `recognizedID >= 0`: flags as `Face Verified` with green badge and green LED.
3. **Dynamic Partial Refresh**:
   - Camera background runs smoothly with `k10.setBgCamerImage(true)`.
   - Results card uses partial bounding box updates to eliminate display flicker.

---

## API and Functions details

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `ai.sendFaceCmd(ENROLL)` | Command enum | `void` | Triggers face enrollment sequence. |
| `ai.sendFaceCmd(RECOGNIZE)` | Command enum | `void` | Triggers face matching sequence. |
| `ai.isRecognized()` | None | `bool` | True when recognition evaluation is complete. |
| `ai.getRecognitionID()` | None | `String` | Returns enrolled user ID (or "-1" if unknown). |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes canvas buffer to display. |

---

## Factory Binary

The precompiled factory binary includes the complete bootloader, partition table, and application firmware:
- `A09_AI_Face_Recognition.bin` (Flash at `0x00000000`, 16MB DIO 80MHz)
