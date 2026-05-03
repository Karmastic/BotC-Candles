#include <Arduino.h>

#include "CandleOperator.h"

CandleOperator::CandleOperator(IDebugStream *debugOutput, uint8_t ledsPerCandle, uint8_t ledsPerPin, const uint8_t *pins, size_t pinCount, float_t animationFadeRate, uint8_t maxBrightness, int32_t wickLEDIndex)
    : debugOutput(debugOutput), ledsPerCandle(ledsPerCandle), ledsPerPin(ledsPerPin), pins(pins), pinCount(pinCount), animationFadeRate(animationFadeRate), maxBrightness(maxBrightness), wickLEDIndex(wickLEDIndex)
{
    this->pixels = new Adafruit_NeoPixel *[pinCount];

    for (size_t i = 0; i < pinCount; i++)
    {
        pinMode(pins[i], OUTPUT);

        this->pixels[i] = new Adafruit_NeoPixel(ledsPerPin, pins[i], NEO_GRB + NEO_KHZ800);
    }

    this->maxCandles = (this->pinCount * this->ledsPerPin) / this->ledsPerCandle;
    this->candleCount = 0;
    this->animationDirection = -1;
    this->animationPercent = min((uint8_t)100, maxBrightness);

    this->flickering = new bool[this->maxCandles];
    this->colors = new uint32_t[this->maxCandles];

    this->Clear();
}

void CandleOperator::Clear()
{
    for (size_t i = 0; i < this->pinCount; i++)
    {
        this->pixels[i]->clear();
    }

    memset(this->flickering, 0, this->maxCandles * sizeof(bool));
}

void CandleOperator::SetCandleStates(JsonArray candleStates)
{
    uint8_t newCandleCount = (uint8_t)candleStates.size();
    for (size_t i = 0; i < newCandleCount && i < this->maxCandles; i++)
    {
        JsonObject candleState = candleStates[i].as<JsonObject>();

        uint32_t red = candleState["red"].as<uint8_t>();
        uint32_t green = candleState["green"].as<uint8_t>();
        uint32_t blue = candleState["blue"].as<uint8_t>();

        uint32_t color = (red << 16) | (green << 8) | blue;
        bool flickering = candleState["flickering"].as<bool>();

        this->SetCandleState(i, color, flickering);
    }

    if (this->candleCount != newCandleCount)
    {
        for (size_t i = newCandleCount; i < this->candleCount; i++)
        {
            this->SetCandleState(i, 0, false);
        }

        this->candleCount = newCandleCount;
    }

    this->Apply();
}

void CandleOperator::SetCandleState(uint8_t candleIndex, uint32_t color, bool flickering)
{
    if (candleIndex >= this->maxCandles)
    {
        return;
    }

    uint8_t pin = candleIndex % this->pinCount;
    uint8_t ledStartIndex = (candleIndex / this->pinCount) * this->ledsPerCandle;

    this->flickering[candleIndex] = flickering;
    this->colors[candleIndex] = color;

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    uint8_t rF;
    uint8_t gF;
    uint8_t bF;

    if (flickering)
    {
        // int32_t pct = random(this->maxBrightness);
        // rF = (r * pct) / 100;
        // gF = (g * pct) / 100;
        // bF = (b * pct) / 100;
        rF = (r * this->animationPercent) / 100;
        gF = (g * this->animationPercent) / 100;
        bF = (b * this->animationPercent) / 100;
        r = 0;
        g = 0;
        b = 0;
    }
    else
    {
        rF = r = (r * this->maxBrightness) / 100;
        gF = g = (g * this->maxBrightness) / 100;
        bF = b = (b * this->maxBrightness) / 100;
    }

    color = (r << 16) | (g << 8) | b;
    uint32_t colorF = (rF << 16) | (gF << 8) | bF;

    for (uint8_t i = 0; i < this->ledsPerCandle; i++)
    {
        uint32_t c = (flickering && this->flickerCandleIndex(i)) ? colorF : color;
        this->pixels[pin]->setPixelColor(i + ledStartIndex, c);
    }

    if (candleIndex >= this->pinCount)
    {
        this->debugOutput->printf(" > Candle wrap-around. Candle %d - ledStartIndex = %d\n", candleIndex, ledStartIndex);
    }
}

void CandleOperator::Animate()
{
    this->animationPercent += this->animationFadeRate * this->animationDirection;
    if (this->animationPercent >= this->maxBrightness)
    {
        this->animationPercent = this->maxBrightness;
        this->animationDirection = -1;
    }
    else if (this->animationPercent <= 0)
    {
        this->animationPercent = 0;
        this->animationDirection = 1;
    }

    for (size_t i = 0; i < this->maxCandles; i++)
    {
        if (!this->flickering[i])
        {
            continue;
        }

        this->SetCandleState(i, this->colors[i], true);
    }

    this->Apply();
}

void CandleOperator::Apply()
{
    for (size_t i = 0; i < this->pinCount; i++)
    {
        this->pixels[i]->show();
    }
}
