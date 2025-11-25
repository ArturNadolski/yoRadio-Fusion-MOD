/*************************************************************************************
    ILI9488 480X320 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayILI9488conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displayILI9488conf_h
#define displayILI9488conf_h

#include "../../core/config.h"

#define DSP_WIDTH       480
#define DSP_HEIGHT      320
#define TFT_FRAMEWDT    10
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2

#if BITRATE_FULL
  #define TITLE_FIX 44
#else
  #define TITLE_FIX 0
#endif
#define bootLogoTop     110

/* SROLLS  */                       /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf   PROGMEM = {{ TFT_FRAMEWDT, 5, 4, WA_CENTER }, 140, false, MAX_WIDTH+15, 5000, 7, 40 };
const ScrollConfig title1Conf   PROGMEM = {{ TFT_FRAMEWDT, 50, 3, WA_LEFT }, 140, false, MAX_WIDTH-TITLE_FIX+8, 5000, 7, 40 };
const ScrollConfig title2Conf    PROGMEM = {{ TFT_FRAMEWDT, 74, 3, WA_LEFT }, 140, false, MAX_WIDTH-TITLE_FIX+8, 5000, 7, 40 };
const ScrollConfig playlistConf  PROGMEM = {{ TFT_FRAMEWDT, 146, 3, WA_LEFT }, 140, false, MAX_WIDTH, 1000, 7, 40 };
const ScrollConfig apTitleConf  PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 4, WA_CENTER }, 140, false, MAX_WIDTH, 0, 7, 40 };
const ScrollConfig apSettConf  PROGMEM = {{ TFT_FRAMEWDT, 320-TFT_FRAMEWDT-16, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 7, 40 };
const ScrollConfig weatherConf    PROGMEM = {{ TFT_FRAMEWDT, 99, 3, WA_LEFT }, 250, false, MAX_WIDTH, 0, 3, 60 };  // Weather (погода)

/* BACKGROUNDS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH, 50, false };
const FillConfig   metaBGConfLine PROGMEM = {{3, 45, 0, WA_CENTER}, DSP_WIDTH - 6, 1, true};
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 50, 0, WA_LEFT }, DSP_WIDTH, 2, false };
const FillConfig   volbarConf     PROGMEM = {{ 0, DSP_HEIGHT-19, 0, WA_LEFT }, DSP_WIDTH, 8, true };
const FillConfig  playlBGConf    PROGMEM = {{ 0, 138, 0, WA_LEFT }, DSP_WIDTH, 36, false };
const FillConfig  heapbarConf   PROGMEM = {{ 0, DSP_HEIGHT-7, 0, WA_LEFT }, DSP_WIDTH, 6, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 235, 2, WA_CENTER };
const WidgetConfig bitrateConf    PROGMEM = { 0, 63, 2, WA_RIGHT };
const WidgetConfig voltxtConf     PROGMEM = { 0, DSP_HEIGHT-38, 2, WA_CENTER };
const WidgetConfig  iptxtConf     PROGMEM = { TFT_FRAMEWDT, DSP_HEIGHT-38, 2, WA_LEFT };
const WidgetConfig   rssiConf     PROGMEM = { TFT_FRAMEWDT-5, DSP_HEIGHT-38-6, 3, WA_RIGHT };
const WidgetConfig numConf        PROGMEM = { 0, 200, 70, WA_CENTER };
const WidgetConfig apNameConf     PROGMEM = { TFT_FRAMEWDT, 88, 3, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { TFT_FRAMEWDT, 120, 3, WA_CENTER };
const WidgetConfig apPassConf     PROGMEM = { TFT_FRAMEWDT, 173, 3, WA_CENTER };
const WidgetConfig apPass2Conf    PROGMEM = { TFT_FRAMEWDT, 205, 3, WA_CENTER };

const WidgetConfig bootWdtConf    PROGMEM = { 0, 216, 2, WA_CENTER };
const ProgressConfig bootPrgConf  PROGMEM = { 90, 14, 4 };

//const BitrateConfig fullbitrateConf PROGMEM = {{DSP_WIDTH-TITLE_FIX-2, 66, 2, WA_LEFT}, TITLE_FIX+2 };
inline BitrateConfig getfullbitrateConf() {
  switch (config.store.vuLayout) {
    case 1: return {{TFT_FRAMEWDT+15, 135, 3, WA_LEFT}, 70 };  // Streamline
    case 2: return {{TFT_FRAMEWDT+15, 135, 3, WA_LEFT}, 70 };  // Boombox
    case 3: return {{TFT_FRAMEWDT+15, 135, 3, WA_LEFT}, 70 };  // Studio
    default: return {{DSP_WIDTH-TFT_FRAMEWDT-36, 50, 2, WA_LEFT}, 42 }; // Default
  }
}

/* BANDS  */                             /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
//const VUBandsConfig bandsConf     PROGMEM = { (MAX_WIDTH-TFT_FRAMEWDT*3)/2, 20, TFT_FRAMEWDT, 0, 10, 5 };	//Boombox
inline VUBandsConfig getbandsConf() {
  switch (config.store.vuLayout) {
    case 1: return { 220, config.store.vuBarHeightStr, 20, config.store.vuBarGapStr,  config.store.vuBarCountStr,  config.store.vuFadeSpeedStr };  // Streamline
    case 2: return { 220, config.store.vuBarHeightBbx, 20, config.store.vuBarGapBbx,  config.store.vuBarCountBbx,  config.store.vuFadeSpeedBbx };  // Boombox
    case 3: return { 460, config.store.vuBarHeightStd, config.store.vuBarGapStd, 3, config.store.vuBarCountStd,  config.store.vuFadeSpeedStd };  // Studio
    default: return { config.store.vuBarHeightDef, 120, 4, config.store.vuBarGapDef,  config.store.vuBarCountDef,  config.store.vuFadeSpeedDef }; // Default
  }
}

//const WidgetConfig vuConf         PROGMEM = { TFT_FRAMEWDT, 137, 1, WA_LEFT };
inline WidgetConfig getvuConf() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT, DSP_HEIGHT - 80, 1, WA_CENTER };  // Streamline
    case 2: return { TFT_FRAMEWDT, DSP_HEIGHT - 80, 1, WA_CENTER };  // Boombox
    case 3: return { TFT_FRAMEWDT, DSP_HEIGHT - 80, 1, WA_CENTER };  // Studio
    default: return { TFT_FRAMEWDT, 120, 1, WA_LEFT }; // Default
  }
}

