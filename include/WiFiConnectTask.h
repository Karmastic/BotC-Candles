#pragma once

#include "Task.h"
#include "SavedConfig.h"

class WiFiConnectTask : public Task {
public:    
    WiFiConnectTask(IDebugStream *debugOutput, SavedConfig& config, std::function<void(void)> connectedCallback, std::function<void(void)> failedCallback);

    static const char *TaskName;

    virtual const char *Name();
    virtual void setup();
    virtual void loop();

private:
    SavedConfig& config;
    uint64_t timeoutTime;
    std::function<void(void)>  connectedCallback;
    std::function<void(void)>  failedCallback;
};
