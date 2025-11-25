/*************************************************************************************
    ST7789 284x76 displays configuration file.
    Copy this file to yoRadio/src/displays/conf/displayST7789conf_custom.h
    and modify it
    More info on https://github.com/e2002/yoradio/wiki/Widgets#widgets-description
*************************************************************************************/

#ifndef displayST7789_76conf_h
#define displayST7789_76conf_h

#include "../../core/config.h"

#define DSP_WIDTH       284
#define DSP_HEIGHT      76
#define TFT_FRAMEWDT    2
#define MAX_WIDTH       DSP_WIDTH-TFT_FRAMEWDT*2

#define bootLogoTop     8

//#define HIDE_HEAPBAR
#define HIDE_VOL
//#define HIDE_VU
//#define HIDE_TITLE2

#ifndef BATTERY_OFF
//  #define RSSI_DIGIT true
  #define BatX      113		// X coordinate for batt. (���������� X ��� ���������)
  #define BatY      51		// Y coordinate for batt. (���������� Y ��� ���������)
  #define BatFS     1		// FontSize for batt. (������ ������ ��� ���������)
  #define ProcX     137		// X coordinate for percent (���������� X ��� ��������� ������)
  #define ProcY     51		// Y coordinate for percent (���������� Y ��� ��������� ������)
  #define ProcFS    1		// FontSize for percent (������ ������ ��� ��������� ������)
  #define VoltX      125	// X coordinate for voltage (���������� X ��� ����������)
  #define VoltY      40		// Y coordinate for voltage (���������� Y ��� ����������)
  #define VoltFS     1		// FontSize for voltage (������ ������ ��� ����������)
#endif

/* SROLLS  */                            /* {{ left, top, fontsize, align }, buffsize, uppercase, width, scrolldelay, scrolldelta, scrolltime } */
const ScrollConfig metaConf     PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 2, WA_CENTER }, 140, true, MAX_WIDTH, 5000, 2, 25 };
const ScrollConfig title1Conf     PROGMEM = {{ TFT_FRAMEWDT, 20, 1, WA_LEFT }, 140, false, DSP_WIDTH/2+23, 5000, 2, 25 };
const ScrollConfig title2Conf     PROGMEM = {{ TFT_FRAMEWDT, 30, 1, WA_LEFT }, 140, false, DSP_WIDTH/2+23, 5000, 2, 25 };
const ScrollConfig playlistConf   PROGMEM = {{ TFT_FRAMEWDT, 30, 1, WA_LEFT }, 140, false, MAX_WIDTH, 500, 2, 25 };
const ScrollConfig apTitleConf   PROGMEM = {{ TFT_FRAMEWDT, TFT_FRAMEWDT, 2, WA_CENTER }, 140, false, MAX_WIDTH, 0, 2, 25 };
const ScrollConfig apSettConf   PROGMEM = {{ TFT_FRAMEWDT+10, 64, 1, WA_LEFT }, 140, false, MAX_WIDTH, 0, 2, 25 };
const ScrollConfig weatherConf PROGMEM = {{ TFT_FRAMEWDT, DSP_HEIGHT-15, 1, WA_LEFT }, 140, false, MAX_WIDTH, 0, 1, 25 }; // Idojaras  (������)

/* BACKGROUNDS  */                       /* {{ left, top, fontsize, align }, width, height, outlined } */
const FillConfig   metaBGConf     PROGMEM = {{ 0, 0,  0, WA_LEFT }, DSP_WIDTH, 18, false };
const FillConfig   metaBGConfLine PROGMEM = {{ 1, 18, 0, WA_CENTER }, DSP_WIDTH - 2, 1, true };
const FillConfig   metaBGConfInv  PROGMEM = {{ 0, 19, 0, WA_LEFT }, DSP_WIDTH, 1,  false };
const FillConfig   volbarConf     PROGMEM = {{ TFT_FRAMEWDT, DSP_HEIGHT-6, 0, WA_LEFT }, DSP_WIDTH-TFT_FRAMEWDT*2, 3, true };
const FillConfig  playlBGConf     PROGMEM = {{ 0, 26, 0, WA_LEFT }, DSP_WIDTH, 12, false };
const FillConfig  heapbarConf    PROGMEM = {{ 0, 74, 0, WA_LEFT }, DSP_WIDTH, 2, false };

/* WIDGETS  */                           /* { left, top, fontsize, align } */
const WidgetConfig bootstrConf  PROGMEM = { 0, DSP_HEIGHT-10, 1, WA_CENTER };
const WidgetConfig bitrateConf   PROGMEM = { TFT_FRAMEWDT+24, 51, 1, WA_LEFT };
//const WidgetConfig voltxtConf     PROGMEM = { TFT_FRAMEWDT, 108, 1, WA_RIGHT };
const WidgetConfig  iptxtConf     PROGMEM = { TFT_FRAMEWDT, 40, 1, WA_LEFT };
const WidgetConfig   rssiConf      PROGMEM = { TFT_FRAMEWDT, 50, 1, WA_LEFT };
const WidgetConfig numConf      PROGMEM = { TFT_FRAMEWDT, 56, 35, WA_CENTER };
const WidgetConfig apNameConf     PROGMEM = { 0, 21, 1, WA_CENTER };
const WidgetConfig apName2Conf    PROGMEM = { 0, 33, 1, WA_CENTER };
const WidgetConfig apPassConf    PROGMEM = { 0, 48, 1, WA_CENTER };
const WidgetConfig apPass2Conf  PROGMEM = { 0, 65, 1, WA_CENTER };

