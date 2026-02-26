#pragma once

#include <WebSocketsClient.h>
#include "IDebugStream.h"

class WSClient
{
public:
    WSClient(IDebugStream *debugOutput, const char *host, const char *url, bool useSSL = true, uint16_t port = 443, uint32_t reconnectInterval = 500);

    virtual void handleEvent(WStype_t type, uint8_t * payload, size_t length);
    virtual void handlePayload(uint8_t * payload, size_t length) = 0;

    virtual void connect();
    void doLoop();
    virtual void disconnect();

    bool SendText(const char *message);

private:
    IDebugStream *debug;
    WebSocketsClient wsClient;

    bool useSSL;
    const char *host;
    const char *url;
    uint16_t port;
};
