#pragma once

class ITask {
public:
    virtual const char *Name() = 0;

    virtual void setup() = 0;

    // Run loop.  Returns true if task is complete and should be removed from active tasks
    virtual bool loop() = 0;
};
