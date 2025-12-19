#include "../../core/options.h"
#undef HIDE_DATE
#define HIDE_DATE
#if DSP_MODEL!=DSP_DUMMY
#ifdef NAMEDAYS_FILE
#include "../../core/namedays.h"
#endif
#include "../dspcore.h"
#include "../fonts/clockfont_api.h"
#include "../fonts/iconsweather.h"
#include "Arduino.h"
#include "widgets.h"
#include "../../core/player.h"    //  for VU widget
#include "../../core/network.h"   //  for Clock widget
#include "../../core/config.h"
#include "../tools/l10n.h"
#include "../tools/psframebuffer.h"
#include <math.h> 

static inline void _pad2(char* out, int v) {
  out[0] = '0' + (v / 10);
  out[1] = '0' + (v % 10);
  out[2] = '\0';
}

static void formatDateCustom(char* out, size_t outlen, const tm& t, uint8_t fmt) {
  const char* month = LANG::mnths[t.tm_mon];
  const char* dow   = LANG::dowf[t.tm_wday];

  char d2[3], m2[3];
  _pad2(d2, t.tm_mday);
  _pad2(m2, t.tm_mon + 1);
  int y = t.tm_year + 1900;

#if L10N_LANGUAGE == HU
  // Hungarian
  switch (fmt) {
    case 1: // 2025. 09. 16. - kedd 
      snprintf(out, outlen, "%d. %s. %s. - %s", y, m2, d2, dow);
      break;

    case 2: // 2025. szeptember 16.  
      snprintf(out, outlen, "%d. %s %d.", y, month, t.tm_mday);
      break;

    case 3: // 2025 szeptember 16. kedd  
      snprintf(out, outlen, "%d %s %d. %s", y, month, t.tm_mday, dow);
      break;

    case 4: // 09. 16. - kedd 
      snprintf(out, outlen, "%s. %s. - %s", m2, d2, dow);
      break;

    default: // 2025. 09. 16. 
      snprintf(out, outlen, "%d. %s. %s.", y, m2, d2);
      break;
  }

#else 

  #ifdef USDATE
    // US format MM/DD/YYYY)
    switch (fmt) {
      case 1: // Tue - 09.16.2025
        snprintf(out, outlen, "%s - %s.%s.%d", dow, m2, d2, y);
        break;

      case 2: // September 16, 2025
        snprintf(out, outlen, "%s %d, %d", month, t.tm_mday, y);
        break;

      case 3: // Tue, September 16, 2025
        snprintf(out, outlen, "%s, %s %d, %d", dow, month, t.tm_mday, y);
        break;

      case 4: // Tue - 09.16.
        snprintf(out, outlen, "%s - %s.%s.", dow, m2, d2);
        break;

      default: // 09.16.2025
        snprintf(out, outlen, "%s.%s.%d", m2, d2, y);
        break;
    }

  #else
    // National format (DD/MM/YYYY)
    switch (fmt) {
      case 1: // Tue - 16.09.2025 
        snprintf(out, outlen, "%s - %s. %s. %d", dow, d2, m2, y);
        break;

      case 2: // 16 September 2025   
        snprintf(out, outlen, "%d. %s %d", t.tm_mday, month, y);
        break;

      case 3: // Tue, 16 September 2025  
        snprintf(out, outlen, "%s, %d %s %d", dow, t.tm_mday, month, y);
        break;

      case 4: // Tue - 16.09. 
        snprintf(out, outlen, "%s - %s. %s.", dow, d2, m2);
        break;

      default: // 16.09.2025 
        snprintf(out, outlen, "%s. %s. %d", d2, m2, y);
        break;
    }
  #endif

#endif

}

static inline size_t utf8len(const char* s) {
  size_t len = 0;
  for (const unsigned char* p = (const unsigned char*)s; *p; ++p) {
    if ( (*p & 0xC0) != 0x80 ) ++len; 
  }
  return len;
}

static inline float _applyVuCurve(float x, uint8_t ly) {
    // 0..1 normalizált bemenet
    float floor = REF_BY_LAYOUT(config.store, vuFloor, ly) / 100.0f;
    float ceil  = REF_BY_LAYOUT(config.store, vuCeil,  ly) / 100.0f;
    float expo  = REF_BY_LAYOUT(config.store, vuExpo,  ly) / 100.0f;  if (expo < 0.01f) expo = 0.01f;
    float gain  = REF_BY_LAYOUT(config.store, vuGain,  ly) / 100.0f;
    float knee  = REF_BY_LAYOUT(config.store, vuKnee,  ly) / 100.0f;

    // normalizálás floor..ceil tartományra
    float denom = (ceil - floor);
    if (denom < 0.001f) denom = 0.001f;
    x = (x - floor) / denom;
    // puha küszöb (smoothstep a 0..knee sávban)
    if (knee > 0.0f) {
      float t = fminf(fmaxf(x / knee, 0.0f), 1.0f);
      float smooth = t * t * (3.0f - 2.0f * t);   // smoothstep(0..1)
      x = (x < knee) ? (smooth * knee) : x;
    }
    x = fminf(fmaxf(x, 0.0f), 1.0f);
    // exponenciális alakítás + gain
    x = powf(x, expo) * gain;
    return fminf(fmaxf(x, 0.0f), 1.0f);
  }

/************************
      FILL WIDGET
 ************************/
void FillWidget::init(FillConfig conf, uint16_t bgcolor){
  Widget::init(conf.widget, bgcolor, bgcolor);
  _width = conf.width;
  _height = conf.height;
  
}

void FillWidget::_draw(){
  if(!_active) return;
  dsp.fillRect(_config.left, _config.top, _width, _height, _bgcolor);
}

void FillWidget::setHeight(uint16_t newHeight){
  _height = newHeight;
  //_draw();
}
/************************
      TEXT WIDGET
 ************************/
TextWidget::~TextWidget() {
  free(_text);
  free(_oldtext);
}

void TextWidget::_charSize(uint8_t textsize, uint8_t& width, uint16_t& height){
#ifndef DSP_LCD
  width = textsize * CHARWIDTH;
  height = textsize * CHARHEIGHT;
#else
  width = 1;
  height = 1;
#endif
}

void TextWidget::init(WidgetConfig wconf, uint16_t buffsize, bool uppercase, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(wconf, fgcolor, bgcolor);
  _buffsize = buffsize;
  _text = (char *) malloc(sizeof(char) * _buffsize);
  memset(_text, 0, _buffsize);
  _oldtext = (char *) malloc(sizeof(char) * _buffsize);
  memset(_oldtext, 0, _buffsize);
  _charSize(_config.textsize, _charWidth, _textheight);
  _textwidth = _oldtextwidth = _oldleft = 0;
  _uppercase = uppercase;
}

void TextWidget::setText(const char* txt) {
  strlcpy(_text, utf8To(txt, _uppercase), _buffsize);
  _textwidth = utf8len(_text) * _charWidth;
  if (strcmp(_oldtext, _text) == 0) return;
  if (_active) dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()),  _config.top, max(_oldtextwidth, _textwidth), _textheight, _bgcolor);
  _oldtextwidth = _textwidth;
  _oldleft = _realLeft();
  if (_active) _draw();
}

void TextWidget::setText(int val, const char *format){
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, val);
  setText(buf);
}

void TextWidget::setText(const char* txt, const char *format){
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, txt);
  setText(buf);
}

/*uint16_t TextWidget::_realLeft(bool w_fb) {
  uint16_t realwidth = (_width>0 && w_fb)?_width:dsp.width();
  switch (_config.align) {
    case WA_CENTER: return (realwidth - _textwidth) / 2; break;
    case WA_RIGHT: return (realwidth - _textwidth - (!w_fb?_config.left:0)); break;
    default: return !w_fb?_config.left:0; break;
  }
}*/
uint16_t TextWidget::_realLeft(bool w_fb) {
  uint16_t realwidth = (_width>0 && w_fb) ? _width : dsp.width();
  int32_t calc = 0;
  switch (_config.align) {
    case WA_CENTER:
      calc = (int32_t)realwidth - (int32_t)_textwidth;
      calc /= 2;
      break;
    case WA_RIGHT:
      calc = (int32_t)realwidth - (int32_t)_textwidth - (!w_fb ? _config.left : 0);
      break;
    case WA_LEFT:
    default:
      calc = !w_fb ? _config.left : 0;
      break;
  }
  if (calc < 0) calc = 0;               // ← fontos!
  return (uint16_t)calc;
}

