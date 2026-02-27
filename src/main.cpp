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

#define RESET_NETWORK_PIN 14

static SavedConfig config;

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();

    pinMode(RESET_NETWORK_PIN, INPUT);

    bool resetNetwork = digitalRead(RESET_NETWORK_PIN) == LOW;
    if (resetNetwork)
    {
        Serial.println("Reset network button pressed.  Forcing SelectNetwork Task.");
    }

    IDebugStream *pDebug = new SerialDebugStream(Serial);
    IDebugStream &debug = *pDebug;
    config.Load(debug);

    AppTasks *appTasks = new AppTasks(&debug);
    ITask *botCTask = new BotCTask(&debug, config);
    appTasks->AddTask(botCTask);
    ITask *activityLEDTask = new ActivityLEDTask(&debug, LED_PIN, LED_BLINK_INTERVAL_MS);
    appTasks->AddTask(activityLEDTask);
    appTasks->ActivateTask(ActivityLEDTask::TaskName);

    std::function<void(void)> wifiCB = [appTasks, pDebug](void) -> void
    {
        appTasks->RemoveTask(WiFiConnectTask::TaskName);
        appTasks->ActivateTask(BotCTask::TaskName);
        appTasks->RemoveTask(SelectNetworkTask::TaskName);

        // In case we changed networks, save it once we successfully connect
        // NOTE: If nothing changes, this won't actually write to EEPROM.
        config.Save(*pDebug);
    };
    std::function<void(void)> failCB = [appTasks](void) -> void
    {
        appTasks->DeactivateTask(WiFiConnectTask::TaskName);
        appTasks->ActivateTask(SelectNetworkTask::TaskName);
    };
    ITask *wifiTask = new WiFiConnectTask(&debug, config, wifiCB, failCB);
    appTasks->AddTask(wifiTask);

    std::function<void(const char *ssid, const char *pwd)> selectedCB = [appTasks](const char *ssid, const char *pwd) -> void
    {
        memset(config.SSID, 0, sizeof(config.SSID));
        memset(config.WiFiPassword, 0, sizeof(config.WiFiPassword));
        strcpy(config.SSID, ssid);
        strcpy(config.WiFiPassword, pwd);

        // Deactivate selection task in case we need to try again
        appTasks->DeactivateTask(SelectNetworkTask::TaskName);
        appTasks->ActivateTask(WiFiConnectTask::TaskName);
    };
    ITask *selectTask = new SelectNetworkTask(&debug, selectedCB);
    appTasks->AddTask(selectTask);

    if (config.SSID[0] == '\0' || resetNetwork)
    {
        appTasks->ActivateTask(SelectNetworkTask::TaskName);
    }
    else
    {
        // NOTE: This will block until it connects or fails
        appTasks->ActivateTask(WiFiConnectTask::TaskName);
    }
}

void loop()
{
    AppTasks::Instance()->ProcessLoop();
}