const WidgetConfig bootWdtConf  PROGMEM = { 0, DSP_HEIGHT-23, 1, WA_CENTER };
const ProgressConfig bootPrgConf  PROGMEM = { 90, 10, 4 };

/* BANDS  */                             /* { onebandwidth,               onebandheight, bandsHspace, bandsVspace, numofbands, fadespeed } */
//const VUBandsConfig bandsConf     PROGMEM = { DSP_WIDTH/2-TFT_FRAMEWDT-4, 7, 2, 1, 17, 2 };
inline VUBandsConfig getbandsConf() {
  switch (config.store.vuLayout) {
    case 1: return { DSP_WIDTH/2-TFT_FRAMEWDT-6, config.store.vuBarHeightStr, 10, config.store.vuBarGapStr,  config.store.vuBarCountStr,  config.store.vuFadeSpeedStr };  // Streamline
    case 2: return { DSP_WIDTH/2-TFT_FRAMEWDT-6, config.store.vuBarHeightBbx, 10, config.store.vuBarGapBbx,  config.store.vuBarCountBbx,  config.store.vuFadeSpeedBbx };  // Boombox
    case 3: return { DSP_WIDTH-TFT_FRAMEWDT*2, config.store.vuBarHeightStd, config.store.vuBarGapStd, 3, config.store.vuBarCountStd,  config.store.vuFadeSpeedStd };  // Studio
    default: return { config.store.vuBarHeightDef, 80, 6, config.store.vuBarGapDef,  config.store.vuBarCountDef,  config.store.vuFadeSpeedDef }; // Default
  }
}

//const WidgetConfig vuConf         PROGMEM = { TFT_FRAMEWDT, DSP_HEIGHT-15, 1, WA_CENTER };
inline WidgetConfig getvuConf() {
  switch (config.store.vuLayout) {
    case 1: return { TFT_FRAMEWDT, DSP_HEIGHT-15, 1, WA_CENTER };  // Streamline
    case 2: return { TFT_FRAMEWDT, DSP_HEIGHT-15, 1, WA_CENTER };  // Boombox
    case 3: return { TFT_FRAMEWDT, DSP_HEIGHT-15, 1, WA_CENTER };  // Studio
    default: return { TFT_FRAMEWDT, DSP_HEIGHT-15, 1, WA_LEFT }; // Default
  }
}

//const WidgetConfig clockConf      PROGMEM = { 0, 54, 35, WA_RIGHT};  /* A 35 egy fix betumeret. ne valtoztassa meg. */
inline WidgetConfig getclockConf() {
  switch (config.store.vuLayout) {
    case 1: return { 0, 54, 35, WA_RIGHT };  // Streamline
    case 2: return { 0, 54, 35, WA_RIGHT };  // Boombox
    case 3: return { 0, 54, 35, WA_RIGHT };  // Studio
    default: return { 0, 54, 35, WA_RIGHT }; // Default
  }
}

//const ScrollConfig dateConf       PROGMEM = {{ DSP_WIDTH-TFT_FRAMEWDT, 185, 2, WA_RIGHT }, 128, false, 220, 5000, 1, 50};
//const ScrollConfig weatherConf PROGMEM = {{ TFT_FRAMEWDT, DSP_HEIGHT-15, 1, WA_LEFT }, 140, false, MAX_WIDTH, 0, 1, 25 }; // Idojaras  (������)
static constexpr ScrollConfig kDateBase = {
  { TFT_FRAMEWDT, 40, 1, WA_LEFT }, 128, false, 128, 5000, 1, 50 };

static inline uint16_t dateTopByLayout(uint8_t ly) {
  switch (ly) {
    case 1:  return 40;  // StreamLine
    case 2:  return 40;  // BoomBox
    case 3:  return 40;  // Studio
    default: return 40;  // Default
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
const char           rssiFmt[]    PROGMEM = "%d";
const char          iptxtFmt[]    PROGMEM = "%s";
//const char         voltxtFmt[]    PROGMEM = "%d";
const char        bitrateFmt[]    PROGMEM = "%d kBs";

/* MOVES  */                             /* { left, top, width (0 - auto, -1 - lock )} */
//const MoveConfig    clockMove     PROGMEM = { 0, 0, -1 };
inline MoveConfig getclockMove() {
  switch (config.store.vuLayout) {
    case 1: return { 0, 54, -1 };  // Streamline
    case 2: return { 0, 54, -1 };  // BoomBox
    case 3: return { 0, 54, -1 };  // Studio
    default: return { 0, 54, -1 }; // Default
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

const MoveConfig   weatherMove    PROGMEM = {TFT_FRAMEWDT, DSP_HEIGHT-15, MAX_WIDTH };
const MoveConfig   weatherMoveVU  PROGMEM = {TFT_FRAMEWDT, 40, DSP_WIDTH/2+23 };

#endif
