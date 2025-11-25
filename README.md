# yoRadio Fusion – ESP32 Web Radio
**Customized ESP32-S3 internet radio with advanced display drivers and Web UI**

It includes custom display drivers for NV3041A, GC9A01, GC9A01_I80 devices.
Supports namedays, multiple VU-meter layouts, theme upload, and high-bitrate audio playback.

This project is based on **ёRadio v0.9.720** by e2002:  
https://github.com/e2002/yoradio  
📌 *Reading the original documentation is strongly recommended.*

---

## 🚀 Features
- High-bitrate audio playback (IDF-modified audio pipeline included)  
- Modern Web UI with configuration for VU, clock fonts, themes, timers  
- Multiple optimized Arduino-GFX based display drivers (mutex-safe rendering) 
- Theme upload + color customization 
- Multilingual nameday system  

---

## 🖥 Supported Hardware (tested)

**CYD ESP32-2432S028** with additional PSRAM
(you can find instructions for adding PSRAM in the boards_samples folder)

**Guition JC4827W543C**

ESP32-S3 with 4–16 MB flash and PSRAM

External DACs (PCM5102A, **CS4344** depending on setup)

TF/SD modules, IR receivers, rotary encoders

**CST816** / GT911 / XPT2046 touch controllers

## 📺 Supported Displays

- NV3041A 480×272
- GC9A01 Round 240x240
- ILI9488, ILI9486 480x320
- ILI9431 320x240
- ST7796 480x320
- ST7789 320x240
- ST7789 284x76
- ST7789 240x240
- ST7789 320x170


## ⚠️ Important: SD Card Compatibility (Required File Replacement)

For the following hardware models, **SD card support will NOT work** unless you replace  
`sdmanager.cpp` and `sdmanager.h` with the versions provided in this repository:

- **ESP32-2432S028**
- **Guition JC4827W543C**
- **GC9A01_ROUND**  (not working yet)
- **GC9A01_I80_ROUND**  (not working yet)

The correct SD manager files are located inside each display’s specific subfolder under:
boards_samples/<display_model>/


### Why is this required?
These boards use different SPI/I80/QSPI line mappings or bus arbitration methods, and therefore require a board-specific SD manager implementation with proper mutex locking and bus handling.

### Notes
- Only **sdmanager.cpp** and **sdmanager.h** must be replaced.  
- The same subfolder also contains additional display drivers (QSPI, I80, touch, etc.), but **these are already included** in yoRadio Fusion and **do not need** to be replaced.  
- Those files are provided only for convenience, in case you want to use the same displays with other yoRadio firmware versions.


## ⚠️ Compatibility & Recommended Kernel Versions

This firmware cannot be used on devices without PSRAM.

Recommended ESP32 Arduino Core Versions

To ensure full driver compatibility and avoid audio stack instability:

ESP32-S3: Arduino Core 3.3.3 (recommended)

ESP32 (classic): Arduino Core 3.3.0

IDF MOD Compatibility

For ESP32 devices using Arduino Core 3.3.0, the included
IDF 5.4 MOD (Audio_IDF_MOD) remains compatible because this core version still contains the older ESP32-specific audio libraries that the fix patches were designed for.

(ESP32-S3 builds do not use this ESP32-only fix.)

## 🎚 VU Meter Calibration

yoRadio Fusion includes a fully tunable VU engine with dynamic range shaping, soft-knee compression, smoothing and gain controls.  
These parameters allow the VU meter to behave naturally across different hardware, audio sources and display sizes.

### 🔧 Parameter Overview

**Floor (%)**  
Noise threshold. Below this level the VU does not move.  
Lower it to make very quiet passages visible.

**Ceil (%)**  
Upper limit (headroom). Above this value the signal is compressed.  
`100%` = no ceiling, full dynamic range.

**Expo (γ)** – Curve steepness  
- `1.0` → linear  
- `> 1.0` → compresses low end, emphasizes peaks  
- `< 1.0` → expands low end, makes quiet parts more “alive”

**Gain**  
Overall multiplier.  
Increase if the VU appears too low, decrease if it constantly maxes out.

**Knee (%)**  
Soft entry threshold.  
Higher values → smoother and less “snappy” transitions.

### 🔁 Processing Order in the Engine
`normalize(floor..ceil) → smoothstep(knee) → powf(expo) → * gain`

---

## ⚡ Quick Setup (4 Steps)

1. **Floor**  
   Lower it until you see movement even in quiet sections.  
   - Small displays: 20–40%  
   - Large VUs: 10–25%

2. **Ceil**  
   Start with **100%**.  
   If peaks slam into the top, reduce to **90–95%**.

3. **Expo**  
   - VU moves only at the top → decrease (`1.6 → 1.2`)  
   - Too much low-end activity → increase (`1.0 → 1.3–1.6`)

4. **Gain**  
   Adjust last to fit the full range.  
   Increase if it's too low, decrease if it hits the ceiling.

---

## 🎛 Fine Tuning

**Knee**  
- **0–5%** → fast, snappy, more aggressive  
- **10–20%** → smoother, less jittery motion

**Smoothing Controls**  
- `alphaUp` → smaller = slower rise, larger = faster  
- `alphaDown` → larger = slower decay  
- `peakUp / peakDown` → peak indicator sensitivity

---

## 🩺 Symptoms → Fixes

