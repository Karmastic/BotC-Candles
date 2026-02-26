#include <EEPROM.h>

#include "config.h"
#include "SavedConfig.h"

#define EEPROM_SIZE sizeof(SavedConfig)
#define EEPROM_SIGNATURE 0xDEADBEAD

const char *SavedConfig::RootCandleURL = APIRootCandleURL;
const char *SavedConfig::Host = APIHost;
uint32_t SavedConfig::Port = APIPort;

bool SavedConfig::LoadConfig(IDebugStream& debug) {
    bool loaded = false;
    
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.readBytes(0, this, sizeof(*this));

    if (this->signature != EEPROM_SIGNATURE) {
        debug.println("\nStored Config Invalid - initializing...");
        strncpy(this->APIToken, APIToken, sizeof(this->APIToken));
        strncpy(this->APICandleID, APICandleID, sizeof(this->APICandleID));
        strncpy(this->SSID, WiFiSSID, sizeof(this->SSID));
        strncpy(this->WiFiPassword, WiFiPassword, sizeof(this->WiFiPassword));
        this->signature = EEPROM_SIGNATURE;

        auto written = EEPROM.writeBytes(0, this, sizeof(*this));
        debug.printf("%d bytes written to EEPROM\n", written);
    } else {
        debug.println("\nReusing Stored Config");
        loaded = true;
    }

    EEPROM.end();

    return loaded;
}