void TextWidget::_draw() {
  if(!_active) return;
  dsp.setTextColor(_fgcolor, _bgcolor);
  dsp.setCursor(_realLeft(), _config.top);
  dsp.setFont();
  dsp.setTextSize(_config.textsize);
  dsp.print(_text);
  strlcpy(_oldtext, _text, _buffsize);
}

/************************
      SCROLL WIDGET
 ************************/
ScrollWidget::ScrollWidget(const char* separator, ScrollConfig conf, uint16_t fgcolor, uint16_t bgcolor) {
  init(separator, conf, fgcolor, bgcolor);
}

ScrollWidget::~ScrollWidget() {
  free(_fb);
  free(_sep);
  free(_window);
}

uint16_t ScrollWidget::_winLeft(bool fb) const {
  if (fb) return 0;

  switch (_config.align) {
    case WA_CENTER:
      return (uint16_t)((dsp.width() - _width) / 2);
    case WA_RIGHT:
      // WA_RIGHT 
      return (uint16_t)(dsp.width() - _width - _config.left);
    case WA_LEFT:
    default:
      return _config.left;
  }
}

void ScrollWidget::init(const char* separator, ScrollConfig conf, uint16_t fgcolor, uint16_t bgcolor) {
  TextWidget::init(conf.widget, conf.buffsize, conf.uppercase, fgcolor, bgcolor);
  _sep = (char *) malloc(sizeof(char) * 4);
  memset(_sep, 0, 4);
  snprintf(_sep, 4, " %.*s ", 1, separator);
  _startscrolldelay = conf.startscrolldelay;
  _scrolldelta      = conf.scrolldelta;
  _scrolltime       = conf.scrolltime;
  _charSize(_config.textsize, _charWidth, _textheight);
  _sepwidth = strlen(_sep) * _charWidth;
  _width = conf.width;
  _backMove.width = _width;
  _window = (char *) malloc(sizeof(char) * (MAX_WIDTH / _charWidth + 1));
  memset(_window, 0, (MAX_WIDTH / _charWidth + 1));
  _doscroll = false;
  const uint16_t wl  = _winLeft(false);
  const uint16_t fbl = _fb && _fb->ready() ? 0 : wl;
  _x = fbl;

#ifdef PSFBUFFER
  _fb = new psFrameBuffer(dsp.width(), dsp.height());
  _fb->begin(&dsp, wl, _config.top, _width, _textheight, _bgcolor);
#endif
}


void ScrollWidget::_setTextParams() {
  if (_config.textsize == 0) return;
  if(_fb->ready()){
  #ifdef PSFBUFFER
    _fb->setTextSize(_config.textsize);
    _fb->setTextColor(_fgcolor, _bgcolor);
  #endif
  }else{
    dsp.setTextSize(_config.textsize);
    dsp.setTextColor(_fgcolor, _bgcolor);
  }
}

bool ScrollWidget::_checkIsScrollNeeded() {
  return _textwidth > _width;
}

void ScrollWidget::setText(const char* txt) {
  strlcpy(_text, utf8To(txt, _uppercase), _buffsize - 1);
  if (strcmp(_oldtext, _text) == 0) return;

  _textwidth = strlen(_text) * _charWidth;

  const uint16_t wl  = _winLeft(false);
  const uint16_t fbl = _fb->ready() ? 0 : wl;
  _x = fbl;

  _doscroll = _checkIsScrollNeeded();
  if (dsp.getScrollId() == this) dsp.setScrollId(NULL);
  _scrolldelay = millis();

  if (_active) {
    _setTextParams();

    if (_doscroll) {
#ifdef PSFBUFFER
      if (_fb->ready()) {
        _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
        _fb->setCursor(0, 0);
        snprintf(_window, _width / _charWidth + 1, "%s", _text); // első ablak
        _fb->print(_window);
        _fb->display();
      } else
#endif
      {
        dsp.fillRect(wl, _config.top, _width, _textheight, _bgcolor);
        // első ablakot kiírjuk, clippinggel
        dsp.setClipping({wl, _config.top, _width, _textheight});
        dsp.setCursor(wl, _config.top);  // első pozíció: window bal széle
        snprintf(_window, _width / _charWidth + 1, "%s", _text);
        dsp.print(_window);
        dsp.clearClipping();
      }
    } else {
#ifdef PSFBUFFER
      if (_fb->ready()) {
        _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
        _fb->setCursor(_realLeft(true), 0);
        _fb->print(_text);
        _fb->display();
      } else
#endif
      {
        dsp.fillRect(wl, _config.top, _width, _textheight, _bgcolor);
        dsp.setCursor(_realLeft(), _config.top);  // TextWidget kezeli az igazítást
        dsp.setClipping({wl, _config.top, _width, _textheight});
        dsp.print(_text);
        dsp.clearClipping();
      }
    }

    strlcpy(_oldtext, _text, _buffsize);
  }
}

void ScrollWidget::setText(const char* txt, const char *format){
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, txt);
  setText(buf);
}

void ScrollWidget::loop() {
  if (_locked) return;
  if (!_doscroll || _config.textsize == 0 || (dsp.getScrollId() != NULL && dsp.getScrollId() != this)) return;

  const uint16_t wl  = _winLeft(false);
  const uint16_t fbl = _fb->ready() ? 0 : wl;

  if (_checkDelay(_x == fbl ? _startscrolldelay : _scrolltime, _scrolldelay)) {
    _calcX();
    if (_active) _draw();
  }
}


void ScrollWidget::_clear(){
  if(_fb->ready()){
    #ifdef PSFBUFFER
    _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
    _fb->display();
    #endif
  } else {
    const uint16_t wl = _winLeft(false);
    //dsp.fillRect(_config.left, _config.top, _width, _textheight, _bgcolor);
    dsp.fillRect(wl, _config.top, _width, _textheight, _bgcolor);
  }
}

void ScrollWidget::_draw() {
  if (!_active || _locked) return;
  _setTextParams();

  const uint16_t wl  = _winLeft(false);
  const uint16_t fbl = _fb->ready() ? 0 : wl;

  if (_doscroll) {
    uint16_t _newx = fbl - _x;
    const char* _cursor = _text + _newx / _charWidth;
    uint16_t hiddenChars = _cursor - _text;
    uint8_t addChars = _fb->ready() ? 2 : 1;

    if (hiddenChars < strlen(_text)) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation="
      snprintf(_window, _width / _charWidth + addChars, "%s%s%s", _cursor, _sep, _text);
#pragma GCC diagnostic pop
    } else {
      const char* _scursor = _sep + (_cursor - (_text + strlen(_text)));
      snprintf(_window, _width / _charWidth + addChars, "%s%s", _scursor, _text);
    }
    if (_fb->ready()) {
#ifdef PSFBUFFER
      _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
      _fb->setCursor(_x + hiddenChars * _charWidth, 0);
      _fb->print(_window);
      _fb->display();
#endif
    } else {
      dsp.setClipping({wl, _config.top, _width, _textheight});
      // kurzor: window bal (wl) + lokális eltolás (_x - fbl) + rejtett karakterek szélessége
      dsp.setCursor(wl + (_x - fbl) + hiddenChars * _charWidth, _config.top);
      dsp.print(_window);
#ifndef DSP_LCD
      dsp.print(" ");
#endif
      dsp.clearClipping();
    }
  } else {
    if (_fb->ready()) {
#ifdef PSFBUFFER
      _fb->fillRect(0, 0, _width, _textheight, _bgcolor);
      _fb->setCursor(_realLeft(true), 0);
      _fb->print(_text);
      _fb->display();
#endif
    } else {
      dsp.fillRect(wl, _config.top, _width, _textheight, _bgcolor);
      dsp.setCursor(_realLeft(), _config.top);
      dsp.setClipping({wl, _config.top, _width, _textheight});
      dsp.print(_text);
      dsp.clearClipping();
    }
  }
}

void ScrollWidget::_calcX() {
  if (!_doscroll || _config.textsize == 0) return;

  _x -= _scrolldelta;

  const uint16_t wl  = _winLeft(false);
  const uint16_t fbl = _fb->ready() ? 0 : wl;

  if (-_x > _textwidth + _sepwidth - fbl) {
    _x = fbl;
    dsp.setScrollId(NULL);
  } else {
    dsp.setScrollId(this);
  }
}


bool ScrollWidget::_checkDelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

