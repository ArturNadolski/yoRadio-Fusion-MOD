#ifndef displayGC9A01_I80_h
#define displayGC9A01_I80_h
#pragma once

#include "../core/options.h"
#if DSP_MODEL == DSP_GC9A01_I80

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include "fonts/dsfont52.h"

typedef GFXcanvas16 Canvas;
class Arduino_GFX;
class Arduino_DataBus;

class yoDisplay : public Adafruit_GFX {
public:
  yoDisplay();
  bool begin();

  // kijelző API
  void setRotation(uint8_t r) override;
  void invertDisplay(bool i);
  void displayOn();
  void displayOff();

  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void writePixel(int16_t x, int16_t y, uint16_t color) override { drawPixel(x,y,color); }
  void startWrite() override;
  void endWrite() override;
  void setAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h);
  void writePixels(uint16_t *data, uint32_t len);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

  void fillScreen(uint16_t color) override;
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h);

  // háttérfény (PWM)
  void setBrightness(uint8_t duty);          // 0..255
  void setBrightnessPercent(uint8_t pct);    // 0..100

  Arduino_GFX* raw();

private:
  Arduino_GFX* _gfx = nullptr;
  int16_t _win_x = 0, _win_y = 0;
  uint16_t _win_w = 0, _win_h = 0;
  bool _bl_pwm_attached = false;
};

#include "widgets/widgets.h"
#include "widgets/pages.h"
#include "fonts/bootlogo99x64.h"

#ifndef LOGO_WIDTH
  #define LOGO_WIDTH  99
#endif
#ifndef LOGO_HEIGHT
  #define LOGO_HEIGHT 64
#endif

#if __has_include("conf/displayGC9A01Aconf_custom.h")
  #include "conf/displayGC9A01Aconf_custom.h"
#else
  #include "conf/displayGC9A01Aconf.h"
#endif

#include "tools/commongfx.h"

extern DspCore dsp;

#endif // DSP_MODEL
#endif // guard
