#include "../core/options.h"
#if DSP_MODEL == DSP_GC9A01_I80

#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "../core/config.h"
#include "displayGC9A01_I80.h"
#include "../core/spidog.h"

// ---------------------------------------------
// PWM háttérfény (marad eredetiben)
// ---------------------------------------------
extern "C" {
  #include <driver/ledc.h>
}

static bool yr_ledc_init_attached = false;

static bool yr_backlight_init_pwm() {
  ledc_timer_config_t tcfg = {};
  tcfg.speed_mode       = LEDC_LOW_SPEED_MODE;
  tcfg.timer_num        = LEDC_TIMER_0;
  tcfg.duty_resolution  = LEDC_TIMER_8_BIT;
  tcfg.freq_hz          = 5000;
  tcfg.clk_cfg          = LEDC_AUTO_CLK;
  if (ledc_timer_config(&tcfg) != ESP_OK) return false;

  ledc_channel_config_t ccfg = {};
  ccfg.gpio_num   = TFT_BL;
  ccfg.speed_mode = LEDC_LOW_SPEED_MODE;
  ccfg.channel    = LEDC_CHANNEL_0;
  ccfg.intr_type  = LEDC_INTR_DISABLE;
  ccfg.timer_sel  = LEDC_TIMER_0;
  ccfg.duty       = 0;
  ccfg.hpoint     = 0;
  if (ledc_channel_config(&ccfg) != ESP_OK) return false;

  yr_ledc_init_attached = true;
  return true;
}

static void yr_backlight_write(uint8_t duty, bool invert) {
  uint32_t v = invert ? (255 - duty) : duty;
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, v);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

// ---------------------------------------------------
// 8080 I80 busz
// ---------------------------------------------------
static Arduino_DataBus* bus =
  new Arduino_ESP32PAR8(
    TFT_DC,
    TFT_CS,
    TFT_WR,
    TFT_RD,
    TFT_D0, TFT_D1, TFT_D2, TFT_D3,
    TFT_D4, TFT_D5, TFT_D6, TFT_D7
  );

// GC9A01 GFX
static Arduino_GFX* make_gc9a01() {
  return new Arduino_GC9A01(bus, TFT_RST, 0 /*rotation*/, true /*IPS*/);
}

// ---------------------------------------------------
// yoDisplay
// ---------------------------------------------------
yoDisplay::yoDisplay() : Adafruit_GFX(240, 240), _gfx(make_gc9a01()) { }

bool yoDisplay::begin() {

#if (TFT_BL >= 0)
  pinMode(TFT_BL, OUTPUT);
  if (yr_backlight_init_pwm()) {
    _bl_pwm_attached = true;
    yr_backlight_write(255, LED_INVERT);
  } else {
    _bl_pwm_attached = false;
    digitalWrite(TFT_BL, LED_INVERT ? LOW : HIGH);
  }
#endif

  pinMode(TFT_CS, OUTPUT); digitalWrite(TFT_CS, HIGH);
  pinMode(TFT_DC, OUTPUT);

#if (TFT_RST >= 0)
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH); delay(10);
  digitalWrite(TFT_RST, LOW);  delay(10);
  digitalWrite(TFT_RST, HIGH); delay(120);
#endif

  sdog.begin();
  sdog.takeMutex();
  bool ok = (_gfx && _gfx->begin());
  sdog.giveMutex();

  if (!ok) return false;

  cp437(true);
  setTextWrap(false);
  setTextSize(1);

  return true;
}

void yoDisplay::setRotation(uint8_t r) {
  if (_gfx) _gfx->setRotation(r);
  Adafruit_GFX::setRotation(r);
}

void yoDisplay::invertDisplay(bool i) { if (_gfx) _gfx->invertDisplay(i); }
void yoDisplay::displayOn()           { if (_gfx) _gfx->displayOn(); }
void yoDisplay::displayOff()          { if (_gfx) _gfx->displayOff(); }

void yoDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (_gfx) _gfx->drawPixel(x, y, color);
}

void yoDisplay::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t c) {
  if (_gfx) _gfx->drawFastHLine(x, y, w, c);
}

void yoDisplay::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t c) {
  if (_gfx) _gfx->drawFastVLine(x, y, h, c);
}

void yoDisplay::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {
  if (_gfx) _gfx->fillRect(x, y, w, h, c);
}

void yoDisplay::fillScreen(uint16_t c) {
  if (_gfx) _gfx->fillScreen(c);
}

void yoDisplay::drawRGBBitmap(int16_t x, int16_t y,
                              const uint16_t *bmp, int16_t w, int16_t h)
{
  sdog.takeMutex();
  if (_gfx) _gfx->draw16bitRGBBitmap(x, y, const_cast<uint16_t*>(bmp), w, h);
  sdog.giveMutex();
}

Arduino_GFX* yoDisplay::raw() { return _gfx; }

void yoDisplay::startWrite() { if (!sdog.isLocked()) sdog.takeMutex(); }
void yoDisplay::endWrite()   { if (sdog.isLocked())  sdog.giveMutex(); }

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

// ----------------------------------------------
// PWM brightness
// ----------------------------------------------
void yoDisplay::setBrightness(uint8_t duty) {
#if (TFT_BL >= 0)
  if (_bl_pwm_attached) {
    yr_backlight_write(duty, LED_INVERT);
  } else {
    digitalWrite(TFT_BL, (LED_INVERT ? (duty ? LOW : HIGH)
                                     : (duty ? HIGH : LOW)));
  }
#endif
}

void yoDisplay::setBrightnessPercent(uint8_t pct) {
  if (pct > 100) pct = 100;
  uint8_t duty = (uint16_t)pct * 255 / 100;
  setBrightness(duty);
}

// ---------------------------------------------------
// DspCore
// ---------------------------------------------------
DspCore::DspCore() : yoDisplay() {}

void DspCore::initDisplay() {
  begin();

  setRotation(
    ROTATE_90
      ? (config.store.flipscreen ? 0 : 2)
      : (config.store.flipscreen ? 2 : 0)
  );

  invertDisplay(config.store.invertdisplay);

  fillScreen(0x0000);
}

void DspCore::clearDsp(bool black) {
  fillScreen(black ? 0 : config.theme.background);
}

void DspCore::flip() {
  setRotation(
    ROTATE_90
      ? (config.store.flipscreen ? 0 : 2)
      : (config.store.flipscreen ? 2 : 0)
  );
}

void DspCore::invert() { invertDisplay(config.store.invertdisplay); }

void DspCore::sleep()  { displayOff(); }
void DspCore::wake()   { displayOn(); }

uint16_t DspCore::textWidth(const char *txt) {
  int16_t x1, y1; uint16_t w, h;
  getTextBounds((char*)txt, 0, 0, &x1, &y1, &w, &h);
  return w;
}

#endif   // DSP_MODEL == DSP_GC9A01_I80

