/* https://trip5.github.io/ehRadio_myoptions/generator.html
   https://github.com/e2002/yoradio
   Használat előtt olvasd el!!! - Read the before use !!!
   https://github.com/VaraiTamas/yoRadio/blob/main/README.md !!! 
*/

#ifndef myoptions_h
#define myoptions_h

#ifndef ARDUINO_ESP32S3_DEV
  #define ARDUINO_ESP32S3_DEV
#endif

#define USE_BUILTIN_LED false // Disable builtin LED

#define L10N_LANGUAGE PL // Language for UI & localisation: HU, PL, GR, EN, RU, NL, SK, UA, IT, EL, DE, PT

#define NAMEDAYS_FILE PL // Nameday language: HU, PL, NL, UA, GR, DE

#define CLOCK_TTS_LANGUAGE "pl" // Text-to-Speech language for the clock: HU, PL, GR, EN, RU, NL, SK, UA, IT, EL, DE, PT, FR, RO

#define DIRECT_CHANNEL_CHANGE // Enables direct numeric channel input via IR remote

//#define AM_PM_STYLE // Enable 12-hour clock format (AM/PM)

//#define MetaStationNameSkip // Enable to display the station name from the playlist instead of the name coming from META data

//#define USDATE // Enable MM/DD/YYYY date format (US style)

//#define IMPERIALUNIT // Enable the use of imperial units for icon and text widgets


/* LCD */
#define DSP_MODEL DSP_ILI9488
#define TFT_DC         9
#define TFT_CS         10
#define TFT_RST        -1
#define TFT_SCLK       12
#define TFT_MOSI       11
#define BRIGHTNESS_PIN 21

/* Touch */
//#define TS_MODEL TS_MODEL_XPT2046
//#define TS_CS    3

// #define NEXTION_RX			15
// #define NEXTION_TX			16

/* PCM5102A  DAC */
#define I2S_DOUT 16
#define I2S_BCLK 17
#define I2S_LRC  15

#define PLAYER_FORCE_MONO false             /*  mono option on boot - false stereo, true mono  */
#define I2S_INTERNAL      false             /*  If true - use esp32 internal DAC  */

#define MUTE_PIN      2
#define MUTE_VAL      LOW

#define VOLUME_PAGE false

/* ENCODER 1 */
#define ENC_BTNL              4       //  1st encoder left rotation (S1, DT)
#define ENC_BTNR              5       //  1st encoder right rotation (S2, CLK)
#define ENC_BTNB              6       //  1st encoder button (Key, SW)
#define ENC_INTERNALPULLUP	false

/* ENCODER 2 */
/*
#define ENC2_BTNR 7  // S2
#define ENC2_BTNL 18  // S1
#define ENC2_BTNB 3  // KEY
#define ENC2_INTERNALPULLUP	false
*/

/* CLOCK MODUL RTC DS3132 */
// #define RTC_SCL			     7
// #define RTC_SDA			     8
// #define RTC_MODULE DS3231

/* REMOTE CONTROL INFRARED RECEIVER */
#define IR_PIN 47
#define IR_TIMEOUT 35

#define WAKE_PIN    6

#define NAME_STRIM                /* Show station name from stream. (MOD Maleksm)  */

/* DS CARD */
//#define SDSPISPEED      4000000     /* 4MHz - Slower speed to prevent display flicker on shared SPI bus */
#define SDC_CS 38

//#define SD_AUTOPLAY      true

/* Az inaktív szegmens megjelenítése az óra számaiban true -> engedélyez, false -> nem engedélyez. 
   Inactive segments of the clock, true or false. */
//#define CLOCKFONT_MONO true

/* Bekapcsolja az eredeti hétszegmenses óra betűtípust.
   Turn on the original seven-segment font. */
//#define CLOCKFONT VT_DIGI_OLD

#define MUTE_PIN      2
#define MUTE_VAL      LOW

/* Google TTS hanggal mondja be az ídőt megadott nyelven és megadott percenként.
   Speaks the time using Google TTS voice in the specified language and every specified minute.
*/
#define CLOCK_TTS_ENABLED          true  // Enabled (true) or disabled (false)
#define CLOCK_TTS_LANGUAGE         "PL"  // Language ( EN, HU, PL, NL, DE, RU, RO ,FR, GR)
#define CLOCK_TTS_INTERVAL_MINUTES 30     // Hány percenként mondja be. - How many times a minute does it say.

/* Ezzel a beállítással nincs görgetés az időjárás sávon.
   With this setting there is no scrolling on the weather bar.
*/
//#define WEATHER_FMT_SHORT

/* A VU méter két fajta kijelzési módot támogat.
BOOMBOX_STYLE stílusa, amikor középről két oldalra leng ki a kijelző. Azt itt tudod beállítani.
Ha a sor elején ott van // jel akkor az alap VU méter működik ami balról jobbra leng ki.
 The VU meter supports two types of display modes.
BOOMBOX_STYLE is the style when the display swings out from the center to two sides. You can set it here.
If there is a // sign at the beginning of the line, the basic VU meter is working, swinging out from left to right. 
*/
#define BOOMBOX_STYLE

/* A VU méter végén megjelenik egy fehér csík a csúcsértékeknél, ha ezt bekapcsolod. A sor elején a // jel letíltja.
 A white bar will appear at the end of the VU meter at the peak values ​​if you enable this. The // at the beginning of the line will disable it.
*/

#define RSSI_DIGIT           false

#define VU_PEAK

/* Az állomások listájából való választásnál nem kell megnyomni a rotary encoder gombját, kilépéskor autómatikusan
átvált a csatorna. (Zsigmond Becskeházi által)
 When selecting from the station list, you do not need to press the rotary encoder button, the channel will automatically
change when you exit. (By Zsigmond Becskehazi) */
#define DIRECT_CHANNEL_CHANGE

/* Mennyi idő múlva lépjen vissza a főképernyőre az állomások listájából. (másodperc)
 How long to return to the main screen from the station list. (seconds) */
#define STATIONS_LIST_RETURN_TIME 2

#define I2S_INTERNAL    false    /*  If true - use esp32 internal DAC. "false" */
#define HIDE_VOLPAGE             /* Hangerő elrejtés, navigálj a hangerő folyamatjelző sávjával.*/

#endif
