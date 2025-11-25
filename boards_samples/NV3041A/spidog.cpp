#include "spidog.h"

SemaphoreHandle_t spiMutex = nullptr;
SPIDog sdog;

bool SPIDog::begin() {
  if (spiMutex == nullptr)
    spiMutex = xSemaphoreCreateMutex();
  return spiMutex != nullptr;
}

bool SPIDog::takeMutex() {
  if (spiMutex == nullptr) return false;
  if (xSemaphoreGetMutexHolder(spiMutex) == xTaskGetCurrentTaskHandle())
    return true; 
  if (xSemaphoreTake(spiMutex, portMAX_DELAY) != pdPASS) return false;
  _busy = true;
  return true;
}

void SPIDog::giveMutex() {
  if (spiMutex && xSemaphoreGetMutexHolder(spiMutex) == xTaskGetCurrentTaskHandle()) {
    xSemaphoreGive(spiMutex);
  }
  _busy = false;
}

bool SPIDog::isLocked() const {
  if (spiMutex == nullptr) return false;
  return xSemaphoreGetMutexHolder(spiMutex) != nullptr;
}


