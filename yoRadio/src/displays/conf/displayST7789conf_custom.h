/*************************************************************************************
    ST7789 320x240 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayST7789conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displayST7789conf_h
#define displayST7789conf_h

#include "../../core/config.h" 

#define DSP_WIDTH       320
#define TFT_FRAMEWDT    8
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2
//#define PLMITEMS        11
//#define PLMITEMLENGHT   40
//#define PLMITEMHEIGHT   22

#if BITRATE_FULL
  #define TITLE_FIX 44
#else
  #define TITLE_FIX 0
#endif
#define bootLogoTop     68

/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 3, WA_CENTER }, 140, true, MAX_WIDTH, 5000, 5, 30 };
const ScrollConfig title1Conf     PROGMEM = {{ TFT_FRAMEWDT, 40, 2, WA_LEFT }, 140, true, MAX_WIDTH-TITLE_FIX, 5000, 4, 30 };
const ScrollConfig title2Conf     PROGMEM = {{ TFT_FRAMEWDT, 60, 2, WA_LEFT }, 140, false, MAX_WIDTH-TITLE_FIX, 5000, 4, 30 };
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 112, 2, WA_LEFT }, 140, false, MAX_WIDTH, 1000, 2, 30 };
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 3, WA_CENTER }, 140, false, MAX_WIDTH, 0, 4, 20 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, 240-TFT_FRAMEWDT-16, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 4, 30 };
const ScrollConfig weatherConf    PROGMEM = {{ TFT_FRAMEWDT, 80, 2, WA_LEFT }, 300, false, MAX_WIDTH, 0, 3, 30 };

/* BACKGROUNDS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH, 38, false };
const FillConfig   metaBGConfLine PROGMEM = {{ 3, 36, 0, WA_CENTER }, DSP_WIDTH - 6, 1, true };
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 38, 0, WA_LEFT }, DSP_WIDTH, 1, false };
const FillConfig   volbarConf     PROGMEM = {{ TFT_FRAMEWDT, 240-TFT_FRAMEWDT-6, 0, WA_LEFT }, MAX_WIDTH, 6, true };
const FillConfig  playlBGConf     PROGMEM = {{ 0, 107, 0, WA_LEFT }, DSP_WIDTH, 24, false };
const FillConfig  heapbarConf     PROGMEM = {{ 0, 239, 0, WA_LEFT }, DSP_WIDTH, 1, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 182, 1, WA_CENTER };
const WidgetConfig bitrateConf    PROGMEM = { 70, 191, 1, WA_LEFT };
const WidgetConfig voltxtConf     PROGMEM = { 0, 208, 2, WA_CENTER };
const WidgetConfig  iptxtConf     PROGMEM = { TFT_FRAMEWDT, 214, 1, WA_LEFT };
const WidgetConfig   rssiConf     PROGMEM = { TFT_FRAMEWDT, 208, 2, WA_RIGHT };
const WidgetConfig numConf        PROGMEM = { 0, 120+30, 0, WA_CENTER };
const WidgetConfig apNameConf     PROGMEM = { TFT_FRAMEWDT, 66, 2, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { TFT_FRAMEWDT, 90, 2, WA_CENTER };
const WidgetConfig apPassConf     PROGMEM = { TFT_FRAMEWDT, 130, 2, WA_CENTER };
const WidgetConfig apPass2Conf    PROGMEM = { TFT_FRAMEWDT, 154, 2, WA_CENTER };
const WidgetConfig bootWdtConf    PROGMEM = { 0, 162, 1, WA_CENTER };

const ProgressConfig bootPrgConf  PROGMEM = { 90, 14, 4 };

//const BitrateConfig fullbitrateConf PROGMEM = {{DSP_WIDTH-TFT_FRAMEWDT-34, 43, 2, WA_LEFT}, 42 };
inline BitrateConfig getfullbitrateConf() {
  switch (config.store.vuLayout) {
    case 1: return {{TFT_FRAMEWDT, 107, 2, WA_LEFT}, 54 };  // Streamline
    case 2: return {{TFT_FRAMEWDT, 107, 2, WA_LEFT}, 54 };  // Boombox
    case 3: return {{TFT_FRAMEWDT, 107, 2, WA_LEFT}, 54 };  // Studio
    default: return {{DSP_WIDTH-TFT_FRAMEWDT-44, 38, 2, WA_LEFT}, 40 }; // Default
  }
}

/* BANDS  */                             /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
//const VUBandsConfig bandsConf     PROGMEM = { 24, 100, 4, 2, 10, 2 };
inline VUBandsConfig getbandsConf() {
  switch (config.store.vuLayout) {
    case 1: return { 142, config.store.vuBarHeightStr, 15, config.store.vuBarGapStr,  config.store.vuBarCountStr,  config.store.vuFadeSpeedStr };  // Streamline
    case 2: return { 142, config.store.vuBarHeightBbx, 15, config.store.vuBarGapBbx,  config.store.vuBarCountBbx,  config.store.vuFadeSpeedBbx };  // Boombox
    case 3: return { 312, config.store.vuBarHeightStd, config.store.vuBarGapStd, 3, config.store.vuBarCountStd,  config.store.vuFadeSpeedStd };  // Studio
    default: return { config.store.vuBarHeightDef, 80, 6, config.store.vuBarGapDef,  config.store.vuBarCountDef,  config.store.vuFadeSpeedDef }; // Default
  }
}

