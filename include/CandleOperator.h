#pragma once

#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

class CandleOperator {
public:
    CandleOperator(uint8_t ledsPerCandle, uint8_t ledsPerPin, const uint8_t *pins, size_t pinCount, float_t animationFadeRate = 0.1, uint8_t maxBrightnessPercent = 100);

    void Clear();

    void SetCandleStates(JsonArray candleStates);

    void Animate();

private:
    void SetCandleState(uint8_t candleIndex, uint32_t color, bool flickering);
    void Apply();

    uint8_t ledsPerCandle;
    uint8_t ledsPerPin;
    const uint8_t *pins;
    size_t pinCount;
    uint8_t maxCandles;
    uint8_t maxBrightness;
    float_t animationFadeRate;
    uint8_t candleCount; // Number of candles currently set (so we can clear candles that are no longer included)

    Adafruit_NeoPixel **pixels;
    bool *flickering;
    uint32_t *colors;
    float_t animationPercent;
    int8_t animationDirection;
};
