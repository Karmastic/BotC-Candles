#include <Arduino.h>

#include <WiFi.h>

#include "main.h"
#include "ActivityLEDTask.h"
#include "AppTasks.h"
#include "BotCTask.h"
#include "SerialDebugStream.h"
#include "SavedConfig.h"
#include "WiFiSelectTask.h"

#define LED_PIN 2
#define LED_BLINK_INTERVAL_MS (500 * 1000)

void setup() {
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

    std::function<void(void)> wifiCB = [](void) -> void {
        auto appTasks = AppTasks::Instance();
        appTasks->RemoveTask(WiFiSelectTask::TaskName);
        appTasks->ActivateTask(BotCTask::TaskName);
        appTasks->ActivateTask(ActivityLEDTask::TaskName);
    };
    ITask *wifiTask = new WiFiSelectTask(&debug, config, loaded, wifiCB);
    appTasks->AddTask(wifiTask);
    appTasks->ActivateTask(WiFiSelectTask::TaskName);
}

void loop() {
    AppTasks::Instance()->ProcessLoop();
 }
