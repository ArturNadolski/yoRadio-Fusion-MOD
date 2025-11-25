#include "options.h"
#if (TS_MODEL==TS_MODEL_CST816)

#include "touchscreen_cst816.h"
#include <Wire.h>
#include <Adafruit_CST8XX.h>

#ifndef TS_TCA6408_PRESENT
#define TS_TCA6408_PRESENT 0
#endif

#ifndef TS_INVERT_X
  #define TS_INVERT_X (TS_TCA6408_PRESENT ? 1 : 0)
#endif
#ifndef TS_INVERT_Y
  #define TS_INVERT_Y 0
#endif
#ifndef TS_SWAP_XY
  #define TS_SWAP_XY  0
#endif

#ifndef TCA6408_ADDR
  #define TCA6408_ADDR 0x20
#endif
#ifndef TCA6408_TS_BIT
  #define TCA6408_TS_BIT 0   // P0 = CST816 INT
#endif
#ifndef TS_I2C_ADDR
  #define TS_I2C_ADDR 0x15   // CST816 I2C 
#endif

static Adafruit_CST8XX cst;
CST816_Adapter ts;

static inline void rotateMap(uint16_t &x, uint16_t &y, uint16_t w, uint16_t h, uint8_t rot) {
  uint16_t nx=x, ny=y;
  switch (rot & 3) {
    case 0: nx = x;          ny = y;          break;
    case 1: nx = h - 1 - y;  ny = x;          break;
    case 2: nx = w - 1 - x;  ny = h - 1 - y;  break;
    case 3: nx = y;          ny = w - 1 - x;  break;
  }
  x = nx; y = ny;
}

