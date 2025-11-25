/*************************************************************************************
    NV3041A 480x272 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayNV3041Aconf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displayNV3041Aconf_h
#define displayNV3041Aconf_h

#include "../../core/config.h"

#define DSP_WIDTH       480
#define DSP_HEIGHT      272
#define TFT_FRAMEWDT    10
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2

#if BITRATE_FULL
  #define TITLE_FIX 44
#else
  #define TITLE_FIX 0
#endif
#define bootLogoTop     110

#ifndef BATTERY_OFF
  #define BatX      325				// X coordinate for batt.
  #define BatY      DSP_HEIGHT-38		// Y cordinate for batt.
  #define BatFS     2			// FontSize for batt.
  #define ProcX     375				// X coordinate for percent
  #define ProcY     DSP_HEIGHT-38		// Y coordinate for percent
  #define ProcFS    2			// FontSize for percent
  #define VoltX      230				// X coordinate for voltage
  #define VoltY      DSP_HEIGHT-38		// Y coordinate for voltage
  #define VoltFS     2			// FontSize for voltage
#endif

/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf       PROGMEM = {{ TFT_FRAMEWDT, 5, 3, WA_CENTER }, 140, true, MAX_WIDTH, 5000, 7, 40 };
const ScrollConfig title1Conf     PROGMEM = {{ TFT_FRAMEWDT, 40, 2, WA_LEFT }, 140, true, MAX_WIDTH, 5000, 7, 40 };
const ScrollConfig title2Conf     PROGMEM = {{ TFT_FRAMEWDT, 61, 2, WA_LEFT }, 140, false, MAX_WIDTH, 5000, 7, 40 };
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 146, 3, WA_LEFT }, 140, true, MAX_WIDTH, 1000, 7, 40 };
const ScrollConfig apTitleConf    PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 4, WA_CENTER }, 140, false, MAX_WIDTH, 0, 7, 40 };
const ScrollConfig apSettConf     PROGMEM = {{ TFT_FRAMEWDT, 320-TFT_FRAMEWDT-16, 2, WA_LEFT }, 140, false, MAX_WIDTH, 0, 7, 40 };
const ScrollConfig weatherConf    PROGMEM = {{ 10, 84, 2, WA_LEFT }, 140, false, MAX_WIDTH+20, 0, 3, 60 };


/* BACKGROUNDS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 0, 0, 0, WA_LEFT }, DSP_WIDTH, 40, false };
const FillConfig   metaBGConfLine PROGMEM = {{ 3, 35, 0, WA_CENTER}, DSP_WIDTH - 6, 1, true};
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 40, 0, WA_LEFT }, DSP_WIDTH, 2, false };
const FillConfig   volbarConf     PROGMEM = {{ TFT_FRAMEWDT, DSP_HEIGHT-TFT_FRAMEWDT-8, 0, WA_LEFT }, MAX_WIDTH, 8, true };
const FillConfig  playlBGConf     PROGMEM = {{ 0, 138, 0, WA_LEFT }, DSP_WIDTH, 36, false };
const FillConfig  heapbarConf     PROGMEM = {{ 0, DSP_HEIGHT-2, 0, WA_LEFT }, DSP_WIDTH, 2, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf    PROGMEM = { 0, 243, 1, WA_CENTER };
const WidgetConfig bitrateConf    PROGMEM = { 6, 62, 2, WA_RIGHT };
const WidgetConfig voltxtConf     PROGMEM = { 0, DSP_HEIGHT-38, 2, WA_CENTER };
const WidgetConfig  iptxtConf     PROGMEM = { TFT_FRAMEWDT, DSP_HEIGHT-38, 2, WA_LEFT };
//const WidgetConfig   rssiConf     PROGMEM = { TFT_FRAMEWDT, DSP_HEIGHT-38-6, 3, WA_RIGHT };
const WidgetConfig   rssiConf     PROGMEM = { TFT_FRAMEWDT, DSP_HEIGHT-38, 2, WA_RIGHT };

const WidgetConfig numConf        PROGMEM = { 0, 170, 1, WA_CENTER };

const WidgetConfig apNameConf     PROGMEM = { TFT_FRAMEWDT, 88, 3, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { TFT_FRAMEWDT, 120, 3, WA_CENTER };
const WidgetConfig apPassConf     PROGMEM = { TFT_FRAMEWDT, 173, 3, WA_CENTER };
const WidgetConfig apPass2Conf    PROGMEM = { TFT_FRAMEWDT, 205, 3, WA_CENTER };


const WidgetConfig bootWdtConf    PROGMEM = { 0, 216, 1, WA_CENTER };
const ProgressConfig bootPrgConf  PROGMEM = { 90, 14, 4 };

//const BitrateConfig fullbitrateConf PROGMEM = {{DSP_WIDTH-TFT_FRAMEWDT-36, 36, 2, WA_LEFT}, 42 };
inline BitrateConfig getfullbitrateConf() {
  switch (config.store.vuLayout) {
    case 1: return {{TFT_FRAMEWDT+20, 110, 3, WA_LEFT}, 68 };  // Streamline
    case 2: return {{TFT_FRAMEWDT+20, 110, 3, WA_LEFT}, 68 };  // Boombox
    case 3: return {{TFT_FRAMEWDT+20, 110, 3, WA_LEFT}, 68 };  // Studio
    default: return {{DSP_WIDTH-TFT_FRAMEWDT-40, 150, 2, WA_LEFT}, 45 }; // Default
  }
}

/* BANDS  */                             /* { onebandwidth, onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
//const VUBandsConfig bandsConf     PROGMEM = { 40, 100, 6, 2, 8, 3 };
inline VUBandsConfig getbandsConf() {
  switch (config.store.vuLayout) {
    case 1: return { 220, config.store.vuBarHeightStr, 20, config.store.vuBarGapStr,  config.store.vuBarCountStr,  config.store.vuFadeSpeedStr };  // Streamline
    case 2: return { 220, config.store.vuBarHeightBbx, 20, config.store.vuBarGapBbx,  config.store.vuBarCountBbx,  config.store.vuFadeSpeedBbx };  // Boombox
    case 3: return { 460, config.store.vuBarHeightStd, config.store.vuBarGapStd, 3, config.store.vuBarCountStd,  config.store.vuFadeSpeedStd };  // Studio
    default: return { config.store.vuBarHeightDef, 100, 6, config.store.vuBarGapDef,  config.store.vuBarCountDef,  config.store.vuFadeSpeedDef }; // Default
  }
}

//const WidgetConfig vuConf         PROGMEM = { TFT_FRAMEWDT, 105, 1, WA_LEFT };
inline WidgetConfig getvuConf() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT, DSP_HEIGHT - 70, 1, WA_CENTER };  // Streamline
    case 2: return { TFT_FRAMEWDT, DSP_HEIGHT - 70, 1, WA_CENTER };  // Boombox
    case 3: return { TFT_FRAMEWDT, DSP_HEIGHT - 70, 1, WA_CENTER };  // Studio
    default: return { TFT_FRAMEWDT, 105, 1, WA_LEFT }; // Default
  }
}

//const WidgetConfig  clockConf     PROGMEM = { 50, 158, 70, WA_RIGHT };  /* 52 is a fixed font size. do not change */
inline WidgetConfig getclockConf() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT+10, 173, 70, WA_RIGHT };  // 10 - 173 Streamline
    case 2: return { TFT_FRAMEWDT+10, 173, 70, WA_RIGHT };  // Boombox
    case 3: return { TFT_FRAMEWDT+10, 173, 70, WA_RIGHT };  // Studio
    default: return { TFT_FRAMEWDT+45, 203, 70, WA_RIGHT }; // Default
  }
}

