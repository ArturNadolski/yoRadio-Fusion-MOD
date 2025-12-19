#include "Arduino.h"
#include "../../core/options.h"
#if L10N_LANGUAGE == PT
#include "../dspcore.h"
#include "utf8To.h"

// Portuguese chars: à á â ã ç é ê í ó ô õ ú À Á Â Ã Ç É Ê Í Ó Ô Õ Ú

#ifndef DSP_LCD
char* utf8To(const char* str, bool uppercase) {
  int index = 0;
  static char strn[BUFLEN];
  strlcpy(strn, str, BUFLEN); 
 
  if (uppercase) {
    for (char *iter = strn; *iter != '\0'; ++iter)
      *iter = toupper(*iter); 
  }

  if(L10N_LANGUAGE==EN) return strn;
  
  while (strn[index]) { 
    
    // ========== BLOK 0xC3 - todos os caracteres portugueses ==========
    if (strn[index] == 0xC3) {
      switch (strn[index + 1]) {
        
        // à (UTF-8: C3 A0)
        case 0xA0: {
          if (!uppercase) {
            strn[index] = 0x85;  // à minúsculo
          } else {
            strn[index] = 0x8f;  // À maiúsculo
          }
          break;
        }
        // À (UTF-8: C3 80)
        case 0x80: {
          strn[index] = 0x8f;  // À maiúsculo
          break;
        }
        
        // á (UTF-8: C3 A1)
        case 0xA1: {
          if (!uppercase) {
            strn[index] = 0x81;  // á minúsculo
          } else {
            strn[index] = 0x84;  // Á maiúsculo
          }
          break;
        }
        // Á (UTF-8: C3 81)
        case 0x81: {
          strn[index] = 0x84;  // Á maiúsculo
          break;
        }
        
        // â (UTF-8: C3 A2)
        case 0xA2: {
          if (!uppercase) {
            strn[index] = 0x83;  // â minúsculo
          } else {
            strn[index] = 0x90;  // Â maiúsculo
          }
          break;
        }
        // Â (UTF-8: C3 82)
        case 0x82: {
          strn[index] = 0x90;  // Â maiúsculo
          break;
        }
        
        // ã (UTF-8: C3 A3)
        case 0xA3: {
          if (!uppercase) {
            strn[index] = 0x86;  // ã minúsculo
          } else {
            strn[index] = 0x91;  // Ã maiúsculo
          }
          break;
        }
        // Ã (UTF-8: C3 83)
        case 0x83: {
          strn[index] = 0x91;  // Ã maiúsculo
          break;
        }
        
        // ç (UTF-8: C3 A7)
        case 0xA7: {
          if (!uppercase) {
            strn[index] = 0x87;  // ç minúsculo
          } else {
            strn[index] = 0x80;  // Ç maiúsculo
          }
          break;
        }
        // Ç (UTF-8: C3 87)
        case 0x87: {
          strn[index] = 0x80;  // Ç maiúsculo
          break;
        }
        
        // é (UTF-8: C3 A9)
        case 0xA9: {
          if (!uppercase) {
            strn[index] = 0x82;  // é minúsculo
          } else {
            strn[index] = 0x89;  // É maiúsculo
          }
          break;
        }
        // É (UTF-8: C3 89)
        case 0x89: {
          strn[index] = 0x89;  // É maiúsculo
          break;
        }
        
        // ê (UTF-8: C3 AA)
        case 0xAA: {
          if (!uppercase) {
            strn[index] = 0x88;  // ê minúsculo
          } else {
            strn[index] = 0x92;  // Ê maiúsculo
          }
          break;
        }
        // Ê (UTF-8: C3 8A)
        case 0x8A: {
          strn[index] = 0x92;  // Ê maiúsculo
          break;
        }
        
        // í (UTF-8: C3 AD)
        case 0xAD: {
          if (!uppercase) {
            strn[index] = 0x8a;  // í minúsculo
          } else {
            strn[index] = 0x93;  // Í maiúsculo
          }
          break;
        }
        // Í (UTF-8: C3 8D)
        case 0x8D: {
          strn[index] = 0x93;  // Í maiúsculo
          break;
        }
        
        // ó (UTF-8: C3 B3)
        case 0xB3: {
          if (!uppercase) {
            strn[index] = 0x8b;  // ó minúsculo
          } else {
            strn[index] = 0x94;  // Ó maiúsculo
          }
          break;
        }
        // Ó (UTF-8: C3 93)
        case 0x93: {
          strn[index] = 0x94;  // Ó maiúsculo
          break;
        }
        
        // ô (UTF-8: C3 B4)
        case 0xB4: {
          if (!uppercase) {
            strn[index] = 0x8c;  // ô minúsculo
          } else {
            strn[index] = 0x95;  // Ô maiúsculo
          }
          break;
        }
        // Ô (UTF-8: C3 94)
        case 0x94: {
          strn[index] = 0x95;  // Ô maiúsculo
          break;
        }
        
        // õ (UTF-8: C3 B5)
        case 0xB5: {
          if (!uppercase) {
            strn[index] = 0x8d;  // õ minúsculo
          } else {
            strn[index] = 0x96;  // Õ maiúsculo
          }
          break;
        }
        // Õ (UTF-8: C3 95)
        case 0x95: {
          strn[index] = 0x96;  // Õ maiúsculo
          break;
        }
        
        // ú (UTF-8: C3 BA)
        case 0xBA: {
          if (!uppercase) {
            strn[index] = 0x8e;  // ú minúsculo
          } else {
            strn[index] = 0x97;  // Ú maiúsculo
          }
          break;
        }
        // Ú (UTF-8: C3 9A)
        case 0x9A: {
          strn[index] = 0x97;  // Ú maiúsculo
          break;
        }
        
        // Caracteres não portugueses - manter inalterados ou mapear para ASCII simples
        default: {
          // Para outros caracteres do bloco C3, usar o caractere base sem acento
          // ou deixar como está se não for reconhecido
          break;
        }
        
      } // end switch 0xC3
      
      // Remover o segundo byte UTF-8 (compactar a string)
      int sind = index + 2;
      while (strn[sind]) {
        strn[sind - 1] = strn[sind];
        sind++;
      }
      strn[sind - 1] = 0;
    }
    
    index++;
  }
  
  return strn;
}
#endif //#ifndef DSP_LCD
#endif