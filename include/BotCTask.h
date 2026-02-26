#pragma once

#include <WebSocketsClient.h>

#include "ITask.h"
#include "IDebugStream.h"
#include "CandleOperator.h"
#include "SavedConfig.h"
#include "WSClient.h"

class BotCTask : public ITask, private WSClient {
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
   
    void setup();
    bool loop();

private:
    CandleOperator candleOperator;
    IDebugStream *debugOutput;
    SavedConfig config;
};