void ScrollWidget::setWindowWidth(uint16_t w) {
  if (w == 0 || w == _width) return;

  _width = w;
  _backMove.width = _width;

#ifdef PSFBUFFER
  if (_fb) {
    _fb->freeBuffer();
    const uint16_t wl = _winLeft(false);
    _fb->begin(&dsp, wl, _config.top, _width, _textheight, _bgcolor);
  }
#endif

  const uint16_t fbl = _fb->ready() ? 0 : _winLeft(false);
  _x = fbl;

  _clear();
  _doscroll = _checkIsScrollNeeded();
}

void ScrollWidget::_reset(){
  dsp.setScrollId(NULL);

  const uint16_t wl  = _winLeft(false);
  const uint16_t fbl = _fb->ready() ? 0 : wl;
  _x = fbl;

  _scrolldelay = millis();
  _doscroll = _checkIsScrollNeeded();

#ifdef PSFBUFFER
  _fb->freeBuffer();
  _fb->begin(&dsp, wl, _config.top, _width, _textheight, _bgcolor);
#endif
}


/************************
      SLIDER WIDGET
 ************************/
void SliderWidget::init(FillConfig conf, uint16_t fgcolor, uint16_t bgcolor, uint32_t maxval, uint16_t oucolor) {
  Widget::init(conf.widget, fgcolor, bgcolor);
  _width = conf.width; _height = conf.height; _outlined = conf.outlined; _oucolor = oucolor, _max = maxval;
  _oldvalwidth = _value = 0;
}

void SliderWidget::setValue(uint32_t val) {
  _value = val;
  if (_active && !_locked) _drawslider();

}

void SliderWidget::_drawslider() {
  uint16_t valwidth = map(_value, 0, _max, 0, _width - _outlined * 2);
  if (_oldvalwidth == valwidth) return;
  dsp.fillRect(_config.left + _outlined + min(valwidth, _oldvalwidth), _config.top + _outlined, abs(_oldvalwidth - valwidth), _height - _outlined * 2, _oldvalwidth > valwidth ? _bgcolor : _fgcolor);
  _oldvalwidth = valwidth;
}

void SliderWidget::_draw() {
  if(_locked) return;
  _clear();
  if(!_active) return;
  if (_outlined) dsp.drawRect(_config.left, _config.top, _width, _height, _oucolor);
  uint16_t valwidth = map(_value, 0, _max, 0, _width - _outlined * 2);
  dsp.fillRect(_config.left + _outlined, _config.top + _outlined, valwidth, _height - _outlined * 2, _fgcolor);
}

void SliderWidget::_clear() {
//  _oldvalwidth = 0;
  dsp.fillRect(_config.left, _config.top, _width, _height, _bgcolor);
}
void SliderWidget::_reset() {
  _oldvalwidth = 0;
}
/************************
      VU WIDGET
 ************************/
#if !defined(DSP_LCD) && !defined(DSP_OLED)
VuWidget::~VuWidget() {
  if(_canvas) free(_canvas);
}

void VuWidget::init(WidgetConfig wconf, VUBandsConfig bands, uint16_t vumaxcolor, uint16_t vumincolor, uint16_t bgcolor) {
  Widget::init(wconf, bgcolor, bgcolor);
  _vumaxcolor = vumaxcolor;
  _vumincolor = vumincolor;
  _bands = bands;

  _maxDimension = _config.align ? _bands.width : _bands.height;  // max szélesség/magasság

  _peakL = 0;
  _peakR = 0;
  _peakFallDelayCounter = 0;

  // VU layout
  switch (config.store.vuLayout) {
    case 3: // Studio 
      _canvas = new Canvas(_maxDimension, _bands.height * 2 + _bands.space);
      break;
    case 2: // Boombox 
    case 1: // Streamline 
      _canvas = new Canvas(_maxDimension * 2 + _bands.space, _bands.height);
      break;
    case 0:
    default: // Default 
      _canvas = new Canvas(_bands.width * 2 + _bands.space, _maxDimension);
      break;
  }
}

