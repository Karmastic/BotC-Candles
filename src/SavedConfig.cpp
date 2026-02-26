#include <EEPROM.h>

#include "config.h"
#include "SavedConfig.h"

#define EEPROM_SIZE sizeof(SavedConfig)
#define EEPROM_SIGNATURE 0xDEADBEAD

const char *SavedConfig::RootCandleURL = Config::APIRootCandleURL;
const char *SavedConfig::Host = Config::APIHost;
uint32_t SavedConfig::Port = Config::APIPort;

static SavedConfig storedConfig;

void SavedConfig::Load(IDebugStream &debug)
{
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.readBytes(0, this, sizeof(*this));

    if (this->signature != EEPROM_SIGNATURE)
    {
        memset(this, 0, sizeof(*this));
        debug.printf("\nStored Config Invalid (sig = 0x%x and not 0x%x) - initializing...\n", this->signature, EEPROM_SIGNATURE);
        strncpy(this->APIToken, Config::APIToken, sizeof(this->APIToken));
        strncpy(this->APICandleID, Config::APICandleID, sizeof(this->APICandleID));
        this->signature = EEPROM_SIGNATURE;

        // auto written = EEPROM.writeBytes(0, this, sizeof(*this));
        // debug.printf("%d bytes written to EEPROM\n", written);
    }
    else
    {
        debug.println("\nReusing Stored Config");

        storedConfig = *this;
    }

    EEPROM.end();
}

void SavedConfig::Save(IDebugStream &debug)
{
    if (0 == memcmp(this, &storedConfig, sizeof(*this)))
    {
        debug.println("Config unchanged - not writing to EEPROM");
        return;
    }

    EEPROM.begin(EEPROM_SIZE);

    auto written = EEPROM.writeBytes(0, this, sizeof(*this));
    debug.printf("%d bytes written to EEPROM\n", written);

    bool ok = EEPROM.commit();
    if (!ok)
    {
        debug.println("Error committing to EEPROM");
    }
    else
    {
        storedConfig = *this;
    }

    EEPROM.end();
}
