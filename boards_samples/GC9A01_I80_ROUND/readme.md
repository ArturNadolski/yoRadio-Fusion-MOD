# **YourCee GC9A01 Round display with 8080 interface – Board Notes**

This folder contains the required configuration, drivers and files for using  
the **GC9A01_I80 (240×240)** display with *yoRadio Fusion*.

---
## 🛒 Hardware Availability (Where to Buy)

▶ 1.28 inch TFT Screen Display Module 1.28" IPS Round Colorful LCD Board GC9A01 Touch 240x240 SPI 240*240 ESP32S3 ESP32S3-N16R8:

https://www.aliexpress.com/item/1005004911604497.html?spm=a2g0o.order_list.order_list_main.114.22231802FnZGjh

You must select the **ESP32S3-N16R8 Capacitive touch I80 interface** option

▶ 1set FFC/FPC Connector + 0.5mm Pitch 50/100/200mm Length FFC Cable **12P** PIN for extended GPIO:

https://www.aliexpress.com/item/4000648860009.html?spm=a2g0o.order_list.order_list_main.104.22231802FnZGjh

## 📚 Required Arduino Libraries

The Guition board requires the following additional library when compiling in Arduino IDE:

- **Arduino_GFX v1.4.7 or newer**
- **Adafruit CST8XX Library** for the capacitive touch screens

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

## 💾 SD Card Support (Under development)

Currently unavailable, SD card not yet working

---

## 📦 Included Files

- **Arduino_setup.png** – recommended Arduino IDE settings  
- Additional driver files (GC9A01_ROUND / touch)  
  - *(Note: these drivers are already included in yoRadio Fusion.  
    They are provided here only if you want to integrate this display  
    into another yoRadio firmware version.)*
