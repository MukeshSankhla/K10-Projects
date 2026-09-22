# 🎓 UNIHIKER K10 Complete Hardware & AI Tutorial (86 Projects)

Welcome to the ultimate **UNIHIKER K10 Embedded & Edge AI Curriculum**! This complete tutorial series contains **86 standalone, fully-documented Arduino projects** taking you progressively from basic hardware control to multi-sensor fusion, edge AI computer vision, offline voice assistants, and IoT wireless mesh networking.

---

## 🗺️ Progressive Learning Roadmap

```text
[Level 1: Beginners]  ──>  [Level 2: Intermediate]  ──>  [Level 3: Advance]  ──>  [Level 4: Expert]
(30 Single Hardware)       (22 Sensor Combinations)      (20 Games & Vision AI)   (14 Edge AI & Wireless)
```

---

## 🟢 Level 1: Beginners (30 Projects)
*Focuses on single-component fundamentals: WS2812 RGB LEDs, tactile buttons, screen primitives, buzzer tones, climate & light sensors, raw 6-axis IMU, and native GPIOs.*

| # | Project Name | What This Project Is Of |
|---|--------------|-------------------------|
| B01 | [`B01_LED_Blink`](./01_Beginner/B01_LED_Blink/) | The embedded 'Hello World'. Demonstrates how to initialize the UNIHIKER K10 onboard WS2812 addressable RGB LEDs and blink them synchronously at regular intervals. |
| B02 | [`B02_LED_ColorCycle`](./01_Beginner/B02_LED_ColorCycle/) | Introduces 24-bit hex color representation (`0xRRGGBB`) by sequencing the onboard RGB LEDs through primary and secondary colors. |
| B03 | [`B03_LED_Rainbow`](./01_Beginner/B03_LED_Rainbow/) | Uses mathematical HSV (Hue, Saturation, Value) color space conversion to generate smooth, organic rainbow color transitions across all 3 onboard LEDs. |
| B04 | [`B04_LED_Breathing`](./01_Beginner/B04_LED_Breathing/) | Demonstrates controlling perceived illumination using pulse-like brightness modulation to create a gentle 'breathing' or standby indicator. |
| B05 | [`B05_Button_StateRead`](./01_Beginner/B05_Button_StateRead/) | Reads digital physical inputs using the onboard tactile Buttons A and B, transmitting real-time state changes over the Serial Monitor. |
| B06 | [`B06_Buzzer_Beep`](./01_Beginner/B06_Buzzer_Beep/) | Introduces acoustic notification by driving the onboard magnetic audio buzzer with clean square-wave tone pulses. |
| B07 | [`B07_Buzzer_Melody`](./01_Beginner/B07_Buzzer_Melody/) | Plays pre-composed musical scores stored within the K10 firmware ROM without blocking the processor. |
| B08 | [`B08_Display_ColorFill`](./01_Beginner/B08_Display_ColorFill/) | Initializes the 2.8-inch ST7789 IPS LCD controller and demonstrates whole-frame background color filling. |
| B09 | [`B09_Display_HelloWorld`](./01_Beginner/B09_Display_HelloWorld/) | Demonstrates the K10 canvas text engine, coordinate positioning, font selection, and text color rendering. |
| B10 | [`B10_Display_GeometricShapes`](./01_Beginner/B10_Display_GeometricShapes/) | Introduces 2D vector drawing primitives: lines, bordered rectangles, solid circles, and hollow shapes. |
| B11 | [`B11_Sensor_LightALS`](./01_Beginner/B11_Sensor_LightALS/) | Samples the onboard Ambient Light Sensor (ALS) to measure real-time room illumination levels. |
| B12 | [`B12_Sensor_TempHumidity`](./01_Beginner/B12_Sensor_TempHumidity/) | Communicates over the internal I2C bus with the onboard AHT20 sensor to measure ambient air temperature and relative humidity. |
| B13 | [`B13_Sensor_AccelerometerRaw`](./01_Beginner/B13_Sensor_AccelerometerRaw/) | Samples raw gravitational force vectors along the X, Y, and Z axes using the onboard 6-axis IMU. |
| B14 | [`B14_GPIO_DigitalBlink`](./01_Beginner/B14_GPIO_DigitalBlink/) | Demonstrates controlling external electronics using the full-featured ESP32-S3 pin P0 on the edge connector. |
| B15 | [`B15_GPIO_AnalogRead`](./01_Beginner/B15_GPIO_AnalogRead/) | Uses the 12-bit Analog-to-Digital Converter (ADC) on pin P1 to read continuous analog voltages between 0V and 3.3V. |
| B16 | [`B16_Sensor_ButtonPolling`](./01_Beginner/B16_Sensor_ButtonPolling/) | Demonstrates synchronous input detection by periodically polling the state of onboard Buttons A and B in the main execution loop. |
| B17 | [`B17_Sensor_ButtonCallback`](./01_Beginner/B17_Sensor_ButtonCallback/) | Demonstrates event-driven button handling using asynchronous function callbacks, supporting Button A, Button B, and simultaneous AB press detection. |
| B18 | [`B18_Audio_PlayMusic`](./01_Beginner/B18_Audio_PlayMusic/) | Demonstrates sound synthesis using the onboard buzzer/speaker, playing preset melodies and raw musical note tone frequencies. |
| B19 | [`B19_Display_SetBackgroundColor`](./01_Beginner/B19_Display_SetBackgroundColor/) | This example demonstrates how to initialize the UNIHIKER K10 onboard 2.8-inch color LCD and cycle through different full-screen background colors in a continuous loop. |
| B20 | [`B20_Display_ShowingText`](./01_Beginner/B20_Display_ShowingText/) | Demonstrates the K10 canvas typography engine by rendering text strings in various font sizes, colors, and line spacing. |
| B21 | [`B21_Display_DrawPoint`](./01_Beginner/B21_Display_DrawPoint/) | Demonstrates plotting individual pixel points on the display canvas using randomized coordinates and dynamic color calculations. |
| B22 | [`B22_Display_DrawFunnel`](./01_Beginner/B22_Display_DrawFunnel/) | Draws an intricate geometric funnel / hourglass optical illusion on the screen by computing converging vector lines. |
| B23 | [`B23_Display_DrawCircle`](./01_Beginner/B23_Display_DrawCircle/) | Demonstrates the K10 vector graphics circle primitive by rendering a series of concentric circular outlines. |
| B24 | [`B24_Display_DrawRectangle`](./01_Beginner/B24_Display_DrawRectangle/) | Demonstrates rendering hollow and filled rectangles on the K10 screen canvas with animated nested coordinate offsets. |
| B25 | [`B25_Display_QRCode`](./01_Beginner/B25_Display_QRCode/) | Generates and renders high-density 2D QR barcodes directly on the UNIHIKER K10 screen from any URL or text payload. |
| B26 | [`B26_Sensor_AccelerateTilt`](./01_Beginner/B26_Sensor_AccelerateTilt/) | Uses the onboard 6-axis inertial measurement unit (IMU) to detect physical tilt orientation and device gestures. |
| B27 | [`B27_Sensor_Pedometer`](./01_Beginner/B27_Sensor_Pedometer/) | Implements a digital pedometer that detects footsteps using inertial acceleration filtering. |
| B28 | [`B28_Sensor_RGBControl`](./01_Beginner/B28_Sensor_RGBControl/) | Demonstrates controlling the onboard WS2812 addressable RGB LEDs, including individual indexing, all-LED addressing, and brightness ramping. |
| B29 | [`B29_GPIO_DigitalIO`](./01_Beginner/B29_GPIO_DigitalIO/) | Demonstrates standard digital input and output using the UNIHIKER K10 full-featured ESP32-S3 pins P0 and P1. |
| B30 | [`B30_GPIO_AnalogPWM`](./01_Beginner/B30_GPIO_AnalogPWM/) | Demonstrates pulse-width modulation (PWM) analog output on pin P0 and analog-to-digital conversion (ADC) voltage sampling on pin P1. |

