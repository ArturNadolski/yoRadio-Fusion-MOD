#ifndef sdmanager_h
#define sdmanager_h

#include <vector> // Wymagane dla mechanizmu sortowania

class SDManager : public SDFS {
  public:
    bool ready;
  public:
    SDManager(FSImplPtr impl) : SDFS(impl), _sdFCount(0), ready(false) {}
    bool start();
    void stop();
    bool cardPresent();
    void listSD(File &plSDfile, File &plSDindex, const char * dirname, uint8_t levels);
    void indexSDPlaylist();

  private:
    uint32_t _sdFCount;
    // Sprawdza, czy plik jest obsługiwanym formatem audio
    bool isAudioFile(const char* filename);
    // Sprawdza obecność pliku .nomedia w folderze
    bool _checkNoMedia(const char* path);
    // Usunięto _endsWith, ponieważ isAudioFile jest wydajniejsze
};

extern SDManager sdman;

#if defined(SD_SPIPINS) || SD_HSPI
extern SPIClass SDSPI;
#endif

#endif