//const ScrollConfig dateConf       PROGMEM = {{ DSP_WIDTH-TFT_FRAMEWDT, 185, 2, WA_RIGHT }, 128, false, 220, 5000, 1, 50};
static constexpr ScrollConfig kDateBase = {
  { TFT_FRAMEWDT+45, 210, 2, WA_RIGHT }, 128, false, 304, 5000, 1, 50 };

static inline uint16_t dateTopByLayout(uint8_t ly) {
  switch (ly) {
    case 1:  return 180;  // StreamLine
    case 2:  return 180;  // BoomBox
    case 3:  return 180;  // Studio
    default: return 210;  // Default
  }
}

static inline uint16_t dateLeftByLayout(uint8_t ly) {
  switch (ly) {
    case 1:  return TFT_FRAMEWDT+10;              // StreamLine
    case 2:  return TFT_FRAMEWDT+10;              // BoomBox
    case 3:  return TFT_FRAMEWDT+10;              // Studio
    default: return TFT_FRAMEWDT+45;              // Default
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
const char         voltxtFmt[]    PROGMEM = "\023\025%d%%";
const char        bitrateFmt[]    PROGMEM = "%d kBs";

/* MOVES  */                             /* { left, top, width } */
//const MoveConfig    clockMove     PROGMEM = { 48, 194, -1 /* MAX_WIDTH */ }; // -1 disables move
inline MoveConfig getclockMove() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT+10, 173, -1 };  // Streamline
    case 2: return { TFT_FRAMEWDT+10, 173, -1 };  // Boombox
    case 3: return { TFT_FRAMEWDT+10, 173, -1 };  // Studio
    default: return { TFT_FRAMEWDT+45, 203, -1 }; // Default
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

const MoveConfig   weatherMove    PROGMEM = { TFT_FRAMEWDT, 84, MAX_WIDTH};
const MoveConfig   weatherMoveVU  PROGMEM = { TFT_FRAMEWDT, 84, MAX_WIDTH};

#endif