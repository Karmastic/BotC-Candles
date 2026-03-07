#pragma once

#include <WebSocketsClient.h>
#include <ESPAsyncWebServer.h>

#include "CandleOperator.h"
#include "SavedConfig.h"
#include "Task.h"
#include "WSClient.h"

class BotCTask : public Task, private WSClient, private AsyncWebServer {
public:    
    BotCTask(IDebugStream *debugOutput, SavedConfig& config);

    bool SendText(const char *message) {
        return WSClient::SendText(message);
    }

    void Clear() {
        this->candleOperator.Clear();
    }

    virtual const char *Name();
    static const char *TaskName;

    // WSClient overrides
    virtual void handlePayload(uint8_t * payload, size_t length);

    // WebServer implementation
    void handleNotFound(AsyncWebServerRequest *request);
    void handleUpdate(AsyncWebServerRequest *request);
    void handleClear(AsyncWebServerRequest *request);
    void handleInstallUpdate(AsyncWebServerRequest *request, String tag);

    void setup();
    void loop();

private:
    CandleOperator candleOperator;
    SavedConfig config;
};