#include "../core/options.h"
#if DSP_MODEL==DSP_NV3041A

#include <Arduino.h>

#include "displayNV3041A.h"
#include "../core/config.h"
#include "../core/spidog.h"

#include <Arduino_GFX_Library.h>

// QSPI bus
static Arduino_DataBus* bus = new Arduino_ESP32QSPI(
  TFT_CS, TFT_SCK, TFT_D0, TFT_D1, TFT_D2, TFT_D3
);

// GFX object
static Arduino_GFX* make_nv3041a() {
  return new Arduino_NV3041A(bus, TFT_RST, 0 /*rotation*/, true /*IPS*/);
}

//
// yoDisplay
//
yoDisplay::yoDisplay()
: Adafruit_GFX(480, 272), _gfx(make_nv3041a()) { }

bool yoDisplay::begin() {
  sdog.begin();
  sdog.takeMutex();
  bool ok = (_gfx && _gfx->begin());
  sdog.giveMutex();
  return ok;
}

void yoDisplay::setRotation(uint8_t r) {
  if (_gfx) _gfx->setRotation(r);
  Adafruit_GFX::setRotation(r);
}

void yoDisplay::invertDisplay(bool i) {
  if (_gfx) _gfx->invertDisplay(i);
}

void yoDisplay::displayOn() {
  if (_gfx) _gfx->displayOn();
}

void yoDisplay::displayOff() {
  if (_gfx) _gfx->displayOff();
}

void yoDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (_gfx) _gfx->drawPixel(x, y, color);
}

void yoDisplay::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  if (_gfx) _gfx->drawFastHLine(x, y, w, color);
}

void yoDisplay::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  if (_gfx) _gfx->drawFastVLine(x, y, h, color);
}

void yoDisplay::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (_gfx) _gfx->fillRect(x, y, w, h, color);
}

void yoDisplay::fillScreen(uint16_t color) {
  if (_gfx) _gfx->fillScreen(color);
}

void yoDisplay::drawRGBBitmap(int16_t x, int16_t y,
                              const uint16_t *bitmap, int16_t w, int16_t h)
{
  sdog.takeMutex();
  if (_gfx) {
    _gfx->draw16bitRGBBitmap(x, y, const_cast<uint16_t*>(bitmap), w, h);
  }
  sdog.giveMutex();
}

Arduino_GFX* yoDisplay::raw() { return _gfx; }

//
// DspCore
//
DspCore::DspCore() : yoDisplay() { }

void DspCore::initDisplay() {
  begin();
  cp437(true);
  setTextWrap(false);
  setTextSize(1);
  
  setRotation(config.store.flipscreen ? 0 : 2);
  invertDisplay(config.store.invertdisplay);
  fillScreen(0x0000);
}

void DspCore::clearDsp(bool black) {
  fillScreen(black ? 0 : config.theme.background);
}

void DspCore::flip() {
  sdog.takeMutex();
  setRotation(config.store.flipscreen ? 0 : 2);
  sdog.giveMutex();
}

void DspCore::invert() {
  sdog.takeMutex();
  invertDisplay(config.store.invertdisplay);
  sdog.giveMutex();
}

void DspCore::sleep() {
  sdog.takeMutex();
  displayOff();
  sdog.giveMutex();
}

void DspCore::wake() {
  sdog.takeMutex();
  displayOn();
  sdog.giveMutex();
}

uint16_t DspCore::textWidth(const char *txt) {
  int16_t x1, y1;
  uint16_t w, h;
  getTextBounds((char*)txt, 0, 0, &x1, &y1, &w, &h);
  return w;
}

void yoDisplay::startWrite() { if (!sdog.isLocked()) sdog.takeMutex(); }
void yoDisplay::endWrite()   { if (sdog.isLocked())  sdog.giveMutex(); }

//
// --- HARDWARE CLIPPING ---
//
void yoDisplay::setAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    _win_x = x;
    _win_y = y;
    _win_w = w;
    _win_h = h;
}

void yoDisplay::writePixels(uint16_t *data, uint32_t len)
{
    if (!_gfx || !_win_w || !_win_h || !data) return;

    sdog.takeMutex();
    _gfx->draw16bitRGBBitmap(_win_x, _win_y, data, _win_w, _win_h);
    sdog.giveMutex();
}

#endif // DSP_MODEL==DSP_NV3041A
