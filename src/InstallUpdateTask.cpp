#include <Logger_M.hpp>

#include "GithubHelper.h"
#include "hal.h"
#include "InstallUpdateTask.h"
#include "version.h"

const char *InstallUpdateTask::TaskName = "InstallUpdateTask";

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
    if (this->tag.length() == 0) // Tag not provided?  Look for latest release
    {
        if (!GithubHelper::getLatestReleaseTag(this->debugOutput, "Karmastic", "BotC-Candles", tag))
        {
            this->debugOutput->println("Failed to get latest release tag");
            return;
        }
    }

    if (tag == APP_VERSION)
    {
        this->debugOutput->println("Already on latest version");
    }
    else
    {
        std::function<void(HTTPClient *)> requestCB = [this](HTTPClient *http)
        {
            Logger_M::printlnV("InstallUpdateTask", "Inside request callback");
            http->addHeader("Accept", "application/octet-stream");
            http->setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
        };
        this->myUpdater.setLedPin(ACTIVITY_LED, LOW);
        char url[256];
        snprintf(url, sizeof(url), "https://github.com/Karmastic/BotC-Candles/releases/download/%s/%s-firmware.bin", this->tag.c_str(), HAL_DEVICE_ID);
        auto result = this->myUpdater.update(this->client, url, "", requestCB);
        if (result == HTTP_UPDATE_OK)
        {
            ESP.restart();
        }
        else if (result == HTTP_UPDATE_NO_UPDATES)
        {
            this->debugOutput->println("No updates available");
        }
        else
        {
            this->debugOutput->printf("Update failed: %s\n", this->myUpdater.getLastErrorString().c_str());
            this->cbFailure();
            return;
        }
    }

    this->cbSuccess();
}