// ========================= TCA6408 ÁG =========================
#if TS_TCA6408_PRESENT
static bool tca_read_u8(uint8_t reg, uint8_t &val) {
  Wire.beginTransmission(TCA6408_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;    // repeated start
  if (Wire.requestFrom(TCA6408_ADDR, (uint8_t)1) != 1) return false;
  val = Wire.read();
  return true;
}
static bool tca_write_u8(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(TCA6408_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static inline bool tca_ts_irq_active() {
  uint8_t in = 0xFF;
  if (!tca_read_u8(0x00, in)) return false;
  return ((in & (1 << TCA6408_TS_BIT)) == 0);
}

static volatile bool g_touch_irq = false;
#if (TS_INT >= 0)
void IRAM_ATTR _tca_isr() { g_touch_irq = true; }
#endif

#else  // ---- NINCS TCA ----
static inline bool tca_ts_irq_active() { return false; }
static volatile bool g_touch_irq = false;
#if (TS_INT >= 0)
void IRAM_ATTR _direct_ts_isr() { g_touch_irq = true; }
#endif
#endif
// ============================================================================

bool CST816_Adapter::begin(uint16_t w, uint16_t h, bool flip) {
  _w = w; _h = h; _flip = flip;

  // I2C indulás
#if defined(TS_SDA) && defined(TS_SCL)
  Wire.begin(TS_SDA, TS_SCL, 400000);   // 400 kHz
#else
  Wire.begin();
  Wire.setClock(400000);
#endif

#if TS_TCA6408_PRESENT
  // TCA6408: polarity 0, minden bemenet (különösen P0)
  tca_write_u8(0x02, 0x00); // Polarity
  tca_write_u8(0x03, 0xFF); // Config: 1=input
  uint8_t dummy; tca_read_u8(0x00, dummy); // első read: INT „clear”

  // Összesített INT (open-drain, aktív LOW) → MCU
  #if (TS_INT >= 0)
    pinMode(TS_INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(TS_INT), _tca_isr, FALLING);
  #endif
#else
  // Közvetlen CST816 INT → MCU
  #if (TS_INT >= 0)
    pinMode(TS_INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(TS_INT), _direct_ts_isr, FALLING);
  #endif
#endif

  // TS_RST a te panelodon GPIO0 → strapping pin! Ne húzzuk le, csak HIGH-on tartsuk.
#if (TS_RST >= 0)
  pinMode(TS_RST, OUTPUT);
  digitalWrite(TS_RST, HIGH);
#endif

  // CST816 init (Adafruit lib gondoskodik a power-on állapotról)
  (void)cst.begin(&Wire, TS_I2C_ADDR);

  setRotation(flip ? 2 : 0);
  _hadTouch = false;
  _last = {0,0};
  return true;
}

void CST816_Adapter::setRotation(uint8_t r) { _rot = (r & 3); }

void CST816_Adapter::read() {
  isTouched = false;

  // Csak akkor szondázz, ha tényleg kell: TCA P0 LOW vagy jött INT
  bool shouldProbe = tca_ts_irq_active();
#if (TS_INT >= 0)
  shouldProbe = shouldProbe || g_touch_irq;
#endif

  // keep-alive: ha semmi jelzés nincs, ritkán azért nézzünk rá (50 ms)
  static uint32_t last_keep = 0;
  uint32_t now = millis();
  if (!shouldProbe && (now - last_keep) < 50) return;
  if (!shouldProbe) last_keep = now;

  // 3 minta, median-of-3 + 0,0 szűrés
  const int N=3;
  uint16_t xs[N], ys[N]; uint8_t n=0;
  for (int i=0; i<N; ++i) {
    if (cst.touched()) {
      auto p = cst.getPoint(0);
      if (!(p.x==0 && p.y==0)) { xs[n]=p.x; ys[n]=p.y; ++n; }
    }
    delay(2);
  }
  if (n == 0) {
#if (TS_INT >= 0)
    // ha megszakítás miatt jöttünk, de nincs adat → „clear”
    g_touch_irq = false;
#endif
    return;
  }

  // minirendezés
  auto sortN = [](uint16_t* a, uint8_t len){
    for (uint8_t i=0;i<len;i++)
      for (uint8_t j=i+1;j<len;j++)
        if (a[j] < a[i]) { uint16_t t=a[i]; a[i]=a[j]; a[j]=t; }
  };
  sortN(xs, n); sortN(ys, n);
  uint16_t x = xs[n/2], y = ys[n/2];

  // rotáció + clamp
  rotateMap(x, y, _w, _h, _rot);
#if TS_SWAP_XY
  uint16_t tx = x; x = y; y = tx;
#endif
#if TS_INVERT_X
  x = (_w - 1) - x;
#endif
#if TS_INVERT_Y
  y = (_h - 1) - y;
#endif
  if (x >= _w) x = _w - 1;
  if (y >= _h) y = _h - 1;
  
  const int MIN_LOCK  = 3;   // csak ekkora elmozdulás felett lockoljunk
  const int AXIS_BIAS = 6;   // kevésbé agresszív
  int dx = (int)x - (int)_last.x;
  int dy = (int)y - (int)_last.y;
  if (abs(dx) > MIN_LOCK || abs(dy) > MIN_LOCK) {
    if (abs(dy) > abs(dx) + AXIS_BIAS)      x = _last.x; // vertikálisra zár
    else if (abs(dx) > abs(dy) + AXIS_BIAS) y = _last.y; // horizontálisra zár
  }

  // (deadzone)
  const uint16_t DZ = 4;
  if (_hadTouch &&
      (abs((int)x - (int)_last.x) <= DZ) &&
      (abs((int)y - (int)_last.y) <= DZ)) {

  }

  points[0].x = _last.x = x;
  points[0].y = _last.y = y;
  isTouched   = true;
  _hadTouch   = true;

#if (TS_INT >= 0)
  // INT „clear”: ha már nem aktív a TCA P0, engedd el a flaget
  if (!tca_ts_irq_active()) g_touch_irq = false;
#endif
}

bool CST816_Adapter::touched() const { return isTouched; }

#endif // TS_MODEL==TS_MODEL_CST816
