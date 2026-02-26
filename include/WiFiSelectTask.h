#include "IDebugStream.h"
#include "Task.h"
#include "SavedConfig.h"

class WiFiSelectTask : public Task {
public:    
    WiFiSelectTask(IDebugStream *debugOutput, SavedConfig& config, bool configLoaded, std::function<void(void)> callback);

    static const char *TaskName;

    virtual const char *Name();
    virtual void setup();
    virtual void loop();

private:
    SavedConfig& config;
    bool configLoaded;
    std::function<void(void)>  connectedCallback;
};
