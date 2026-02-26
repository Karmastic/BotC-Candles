#pragma once

#include <Arduino.h>

#include "IDebugStream.h"

#pragma pack(push, 2)
struct SavedConfig {
    bool LoadConfig(IDebugStream& debugOutput);

    static const char *RootCandleURL;
    static const char *Host;
    static uint32_t Port;

    uint32_t signature;
    char APIToken[64];
    char APICandleID[64];
    char SSID[32];
    char WiFiPassword[32];
    uint8_t reserved[1024];
};
#pragma pack(pop)
