#pragma once
#include <Arduino.h>

struct TP_Point {
  uint16_t x = 0;
  uint16_t y = 0;
};

class CST816_Adapter {
public:
  bool begin(uint16_t w, uint16_t h, bool flip);
  void setRotation(uint8_t r);
  void read();
  bool touched() const;

public:
  bool     isTouched = false;
  TP_Point points[1];

private:
  uint16_t _w = 0, _h = 0;
  uint8_t  _rot = 0;
  bool     _flip = false;

  // belső: szűréshez
  TP_Point _last = {0,0};
  bool     _hadTouch = false;
};

extern CST816_Adapter ts;
