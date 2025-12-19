#ifndef dsp_full_loc
#define dsp_full_loc
#include <pgmspace.h>
#include "../myoptions.h"
/*************************************************************************************
    HOWTO:
    Copy this file to yoRadio/locale/displayL10n_custom.h
    and modify it
*************************************************************************************/
const char mon[] PROGMEM = "seg";
const char tue[] PROGMEM = "ter";
const char wed[] PROGMEM = "qua";
const char thu[] PROGMEM = "qui";
const char fri[] PROGMEM = "sex";
const char sat[] PROGMEM = "sáb";
const char sun[] PROGMEM = "dom";

const char monf[] PROGMEM = "segunda-feira";
const char tuef[] PROGMEM = "terça-feira";
const char wedf[] PROGMEM = "quarta-feira";
const char thuf[] PROGMEM = "quinta-feira";
const char frif[] PROGMEM = "sexta-feira";
const char satf[] PROGMEM = "sábado";
const char sunf[] PROGMEM = "domingo";
const char jan[] PROGMEM = "janeiro";
const char feb[] PROGMEM = "fevereiro";
const char mar[] PROGMEM = "março";
const char apr[] PROGMEM = "abril";
const char may[] PROGMEM = "maio";
const char jun[] PROGMEM = "junho";
const char jul[] PROGMEM = "julho";
const char aug[] PROGMEM = "agosto";
const char sep[] PROGMEM = "setembro";
const char octt[] PROGMEM = "outubro";
const char nov[] PROGMEM = "novembro";
const char decc[] PROGMEM = "dezembro";
/*
const char jan[] PROGMEM = "01";
const char feb[] PROGMEM = "02";
const char mar[] PROGMEM = "03";
const char apr[] PROGMEM = "04";
const char may[] PROGMEM = "05";
const char jun[] PROGMEM = "06";
const char jul[] PROGMEM = "07";
const char aug[] PROGMEM = "08";
const char sep[] PROGMEM = "09";
const char octt[] PROGMEM = "10";
const char nov[] PROGMEM = "11";
const char decc[] PROGMEM = "12";
*/

const char wn_N[]      PROGMEM = "NORTE"; //norte 
const char wn_NNE[]    PROGMEM = "NNE"; //nor-nordeste
const char wn_NE[]     PROGMEM = "NE"; //nordeste
const char wn_ENE[]    PROGMEM = "ENE"; //és-nordeste
const char wn_E[]      PROGMEM = "ESTE"; //este 
const char wn_ESE[]    PROGMEM = "ESE"; //és-sudeste 
const char wn_SE[]     PROGMEM = "SE"; //sudeste 
const char wn_SSE[]    PROGMEM = "SSE"; //sul-sudeste 
const char wn_S[]      PROGMEM = "SUL"; //sul 
const char wn_SSW[]    PROGMEM = "SSW"; //sul-sudoeste 
const char wn_SW[]     PROGMEM = "SW"; //sudoeste 
const char wn_WSW[]    PROGMEM = "WSW"; //oés-sudoeste 
const char wn_W[]      PROGMEM = "OESTE"; //oeste 
const char wn_WNW[]    PROGMEM = "WNW"; //oés-noroeste  
const char wn_NW[]     PROGMEM = "NW"; //noroeste 
const char wn_NNW[]    PROGMEM = "NNW"; //nor-noroeste 

const char* const dow[]     PROGMEM = { sun, mon, tue, wed, thu, fri, sat };
const char* const dowf[]    PROGMEM = { sunf, monf, tuef, wedf, thuf, frif, satf };
const char* const mnths[]   PROGMEM = { jan, feb, mar, apr, may, jun, jul, aug, sep, octt, nov, decc };
const char* const wind[]    PROGMEM = { wn_N, wn_NNE, wn_NE, wn_ENE, wn_E, wn_ESE, wn_SE, wn_SSE, wn_S, wn_SSW, wn_SW, wn_WSW, wn_W, wn_WNW, wn_NW, wn_NNW, wn_N };

const char    const_PlReady[]    PROGMEM = "[pronto]";
const char  const_PlStopped[]    PROGMEM = "[parado]";
const char  const_PlConnect[]    PROGMEM = "[conectando]";
const char  const_DlgVolume[]    PROGMEM = "VOLUME";
const char    const_DlgLost[]    PROGMEM = "* PERDIDO *";
const char  const_DlgUpdate[]    PROGMEM = "* ACTUALIZANDO *";
const char const_DlgNextion[]    PROGMEM = "* NEXTION *";
const char const_getWeather[]    PROGMEM = "";
const char  const_waitForSD[]    PROGMEM = "INDEX SD";
const char  const_DlgSleeping[]    PROGMEM = "SUSPENDER";

const char        apNameTxt[]    PROGMEM = "NOME AP";
const char        apPassTxt[]    PROGMEM = "SENHA";
const char       bootstrFmt[]    PROGMEM = "Conectando %s";
const char        apSettFmt[]    PROGMEM = "PÁGINA DE CONFIGURAÇÕES EM: HTTP://%s/";
#if EXT_WEATHER
 #ifdef IMPERIALUNIT
 const char       weatherFmt[]    PROGMEM = "%s, %.1f\011F \007 sensação térmica: %.1f\011F \007 pressão: %d inHg \007 humidade: %d%% \007 vento: %.1f mph [%s]";
 #else
 const char       weatherFmt[]    PROGMEM = "%s, %.1f\011C \007 sensação térmica: %.1f\011C \007 pressão: %d hPa \007 humidade: %d%% \007 vento: %.1f km/h [%s]";
 #endif
#else
 #ifdef IMPERIALUNIT
 const char       weatherFmt[]    PROGMEM = "%s, %.1f\011F \007 pressão: %d inHg \007 humidade: %d%%";
 #else
 const char       weatherFmt[]    PROGMEM = "%s, %.1f\011C \007 pressão: %d hPa \007 humidade: %d%%";
 #endif
#endif
#ifdef IMPERIALUNIT
const char     weatherUnits[]    PROGMEM = "imperial";   /* standard, metric, imperial */
#else
const char     weatherUnits[]    PROGMEM = "metric";   /* standard, metric, imperial */
#endif
const char      weatherLang[]    PROGMEM = "pt";       /* https://openweathermap.org/current#multi */

#endif
