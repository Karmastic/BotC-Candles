#include <functional>

#include "WSClient.h"

WSClient::WSClient(IDebugStream *debugOutput, const char *host, const char *url, bool useSSL, uint16_t port, uint32_t reconnectInterval) {
    this->debugOutput = debugOutput;
    this->host = host;
    this->url = url;
    this->useSSL = useSSL;
    this->port = port;

    this->wsClient.setReconnectInterval(reconnectInterval);
    this->wsClient.onEvent(std::bind(&WSClient::handleEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void WSClient::handleEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            this->debugOutput->printf("[WSClient] Disconnected!\n");
            break;

        case WStype_CONNECTED:
            this->debugOutput->printf("[WSClient] Connected to url: %s\n",  payload);

            // send message to server when Connected
            // this->wsClient.sendTXT("Connected");
            break;

        case WStype_TEXT:
            this->debugOutput->printf("[WSClient] get text: %s\n", payload);
            this->handlePayload(payload, length);

			// send message to server
			// webSocket.sendTXT("message here");
            break;

        case WStype_BIN:
            this->debugOutput->printf("[WSClient] get binary length: %u\n", length);
            // hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;

        case WStype_ERROR:
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }
}

void WSClient::connect() {
    if (this->useSSL) {
        this->wsClient.beginSSL(this->host, this->port, this->url);
    } else {
        this->wsClient.begin(this->host, this->port, this->url);
    }   
}

void WSClient::doLoop() {
    this->wsClient.loop();
}

void WSClient::disconnect() {
    this->wsClient.disconnect();
}

bool WSClient::SendText(const char *message) {
    return this->wsClient.sendTXT(message);
}