## 🟡 Level 2: Intermediate (22 Projects)
*Combines two or three onboard peripherals into functional interactive modules: Button-controlled LEDs, pocket piano, smart nightlight, climate dashboard, sound meter, pedometer, dice roller, spirit level, and TF camera snapshots.*

| # | Project Name | What This Project Is Of |
|---|--------------|-------------------------|
| I01 | [`I01_Button_LED_Toggle`](./02_Intermediate/I01_Button_LED_Toggle/) | Combines physical tactile switches with addressable RGB LEDs to build an interactive multi-mode lighting controller. |
| I02 | [`I02_Button_Speaker_Piano`](./02_Intermediate/I02_Button_Speaker_Piano/) | Combines physical buttons, the audio buzzer, and screen graphics to create an interactive pocket musical instrument with visual note keys. |
| I03 | [`I03_Light_AutoNightLight`](./02_Intermediate/I03_Light_AutoNightLight/) | Combines the Ambient Light Sensor (ALS) and WS2812 RGB LEDs to automatically turn on warm ambient lighting when darkness is detected. |
| I04 | [`I04_TempHumi_ScreenMonitor`](./02_Intermediate/I04_TempHumi_ScreenMonitor/) | Combines the AHT20 sensor with the 2.8" LCD display to create an elegant real-time desktop weather and comfort station. |
| I05 | [`I05_Sound_NoiseMeter`](./02_Intermediate/I05_Sound_NoiseMeter/) | Samples the onboard microphone and maps noise amplitude to the 3 onboard RGB LEDs as a multi-stage VU meter (Green -> Yellow -> Red). |
| I06 | [`I06_Tilt_DirectionIndicator`](./02_Intermediate/I06_Tilt_DirectionIndicator/) | Combines the 6-axis IMU, screen graphics, and RGB LEDs to indicate device tilt direction in real time. |
| I07 | [`I07_Pedometer_StepTracker`](./02_Intermediate/I07_Pedometer_StepTracker/) | Combines onboard pedometer step counting, visual progress bars on the LCD, and audio buzzer celebration chimes when reaching fitness milestones. |
| I08 | [`I08_Shake_DiceRoller`](./02_Intermediate/I08_Shake_DiceRoller/) | Uses the accelerometer to detect physical shaking gestures, rolling an electronic 6-sided dice with sound effects and screen animations. |
| I09 | [`I09_Spirit_BubbleLevel`](./02_Intermediate/I09_Spirit_BubbleLevel/) | Recreates a carpenter's spirit bubble level, displaying a floating circular bubble that aligns with crosshairs when the board is level. |
| I10 | [`I10_Alarm_OverheatAlert`](./02_Intermediate/I10_Alarm_OverheatAlert/) | Combines the AHT20 temperature sensor, buzzer, and red RGB LED to implement an industrial temperature alarm system. |
| I11 | [`I11_Button_Stopwatch`](./02_Intermediate/I11_Button_Stopwatch/) | Uses millisecond hardware timers and tactile buttons to build a high-precision digital stopwatch on the LCD display. |
| I12 | [`I12_TF_DataLogger`](./02_Intermediate/I12_TF_DataLogger/) | Periodically samples the AHT20 climate sensor and writes timestamped rows directly to a CSV spreadsheet on a FAT32 MicroSD (TF) card. |
| I13 | [`I13_TF_AudioPlayback`](./02_Intermediate/I13_TF_AudioPlayback/) | Plays high-fidelity WAV sound files stored on a MicroSD (TF) card through the onboard audio amplifier and speaker. |
| I14 | [`I14_Camera_LiveViewfinder`](./02_Intermediate/I14_Camera_LiveViewfinder/) | Initializes the onboard camera sensor and streams live high-speed video frames directly to the 2.8" LCD display. |
| I15 | [`I15_Camera_SnapshotTF`](./02_Intermediate/I15_Camera_SnapshotTF/) | Combines camera viewfinder streaming, Button A trigger, and TF card storage into a point-and-shoot camera. |
| I16 | [`I16_PWM_LEDBreathKnob`](./02_Intermediate/I16_PWM_LEDBreathKnob/) | Reads an analog potentiometer connected to pin P1 and maps its voltage to control the RGB LED brightness. |
| I17 | [`I17_EdgeGPIO_RelayControl`](./02_Intermediate/I17_EdgeGPIO_RelayControl/) | Controls an external 5V relay module wired to edge connector expansion pin eP2 using Button A with status on the LCD screen. |
| I18 | [`I18_Display_ElectronicPhotoAlbum`](./02_Intermediate/I18_Display_ElectronicPhotoAlbum/) | This example turns the UNIHIKER K10 into a portable digital camera and photo viewer. It captures snapshots using the onboard camera, saves them to a MicroSD (TF) card, and displays them on the 2.8-inch screen. |
| I19 | [`I19_Display_MeteorShower`](./02_Intermediate/I19_Display_MeteorShower/) | Creates an animated meteor shower simulation with dynamic trajectories, lengths, colors, and speeds. |
| I20 | [`I20_Sensor_AccelerateBall`](./02_Intermediate/I20_Sensor_AccelerateBall/) | A real-time physics simulation of a rolling ball driven by physical gravitational tilt vectors. |
| I21 | [`I21_Audio_RecordAndPlay`](./02_Intermediate/I21_Audio_RecordAndPlay/) | Records voice audio from the onboard microphone directly to a MicroSD (TF) card as a standard WAV audio file, then plays it back. |
| I22 | [`I22_GPIO_ExtendedIO`](./02_Intermediate/I22_GPIO_ExtendedIO/) | Controls additional digital IO pins on the micro:bit-style Edge Connector (pins eP2 to eP19) through the onboard I2C GPIO expansion chip. |

