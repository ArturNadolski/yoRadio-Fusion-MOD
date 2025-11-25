#ifndef displayNV3041A_h
#define displayNV3041A_h
#pragma once

#include "../core/options.h"
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include "fonts/bootlogo99x64.h"
#include "fonts/dsfont70.h"

typedef GFXcanvas16 Canvas;

class Arduino_GFX;

class yoDisplay : public Adafruit_GFX {
public:
  yoDisplay(); 
  bool begin();

  void setRotation(uint8_t r) override;
  void invertDisplay(bool i);
  void displayOn();
  void displayOff();

  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void writePixel(int16_t x, int16_t y, uint16_t color) override { drawPixel(x, y, color); }
  void startWrite() override;
  void endWrite() override;
  void setAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h);
  void writePixels(uint16_t *data, uint32_t len);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

  void fillScreen(uint16_t color) override;
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h);

  Arduino_GFX* raw();

private:
  Arduino_GFX* _gfx;
  int16_t _win_x = 0, _win_y = 0;
  uint16_t _win_w = 0, _win_h = 0;
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

#if __has_include("conf/displayNV3041Aconf_custom.h")
  #include "conf/displayNV3041Aconf_custom.h"
#else
  #include "conf/displayNV3041Aconf.h"
#endif

#include "tools/commongfx.h"

extern DspCore dsp;

#endif
