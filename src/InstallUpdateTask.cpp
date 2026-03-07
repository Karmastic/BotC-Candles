#include "InstallUpdateTask.h"
#include <Logger_M.hpp>

const char *InstallUpdateTask::TaskName = "InstallUpdateTask";
#define LED_PIN 2

InstallUpdateTask::InstallUpdateTask(IDebugStream *debugOutput, String tag, std::function<void(void)> cbSuccess, std::function<void(void)> cbFailure)
    : Task(debugOutput), tag(tag), cbSuccess(cbSuccess), cbFailure(cbFailure)
{
}

const char *InstallUpdateTask::Name()
{
    return InstallUpdateTask::TaskName;
}

void InstallUpdateTask::setup()
{
    this->debugOutput->println("Setting up InstallUpdateTask");

    this->client.setInsecure();
    Logger_M::begin(&Serial, LOG_LEVEL_VERBOSE_M, true);
}

void InstallUpdateTask::loop()
{
    std::function<void(HTTPClient *)> requestCB = [this](HTTPClient *http) {
        Logger_M::printlnV("InstallUpdateTask", "Inside request callback");
        http->addHeader("Accept", "application/octet-stream");
        http->setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    };
    this->myUpdater.setLedPin(LED_PIN, LOW);
    char url[256];
    snprintf(url, sizeof(url), "https://github.com/Karmastic/BotC-Candles/releases/download/%s/firmware.bin", this->tag.c_str());
    auto result = this->myUpdater.update(this->client, url, "", requestCB);
    if (result == HTTP_UPDATE_OK)
    {
        this->cbSuccess();
        ESP.restart();
    }
    else if (result == HTTP_UPDATE_NO_UPDATES)
    {
        this->debugOutput->println("No updates available");
        this->cbSuccess();
    }
    else
    {
        this->debugOutput->printf("Update failed: %s\n", this->myUpdater.getLastErrorString().c_str());
        this->cbFailure();
    }
}