## 🟠 Level 3: Advance (20 Projects)
*Introduces interactive arcade games, 2D physics simulations, real-time vision AI (face tracking, pet detection, motion spotlight, QR triggers), offline speech recognition (ASR), and Wi-Fi web servers.*

| # | Project Name | What This Project Is Of |
|---|--------------|-------------------------|
| A01 | [`A01_Game_TiltMazeBall`](./03_Advance/A01_Game_TiltMazeBall/) | A complete 2D physics game where tilting the UNIHIKER K10 steers an on-screen ball through maze walls to a target goal zone. |
| A02 | [`A02_Game_FlappyBirdClone`](./03_Advance/A02_Game_FlappyBirdClone/) | Recreates the classic arcade game on the K10. Tap Button A to flap wings against gravity, navigating through pipe gaps. |
| A03 | [`A03_Game_ReactionTimer`](./03_Advance/A03_Game_ReactionTimer/) | Tests human reflex speed. The screen waits a random interval, turns bright green, and measures how many milliseconds it takes to press Button A. |
| A04 | [`A04_AI_FaceBoxTracker`](./03_Advance/A04_AI_FaceBoxTracker/) | Combines camera video feed and the AI neural vision pipeline to track human faces and overlay dynamic bounding boxes and coordinates. |
| A05 | [`A05_AI_PetDetectorAlarm`](./03_Advance/A05_AI_PetDetectorAlarm/) | Uses onboard neural pet recognition to identify cats and dogs, playing a friendly chime and flashing green when a pet is detected. |
| A06 | [`A06_AI_MotionSecurityLight`](./03_Advance/A06_AI_MotionSecurityLight/) | Uses optical frame motion detection to trigger high-intensity white LED security spotlights whenever movement occurs in the camera frame. |
| A07 | [`A07_AI_QRCodeActionTrigger`](./03_Advance/A07_AI_QRCodeActionTrigger/) | Scans QR codes with the camera and executes physical actions based on keywords encoded in the QR code (e.g. 'RED', 'BLUE', 'MUSIC'). |
| A08 | [`A08_Voice_LightController`](./03_Advance/A08_Voice_LightController/) | Uses onboard offline Automatic Speech Recognition (ASR) to control the K10 RGB lights using voice commands without Wi-Fi. |
| A09 | [`A09_Voice_TalkingThermostat`](./03_Advance/A09_Voice_TalkingThermostat/) | Responds to spoken queries ('temperature' or 'humidity') by querying the AHT20 sensor and updating the on-screen dashboard. |
| A10 | [`A10_Audio_VoiceMemoRecorder`](./03_Advance/A10_Audio_VoiceMemoRecorder/) | Records voice memos to a MicroSD card with on-screen recording timer and visual playback feedback. |
| A11 | [`A11_UI_MultiScreenDashboard`](./03_Advance/A11_UI_MultiScreenDashboard/) | A complete multi-screen user interface where pressing Button A or B pages through multiple telemetry dashboards (Climate, IMU, System). |
| A12 | [`A12_Sensor_FallDetectorAlert`](./03_Advance/A12_Sensor_FallDetectorAlert/) | Monitors the 6-axis IMU for free-fall signatures (near 0G weightlessness) followed by high-G impact, triggering an emergency siren. |
| A13 | [`A13_WiFi_NTPDigitalClock`](./03_Advance/A13_WiFi_NTPDigitalClock/) | Connects to Wi-Fi, synchronizes with global atomic NTP time servers, and renders a desk clock on the 2.8" LCD display. |
| A14 | [`A14_WiFi_LocalWebServer`](./03_Advance/A14_WiFi_LocalWebServer/) | Hosts a lightweight HTTP web server on the K10, allowing any smartphone or PC on the same Wi-Fi network to control RGB LEDs and read sensors via web browser. |
| A15 | [`A15_BLE_WirelessPresenter`](./03_Advance/A15_BLE_WirelessPresenter/) | Uses ESP32-S3 Bluetooth Low Energy (BLE) to act as a wireless presentation remote control for PowerPoint or Keynote slides. |
| A16 | [`A16_AI_FaceDetection`](./03_Advance/A16_AI_FaceDetection/) | Uses the onboard camera and neural network acceleration coprocessor to detect human faces in real time. |
| A17 | [`A17_AI_DogCatRecognition`](./03_Advance/A17_AI_DogCatRecognition/) | Runs an onboard neural network model trained to identify cats and dogs from the camera video stream. |
| A18 | [`A18_AI_MovementDetection`](./03_Advance/A18_AI_MovementDetection/) | Uses temporal frame differencing on the camera video feed to detect moving objects and optical motion. |
| A19 | [`A19_AI_QRCodeScanner`](./03_Advance/A19_AI_QRCodeScanner/) | Transforms the UNIHIKER K10 camera into a real-time QR code and 2D barcode scanner. |
| A20 | [`A20_AI_SpeechRecognition`](./03_Advance/A20_AI_SpeechRecognition/) | Demonstrates completely offline local voice control using the onboard dual microphones and speech recognition coprocessor. |

