#ifndef CLOCK_TTS_H
#define CLOCK_TTS_H
#pragma once
#include <Arduino.h>

#ifndef CLOCK_TTS_LANGUAGE
#define CLOCK_TTS_LANGUAGE "en"
#endif

extern char clock_tts_language[32];
extern TaskHandle_t clock_tts_task;
extern bool clock_tts_enabled;
extern int  clock_tts_interval;

void clock_tts_setup();
void clock_tts_task_func(void*);

inline void clock_tts_enable(bool en) { clock_tts_enabled = en; }
inline void clock_tts_set_interval(int iv) { clock_tts_interval = iv; }
inline void clock_tts_loop() {} 

#endif
