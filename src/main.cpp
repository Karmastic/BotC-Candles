#include <Arduino.h>

#include <WiFi.h>

#include "main.h"
#include "ActivityLEDTask.h"
#include "AppTasks.h"
#include "BotCTask.h"
#include "SerialDebugStream.h"
#include "SavedConfig.h"

#define LED_PIN 2
#define LED_BLINK_INTERVAL_MS (500 * 1000)

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();
    Serial.println();

    IDebugStream &debug = *(new SerialDebugStream(Serial));

    AppTasks *appTasks = new AppTasks(&debug);

    SavedConfig config;
    bool loaded = config.LoadConfig(debug);

    debug.printf("\nConnecting to WiFi network: %s\n\n", config.SSID);
 
    WiFi.begin(config.SSID, config.WiFiPassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        debug.print(".");
    }
    debug.println("\nWiFi connected");
    debug.print("IP address: ");
    debug.println(WiFi.localIP());

    ITask *botCTask = new BotCTask(&debug, config);
    appTasks->AddTask(botCTask);

    ITask *activityLEDTask = new ActivityLEDTask(&debug, LED_PIN, LED_BLINK_INTERVAL_MS);
    appTasks->AddTask(activityLEDTask);

    appTasks->ActivateTask(BotCTask::TaskName);
    appTasks->ActivateTask(ActivityLEDTask::TaskName);
}

void loop() {
    AppTasks::Instance()->ProcessLoop();
 }
