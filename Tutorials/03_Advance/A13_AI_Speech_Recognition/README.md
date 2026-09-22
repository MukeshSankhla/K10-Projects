## What this tutorial is??

This tutorial demonstrates using the UNIHIKER K10 onboard **Dual-Microphone Offline Neural Speech Recognition Engine** (`asr.h`) to detect English wake words (`"Hi Telly"` / `"Jarvis"`) and control onboard hardware with spoken commands (`"Switch on"` / `"Switch off"`).

Features:
- **Offline Neural Wake-Word Detection**: Listens continuously for the English wake words `"Hi Telly"` or `"Jarvis"` without any cloud or internet connection.
- **Voice Command Automation**: Recognizes spoken command phrases (`"Switch on"` and `"Switch off"`) to toggle the onboard RGB illumination.
- **Continuous Lively Audio Spectrum**: An active, animated 9-band frequency visualizer that pulses during standby to show the I2S microphone is actively listening, and jumps dynamically during active speech recognition.
- **Dual Partition Merged Flash Binary**: The pre-built `A13_AI_Speech_Recognition.bin` (12.58 MB) includes:
  - English Neural Acoustic Model (`srmodels4.bin`) merged at flash offset `0x510000`
  - English Speech Data (`esp_tts_voice_data_xiaoxin.dat`) merged at flash offset `0x985000`
- **Minimalist Light Theme**: Soft slate off-white canvas (`#F8FAFC`), pure white cards (`#FFFFFF`), delicate 1px borders (`#E2E8F0`), and clean violet brand accent (`#7C3AED`).

---

## Why ASR Requires Both Partitions

1. **Acoustic Model Partition at `0x510000`**:
   - Stores the pre-trained neural network acoustic model (`srmodels4.bin`) for WakeNet & MultiNet English.
2. **Speech Data Partition at `0x985000`**:
   - The underlying DFRobot ESP-SR / TTS framework expects the voice data table (`esp_tts_voice_data_xiaoxin.dat`) at `0x985000`.
   - Without both partitions, `asr.asrInit()` fails to initialize or stalls.
3. **Hardware Initialization Order**:
   - `asr.asrInit(CONTINUOUS, EN_MODE, 6000)` must be called immediately after `k10.begin()` so DMA buffers for the I2S microphone are cleanly allocated before the LCD canvas buffer is created.
4. **Trained Vocabulary**:
   - Wake words: `"Hi Telly"` or `"Jarvis"`
   - Command 1: `"Switch on"` (turns onboard LED on white)
   - Command 2: `"Switch off"` (turns onboard LED off)

---

## API and Functions details

| API / Method | Arguments | Return Type | Description |
| :--- | :--- | :--- | :--- |
| `asr.asrInit(mode, lang, time)` | Mode, Language, Timeout | `void` | Initializes offline ASR engine (`EN_MODE` = English). |
| `asr.addASRCommand(id, cmd)` | Command ID, string | `void` | Registers voice phrase to vocabulary. |
| `asr.isWakeUp()` | None | `bool` | True when wake word has been heard and engine is listening. |
| `asr.isDetectCmdID(id)` | Command ID | `bool` | True when registered command phrase is detected. |
| `k10.rgb->brightness(val)` | 0 - 9 | `void` | Sets onboard RGB LED brightness. |
| `k10.canvas->updateCanvas()` | None | `void` | Flushes graphics buffer to display. |

---

## Factory Binary

The precompiled factory binary includes the complete bootloader, partition table, application, ASR model (`0x510000`), and speech data (`0x985000`):
- `A13_AI_Speech_Recognition.bin` (Flash at `0x00000000`, 16MB DIO 80MHz)
