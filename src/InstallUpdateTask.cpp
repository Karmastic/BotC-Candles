#include "InstallUpdateTask.h"
#include <Logger_M.hpp>

const char *InstallUpdateTask::TaskName = "InstallUpdateTask";
#define LED_PIN 2

InstallUpdateTask::InstallUpdateTask(IDebugStream *debugOutput, std::function<void(void)> cbSuccess, std::function<void(void)> cbFailure)
    : Task(debugOutput), cbSuccess(cbSuccess), cbFailure(cbFailure)
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
    auto result = this->myUpdater.update(this->client, "https://api.github.com/repos/Karmastic/BotC-Candles/releases/assets/367856343", "", requestCB);
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