void VuWidget::_draw() {
  if(!_active || _locked) return;

  static uint16_t measL = 0, measR = 0;
  static float smoothL = 0, smoothR = 0;
  static float peakSmoothL = 0, peakSmoothR = 0;

  uint16_t bandColor;
  const uint16_t _vumidcolor = config.store.vuMidColor;

  // --- input scaling ---
  uint16_t vulevel_raw = player.getVUlevel(_maxDimension);  // "audio_change" nem kell paraméter
  uint16_t L_raw = (vulevel_raw >> 8) & 0xFF;
  uint16_t R_raw = (vulevel_raw) & 0xFF;

  uint16_t L_lin = 255 - L_raw;
  uint16_t R_lin = 255 - R_raw;

  float vuNorm_L = (float)L_lin / 255.0f;
  float vuNorm_R = (float)R_lin / 255.0f;

  uint8_t ly = config.store.vuLayout;
  float vuAdj_L = _applyVuCurve(vuNorm_L, ly);
  float vuAdj_R = _applyVuCurve(vuNorm_R, ly);

  uint16_t L_scaled = (uint16_t)roundf(vuAdj_L * _maxDimension);
  uint16_t R_scaled = (uint16_t)roundf(vuAdj_R * _maxDimension);
  if (L_scaled > _maxDimension) L_scaled = _maxDimension;
  if (R_scaled > _maxDimension) R_scaled = _maxDimension;

  // --- bands param ---
  uint8_t bandsCount = _bands.perheight;
  uint16_t h = _maxDimension / bandsCount;
  const float alpha_up   = REF_BY_LAYOUT(config.store, vuAlphaUp, ly) / 100.0f;
  const float alpha_down = REF_BY_LAYOUT(config.store, vuAlphaDn, ly) / 100.0f;
  uint8_t midPct  = REF_BY_LAYOUT(config.store, vuMidPct,  ly);
  uint8_t highPct = REF_BY_LAYOUT(config.store, vuHighPct, ly);
  const uint16_t peakColor = config.store.vuPeakColor;
  int cfgBars = (int)REF_BY_LAYOUT(config.store, vuBarCount, ly);
  int effectiveBars = (int)bandsCount;
  if (effectiveBars <= 0) effectiveBars = cfgBars;
  
  int midBandIndex  = (effectiveBars * midPct  + 99) / 100;
  int highBandIndex = (effectiveBars * highPct + 99) / 100;
  
  if (midBandIndex  < 0)               midBandIndex  = 0;
  if (highBandIndex < 0)               highBandIndex = 0;
  if (midBandIndex  > effectiveBars)   midBandIndex  = effectiveBars;
  if (highBandIndex > effectiveBars)   highBandIndex = effectiveBars;
  if (highBandIndex < midBandIndex)    highBandIndex = midBandIndex;
  
  // --- up/down smoothing ---
  smoothL = (L_scaled > smoothL) ? (alpha_up * L_scaled + (1.0f - alpha_up) * smoothL)
                                 : (alpha_down * L_scaled + (1.0f - alpha_down) * smoothL);
  smoothR = (R_scaled > smoothR) ? (alpha_up * R_scaled + (1.0f - alpha_up) * smoothR)
                                 : (alpha_down * R_scaled + (1.0f - alpha_down) * smoothR);

  bool played = player.isRunning();
  if (played) {
    measL = (L_raw >= measL) ? (measL + _bands.fadespeed) : L_raw;
    measR = (R_raw >= measR) ? (measR + _bands.fadespeed) : R_raw;
  } else {
    if (measL < _maxDimension) measL += _bands.fadespeed;
    if (measR < _maxDimension) measR += _bands.fadespeed;
    peakSmoothL = 0; peakSmoothR = 0;
  }
  if (measL > _maxDimension) measL = _maxDimension;
  if (measR > _maxDimension) measR = _maxDimension;

  switch (config.store.vuLayout) {
    case 3: { // Studio 
      _canvas->fillRect(0, 0, _maxDimension, _bands.height * 2 + _bands.space, _bgcolor);

      uint16_t drawL = (uint16_t)smoothL;
      uint16_t drawR = (uint16_t)smoothR;
      uint16_t snappedL = ((drawL + h - 1) / h) * h;
      uint16_t snappedR = ((drawR + h - 1) / h) * h;

      // peak 
      const float p_up       = REF_BY_LAYOUT(config.store, vuPeakUp,  ly) / 100.0f;
      const float p_down     = REF_BY_LAYOUT(config.store, vuPeakDn,  ly) / 100.0f;
      peakSmoothL = (drawL > peakSmoothL) ? (p_up * drawL + (1 - p_up) * peakSmoothL)
                                          : (p_down * drawL + (1 - p_down) * peakSmoothL);
      peakSmoothR = (drawR > peakSmoothR) ? (p_up * drawR + (1 - p_up) * peakSmoothR)
                                          : (p_down * drawR + (1 - p_down) * peakSmoothR);

      int peakWidth = 4;
      int peakOfs = 10;
      int peakX_L = constrain((int)std::max((int)snappedL, (int)peakSmoothL + peakOfs), 0, (int)_maxDimension - peakWidth);
      int peakX_R = constrain((int)std::max((int)snappedR, (int)peakSmoothR + peakOfs), 0, (int)_maxDimension - peakWidth);

      for (int band = 0; band < bandsCount; band++) {
        int i = band * h;
        int bandWidth = h - _bands.space; if (band == bandsCount - 1) bandWidth = h;
        bandColor = (band < midBandIndex) ? _vumincolor : (band < highBandIndex ? _vumidcolor : _vumaxcolor);
        if (i + bandWidth <= snappedL)
          _canvas->fillRect(i, 0, bandWidth, _bands.height, bandColor);
        if (i + bandWidth <= snappedR)
          _canvas->fillRect(i, _bands.height + _bands.space, bandWidth, _bands.height, bandColor);
      }
      _canvas->fillRect(peakX_L, 0, peakWidth, _bands.height, peakColor);
      _canvas->fillRect(peakX_R, _bands.height + _bands.space, peakWidth, _bands.height, peakColor);

      dsp.drawRGBBitmap(_config.left, _config.top, _canvas->getBuffer(), _maxDimension, _bands.height * 2 + _bands.space);
    } break;

    case 2: { // Boombox 
      _canvas->fillRect(0, 0, _maxDimension * 2 + _bands.space, _bands.height, _bgcolor);

      uint16_t drawL = (uint16_t)smoothL;
      uint16_t drawR = (uint16_t)smoothR;
      uint16_t snappedL = ((drawL + h - 1) / h) * h;
      uint16_t snappedR = ((drawR + h - 1) / h) * h;

      const float p_up       = REF_BY_LAYOUT(config.store, vuPeakUp,  ly) / 100.0f;
      const float p_down     = REF_BY_LAYOUT(config.store, vuPeakDn,  ly) / 100.0f;
      peakSmoothL = (drawL > peakSmoothL) ? (p_up * drawL + (1 - p_up) * peakSmoothL)
                                          : (p_down * drawL + (1 - p_down) * peakSmoothL);
      peakSmoothR = (drawR > peakSmoothR) ? (p_up * drawR + (1 - p_up) * peakSmoothR)
                                          : (p_down * drawR + (1 - p_down) * peakSmoothR);

      int peakWidth = 4, peakOfs = 10;
      int peakLenL = (int)std::max((int)snappedL, (int)peakSmoothL); // hossz a középtől
      int peakX_L  = _bands.width - peakLenL - peakOfs - peakWidth;  // kifelé (balra) toljuk
      peakX_L = std::max(0, std::min((int)_bands.width - peakWidth, peakX_L));

      int peakLenR = (int)std::max((int)snappedR, (int)peakSmoothR); // hossz a középtől
      int peakX_R  = _bands.width + _bands.space + peakLenR + peakOfs; // kifelé (jobbra) toljuk
      peakX_R = std::max((int)_bands.width + (int)_bands.space,
                   std::min((int)(_bands.width + _bands.space + _bands.width - peakWidth), peakX_R));

      for (int band = 0; band < bandsCount; band++) {
        int i = band * h;
        int bandWidth = h - _bands.vspace; if (band == bandsCount - 1) bandWidth = h;
        bandColor = (band < midBandIndex) ? _vumincolor : (band < highBandIndex ? _vumidcolor : _vumaxcolor);
        if (i + bandWidth <= snappedL)
          _canvas->fillRect(_bands.width - i - bandWidth, 0, bandWidth, _bands.height, bandColor);
        if (i + bandWidth <= snappedR)
          _canvas->fillRect(i + _bands.width + _bands.space, 0, bandWidth, _bands.height, bandColor);
      }
      _canvas->fillRect(peakX_L, 0, peakWidth, _bands.height, peakColor);
      _canvas->fillRect(peakX_R, 0, peakWidth, _bands.height, peakColor);

      dsp.drawRGBBitmap(_config.left, _config.top, _canvas->getBuffer(), _maxDimension * 2 + _bands.space, _bands.height);
    } break;

    case 1: { // Streamline 
      _canvas->fillRect(0, 0, _maxDimension * 2 + _bands.space, _bands.height, _bgcolor);

      uint16_t drawL = (uint16_t)smoothL;
      uint16_t drawR = (uint16_t)smoothR;
      uint16_t snappedL = ((drawL + h - 1) / h) * h;
      uint16_t snappedR = ((drawR + h - 1) / h) * h;

      const float p_up       = REF_BY_LAYOUT(config.store, vuPeakUp,  ly) / 100.0f;
      const float p_down     = REF_BY_LAYOUT(config.store, vuPeakDn,  ly) / 100.0f;
      peakSmoothL = (drawL > peakSmoothL) ? (p_up * drawL + (1 - p_up) * peakSmoothL)
                                          : (p_down * drawL + (1 - p_down) * peakSmoothL);
      peakSmoothR = (drawR > peakSmoothR) ? (p_up * drawR + (1 - p_up) * peakSmoothR)
                                          : (p_down * drawR + (1 - p_down) * peakSmoothR);

      int peakWidth = 4, peakOfs = 4;
      int peakX_L = constrain((int)std::max((int)snappedL, (int)peakSmoothL + peakOfs), 0, (int)_bands.width - peakWidth);
      int peakX_R = constrain((int)std::max((int)snappedR, (int)peakSmoothR + peakOfs), 0, (int)_bands.width - peakWidth);

      for (int band = 0; band < bandsCount; band++) {
        int i = band * h;
        int bandWidth = h - _bands.vspace; if (band == bandsCount - 1) bandWidth = h;
        bandColor = (band < midBandIndex) ? _vumincolor : (band < highBandIndex ? _vumidcolor : _vumaxcolor);
        if (i + bandWidth/2 < snappedL)
          _canvas->fillRect(i, 0, bandWidth, _bands.height, bandColor);
        if (i + bandWidth <= snappedR)
          _canvas->fillRect(i + _bands.width + _bands.space, 0, bandWidth, _bands.height, bandColor);
      }
      _canvas->fillRect(peakX_L, 0, peakWidth, _bands.height, peakColor);
      _canvas->fillRect(peakX_R + _bands.width + _bands.space, 0, peakWidth, _bands.height, peakColor);

      dsp.drawRGBBitmap(_config.left, _config.top, _canvas->getBuffer(), _maxDimension * 2 + _bands.space, _bands.height);
    } break;

    case 0:
    default: { // Default 
      _canvas->fillRect(0, 0, _bands.width * 2 + _bands.space, _maxDimension, _bgcolor);

      uint16_t drawL = (uint16_t)smoothL;
      uint16_t drawR = (uint16_t)smoothR;
      uint16_t snappedL = ((drawL + h - 1) / h) * h;
      uint16_t snappedR = ((drawR + h - 1) / h) * h;

      const float p_up       = REF_BY_LAYOUT(config.store, vuPeakUp,  ly) / 100.0f;
      const float p_down     = REF_BY_LAYOUT(config.store, vuPeakDn,  ly) / 100.0f;
      peakSmoothL = (drawL > peakSmoothL) ? (p_up * drawL + (1 - p_up) * peakSmoothL)
                                          : (p_down * drawL + (1 - p_down) * peakSmoothL);
      peakSmoothR = (drawR > peakSmoothR) ? (p_up * drawR + (1 - p_up) * peakSmoothR)
                                          : (p_down * drawR + (1 - p_down) * peakSmoothR);

      int peakHeight = 4, peakOfs = 4;
      int peakY_L = constrain((int)peakSmoothL + peakOfs, 0, (int)_maxDimension - peakHeight);
      int peakY_R = constrain((int)peakSmoothR + peakOfs, 0, (int)_maxDimension - peakHeight);

      for (int band = 0; band < bandsCount; band++) {
        int i = band * h;
        int bandHeight = h - _bands.vspace; if (band == bandsCount - 1) bandHeight = h;
        bandColor = (band < midBandIndex) ? _vumincolor : (band < highBandIndex ? _vumidcolor : _vumaxcolor);
        if (i + bandHeight/2 < snappedL)
          _canvas->fillRect(0, _maxDimension - i - bandHeight, _bands.width, bandHeight, bandColor);
        if (i + bandHeight <= snappedR)
          _canvas->fillRect(_bands.width + _bands.space, _maxDimension - i - bandHeight, _bands.width, bandHeight, bandColor);
      }
      _canvas->fillRect(0, _maxDimension - peakY_L - peakHeight, _bands.width, peakHeight, peakColor);
      _canvas->fillRect(_bands.width + _bands.space, _maxDimension - peakY_R - peakHeight, _bands.width, peakHeight, peakColor);

      dsp.drawRGBBitmap(_config.left, _config.top, _canvas->getBuffer(), _bands.width * 2 + _bands.space, _maxDimension);
    } break;
  }
}

