# **Guition JC4827W543C – Board Notes**

This folder contains the required configuration, drivers and files for using  
the **Guition JC4827W543C (NV3041A 480×272)** display with *yoRadio Fusion*.

---

## 📚 Required Arduino Libraries

The Guition board requires the following additional library when compiling in Arduino IDE:

- **Arduino_GFX v1.4.7 or newer**

Make sure it is installed via:

**Arduino IDE → Tools → Manage Libraries → “Arduino GFX” → Install**

---

## ⚙️ Arduino Upload Settings

The correct Arduino board configuration (cores, PSRAM mode, etc.)  
is shown in **`Arduino_setup.png`** in this folder.

Important settings:

- **Arduino Core → Core 1**  
- **Events / WiFi → Core 0**  
- **PSRAM → OPI PSRAM (Octal) – Enabled**  
- **Flash Erase on Upload → Erase All Flash (Full Erase)** *(required on first upload)*

---

## 💾 SD Card Support (Required File Replacement)

To enable SD card functionality, you **must replace** the following files in the  
main yoRadio firmware source:

- `sdmanager.cpp`
- `sdmanager.h`

Use the versions included in this folder.  
They contain the correct bus/mutex logic for the Guition NV3041A board.

---

## 📦 Included Files

- **Arduino_setup.png** – recommended Arduino IDE settings  
- **sdmanager.cpp**, **sdmanager.h** – corrected SD manager implementation  
- Additional driver files (NV3041A / touch)  
  - *(Note: these drivers are already included in yoRadio Fusion.  
    They are provided here only if you want to integrate this display  
    into another yoRadio firmware version.)*

---

## 👍 Notes

This board **requires PSRAM**, and is fully supported by yoRadio Fusion  
using the NV3041A display driver and the custom SD manager implementation.

