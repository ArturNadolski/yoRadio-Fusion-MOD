#pragma once
#include <Adafruit_GFX.h>

#ifdef __cplusplus
extern "C" {
#endif

inline uint8_t     clockfont_clamp_id(uint8_t id);
inline const GFXfont* clockfont_get(uint8_t id);
inline int8_t      clockfont_baseline(uint8_t id);
inline uint8_t     clockfont_advance(uint8_t id);
inline uint8_t     clockfont_count();

#ifdef __cplusplus
}
#endif
