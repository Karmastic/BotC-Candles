#include <Arduino.h>

#include "ActivityLEDTask.h"

const char *ActivityLEDTask::TaskName = "ActivityLEDTask";

ActivityLEDTask::ActivityLEDTask(IDebugStream *debugOutput, uint8_t pin, uint32_t blinkIntervalMs)
        : Task(debugOutput)
        , pin(pin)
        , blinkIntervalMs(blinkIntervalMs)
        , nextLEDBlinkTime(0) {
}

const char *ActivityLEDTask::Name() {
    return ActivityLEDTask::TaskName;
}

void ActivityLEDTask::setup() {
    pinMode(this->pin, OUTPUT);
}

void ActivityLEDTask::loop() {
    uint64_t time = esp_timer_get_time();
    if (time >= nextLEDBlinkTime) {
        nextLEDBlinkTime = time + this->blinkIntervalMs;
        digitalWrite(this->pin, !digitalRead(this->pin));
    }
}
