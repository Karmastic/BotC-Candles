#include "InstallUpdateTask.h"

const char *InstallUpdateTask::TaskName = "InstallUpdateTask";

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
    auto result = this->myUpdater.update(this->client, "https://github.com/Karmastic/BotC-Candles/releases/download/0.1.3/firmware.bin");
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

void InstallUpdateTask::loop()
{
}