//const WidgetConfig  clockConf     PROGMEM = { 50, 158, 70, WA_RIGHT };  /* 52 is a fixed font size. do not change */
inline WidgetConfig getclockConf() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT, 205, 70, WA_RIGHT };  // Streamline
    case 2: return { TFT_FRAMEWDT, 205, 70, WA_RIGHT };  // Boombox
    case 3: return { TFT_FRAMEWDT, 205, 70, WA_RIGHT };  // Studio
    default: return { TFT_FRAMEWDT, 224, 70, WA_RIGHT }; // Default
  }
}

//const ScrollConfig dateConf       PROGMEM = {{ DSP_WIDTH-TFT_FRAMEWDT, 185, 2, WA_RIGHT }, 128, false, 220, 5000, 1, 50};
static constexpr ScrollConfig kDateBase = {
  { TFT_FRAMEWDT+4, 234, 2, WA_RIGHT }, 128, false, 304, 5000, 1, 50 };

static inline uint16_t dateTopByLayout(uint8_t ly) {
  switch (ly) {
    case 1:  return 215;  // StreamLine
    case 2:  return 215;  // BoomBox
    case 3:  return 215;  // Studio
    default: return 234;  // Default
  }
}

static inline uint16_t dateLeftByLayout(uint8_t ly) {
  switch (ly) {
    case 1:  return  TFT_FRAMEWDT+4;              // StreamLine
    case 2:  return  TFT_FRAMEWDT+4;              // BoomBox
    case 3:  return  TFT_FRAMEWDT+4;              // Studio
    default: return  TFT_FRAMEWDT+4;              // Default
  }
}

inline ScrollConfig getDateConf(uint8_t ly) {
  ScrollConfig c = kDateBase;
  c.widget.top   = dateTopByLayout(ly);
  c.widget.left  = dateLeftByLayout(ly);
  return c;
}

inline ScrollConfig getDateConf() {
  return getDateConf(config.store.vuLayout);
}

/* STRINGS  */
const char         numtxtFmt[]    PROGMEM = "%d";
const char           rssiFmt[]    PROGMEM = "WiFi %d";
const char          iptxtFmt[]    PROGMEM = "\010%s";
const char         voltxtFmt[]    PROGMEM = "\023\025%d";
const char        bitrateFmt[]    PROGMEM = "%d kBs";

/* MOVES  */                             /* { left, top, width (0 - auto, -1 - lock )} */
//const MoveConfig    clockMove     PROGMEM = { TFT_FRAMEWDT+30, 237, 0 };
//const MoveConfig    clockMove     PROGMEM = { 48, 194, -1 /* MAX_WIDTH */ }; // -1 disables move
inline MoveConfig getclockMove() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT, 205, -1 };  // Streamline
    case 2: return { TFT_FRAMEWDT, 205, -1 };  // BoomBox
    case 3: return { TFT_FRAMEWDT, 205, -1 };  // Studio
    default: return { TFT_FRAMEWDT, 224, -1 }; // Default
  }
}

inline MoveConfig getdateMove() {
  const ScrollConfig& dc = getDateConf();
  const auto cc = getclockConf(); 
  const auto cm = getclockMove();
  const int16_t dy = (int16_t)cm.y - (int16_t)cc.top;
  const int16_t dx = (int16_t)cm.x - (int16_t)cc.left;

  MoveConfig m;
  m.x     = (uint16_t)((int16_t)dc.widget.left + dx);
  m.y     = (uint16_t)((int16_t)dc.widget.top + dy);
  m.width = dc.width;
  return m;
}

const MoveConfig   weatherMove    PROGMEM = { TFT_FRAMEWDT, 100, MAX_WIDTH };
const MoveConfig   weatherMoveVU  PROGMEM = { TFT_FRAMEWDT, 100, MAX_WIDTH };

#endif