//const WidgetConfig vuConf         PROGMEM = { TFT_FRAMEWDT, 100, 1, WA_LEFT };
inline WidgetConfig getvuConf() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT, 185, 1, WA_CENTER };  // Streamline
    case 2: return { TFT_FRAMEWDT, 185, 1, WA_CENTER };  // Boombox
    case 3: return { TFT_FRAMEWDT, 185, 1, WA_CENTER };  // Studio
    default: return { TFT_FRAMEWDT, 100, 1, WA_LEFT }; // Default
  }
}

//const WidgetConfig  clockConf     PROGMEM = { 8, 176, 0, WA_RIGHT };
inline WidgetConfig getclockConf() {
  switch (config.store.vuLayout) {
    case 1: return { 8, 157, 52, WA_RIGHT };  // Streamline
    case 2: return { 8, 157, 52, WA_RIGHT };  // Boombox
    case 3: return { 8, 157, 52, WA_RIGHT };  // Studio
    default: return { 8, 176, 52, WA_RIGHT }; // Default
  }
}

//const ScrollConfig dateConf       PROGMEM = {{ DSP_WIDTH-TFT_FRAMEWDT, 185, 2, WA_RIGHT }, 128, false, 220, 5000, 1, 50};
static constexpr ScrollConfig kDateBase = {
  { TFT_FRAMEWDT, 183, 2, WA_RIGHT }, 128, false, 304, 5000, 1, 50 };

static inline uint16_t dateTopByLayout(uint8_t ly) {
  switch (ly) {
    case 1:  return 164;  // StreamLine
    case 2:  return 164;  // BoomBox
    case 3:  return 164;  // Studio
    default: return 183;  // Default
  }
}

static inline uint16_t dateLeftByLayout(uint8_t ly) {
  switch (ly) {
    case 1:  return  TFT_FRAMEWDT;              // StreamLine
    case 2:  return  TFT_FRAMEWDT;              // BoomBox
    case 3:  return  TFT_FRAMEWDT;              // Studio
    default: return  TFT_FRAMEWDT;              // Default
  }
}

inline ScrollConfig getDateConf(uint8_t ly) {
  ScrollConfig c = kDateBase;
  c.widget.top   = dateTopByLayout(ly);
  return c;
}

inline ScrollConfig getDateConf() {
  return getDateConf(config.store.vuLayout);
}

/* STRINGS  */
const char         numtxtFmt[]    PROGMEM = "%d";
const char           rssiFmt[]    PROGMEM = "WiFi %d";
const char          iptxtFmt[]    PROGMEM = "\010 %s";
const char         voltxtFmt[]    PROGMEM = "\023\025%d%%";
const char        bitrateFmt[]    PROGMEM = "%d kBs";

/* MOVES  */                             /* { left, top, width } */
//const MoveConfig    clockMove     PROGMEM = { 8, 180, -1 };
inline MoveConfig getclockMove() {
  switch (config.store.vuLayout) {
    case 1: return { 8, 157, -1 };  // Streamline
    case 2: return { 8, 157, -1 };  // BoomBox
    case 3: return { 8, 157, -1 };  // Studio
    default: return { 8, 176, -1 }; // Default
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

const MoveConfig   weatherMove    PROGMEM = { 8, 80, MAX_WIDTH };
const MoveConfig   weatherMoveVU  PROGMEM = { 8, 80, MAX_WIDTH-8+TFT_FRAMEWDT };

#endif
