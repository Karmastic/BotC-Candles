#ifdef ESP32_32E

// This is a test program for the Adafruit ILI9341 library for the Cheap Yellow Display.
// It is based on the Adafruit TouchScreenTest program, but uses the XPT2046_Touchscreen
// library instead of the Adafruit_TouchScreen library.
// Modified by 2024-01-18  Marcel Verpaalen to use HW spi on the ESP32-2432S028R board
// As display and touch are using different SPI configurations.

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

#include "TouchDisplayTask.h"

#define TFT_BL 21
#define TFT_CS 15
#define TFT_DC 2
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_RST -1

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

// Note: the ESP32 has 2 SPI ports, to have ESP32-2432S028R work with the TFT and Touch on different SPI ports each needs to be defined and passed to the library
SPIClass hspi = SPIClass(HSPI);
SPIClass vspi = SPIClass(VSPI);

XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
// XPT2046_Touchscreen ts(XPT2046_CS); //also works, but then the IRQ is not used
Adafruit_ILI9341 tft = Adafruit_ILI9341(&hspi, TFT_DC, TFT_CS, TFT_RST);

const char *TouchDisplayTask::TaskName = "TouchDisplayTask";

TouchDisplayTask::TouchDisplayTask(IDebugStream *debugOutput) : Task(debugOutput)
{
}

const char *TouchDisplayTask::Name()
{
    return TouchDisplayTask::TaskName;
}

void TouchDisplayTask::setup()
{
    // Note: The Adafruit librarys is not setting the backlight on, so we need to do that in code
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH);

    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);

    // Setup the VSPI to use custom pins for the touch
    vspi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    ts.begin(vspi);

    ts.setRotation(1);
}

void touch_me(uint16_t color)
{
    tft.setTextSize(2);
    tft.setTextColor(color);
    tft.setCursor(30, 10);
    tft.print("Touch Test.... Touch me");
    tft.setTextSize(1);
}

boolean wastouched = true;

void TouchDisplayTask::loop()
{
    boolean istouched = ts.touched();
    if (istouched)
    {
        TS_Point p = ts.getPoint();
        if (!wastouched)
        {
            tft.fillScreen(ILI9341_BLACK);
            tft.setTextColor(ILI9341_YELLOW);
            tft.setCursor(60, 80);
            tft.print("Touch");
            touch_me(ILI9341_CYAN);
        }
        tft.fillRect(100, 150, 140, 60, ILI9341_BLACK);
        tft.setTextColor(ILI9341_GREEN);

        tft.setCursor(100, 150);
        tft.print("X = ");
        tft.print(p.x);
        tft.setCursor(100, 180);
        tft.print("Y = ");
        tft.print(p.y);
        Serial.print(", x = ");
        Serial.print(p.x);
        Serial.print(", y = ");
        Serial.println(p.y);
    }
    else
    {
        if (wastouched)
        {
            tft.fillScreen(ILI9341_BLACK);
            tft.setTextColor(ILI9341_RED);
            tft.setCursor(120, 50);
            tft.print("No");
            tft.setCursor(80, 120);
            tft.print("Touch");
            touch_me(ILI9341_CYAN);
        }
        Serial.println("no touch");
    }
    wastouched = istouched;
}

#endif // ESP32_32E