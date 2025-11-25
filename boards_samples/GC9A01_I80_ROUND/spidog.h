#ifndef spidog_h
#define spidog_h
#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class SPIDog {
public:
  bool begin();
  bool takeMutex();
  void giveMutex();
  bool isLocked() const;
private:
  bool _busy;
};

extern SemaphoreHandle_t spiMutex;
extern SPIDog sdog;

#endif
