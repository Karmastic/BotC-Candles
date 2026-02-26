#include <Arduino.h>

#include <EEPROM.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <WiFi.h>

#include "main.h"
#include "AppTasks.h"
#include "BotCTask.h"
#include "SerialDebugStream.h"
#include "SavedConfig.h"

#define LED_PIN 2
#define LED_BLINK_INTERVAL_MS (500 * 1000)
static uint64_t nextLEDBlinkTime = 0;

const char *REQUEST_STATUS_MESSAGE = "{\"event\":\"request_candle_status_update\"}";

AppTasks appTasks(new SerialDebugStream(Serial));

WebServer server(80);

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

    debug.println("Creating BotCTask...");
    ITask *botCTask = new BotCTask(&debug, config);
    debug.println("Adding BotCTask...");
    appTasks.AddTask(botCTask);

    server.onNotFound(handle_NotFound);
    server.on("/u", handle_update);
    server.on("/c", handle_clear);

    server.begin();
    debug.println("HTTP server started");

    appTasks.ActivateTask(BotCTask::TaskName);
}

void loop() {
    appTasks.ProcessLoop();
    server.handleClient();

    uint64_t time = esp_timer_get_time();
    if (time >= nextLEDBlinkTime) {
        nextLEDBlinkTime = time + LED_BLINK_INTERVAL_MS;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}

void handle_update() {
    auto task = (BotCTask*)appTasks.LookupTask(BotCTask::TaskName);
    if (!task) {
        Serial.println("BotCTask not found");
        server.send(500, "text/html", "BotCTask not found");
        return;
    }

    task->SendText(REQUEST_STATUS_MESSAGE);
    server.send(200, "text/html", "Request sent");
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void handle_clear() {
    auto task = (BotCTask*)appTasks.LookupTask(BotCTask::TaskName);
    if (!task) {
        Serial.println("BotCTask not found");
        server.send(500, "text/html", "BotCTask not found");
        return;
    }

    task->Clear();
    server.send(200, "text/html", "OK");
}
