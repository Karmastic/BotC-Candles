#include <ArduinoJson.h>

#include "AppTasks.h"
#include "BotCTask.h"
#include "hal.h"
#include "InstallUpdateTask.h"

const char *BotCTask::TaskName = "BotCTask";
const char *STATUS_MESSAGE = "candle_status_update";
const char *REQUEST_STATUS_MESSAGE = "{\"event\":\"request_candle_status_update\"}";

const uint8_t pins[] = CANDLE_LED_PINS;

#define countof(x) (sizeof(x) / sizeof((x)[0]))

static const char *urlFromConfig(SavedConfig &config)
{
    char url[256];
    snprintf(url, sizeof(url), "%s%s?token=%s", SavedConfig::RootCandleURL, config.APICandleID, config.APIToken);

    return strdup(url);
}

BotCTask::BotCTask(IDebugStream *debugOutput, SavedConfig &config)
    : Task(debugOutput), WSClient(debugOutput, SavedConfig::Host, urlFromConfig(config), true, 443, 1000), candleOperator(2, 6, pins, countof(pins), 0.05, 50), AsyncWebServer(80)
{
    this->config = config;
}

const char *BotCTask::Name()
{
    return BotCTask::TaskName;
}

void BotCTask::setup()
{
    // Connect to WebSocket server
    this->connect();

    AsyncWebServer::onNotFound(std::bind(&BotCTask::handleNotFound, this, std::placeholders::_1));
    AsyncWebServer::on(AsyncURIMatcher::exact("/u"), HTTP_GET, [this](AsyncWebServerRequest *request)
                       { this->handleUpdate(request); });
    AsyncWebServer::on(AsyncURIMatcher::exact("/c"), HTTP_GET, [this](AsyncWebServerRequest *request)
                       { this->handleClear(request); });
    AsyncWebServer::on(AsyncURIMatcher::exact("/i"), HTTP_GET, [this](AsyncWebServerRequest *request)
                       {
        String tag;
        if (request->hasParam("tag", false))
        {
            tag = request->getParam("tag", false)->value();
        }
        this->handleInstallUpdate(request, tag); });

    AsyncWebServer::begin();

    this->debugOutput->println("HTTP server started");
}

void BotCTask::loop()
{
    WSClient::doLoop();
    this->candleOperator.Animate();
}

void BotCTask::handlePayload(uint8_t *payload, size_t length)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error)
    {
        this->debugOutput->print("deserializeJson() failed: ");
        this->debugOutput->println(error.c_str());
        return;
    }

    const char *event = doc["event"];
    this->debugOutput->print("Event: ");
    this->debugOutput->println(event);

    if (0 != strcmp(event, STATUS_MESSAGE))
    {
        this->debugOutput->println("Unknown event");
        return;
    }

    JsonArray data = doc["payload"].as<JsonArray>();
    this->debugOutput->printf("Payload: %d Candles\n", data.size());

    this->candleOperator.SetCandleStates(data);
}

void BotCTask::handleNotFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void BotCTask::handleUpdate(AsyncWebServerRequest *request)
{
    this->SendText(REQUEST_STATUS_MESSAGE);
    request->send(200, "text/html", "Request sent");
}

void BotCTask::handleClear(AsyncWebServerRequest *request)
{
    this->Clear();
    request->send(200, "text/html", "OK");
}

void BotCTask::handleInstallUpdate(AsyncWebServerRequest *request, String tag)
{
    // Get JSON for latest release / tag
    // https://api.github.com/repos/Karmastic/BotC-Candles/releases/latest
    // Latest Tag is ".tag_name" (string eg "0.3.0")
    // Parse that string and compare against current version.

    std::function<void(void)> successCB = [](void) -> void
    {
        AppTasks::Instance()->RemoveTask(InstallUpdateTask::TaskName);
    };
    std::function<void(void)> failCB = [](void) -> void
    {
        AppTasks::Instance()->RemoveTask(InstallUpdateTask::TaskName);
    };

    InstallUpdateTask *updateTask = new InstallUpdateTask(this->debugOutput, tag, successCB, failCB);
    AppTasks::Instance()->AddTask(updateTask);
    AppTasks::Instance()->ActivateTask(InstallUpdateTask::TaskName);

    request->send(200, "text/html", "OK");
}