void VuWidget::loop(){
  if(_active && !_locked) _draw();
}

void VuWidget::_clear() {
  switch (config.store.vuLayout) {
    case 3: dsp.fillRect(_config.left, _config.top, _maxDimension, _bands.height * 2 + _bands.space, _bgcolor); break;
    case 2:
    case 1: dsp.fillRect(_config.left, _config.top, _maxDimension * 2 + _bands.space, _bands.height, _bgcolor); break;
    case 0:
    default: dsp.fillRect(_config.left, _config.top, _bands.width * 2 + _bands.space, _maxDimension, _bgcolor); break;
  }
}
#else // DSP_LCD
VuWidget::~VuWidget() {}
void VuWidget::init(WidgetConfig wconf, VUBandsConfig bands, uint16_t vumaxcolor, uint16_t vumincolor, uint16_t bgcolor) {
  Widget::init(wconf, bgcolor, bgcolor);
}
void VuWidget::_draw() {}
void VuWidget::loop() {}
void VuWidget::_clear() {}
#endif

/************************
      NUM & CLOCK
 ************************/
#if !defined(DSP_LCD)
  #if TIME_SIZE<19 //19->NOKIA
    const GFXfont* Clock_GFXfontPtr = nullptr;
    #define CLOCKFONT5x7
  #else
    const GFXfont* Clock_GFXfontPtr = nullptr;
  #endif
#endif //!defined(DSP_LCD)

static inline void applyClockFontFromConfig() {
  #if !defined(DSP_LCD)
    const uint8_t id = clockfont_clamp_id(config.store.clockFontId); // 0..4
    Clock_GFXfontPtr = clockfont_get(id);
  #endif
}

#if !defined(CLOCKFONT5x7) && !defined(DSP_LCD)
// --- GFXfont metrika-alapú számolás (nincs ':' kivétel, nincs API-advance) ---
inline const GFXglyph* glyphPtr(const GFXfont* f, unsigned char c){
  if (!f) return nullptr;
  if (c < f->first || c > f->last) return nullptr;
  return f->glyph + (c - f->first);
}

uint8_t _charWidth(unsigned char c){
  if (!Clock_GFXfontPtr) return TIME_SIZE;        // 5x7 fallback biztosíték
  const GFXglyph* g = glyphPtr(Clock_GFXfontPtr, c);
  return g ? pgm_read_byte(&g->xAdvance) : 0;
}

uint16_t _textHeight(){
  if (!Clock_GFXfontPtr) return TIME_SIZE;        // 5x7 fallback biztosíték
  const GFXglyph* g = glyphPtr(Clock_GFXfontPtr, '8'); // reprezentatív magas szám
  return g ? pgm_read_byte(&g->height) : TIME_SIZE;
}

#else // CLOCKFONT5x7 || DSP_LCD
// --- 5x7 / LCD fallback: marad a régi skálázás ---
uint8_t _charWidth(unsigned char){
#ifndef DSP_LCD
  return CHARWIDTH * TIME_SIZE;
#else
  return 1;
#endif
}
uint16_t _textHeight(){
  return CHARHEIGHT * TIME_SIZE;
}
#endif

uint16_t _textWidth(const char *txt){
  uint16_t w = 0, l=strlen(txt);
  for(uint16_t c=0;c<l;c++) w+=_charWidth(txt[c]);
  //#if DSP_MODEL==DSP_ILI9225
  //return w+l;
  //#else
  return w;
  //#endif
}

/************************
      NUM WIDGET
 ************************/
void NumWidget::init(WidgetConfig wconf, uint16_t buffsize, bool uppercase, uint16_t fgcolor, uint16_t bgcolor) {
  Widget::init(wconf, fgcolor, bgcolor);
  _buffsize = buffsize;
  _text = (char *) malloc(sizeof(char) * _buffsize);
  memset(_text, 0, _buffsize);
  _oldtext = (char *) malloc(sizeof(char) * _buffsize);
  memset(_oldtext, 0, _buffsize);
  _textwidth = _oldtextwidth = _oldleft = 0;
  _uppercase = uppercase;
  _textheight = TIME_SIZE/*wconf.textsize*/;
}

void NumWidget::setText(const char* txt) {
  strlcpy(_text, txt, _buffsize);
  _getBounds();
  if (strcmp(_oldtext, _text) == 0) return;
  uint16_t realth = _textheight;
#if defined(DSP_OLED) && DSP_MODEL!=DSP_SSD1322
  if(Clock_GFXfontPtr==nullptr) realth = _textheight * 8; //CHARHEIGHT
#endif
  if (_active)
  #ifndef CLOCKFONT5x7
    dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()),  _config.top-_textheight+1, max(_oldtextwidth, _textwidth), realth, _bgcolor);
  #else
    dsp.fillRect(_oldleft == 0 ? _realLeft() : min(_oldleft, _realLeft()),  _config.top, max(_oldtextwidth, _textwidth), realth, _bgcolor);
  #endif

  _oldtextwidth = _textwidth;
  _oldleft = _realLeft();
  if (_active) _draw();
}

void NumWidget::setText(int val, const char *format){
  char buf[_buffsize];
  snprintf(buf, _buffsize, format, val);
  setText(buf);
}

void NumWidget::_getBounds() {
  _textwidth= _textWidth(_text);
}

void NumWidget::_draw() {
#ifndef DSP_LCD
  if(!_active || TIME_SIZE<2) return;
  applyClockFontFromConfig();
  dsp.setTextSize(Clock_GFXfontPtr==nullptr?TIME_SIZE:1);
  dsp.setFont(Clock_GFXfontPtr);
  dsp.setTextColor(_fgcolor, _bgcolor);
#endif
  if(!_active) return;
  dsp.setCursor(_realLeft(), _config.top);
  dsp.print(_text);
  strlcpy(_oldtext, _text, _buffsize);
  dsp.setFont();
}

/**************************
      PROGRESS WIDGET
 **************************/
void ProgressWidget::_progress() {
  char buf[_width + 1];
  snprintf(buf, _width, "%*s%.*s%*s", _pg <= _barwidth ? 0 : _pg - _barwidth, "", _pg <= _barwidth ? _pg : 5, ".....", _width - _pg, "");
  _pg++; if (_pg >= _width + _barwidth) _pg = 0;
  setText(buf);
}

bool ProgressWidget::_checkDelay(int m, uint32_t &tstamp) {
  if (millis() - tstamp > m) {
    tstamp = millis();
    return true;
  } else {
    return false;
  }
}

void ProgressWidget::loop() {
  if (_checkDelay(_speed, _scrolldelay)) {
    _progress();
  }
}

/**************************
      CLOCK WIDGET
 **************************/
void ClockWidget::init(WidgetConfig wconf, uint16_t fgcolor, uint16_t bgcolor){
  Widget::init(wconf, fgcolor, bgcolor);
  _timeheight = _textHeight();
  _fullclock = TIME_SIZE>35 || DSP_MODEL==DSP_ILI9225 || DSP_MODEL==DSP_ST7789_170;
#if DSP_MODEL == DSP_ST7789 || DSP_MODEL==DSP_ILI9341
    if (config.store.vuLayout != 0) {
        _fullclock = false;
    }
#endif
  if(_fullclock) _superfont = TIME_SIZE / 17; //magick
  else if(TIME_SIZE==19 || TIME_SIZE==2) _superfont=1;
  else _superfont=0;
  _space = (5*_superfont)/2; //magick
  #ifndef HIDE_DATE
  if(_fullclock){
    _dateheight = _superfont<4?1:2;
    _clockheight = _timeheight + _space + CHARHEIGHT * _dateheight;
  } else {
    _clockheight = _timeheight;
    _dateheight = 0;
  }
  #else
    _clockheight = _timeheight;
  #endif
  applyClockFontFromConfig();

  auto& gfx = getRealDsp();
  gfx.setTextSize(Clock_GFXfontPtr==nullptr?TIME_SIZE:1);
  gfx.setFont(Clock_GFXfontPtr);
  
  _getTimeBounds();
#ifdef PSFBUFFER
  _fb = new psFrameBuffer(dsp.width(), dsp.height());
  _begin();
#endif
}

