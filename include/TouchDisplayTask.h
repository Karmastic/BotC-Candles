#pragma once
#ifdef ESP32_32E

#include "IDebugStream.h"
#include "IDisplay.h"
#include "Task.h"

class TouchDisplayTask : public Task, public IDisplay
{
public:
    TouchDisplayTask(IDebugStream *debugOutput);
    static const char *TaskName;

    virtual void setup();
    virtual void loop();

    virtual const char *Name();

    virtual void WriteLine(uint8_t row, uint8_t size, const char *text);

private:
};

#endif // ESP32_32E