## 🔴 Level 4: Expert (14 Projects)
*Engineering-grade solutions: biometric facial access control, automated time attendance, security intruder trap, 3D IMU perspective graphics, ESP-NOW wireless mesh telemetry, cloud MQTT IoT, audio spectrum visualizers, and master smart home consoles.*

| # | Project Name | What This Project Is Of |
|---|--------------|-------------------------|
| E01 | [`E01_AI_BiometricDoorLock`](./04_Expert/E01_AI_BiometricDoorLock/) | Combines facial biometric enrollment, neural verification, and external relay actuation on edge pin eP2 to build a secure smart door lock. |
| E02 | [`E02_AI_SmartTimeAttendance`](./04_Expert/E02_AI_SmartTimeAttendance/) | Identifies individuals via facial recognition and logs their clock-in timestamps into a CSV attendance ledger on the MicroSD card. |
| E03 | [`E03_Security_IntruderTrap`](./04_Expert/E03_Security_IntruderTrap/) | Armed security camera. When visual motion is detected, captures an immediate high-resolution snapshot to the TF card and sounds an alarm siren. |
| E04 | [`E04_Voice_MultiCommandHub`](./04_Expert/E04_Voice_MultiCommandHub/) | Implements an offline voice command parser handling 6+ distinct natural language instructions with visual and acoustic feedback. |
| E05 | [`E05_Sensor_IMU_Orientation3D`](./04_Expert/E05_Sensor_IMU_Orientation3D/) | Computes real-time 3D perspective matrix transformations on the 6-axis IMU angles to render a rotating 3D wireframe cube matching the board's physical attitude. |
| E06 | [`E06_Sensor_SleepTracker`](./04_Expert/E06_Sensor_SleepTracker/) | Tracks restlessness and ambient light levels overnight, recording actigraphy data to the TF card for sleep quality scoring. |
| E07 | [`E07_WiFi_WeatherStation`](./04_Expert/E07_WiFi_WeatherStation/) | Fetches real-time internet weather forecasts using HTTP REST API, parses the JSON payload, and renders a live weather screen. |
| E08 | [`E08_IoT_MQTT_Telemetry`](./04_Expert/E08_IoT_MQTT_Telemetry/) | Publishes real-time telemetry packets (temperature, humidity, light) to an MQTT message broker for Home Assistant or Node-RED. |
| E09 | [`E09_ESPNOW_RemoteController`](./04_Expert/E09_ESPNOW_RemoteController/) | Uses the connectionless ESP-NOW protocol (<5ms latency) to transmit button clicks and IMU tilt coordinates wirelessly to a receiver K10. |
| E10 | [`E10_ESPNOW_ReceiverHub`](./04_Expert/E10_ESPNOW_ReceiverHub/) | Receives wireless ESP-NOW packets from peer transmitter boards, rendering remote tilt positions on screen and actuating RGB LEDs. |
| E11 | [`E11_Audio_SpectrumVisualizer`](./04_Expert/E11_Audio_SpectrumVisualizer/) | Samples microphone audio at high speed and renders an animated dynamic multi-band equalizer spectrum on the 2.8" LCD display. |
| E12 | [`E12_System_DiagnosticsBench`](./04_Expert/E12_System_DiagnosticsBench/) | An automated self-test test bench verifying the screen, RGB LEDs, AHT20 sensor, light sensor, IMU, and buttons in a single pass. |
| E13 | [`E13_SmartHome_MasterConsole`](./04_Expert/E13_SmartHome_MasterConsole/) | The flagship project: combines offline voice commands, climate telemetry, tactile button controls, and status display into an integrated home console. |
| E14 | [`E14_AI_FaceRecognition`](./04_Expert/E14_AI_FaceRecognition/) | Demonstrates biometric face enrollment (learning) and identity recognition with visual feedback via onboard RGB LEDs. |