void ClockWidget::_begin(){
#ifdef PSFBUFFER
  _fb->begin(&dsp, _clockleft, _config.top-_timeheight, _clockwidth, _clockheight+1, config.theme.background);
#endif
}

bool ClockWidget::_getTime(){
  #if defined AM_PM_STYLE
  strftime(_timebuffer, sizeof(_timebuffer), "%I:%M", &network.timeinfo);
  #else
  strftime(_timebuffer, sizeof(_timebuffer), "%H:%M", &network.timeinfo);
  #endif
  bool ret = network.timeinfo.tm_sec==0 || _forceflag!=network.timeinfo.tm_year;
  _forceflag = network.timeinfo.tm_year;
  return ret;
}


uint16_t ClockWidget::_left(){
  if(_fb->ready()) return 0; else return _clockleft;
}
uint16_t ClockWidget::_top(){
  if(_fb->ready()) return _timeheight; else return _config.top;
}

void ClockWidget::_getTimeBounds() {
  _timewidth = _textWidth(_timebuffer);
  uint8_t fs = _superfont>0?_superfont:TIME_SIZE;
  uint16_t rightside = CHARWIDTH * fs * 2; // seconds
  if(_fullclock){
    rightside += _space*2+1; //2space+vline
    _clockwidth = _timewidth+rightside;
  } else {
    if(_superfont==0)
      _clockwidth = _timewidth;
    else
      _clockwidth = _timewidth + rightside;
  }
  switch(_config.align){
    case WA_LEFT: _clockleft = _config.left; break;
    case WA_RIGHT: _clockleft = dsp.width()-_clockwidth-_config.left; break;
    default:
      _clockleft = (dsp.width()/2 - _clockwidth/2)+_config.left;
      break;
  }
  char buf[4];
  strftime(buf, 4, "%H", &network.timeinfo);
  _dotsleft=_textWidth(buf);
}

#ifndef DSP_LCD

  Adafruit_GFX& ClockWidget::getRealDsp(){
  #ifdef PSFBUFFER
    if (_fb && _fb->ready()) return *_fb;
  #endif
    return dsp;
  }

void ClockWidget::_printClock(bool force){
  time_t now = time(nullptr);
  struct tm ti;
  localtime_r(&now, &ti);
  
  auto& gfx = getRealDsp();
  applyClockFontFromConfig();
  gfx.setTextSize(Clock_GFXfontPtr==nullptr?TIME_SIZE:1);
  gfx.setFont(Clock_GFXfontPtr);
  const int16_t timeLeft        = _left();
  const int16_t timeTop         = _top() - _timeheight;
  const int16_t timeWidthLive   = _textWidth(_timebuffer);
  const int16_t vlineX          = timeLeft + timeWidthLive + _space;
  const int16_t colTop    = _top() - _timeheight;
  const int16_t colBottom = _top();
  const int16_t colCenter = (colTop + colBottom) / 2;
  const int16_t secCellH  = CHARHEIGHT * _superfont;
  const int16_t secBaseY = colCenter - (secCellH / 2);
  const int16_t secLeftFull    = timeLeft + timeWidthLive + _space*2;
  
  static const GFXfont* s_lastPrintFont = nullptr;
  if (Clock_GFXfontPtr != s_lastPrintFont) {
    _getTimeBounds();
    s_lastPrintFont = Clock_GFXfontPtr;
  }

  bool clockInTitle=!config.isScreensaver && _config.top<_timeheight; //DSP_SSD1306x32
  if(force){
    _clearClock();
    _getTimeBounds();
    #ifndef DSP_OLED
    if (config.store.clockFontMono) {
      gfx.setTextColor(config.theme.clockbg, config.theme.background);
      gfx.setCursor(_left(), _top());
      gfx.print("88:88");
    }
    #endif
    if (clockInTitle) {
      gfx.setTextColor(config.theme.meta, config.theme.metabg);
    } else {
      gfx.setTextColor(config.theme.clock, config.theme.background);
    }

    gfx.setCursor(_left(), _top());

    #if defined(AM_PM_STYLE)
    if (_timebuffer[0] == '0') {
      int16_t leadX = _left();
      int16_t leadY = _top() - _timeheight + 1; 
      uint16_t leadW = _charWidth('0'); 
      uint16_t leadH = _timeheight + 2;

     gfx.fillRect(leadX, leadY, leadW, leadH, config.theme.background);

      if (clockInTitle) {
        gfx.setTextColor(config.theme.meta, config.theme.metabg);
      } else {
        gfx.setTextColor(config.theme.clock, config.theme.background);
      }

      gfx.setCursor(leadX + leadW, _top());
      gfx.print(_timebuffer + 1);
    } else {
      gfx.print(_timebuffer);
    }
    #else
    gfx.print(_timebuffer);
    #endif

    if(_fullclock){
      // lines, date & dow
      bool fullClockOnScreensaver = (!config.isScreensaver || (_fb->ready() && FULL_SCR_CLOCK));
      _linesleft = _left()+_timewidth+_space;
      if(fullClockOnScreensaver){
        int16_t hlineY  = secBaseY + secCellH;
        if (hlineY >= _top()) hlineY = _top() - 1;
    #ifdef AM_PM_STYLE
        //gfx.drawFastVLine(_linesleft, secBaseY - 5, _top() - secBaseY + 10, config.theme.div);
        //gfx.drawFastHLine(_linesleft, hlineY + _space/2 -8, CHARWIDTH * _superfont * 2 + _space +15, config.theme.div);
        gfx.setFont();
        gfx.setTextSize(2);
        if(TIME_SIZE==70) {
          gfx.setCursor(_linesleft+_space+1, hlineY + CHARHEIGHT - 2);  //másodperc alatt
        } 
        if(TIME_SIZE==52) {
          gfx.setCursor(_linesleft+_space+1, hlineY + CHARHEIGHT - 8);  //másodperc alatt
        }
        if(TIME_SIZE==35) {
          gfx.setCursor(_linesleft+_space+1, hlineY + CHARHEIGHT - 12);  //másodperc alatt
        }
        gfx.setTextColor(config.theme.dow, config.theme.background);
        char buf[3];
        strftime(buf, sizeof(buf), "%p", &network.timeinfo);
        gfx.print(buf);  // AM vagy PM kiírása
    #else
        //gfx.drawFastVLine(_linesleft, secBaseY, _top() - secBaseY , config.theme.div);
        //gfx.drawFastHLine(_linesleft, hlineY + _space/2, CHARWIDTH * _superfont * 2 + _space, config.theme.div);
    #endif
        formatDateCustom(_tmp, sizeof(_tmp), ti, config.store.dateFormat);
        #ifndef HIDE_DATE
        strlcpy(_datebuf, utf8To(_tmp, true), sizeof(_datebuf));
        uint16_t _datewidth = strlen(_datebuf) * CHARWIDTH*_dateheight;
        gfx.setTextSize(_dateheight);
        #if DSP_MODEL==DSP_GC9A01A || DSP_MODEL==DSP_GC9A01 || DSP_MODEL==DSP_GC9A01_I80
        gfx.setCursor((dsp.width()-_datewidth)/2, _top() + _space);
        #else
        gfx.setCursor(_left()+_clockwidth-_datewidth, _top() + _space);
        #endif
        gfx.setTextColor(config.theme.date, config.theme.background);
        gfx.print(_datebuf);
        #endif
      }
    }
  }
  if(_fullclock || _superfont>0){
    gfx.setFont();
    gfx.setTextSize(_superfont);
    if(!_fullclock){
      gfx.setCursor(timeLeft + timeWidthLive + _space, secBaseY);
      #ifndef CLOCKFONT5x7
      gfx.setCursor(timeLeft + timeWidthLive + _space, secBaseY);
      #else
      gfx.setCursor(timeLeft + timeWidthLive + _space, secBaseY);
      #endif
    }else{
    #ifdef AM_PM_STYLE
      gfx.setCursor(secLeftFull, secBaseY-8);
    #else
      gfx.setCursor(secLeftFull, secBaseY);
    #endif
    }
    int sec = ti.tm_sec;
    if (_lastSec >= 0) {
      int diff = sec - _lastSec;
      if (diff < 0 && diff > -3) sec = _lastSec;   // clamp
    }
    gfx.setTextColor(config.theme.seconds, config.theme.background);
    sprintf(_tmp, "%02d", sec);
    gfx.print(_tmp);
    _lastSec = sec;
  }
  applyClockFontFromConfig();
  gfx.setTextSize(Clock_GFXfontPtr==nullptr?TIME_SIZE:1);
  gfx.setFont(Clock_GFXfontPtr);
  bool even = (ti.tm_sec % 2) == 0;
#ifndef DSP_OLED
  uint16_t bg = config.store.clockFontMono ? config.theme.clockbg : config.theme.background;
  uint16_t fg = even ? config.theme.clock : bg;
  gfx.setTextColor(fg, bg);
#else
  if(clockInTitle)
    gfx.setTextColor(even ? config.theme.meta : config.theme.metabg, config.theme.metabg);
  else
    gfx.setTextColor(even ? config.theme.clock : config.theme.background, config.theme.background);
#endif

  gfx.setCursor(_left()+_dotsleft, _top());
  gfx.print(":");
  gfx.setFont();
  if(_fb->ready()) _fb->display();
}

