#pragma once
#include <Arduino.h>

class IDisplay
{
public:
    virtual void WriteLine(uint8_t row, uint8_t size, const char *text) = 0;
};
