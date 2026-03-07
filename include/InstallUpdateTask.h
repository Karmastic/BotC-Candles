#pragma once

#include "Task.h"
#include "UpdateOTA_M.hpp"
#include "WiFiClientSecure.h"

class InstallUpdateTask : public Task
{
public:
    InstallUpdateTask(IDebugStream *debugOutput, String tag, std::function<void(void)> cbSuccess, std::function<void(void)> cbFailure);
    static const char *TaskName;

    virtual const char *Name();
    virtual void setup();
    virtual void loop();

private:
    WiFiClientSecure client;
    UpdateOTA_M myUpdater;
    String tag;
    std::function<void(void)> cbSuccess;
    std::function<void(void)> cbFailure;
};
