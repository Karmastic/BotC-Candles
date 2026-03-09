#pragma once
#ifdef ESP32_32E

#include "IDebugStream.h"
#include "Task.h"

class TouchDisplayTask : public Task
{
public:
    TouchDisplayTask(IDebugStream *debugOutput);
    static const char *TaskName;

    virtual void setup();
    virtual void loop();

    virtual const char *Name();

private:
};

#endif // ESP32_32E