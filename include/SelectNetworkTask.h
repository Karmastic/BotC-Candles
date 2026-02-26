#pragma once

#include <functional>
#include <list>

#include <ESPAsyncWebServer.h>

#include "Task.h"

class SelectNetworkTask : public Task, private AsyncWebServer
{
public:
    static const char *TaskName;

    SelectNetworkTask(IDebugStream *debug, std::function<void(const char *ssid, const char *pwd)> wifiSelectedCB);

    virtual const char *Name() override
    {
        return SelectNetworkTask::TaskName;
    }

    void setup() override;
    void loop() override;

    // WebServer implementation
    void handleNotFound(AsyncWebServerRequest *request);
    void handleRoot(AsyncWebServerRequest *request);
    void handleSelectionSubmit(AsyncWebServerRequest *request);

private:
    std::function<void(const char *ssid, const char *pwd)> cb;
    std::list<String> ssidList;
};