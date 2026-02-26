#include "IDebugStream.h"
#include "ITask.h"

class ActivityLEDTask : public ITask {
public:    
    ActivityLEDTask(IDebugStream *debugOutput, uint8_t pin, uint32_t blinkIntervalMs = 500);

    static const char *TaskName;

    virtual const char *Name();
    virtual void setup();
    virtual bool loop();

private:
    IDebugStream *debugOutput;
    uint8_t pin;
    uint32_t blinkIntervalMs;
    uint64_t nextLEDBlinkTime;
};