void ClockWidget::_clearClock(){
#ifdef PSFBUFFER
  if(_fb->ready()) _fb->clear();
  else
#endif
#ifndef CLOCKFONT5x7
  dsp.fillRect(_left(), _top()-_timeheight, _clockwidth, _clockheight+1, config.theme.background);
#else
  dsp.fillRect(_left(), _top(), _clockwidth+1, _clockheight+1, config.theme.background);
#endif
}

void ClockWidget::draw(bool force){
  if(!_active) return;

  applyClockFontFromConfig();

  static const GFXfont* s_lastFontPtr = nullptr;
  if (Clock_GFXfontPtr != s_lastFontPtr) {
    _getTimeBounds(); 
    s_lastFontPtr = Clock_GFXfontPtr;
    _printClock(true);
  } else {
    _printClock(_getTime());
  }
}

void ClockWidget::_draw(){
  if(!_active) return;
  _printClock(true);
}

uint16_t ClockWidget::clockWidth() const {
  return _clockwidth;
}

uint16_t ClockWidget::clockHeight() const {
  return _clockheight;
}

uint16_t ClockWidget::dateSize() const {
#ifdef HIDE_DATE
  return 0; 
#else
  return _fullclock ? (CHARHEIGHT * _dateheight) : 0;
#endif
}

void ClockWidget::_reset(){
#ifdef PSFBUFFER
  if(_fb->ready()) {
    _fb->freeBuffer();
    _getTimeBounds();
    _begin();
  }
#endif
}

void ClockWidget::_clear(){
  _clearClock();
}
#else //#ifndef DSP_LCD

void ClockWidget::_printClock(bool force){
  strftime(_timebuffer, sizeof(_timebuffer), "%H:%M", &ti);
  if(force){
    dsp.setCursor(dsp.width()-5, 0);
    dsp.print(_timebuffer);
  }
  dsp.setCursor(dsp.width()-5+2, 0);
  dsp.print((network.timeinfo.tm_sec % 2 == 0)?":":" ");
}

void ClockWidget::_clearClock(){}

void ClockWidget::draw(bool force){
  if(!_active) return;
  _printClock(true);
}
void ClockWidget::_draw(){
  if(!_active) return;
  _printClock(true);
}
void ClockWidget::_reset(){}
void ClockWidget::_clear(){}
#endif //#ifndef DSP_LCD

/**************************
      BITRATE WIDGET
 **************************/
void BitrateWidget::init(BitrateConfig bconf, uint16_t fgcolor, uint16_t bgcolor) {
    Widget::init(bconf.widget, fgcolor, bgcolor);
    _dimension = bconf.dimension;      // ez lesz a magasság
    _bitrate   = 0;
    _format    = BF_UNKNOWN;

    _charSize(bconf.widget.textsize, _charWidth, _textheight);
    memset(_buf, 0, sizeof(_buf));
}

void BitrateWidget::setBitrate(uint16_t bitrate) {
    _bitrate = bitrate;

    // Ha nagyobb mint 20000 → kbps konverzió
    if (_bitrate > 20000)
        _bitrate /= 1000;

    _draw();
}

void BitrateWidget::setFormat(BitrateFormat format) {
    _format = format;
    _draw();
}

void BitrateWidget::_charSize(uint8_t textsize, uint8_t &width, uint16_t &height) {
#ifndef DSP_LCD
    width  = textsize * CHARWIDTH;
    height = textsize * CHARHEIGHT;
#else
    width  = 1;
    height = 1;
#endif
}

void BitrateWidget::_draw() {
    _clear();

    if (!_active || _format == BF_UNKNOWN || _bitrate == 0)
        return;

    uint16_t boxW = _dimension * 2;      // teljes szélesség
    uint16_t boxH = _dimension / 2;      // magasság

    // --- Keret és kitöltés ---
    dsp.drawRect(_config.left, _config.top, boxW, boxH, _fgcolor);
    dsp.fillRect(_config.left + _dimension, _config.top, _dimension, boxH, _fgcolor);

    dsp.setFont();
    dsp.setTextSize(_config.textsize);
    dsp.setTextColor(_fgcolor, _bgcolor);

    // --- Bitrate string ---
    if (_bitrate < 1000)
        snprintf(_buf, sizeof(_buf), "%d", _bitrate);
    else
        snprintf(_buf, sizeof(_buf), "%.1f", _bitrate / 1000.0);

    // Bitrate középre → BAL blokk
    uint16_t leftX = _config.left;
    uint16_t centerX = leftX + (_dimension / 2);
    uint16_t textW = strlen(_buf) * _charWidth;
    uint16_t textH = _textheight;

    dsp.setCursor(centerX - textW/2,
                  _config.top + boxH/2 - textH/2);
    dsp.print(_buf);

    // --- Formátum → JOBB blokk ---
    dsp.setTextColor(_bgcolor, _fgcolor);

    char fmt[4] = "";
    switch(_format){
        case BF_MP3: strcpy(fmt,"MP3"); break;
        case BF_AAC: strcpy(fmt,"AAC"); break;
        case BF_FLAC:strcpy(fmt,"FLC"); break;
        case BF_OGG: strcpy(fmt,"OGG"); break;
        case BF_WAV: strcpy(fmt,"WAV"); break;
        case BF_VOR: strcpy(fmt,"VOR"); break;
        case BF_OPU: strcpy(fmt,"OPU"); break;
        default: break;
    }

    uint16_t rightCenterX = _config.left + _dimension + (_dimension/2);
    uint16_t fmtW = strlen(fmt) * _charWidth;

    dsp.setCursor(rightCenterX - fmtW/2,
                  _config.top + boxH/2 - textH/2);
    dsp.print(fmt);
}

void BitrateWidget::_clear() {
    dsp.fillRect(_config.left, _config.top,
                 _dimension * 2, _dimension / 2,
                 _bgcolor);
}

/**************************
      PLAYLIST WIDGET
 **************************/
void PlayListWidget::init(ScrollWidget* current){
  Widget::init({0, 0, 0, WA_LEFT}, 0, 0);
  _current = current;
  #ifndef DSP_LCD
  _plItemHeight = playlistConf.widget.textsize*(CHARHEIGHT-1)+playlistConf.widget.textsize*4;
  _plTtemsCount = round((float)dsp.height()/_plItemHeight);
  if(_plTtemsCount%2==0) _plTtemsCount++;
  _plCurrentPos = _plTtemsCount/2;
  _plYStart = (dsp.height() / 2 - _plItemHeight / 2) - _plItemHeight * (_plTtemsCount - 1) / 2 + playlistConf.widget.textsize*2;
  #else
  _plTtemsCount = PLMITEMS;
  _plCurrentPos = 1;
  #endif
}

