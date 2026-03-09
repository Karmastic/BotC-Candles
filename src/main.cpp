#include <Arduino.h>

#include <WiFi.h>

#include "main.h"
#include "ActivityLEDTask.h"
#include "AppTasks.h"
#include "BotCTask.h"
#include "hal.h"
#include "SerialDebugStream.h"
#include "SavedConfig.h"
#include "SelectNetworkTask.h"
#include "version.h"
#include "WiFiConnectTask.h"

#ifdef ESP32_32E
#include "TouchDisplayTask.h"
#endif // ESP32_32E

#define LED_BLINK_INTERVAL_MS (500 * 1000)

static SavedConfig config;

void setup()
{
    Serial.begin(SERIAL_DEBUG_BAUDRATE);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();

    Serial.printf("Starting BotC-Candles [v%s]...\n", APP_VERSION);

    pinMode(RESET_CONFIG_INPUT, RESET_CONFIG_INPUT_MODE);

    bool resetNetwork = digitalRead(RESET_CONFIG_INPUT) == RESET_CONFIG_INPUT_STATE;
    if (resetNetwork)
    {
        Serial.println("Reset network button pressed.  Forcing SelectNetwork Task.");
    }

    IDebugStream *pDebug = new SerialDebugStream(Serial);
    IDebugStream &debug = *pDebug;
    config.Load(debug);

    AppTasks *appTasks = new AppTasks(pDebug);
    ITask *botCTask = new BotCTask(pDebug, config);
    appTasks->AddTask(botCTask);
    ActivityLEDTask *activityLEDTask = new ActivityLEDTask(pDebug, ACTIVITY_LED, LED_BLINK_INTERVAL_MS);
    appTasks->AddTask(activityLEDTask);
    appTasks->ActivateTask(ActivityLEDTask::TaskName);

#ifdef ESP32_32E
    TouchDisplayTask *dt = new TouchDisplayTask(pDebug);
    appTasks->AddTask(dt);
    appTasks->ActivateTask(TouchDisplayTask::TaskName);
#endif // ESP32_32E

    std::function<void(void)> wifiCB = [appTasks, activityLEDTask, pDebug](void) -> void
    {
        appTasks->RemoveTask(WiFiConnectTask::TaskName);
        appTasks->ActivateTask(BotCTask::TaskName);
        appTasks->RemoveTask(SelectNetworkTask::TaskName);
        activityLEDTask->SetMultiplier(1.0f);

        // In case we changed networks, save it once we successfully connect
        // NOTE: If nothing changes, this won't actually write to EEPROM.
        config.Save(*pDebug);
    };
    std::function<void(void)> failCB = [appTasks, activityLEDTask](void) -> void
    {
        appTasks->DeactivateTask(WiFiConnectTask::TaskName);
        appTasks->ActivateTask(SelectNetworkTask::TaskName);
        activityLEDTask->SetMultiplier(0.5f);
    };
    ITask *wifiTask = new WiFiConnectTask(pDebug, config, wifiCB, failCB);
    appTasks->AddTask(wifiTask);

    std::function<void(const char *ssid, const char *pwd)> selectedCB = [appTasks, activityLEDTask](const char *ssid, const char *pwd) -> void
    {
        memset(config.SSID, 0, sizeof(config.SSID));
        memset(config.WiFiPassword, 0, sizeof(config.WiFiPassword));
        strcpy(config.SSID, ssid);
        strcpy(config.WiFiPassword, pwd);

        // Deactivate selection task in case we need to try again
        appTasks->DeactivateTask(SelectNetworkTask::TaskName);
        activityLEDTask->SetMultiplier(1.0f);
        appTasks->ActivateTask(WiFiConnectTask::TaskName);
    };
    ITask *selectTask = new SelectNetworkTask(pDebug, selectedCB);
    appTasks->AddTask(selectTask);

    if (config.SSID[0] == '\0' || resetNetwork)
    {
        appTasks->ActivateTask(SelectNetworkTask::TaskName);
        activityLEDTask->SetMultiplier(0.5f);
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
