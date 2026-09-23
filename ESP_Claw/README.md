> Turn your **DFRobot UNIHIKER K10** into an autonomous, edge-native AI agent powered by Espressif's **ESP-Claw** framework—featuring Chat Coding, multimodal camera vision, real-time web search, and multi-platform instant messaging bots.

## 🌟 Overview

For decades, smart IoT devices have been passive executors: waiting silently for a fixed command, toggling a relay, or streaming sensor readings to a remote dashboard. 

**ESP-Claw** completely changes this paradigm. Centered around **"Chat Coding"**, ESP-Claw brings the intelligence of AI agents directly to the edge chip. Rather than merely following hardcoded routines, devices powered by ESP-Claw form an autonomous local closed-loop of **sensing, reasoning, decision-making, and execution**.

The **DFRobot UNIHIKER K10** is the ultimate hardware host for ESP-Claw. Equipped with an **ESP32-S3** dual-core processor (16 MB Flash, 8 MB PSRAM), an integrated **2.8-inch color LCD**, a **2MP camera**, **dual MEMS microphones**, a **2W speaker**, and built-in environmental sensors (temperature, humidity, ambient light, accelerometer), the K10 gives ESP-Claw a powerful, highly integrated physical body.

This repository provides the complete deployment guide, configuration manual, and ready-to-flash 16MB firmware binary (`espclaw.bin`) to get ESP-Claw running on your UNIHIKER K10 in minutes.

---

## 🚀 Key Highlights

* 🤖 **Embedded-Native AI Agent**: Runs an autonomous agent runtime directly on the ESP32-S3 with local capability management, memory storage, and event routing.
* 💬 **"Chat Coding" & Lua Sandbox**: Teach the device new behaviors on the fly through conversation. ESP-Claw dynamically generates and persists deterministic Lua rules without recompiling firmware.
* ⚡ **Event-Driven Architecture**: Local hardware triggers and shortcuts execute with millisecond latency; complex cognitive requests are intelligently dispatched to cloud LLMs.
* 📷 **Multimodal Vision**: Take photos with the onboard 2MP camera and analyze scenes using vision-capable LLMs (`cap_llm_inspect`).
* 🌐 **Live Web Retrieval**: Seamlessly connects to search APIs (Tavily, Brave) for up-to-the-minute sports scores, live weather, documentation, and news summaries.
* 📲 **Multi-Platform Instant Messaging**: Chat with your K10 through your favorite IM platforms: **Telegram**, **WeChat (ClawBot)**, **QQ Bot (OpenClaw)**, **Feishu / Lark**, or the local **Web Chat**.
* 🧠 **Persistent Hierarchical Memory**: Maintains long-term structured memory, hardware pin mappings, user preferences, and customizable personality files (`Soul`, `Identity`, `User Info`).
* ⏰ **Local Task Scheduler**: Configure recurring cron triggers, morning briefings, or one-shot countdown timers that fire autonomously.
* 🖥️ **Integrated Web Console**: Built-in HTTP server at `http://esp-claw.local/` or `http://192.168.4.1` for device configuration, real-time chat, file management, and capability toggles.

## 📶 Network Setup & SoftAP Fallback

When ESP-Claw boots up:
1. It attempts to connect to the configured 2.4 GHz Wi-Fi network.
2. If Wi-Fi is not yet configured or fails to connect, the UNIHIKER K10 automatically opens an access point and displays the **Hotspot SSID** on its 2.8" color screen:
   * **Default SoftAP SSID**: `esp-claw-XXXXXX` (where `XXXXXX` corresponds to the last 6 characters of the MAC address).
   * **Default AP Password**: Open (no password required).

### First-Time Provisioning:
1. Connect your laptop or smartphone to the `esp-claw-XXXXXX` Wi-Fi hotspot.
2. Open your web browser and visit:
   ```text
   http://192.168.4.1
   ```
3. In the configuration portal:
   * Enter your home/office **2.4 GHz Wi-Fi credentials**.
   * Enter your **LLM API Key** and model name.
   * Enter your **Search Engine Key** (Tavily).
   * Enter your **IM Bot tokens** (Telegram, QQ, Feishu, etc.).
4. Click **Save** and restart the device.
5. Once connected to your local network, the K10's screen displays its allocated local IP address. You can now access the full Web Console from any browser on the same LAN at:
   ```text
   http://esp-claw.local/
   ```
   *(or `http://<device-ip>/`)*