---

## ⚡ Instant Flashing with `mergedapp.bin` (No Compile Needed!)

Every single project directory includes a precompiled, production-ready **`mergedapp.bin`** file. This single binary combines the ESP32-S3 bootloader (`0x0`), partition table (`0x8000`), boot_app0 (`0xe000`), and user application code (`0x10000`).

You can flash any example to your UNIHIKER K10 in seconds without installing PlatformIO or Arduino IDE:

### Option A: Using `esptool` (Command Line)
```bash
# Navigate to the project directory
cd Tutorials/01_Beginner/B01_LED_Blink

# Flash directly to offset 0x0 (replace COMx with your port)
esptool.py --chip esp32s3 --port COMx --baud 921600 write_flash 0x0 mergedapp.bin
```

### Option B: Using Web Browser (Web Serial / No Software Needed)
1. Open any Web Serial ESP Flasher in Google Chrome / Edge (such as [ESP Web Flasher](https://espressif.github.io/esptool-js/) or Adafruit WebSerial ESPTool).
2. Connect your UNIHIKER K10 via USB-C and click **Connect**.
3. Add a file entry with Flash Address **`0x0`** (or `0x0000`).
4. Select the **`mergedapp.bin`** file from the desired tutorial folder.
5. Click **Program** / **Flash**.

---

## 🛠️ How to Run Any Tutorial in Arduino IDE

1. **Add UNIHIKER Board Manager URL**:
   In Arduino IDE, open **File > Preferences** and add the following URL to **Additional Boards Manager URLs**:
   ```text
   https://downloadcd.dfrobot.com.cn/UNIHIKER/package_unihiker_index.json
   ```
2. **Install Board Package**:
   Open **Tools > Board > Boards Manager**, search for `UNIHIKER`, and click **Install** on **UNIHIKER ESP32 Boards**.
3. **Select Board**:
   Choose **Tools > Board > UNIHIKER > UNIHIKER K10**.
4. **Open & Upload**:
   Open any `.ino` sketch in the tutorial folders, select the detected COM port under **Tools > Port**, and click **Upload** (arrow icon).

