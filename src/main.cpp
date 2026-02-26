#include <Arduino.h>

#include <WiFi.h>

#include "main.h"
#include "ActivityLEDTask.h"
#include "AppTasks.h"
#include "BotCTask.h"
#include "SerialDebugStream.h"
#include "SavedConfig.h"
#include "SelectNetworkTask.h"
#include "WiFiConnectTask.h"

#define LED_PIN 2
#define LED_BLINK_INTERVAL_MS (500 * 1000)

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();

    IDebugStream &debug = *(new SerialDebugStream(Serial));
    SavedConfig config;
    bool loaded = config.LoadConfig(debug);

    AppTasks *appTasks = new AppTasks(&debug);
    ITask *botCTask = new BotCTask(&debug, config);
    appTasks->AddTask(botCTask);
    ITask *activityLEDTask = new ActivityLEDTask(&debug, LED_PIN, LED_BLINK_INTERVAL_MS);
    appTasks->AddTask(activityLEDTask);
    appTasks->ActivateTask(ActivityLEDTask::TaskName);

    std::function<void(void)> wifiCB = [appTasks](void) -> void
    {
        appTasks->RemoveTask(WiFiConnectTask::TaskName);
        appTasks->ActivateTask(BotCTask::TaskName);
        appTasks->RemoveTask(SelectNetworkTask::TaskName);
    };
    std::function<void(void)> failCB = [appTasks](void) -> void
    {
        appTasks->DeactivateTask(WiFiConnectTask::TaskName);
        appTasks->ActivateTask(SelectNetworkTask::TaskName);
    };
    ITask *wifiTask = new WiFiConnectTask(&debug, config, loaded, wifiCB, failCB);
    appTasks->AddTask(wifiTask);

    std::function<void(const char *ssid, const char *pwd)> selectedCB = [appTasks, &config](const char *ssid, const char *pwd) -> void
    {
        Serial.printf("Selected WiFi network: %s (%s)\n", ssid, pwd);

        memset(config.SSID, 0, sizeof(config.SSID));
        memset(config.WiFiPassword, 0, sizeof(config.WiFiPassword));
        strcpy(config.SSID, ssid);
        strcpy(config.WiFiPassword, pwd);

        // config.SaveConfig(debug);

        // Deactivate selection task in case we need to try again
        appTasks->DeactivateTask(SelectNetworkTask::TaskName);
        appTasks->ActivateTask(WiFiConnectTask::TaskName);
    };
    ITask *selectTask = new SelectNetworkTask(&debug, selectedCB);
    appTasks->AddTask(selectTask);

    if (config.SSID[0] == '\0')
    {
        appTasks->ActivateTask(SelectNetworkTask::TaskName);
    }
    else
    {
        appTasks->ActivateTask(WiFiConnectTask::TaskName);
    }
}

void loop()
{
    AppTasks::Instance()->ProcessLoop();
}
