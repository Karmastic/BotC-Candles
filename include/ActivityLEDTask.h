#pragma once

#include "Task.h"

class ActivityLEDTask : public Task {
public:    
    ActivityLEDTask(IDebugStream *debugOutput, uint8_t pin, uint32_t blinkIntervalMs = 500);

    static const char *TaskName;

    virtual const char *Name();
    virtual void setup();
    virtual void loop();

private:
    uint8_t pin;
    uint32_t blinkIntervalMs;
    uint64_t nextLEDBlinkTime;
};
