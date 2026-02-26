#include <ArduinoJson.h>

#include "BotCTask.h"

const char *BotCTask::TaskName = "BotCTask";
const char *STATUS_MESSAGE = "candle_status_update";
const char *REQUEST_STATUS_MESSAGE = "{\"event\":\"request_candle_status_update\"}";

const uint8_t pins[] = { 13 };

static const char *urlFromConfig(SavedConfig& config) {
    char url[256];
    snprintf(url, sizeof(url), "%s%s?token=%s", SavedConfig::RootCandleURL, config.APICandleID, config.APIToken);

    return strdup(url);
} 

BotCTask::BotCTask(IDebugStream *debugOutput, SavedConfig& config)
        : Task(debugOutput)
        , WSClient(debugOutput, SavedConfig::Host, urlFromConfig(config), true, 443, 1000)
        , candleOperator(1, 4, pins, 1, 0.05, 50)
        , WebServer(80) {
    this->config = config;
}

const char *BotCTask::Name() {
    return BotCTask::TaskName;
}

void BotCTask::setup() {
    // Connect to WebSocket server
    this->connect();

    WebServer::onNotFound(std::bind(&BotCTask::handleNotFound, this));
    WebServer::on("/u", std::bind(&BotCTask::handleUpdate, this));
    WebServer::on("/c", std::bind(&BotCTask::handleClear, this));

    WebServer::begin();

    this->debugOutput->println("HTTP server started");
}

void BotCTask::loop() {
    WSClient::doLoop();
    WebServer::handleClient();
    this->candleOperator.Animate();
}

void BotCTask::handlePayload(uint8_t *payload, size_t length) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
        this->debugOutput->print("deserializeJson() failed: ");
        this->debugOutput->println(error.c_str());
        return;
    }

    const char* event = doc["event"];
    this->debugOutput->print("Event: ");
    this->debugOutput->println(event);

    if (0 != strcmp(event, STATUS_MESSAGE)) {
        this->debugOutput->println("Unknown event");
        return;
    }

    JsonArray data = doc["payload"].as<JsonArray>();
    this->debugOutput->printf("Payload: %d Candles\n", data.size());

    this->candleOperator.SetCandleStates(data);
}

void BotCTask::handleNotFound() {
  this->send(404, "text/plain", "Not found");
}

void BotCTask::handleUpdate() {
    this->SendText(REQUEST_STATUS_MESSAGE);
    this->send(200, "text/html", "Request sent");
}

void BotCTask::handleClear() {
    this->Clear();
    this->send(200, "text/html", "OK");
}
