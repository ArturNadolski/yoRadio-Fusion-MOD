#include "options.h"
#if SDC_CS!=255
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <vector>
#include <algorithm>
#include "vfs_api.h"
#include "sd_diskio.h"
#include "config.h"
#include "sdmanager.h"
#include "display.h"
#include "player.h"

#if defined(SD_SPIPINS)
  SPIClass SDSPI(VSPI);
  #define SDREALSPI SDSPI
#elif SD_HSPI
  SPIClass SDSPI(HSPI);
  #define SDREALSPI SDSPI
#else
  #define SDREALSPI SPI
#endif

#ifndef SDSPISPEED
  #define SDSPISPEED 20000000
#endif

SDManager sdman(FSImplPtr(new VFSImpl()));

// Case-insensitive comparison for sorting
bool compareAlpha(const String& a, const String& b) {
    return strcasecmp(a.c_str(), b.c_str()) < 0;
}

bool SDManager::start(){
  #if defined(SD_SPIPINS)
    SDREALSPI.begin(SD_SPIPINS, SDC_CS);
  #elif SD_HSPI
    SDREALSPI.begin();
  #endif

  ready = begin(SDC_CS, SDREALSPI, SDSPISPEED);
  vTaskDelay(10);
  if(!ready) ready = begin(SDC_CS, SDREALSPI, SDSPISPEED);
  vTaskDelay(20);
  if(!ready) ready = begin(SDC_CS, SDREALSPI, SDSPISPEED);
  vTaskDelay(50);
  if(!ready) ready = begin(SDC_CS, SDREALSPI, SDSPISPEED);
  return ready;
}

void SDManager::stop(){
  end();
  ready = false;
}

#include "diskio_impl.h"
bool SDManager::cardPresent() {
  if(!ready) return false;
  if(sectorSize()<1) return false;
  
  uint8_t buff[512] = { 0 }; // Użycie standardowego rozmiaru sektora
  bool bread = readRAW(buff, 1);
  return bread;
}

bool SDManager::_checkNoMedia(const char* path){
  char buf[256];
  if (path[strlen(path) - 1] == '/')
    snprintf(buf, sizeof(buf), "%s%s", path, ".nomedia");
  else
    snprintf(buf, sizeof(buf), "%s/%s", path, ".nomedia");
  return exists(buf);
}

// Szybkie sprawdzanie rozszerzeń bez modyfikacji stringa wejściowego
bool SDManager::isAudioFile(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot) return false;
    return (strcasecmp(dot, ".mp3") == 0 || 
            strcasecmp(dot, ".wav") == 0 || 
            strcasecmp(dot, ".flac") == 0 || 
            strcasecmp(dot, ".m4a") == 0 || 
            strcasecmp(dot, ".aac") == 0);
}

void SDManager::listSD(File &plSDfile, File &plSDindex, const char* dirname, uint8_t levels) {
    File root = sdman.open(dirname);
    if (!root) {
        Serial.println("##[ERROR]#\tFailed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("##[ERROR]#\tNot a directory");
        root.close();
        return;
    }

    std::vector<String> folderList;
    std::vector<String> fileList;

    // 1. Zbieranie nazw
    while (true) {
        bool isDir;
        String name = root.getNextFileName(&isDir);
        if (name.isEmpty()) break;

        if (isDir) {
            if (levels > 0 && !_checkNoMedia(name.c_str())) {
                folderList.push_back(name);
            }
        } else {
            if (isAudioFile(name.c_str())) {
                fileList.push_back(name);
            }
        }
        if ((folderList.size() + fileList.size()) % 50 == 0) yield();
    }
    root.close();

    // 2. Sortowanie list
    std::sort(folderList.begin(), folderList.end(), compareAlpha);
    std::sort(fileList.begin(), fileList.end(), compareAlpha);

    // 3. NAJPIERW: Rekurencyjnie wchodzimy w posortowane foldery
    // Dzięki temu zawartość podfolderów trafi do pliku wcześniej
    if (levels > 0) {
        for (const String& folderPath : folderList) {
            listSD(plSDfile, plSDindex, folderPath.c_str(), levels - 1);
        }
    }

    // 4. NA KOŃCU: Przetwarzamy pliki w bieżącym folderze
    // Jeśli to jest root ("/"), te pliki będą na samym dole playlisty
    for (const String& path : fileList) {
        const char* fullPath = path.c_str();
        const char* fn = strrchr(fullPath, '/');
        fn = (fn) ? fn + 1 : fullPath;

        uint32_t pos = plSDfile.position();
        plSDfile.printf("%s\t%s\t0\n", fn, fullPath);
        plSDindex.write((uint8_t*)&pos, 4);

        _sdFCount++;
        
        Serial.print(".");
        if (_sdFCount % 64 == 0) Serial.println();
        
        if(display.mode() == SDCHANGE) display.putRequest(SDFILEINDEX, _sdFCount);
        
        if (_sdFCount % 5 == 0) {
            player.loop();
            vTaskDelay(1);
        }
    }
}

void SDManager::indexSDPlaylist() {
  Serial.println("Starting SD Indexing (Sorted)...");
  _sdFCount = 0;
  
  if(exists(PLAYLIST_SD_PATH)) remove(PLAYLIST_SD_PATH);
  if(exists(INDEX_SD_PATH)) remove(INDEX_SD_PATH);

  File playlist = open(PLAYLIST_SD_PATH, "w", true);
  if (!playlist) {
    Serial.println("##[ERROR]#\tCould not create playlist file");
    return;
  }
  
  File index = open(INDEX_SD_PATH, "w", true);
  if (!index) {
    Serial.println("##[ERROR]#\tCould not create index file");
    playlist.close();
    return;
  }

  listSD(playlist, index, "/", SD_MAX_LEVELS);
  
  index.flush();
  index.close();
  playlist.flush();
  playlist.close();
  
  Serial.printf("\nIndexing complete. Found %d files.\n", _sdFCount);
  delay(50);
}
#endif