uint8_t PlayListWidget::_fillPlMenu(int from, uint8_t count) {
  int     ls      = from;
  uint8_t c       = 0;
  bool    finded  = false;
  if (config.playlistLength() == 0) {
    return 0;
  }
  File playlist = config.SDPLFS()->open(REAL_PLAYL, "r");
  File index = config.SDPLFS()->open(REAL_INDEX, "r");
  while (true) {
    if (ls < 1) {
      ls++;
      _printPLitem(c, "");
      c++;
      continue;
    }
    if (!finded) {
      index.seek((ls - 1) * 4, SeekSet);
      uint32_t pos;
      index.readBytes((char *) &pos, 4);
      finded = true;
      index.close();
      playlist.seek(pos, SeekSet);
    }
    bool pla = true;
    while (pla) {
      pla = playlist.available();
      String stationName = playlist.readStringUntil('\n');
      stationName = stationName.substring(0, stationName.indexOf('\t'));
      if(config.store.numplaylist && stationName.length()>0) stationName = String(from+c)+" "+stationName;
      _printPLitem(c, stationName.c_str());
      c++;
      if (c >= count) break;
    }
    break;
  }
  playlist.close();
  return c;
}
/**************************
      DATE WIDGET
 **************************/
void DateWidget::update() {
  if (!_active) return;

  time_t now = time(nullptr);
  struct tm ti;
  localtime_r(&now, &ti);

  char dateBuf[64];
  formatDateCustom(dateBuf, sizeof(dateBuf), ti, config.store.dateFormat);

  uint8_t charWidth =
  #ifndef DSP_LCD
      (_config.textsize * CHARWIDTH);
  #else
      1;
  #endif

  auto utf8len = [](const char* s) -> size_t {
    size_t len = 0;
    for (const unsigned char* p = (const unsigned char*)s; *p; ++p)
      if ( (*p & 0xC0) != 0x80 ) ++len;
    return len;
  };

  uint16_t desired = (uint16_t)(utf8len(dateBuf) * charWidth);
  desired += charWidth * 2; 

  uint16_t minW = charWidth * 6;
  uint16_t maxW = dsp.width() > 10 ? dsp.width() - 10 : dsp.width();
  if (desired < minW) desired = minW;
  if (desired > maxW) desired = maxW;

#if DSP_MODEL == DSP_ST7789_76
  setWindowWidth(dsp.width() / 2 + 23);
#else
  setWindowWidth(desired);
#endif

  char line[192];
  strlcpy(line, dateBuf, sizeof(line));

#ifdef NAMEDAYS_FILE
  if (config.store.showNameday) {
    char nd[160] = {0};
    if (namedays_get_str((uint8_t)ti.tm_mon + 1, (uint8_t)ti.tm_mday, nd, sizeof(nd)) && nd[0]) {
      strlcat(line, " \035 ", sizeof(line));
      strlcat(line, nd,    sizeof(line));
    }
  }
#endif

  setText(utf8To(line, false));
}

/************************
   WEATHER ICON WIDGET
 ************************/
void WeatherIconWidget::init(WidgetConfig wconf, uint16_t bgcolor){
  // fgcolor itt nem kell, az ikon egy bitmap
  Widget::init(wconf, 0, bgcolor);
  _img = nullptr;
  _iw  = 0;
  _ih  = 0;
}

void WeatherIconWidget::_clear(){
  if (!_active) return;
  uint16_t w = _width  ? _width  : (_iw ? _iw : 64);
  uint16_t h = _ih ? _ih : 64;
  dsp.fillRect(_config.left, _config.top, w, h, _bgcolor);
}

void WeatherIconWidget::_draw(){
  if(!_active) return;

  uint16_t w = _iw ? _iw : 64;
  uint16_t h = _ih ? _ih : 64;
  uint16_t areaW = _width ? _width : w;
  
  int16_t iconX = _config.left;
  int16_t iconY = _config.top;

  // ikon kirajzolás
  if (_img) dsp.drawRGBBitmap(iconX, iconY, _img, w, h);

  // ha nincs hőmérséklet → nincs további rajz
  if (_temp[0] == '\0') return;

  dsp.setFont();
  dsp.setTextSize(_config.textsize);
  dsp.setTextColor(config.theme.dow, _bgcolor);

  uint16_t txtW = strlen(_temp) * CHARWIDTH * _config.textsize;
  uint16_t txtH = CHARHEIGHT * _config.textsize;

  // 🔻 **Layout logika**
  bool vertical = (config.store.vuLayout == 0);  
  // default layout = vertical (egymás alatt)  
  // többi layout = side-by-side

  if (vertical) {
      // ===========================
      // ikon fölé szöveg (Default)
      // ===========================
      int16_t tx = _config.left + (areaW - txtW) / 2;
      int16_t ty = _config.top - (txtH-2);
      dsp.setCursor(tx, ty);
      dsp.print(_temp);
  } else {
      // ===========================
      // ikon mellé szöveg (StreamLine, BoomBox, Studio)
      // ===========================
      int16_t tx = iconX + w + 6;
      int16_t ty = iconY + (h - txtH) / 2;

      dsp.setCursor(tx, ty);
      dsp.print(_temp);
  }
}

void WeatherIconWidget::setIcon(const char* code){
  if (!code || !*code) return;

  // Alapértelmezett "eradio" fallback
  _img = eradio;
  _iw  = 62;
  _ih  = 40;

  // Azonosítás a kódrészletek alapján
  if      (strstr(code, "01d")) { _img = img_01d; _iw = 64;  _ih = 64; }
  else if (strstr(code, "01n")) { _img = img_01n; _iw = 64;  _ih = 64; }
  else if (strstr(code, "02d")) { _img = img_02d; _iw = 64;  _ih = 64; }
  else if (strstr(code, "02n")) { _img = img_02n; _iw = 64;  _ih = 64; }
  else if (strstr(code, "03d")) { _img = img_03; _iw = 64; _ih = 64; }
  else if (strstr(code, "03n")) { _img = img_03; _iw = 64; _ih = 64; }
  else if (strstr(code, "04d")) { _img = img_04d;  _iw = 64; _ih = 64; }
  else if (strstr(code, "04n")) { _img = img_04n;  _iw = 64; _ih = 64; }
  else if (strstr(code, "09d")) { _img = img_09d;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "09n")) { _img = img_09n;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "10d")) { _img = img_10d;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "10n")) { _img = img_10n;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "11d")) { _img = img_11;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "11n")) { _img = img_11;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "13d")) { _img = img_13d;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "13n")) { _img = img_13n;  _iw = 64;  _ih = 64; }
  else if (strstr(code, "50d")) { _img = img_50d;  _iw = 64; _ih = 64; }
  else if (strstr(code, "50n")) { _img = img_50n;  _iw = 64; _ih = 64; }

  if (_active && !_locked) {
    _clear();
    _draw();
  }
}

void WeatherIconWidget::setTemp(float tempC) {
#ifdef IMPERIALUNIT
  snprintf(_temp, sizeof(_temp), "%.0f\011F", tempC);
#else
  snprintf(_temp, sizeof(_temp), "%.0f\011C", tempC);
#endif

  _hasTemp = true;

  if (_active && !_locked) {
    _clear();
    _draw();
  }
}


#ifndef DSP_LCD
void PlayListWidget::drawPlaylist(uint16_t currentItem) {
  uint8_t lastPos = _fillPlMenu(currentItem - _plCurrentPos, _plTtemsCount);
  if(lastPos<_plTtemsCount){
    dsp.fillRect(0, lastPos*_plItemHeight+_plYStart, dsp.width(), dsp.height()/2, config.theme.background);
  }
}

void PlayListWidget::_printPLitem(uint8_t pos, const char* item){
  dsp.setTextSize(playlistConf.widget.textsize);
  if (pos == _plCurrentPos) {
    _current->setText(item);
  } else {
    uint8_t plColor = (abs(pos - _plCurrentPos)-1)>4?4:abs(pos - _plCurrentPos)-1;
    dsp.setTextColor(config.theme.playlist[plColor], config.theme.background);
    dsp.setCursor(TFT_FRAMEWDT, _plYStart + pos * _plItemHeight);
    dsp.fillRect(0, _plYStart + pos * _plItemHeight - 1, dsp.width(), _plItemHeight - 2, config.theme.background);
    dsp.print(utf8To(item, true));
  }
}
#else
void PlayListWidget::_printPLitem(uint8_t pos, const char* item){
  if (pos == _plCurrentPos) {
    _current->setText(item);
  } else {
    dsp.setCursor(1, pos);
    char tmp[dsp.width()] = {0};
    strlcpy(tmp, utf8To(item, true), dsp.width());
    dsp.print(tmp);
  }
}

void PlayListWidget::drawPlaylist(uint16_t currentItem) {
  dsp.clear();
  _fillPlMenu(currentItem - _plCurrentPos, _plTtemsCount);
  dsp.setCursor(0,1);
  dsp.write(uint8_t(126));
}

#endif


#endif // #if DSP_MODEL!=DSP_DUMMY
