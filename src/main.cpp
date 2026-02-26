#include <Arduino.h>

#include <WiFi.h>

#include "main.h"
#include "AppTasks.h"
#include "BotCTask.h"
#include "SerialDebugStream.h"
#include "SavedConfig.h"

#define LED_PIN 2
#define LED_BLINK_INTERVAL_MS (500 * 1000)
static uint64_t nextLEDBlinkTime = 0;

AppTasks appTasks(new SerialDebugStream(Serial));

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();
    Serial.println();

    IDebugStream &debug = *(new SerialDebugStream(Serial));

    pinMode(LED_PIN, OUTPUT);

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
    appTasks.AddTask(botCTask);

    appTasks.ActivateTask(BotCTask::TaskName);
}

void loop() {
    appTasks.ProcessLoop();
 
    uint64_t time = esp_timer_get_time();
    if (time >= nextLEDBlinkTime) {
        nextLEDBlinkTime = time + LED_BLINK_INTERVAL_MS;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}