| Symptom               | What to Adjust |
|-----------------------|----------------|
| **“Barely moves”**    | Floor ↓, Expo ↓, Gain ↑ |
| **“Only lights at top”** | Expo ↓, Ceil = 100%, Gain ↓ |
| **“Jumpy / nervous”** | Knee ↑ (10–15%), alphaUp ↓, alphaDown ↑ |
| **“Always maxed out”** | Gain ↓ or Ceil ↓ to 95% |


## 🌍 Languages & Regional Settings  
*(contribution by **Tamás Várai**)*

yoRadio Fusion includes built-in languages and regional formats:  
**HU, PL, GR, EN, RU, NL, SK, UA**

Language can be selected in `myoptions.h`:

🔤 Font System (Adafruit_GFX 5×7 font)

The program uses the built-in Adafruit_GFX font system, which scales a 5×7 bitmap font (glcdfont.c).
If your characters display incorrectly (e.g., WiFi / speaker icons, accented letters), you must replace glcdfont.c with the correct version for your language.

Locations

Arduino IDE:
Documents/Arduino/libraries/Adafruit_GFX_Library/glcdfont.c

PlatformIO:
yoRadio/.pio/libdeps/esp32-s3-devkitc1-n16r8/Adafruit GFX Library/glcdfont.c

Custom language-specific font files (included in this repo)
yoRadio/locale/glcdfont/EN, NL, CZ/glcdfont.c
yoRadio/locale/glcdfont/GR/glcdfont.c
yoRadio/locale/glcdfont/HU/glcdfont.c
yoRadio/locale/glcdfont/PL, SK/glcdfont.c
yoRadio/locale/glcdfont/RU/glcdfont.c
yoRadio/locale/glcdfont/UA/glcdfont.c


Replace the original file with the version matching your selected language.

## 🎉 Nameday Display

*(contribution by **Tamás Várai**)*

The firmware can display namedays for the following languages:
HU, PL, GR

Enable and select language in myoptions.h:

#define NAMEDAYS_FILE HU


Nameday data files are located in:

yoRadio/locale/namedays/namedays_HU.h
yoRadio/locale/namedays/namedays_PL.h
yoRadio/locale/namedays/namedays_GR.h

## 📂 Repository Structure
yoRadio-Fusion/

 ├── Audio_IDF_MOD/        # High-bitrate audio engine patches + docs
 
 ├── boards_samples/       # Tested boards + custom display drivers
 
 ├── yoEditor_v1.0.1/      # Playlist editor (external project)
 
 └── yoRadio/              # Main firmware
 

## 📸 Screenshots

webUI new features

<img width="486" height="451" alt="2025-11-25_16-13-19" src="https://github.com/user-attachments/assets/fc5e6e74-02da-46f4-b34a-4122167a2668" />

Customize.html

<img width="426" height="779" alt="2025-11-25_16-18-09" src="https://github.com/user-attachments/assets/7eed5a0e-3f91-4be2-8918-5184053b96d2" />

<img width="440" height="548" alt="2025-11-25_16-15-56" src="https://github.com/user-attachments/assets/dd5d9218-8b3a-4fb7-ba84-ac3b9823d167" />

<img width="435" height="702" alt="2025-11-25_16-15-28" src="https://github.com/user-attachments/assets/df58c49b-6a0e-4150-9c67-2aef0448bee8" />

<img width="436" height="708" alt="2025-11-25_16-14-56" src="https://github.com/user-attachments/assets/9e3127b1-fd16-4883-acc4-09feb8fd47b2" />

<img width="435" height="610" alt="2025-11-25_16-14-21" src="https://github.com/user-attachments/assets/79794cbc-34fc-4032-be5b-60ec80c47eb7" />

Settings.html

for air pressure data 

<img width="573" height="732" alt="2025-11-25_16-18-52" src="https://github.com/user-attachments/assets/807e4c4e-f05b-4f0f-ae92-7172bbe147f9" />

Timed switch-on/switch-off

<img width="568" height="756" alt="2025-11-25_16-19-30" src="https://github.com/user-attachments/assets/7f37d638-d57c-43bc-8295-34593138cc22" />


## 📦 Installation (Arduino IDE)

1. **Clone or download** this repository.
2. Open **Arduino IDE**.
3. Install the required ESP32 board package  
   (see “Compatibility & Recommended Kernel Versions” in this README).
4. Open the project entry file:  
   `yoRadio/yoRadio.ino`
5. Go to **Tools → Board Settings** and configure the following:

   **Required settings:**
   - **CPU Core:**  
     - *Arduino Core* → **Core 1**  
     - *Events / WiFi tasks* → **Core 0**
   - **PSRAM:** **OPI PSRAM (Octal)** → **Enabled**
   - **Flash Erase on Upload:**  
     **“Erase All Flash (Full Erase)”** — *required for the first upload*
   - **Partition Scheme:**  
     Use the recommended scheme for your device (minimal: **Huge APP...**)

6. Configure your device options in `myoptions.h`  
   (display type, language, namedays, theme options, VU settings, etc.).
7. Compile and upload the firmware to your ESP32-S3 device.
8. After boot, connect the device to Wi-Fi (or use AP mode) and open the  
   **Web UI** in your browser using the device’s IP address.

## 🙌 Credits

Based on ёRadio by e2002.
Extended, refactored, and localized by the yoRadio Fusion contributors.