---

## 🧠 Configuring AI Providers & Services

ESP-Claw supports industry-leading LLMs as well as custom private endpoints:

### 1. Alibaba Cloud Qwen (Recommended Default)
* **Provider**: Select `Qwen`
* **API Key**: Your DashScope API Key (`sk-...`)
* **Recommended Models**: `qwen-plus`, `qwen-turbo`, `qwen-max`, `qwen-vl-max` (for vision)
* **Sign up**: [Alibaba Cloud Model Studio](https://dashscope.console.aliyun.com/)

### 2. OpenAI
* **Provider**: Select `OpenAI`
* **API Key**: OpenAI Platform API Key (`sk-proj-...`)
* **Base URL**: `https://api.openai.com/v1`
* **Recommended Models**: `gpt-4o`, `gpt-4o-mini`

### 3. Anthropic Claude
* **Provider**: Select `Anthropic`
* **API Key**: Anthropic API Key (`sk-ant-...`)
* **Base URL**: `https://api.anthropic.com/v1`
* **Recommended Models**: `claude-3-5-sonnet-20241022`, `claude-3-5-haiku-20241022`

### 4. DeepSeek
* **Provider**: Custom (OpenAI Compatible)
* **Base URL**: `https://api.deepseek.com`
* **API Key**: DeepSeek API Key
* **Recommended Models**: `deepseek-chat`

> [!TIP]
> **Base URL Rule**: For custom OpenAI-compatible endpoints, do **not** include `/chat/completions` or trailing slashes in the Base URL. For example, use `https://api.openai.com/v1` instead of `https://api.openai.com/v1/chat/completions`.

---

## 🔍 Configuring Live Web Search

ESP-Claw connects to real-time search engines so your K10 can answer queries about current events, scores, and weather:

1. **Tavily Search Engine**:
   * Visit the [Tavily Dashboard](https://app.tavily.com/home), register, and copy your free API Key.
   * Paste the key into the **Tavily API Key** field in the Web Console or web flasher.
2. **HTTP Request Allowlist**:
   * Under Network Settings, ensure the HTTP domain allowlist contains `*` (or specific target domains) so the agent can fetch and parse web content.

---

## 🤖 Configuring Instant Messaging (IM) Bots

Connect your K10 to your favorite messaging app so you can talk to your agent from anywhere in the world:

| Platform | Setup Requirement | Instructions |
| :--- | :--- | :--- |
| **Telegram** | `Bot Token` | 1. Open Telegram and chat with [@BotFather](https://t.me/botfather).<br>2. Send `/newbot`, name your bot, and copy the HTTP API token.<br>3. Paste the token into the Telegram field in the Web Console. |
| **WeChat (ClawBot)** | `QR Code Scan` | 1. Open the Web Console → IM Settings → WeChat.<br>2. Click **Generate QR**.<br>3. Scan the code with your WeChat app to bind your account. |
| **QQ Bot (OpenClaw)** | `App ID & App Secret` | Register a developer bot on [QQ Open Platform](https://q.qq.com/qqbot/openclaw/) and input your credentials. |
| **Feishu / Lark** | `App ID & App Secret` | Create an enterprise bot application on the [Feishu Open Platform](https://open.feishu.cn/) and enter the App credentials. |
| **LAN Web Chat** | Built-in | Open `http://esp-claw.local/` in your browser and chat directly without third-party services! |

---

## 💬 Hands-On Interaction Examples

Once your UNIHIKER K10 is running ESP-Claw, talk to it through your IM bot or the Web Console:

### 1. General Conversation & Knowledge
```text
User: Hello Claw! Introduce yourself and list your active capabilities.
Claw: Hello! I am ESP-Claw running on the UNIHIKER K10 ESP32-S3. I have active modules for camera vision, hardware sensors, web search, scheduler, and local Lua scripting!
```

### 2. Camera Snapshot & Vision Inspection
```text
User: Take a photo and tell me what you see.
Claw: [Captures 2MP frame from onboard camera, sends to vision LLM]
      "I see a maker workspace with a soldering iron, a breadboard, and a notebook on the table."
```

### 3. Real-Time Web Search & Weather
```text
User: Who won the latest Formula 1 Grand Prix?
Claw: [Queries Tavily Search API]
      "The latest Formula 1 Grand Prix was won by Charles Leclerc in Monza!"
```

### 4. Hardware Telemetry & Actuation
```text
User: What is the current temperature and humidity in this room?
Claw: [Reads onboard SHTC3 sensor]
      "The temperature is 24.6°C and the relative humidity is 52%."

User: Change the RGB lights to cyan and set brightness to medium.
Claw: [Executes local WS2812 driver]
      "Done! Onboard RGB LEDs set to cyan."
```

### 5. Automated Timers & Alarms
```text
User: Set a timer for 15 minutes for tea.
Claw: [Configures local scheduler timer]
      "Timer set for 15 minutes. I will alert you at 14:45."
```

### 6. Chat Coding & Custom Automation Rules
```text
User: Create a shortcut: whenever the room temperature exceeds 30°C, turn all three RGB LEDs red and send me an alert.
Claw: [Synthesizes dynamic Lua rule, validates against safety policy, saves to Flash]
      "Automation rule registered! SHTC3 temperature monitor will trigger red LEDs and notification if temp > 30°C."
```

---

## 🖥️ Exploring the Web Console

When you navigate to `http://esp-claw.local/` (or the board's IP), the Web Console offers:

* **System Status**: View CPU frequency, free heap memory, PSRAM utilization, Wi-Fi RSSI signal strength, and firmware version.
* **Web Chat**: Live bidirectional conversational terminal with session switching (`/session new`, `/session list`).
* **Memory Management**:
  * **Soul**: Define your agent's core character, temperament, and boundaries.
  * **Identity**: Configure device name, role, and physical environment context.
  * **User Info**: Store facts about you (name, interests, habits) so the agent remembers across sessions.
  * **Hardware Memory**: Permanent record of connected sensors, pin mappings, and peripheral configurations.
* **Capabilities Management**: Selectively enable or disable individual modular capabilities (`cap_camera`, `cap_sensors`, `cap_web_search`, `cap_scheduler`, `cap_mcp`).
* **File Management (Dev / Admin Mode)**: Direct browser-based filesystem explorer to view, upload, or modify Lua scripts, logs, and config files stored in SPIFFS/LittleFS.

---

## ❓ Troubleshooting & FAQ

#### Q1: The K10 is plugged in, but the COM port doesn't appear.
* Ensure you are using a **USB Type-C Data Cable** (some charging cables do not have data lines).
* Put the ESP32-S3 into ROM bootloader mode: Press and hold the **`BOOT`** button on the back of the K10, plug the cable into your computer, then release `BOOT`. Check Windows Device Manager for `USB JTAG/serial debug unit` or `CH340/CP2102`.

#### Q2: The board cannot connect to Wi-Fi.
* ESP32-S3 only supports **2.4 GHz Wi-Fi** networks. Dual-band routers with mixed 2.4/5 GHz under a single SSID may occasionally cause handshake failures; ensure 2.4 GHz is enabled.
* If connection fails, the K10 automatically presents the `esp-claw-XXXXXX` AP. Connect to it, browse to `192.168.4.1`, re-enter the SSID/password, and verify credentials.

#### Q3: Telegram / IM messages receive no response.
* Check that your LLM API Key is valid and has active quota.
* In the Web Console under **Capabilities Management**, confirm that `Local IM` or `cap_im_platform` is enabled.
* Verify device time: ESP-Claw uses HTTPS with TLS certificates; ensure the device has synchronized its time via NTP or through the timezone setting.

#### Q4: How do I restore factory defaults?
* To wipe all stored NVS configurations (Wi-Fi, API keys, tokens), run:
  ```bash
  python -m esptool --chip esp32s3 --port COMx erase_flash
  ```
  Then re-flash `espclaw.bin` at `0x0`.

---

## 📜 Credits & Acknowledgements

* **Espressif Systems**: For creating the groundbreaking [ESP-Claw](https://github.com/espressif/esp-claw) edge agent framework and documentation.
* **DFRobot UNIHIKER Team**: For designing the feature-packed [UNIHIKER K10](https://www.dfrobot.com/product-2904.html) AIoT hardware platform and making embedded AI accessible.
* **Tavily AI**: For high-performance search API support tailored for AI agents.

---