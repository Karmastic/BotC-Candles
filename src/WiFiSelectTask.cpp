#include <WiFi.h>

#include "AppTasks.h"
#include "WiFiSelectTask.h"

const char *WiFiSelectTask::TaskName = "WiFiSelectTask";

WiFiSelectTask::WiFiSelectTask(IDebugStream *debugOutput, SavedConfig& config, bool configLoaded, std::function<void(void)> cb)
    : Task(debugOutput), config(config), configLoaded(configLoaded) {
    this->debugOutput->printf("wfst: config.signature: %u\n", this->config.signature);
    this->connectedCallback = cb;
}

const char *WiFiSelectTask::Name() {
    return WiFiSelectTask::TaskName;
}

void WiFiSelectTask::setup() {
    // if (this->configLoaded) {
    //     this->debugOutput->printf("Connecting to WiFi network: %s\n", this->config.SSID);
    //     WiFi.begin(this->config.SSID, this->config.WiFiPassword);
    // } else {
    //     this->debugOutput->println("No WiFi config found.  Not connecting to WiFi.");
    // }
    this->debugOutput->printf("\nConnecting to WiFi network: %s\n\n", config.SSID);
 
    WiFi.begin(config.SSID, config.WiFiPassword);
}

void WiFiSelectTask::loop() {
    if (WiFi.status() == WL_CONNECTED) {
        this->debugOutput->println("\nWiFi connected");
        this->debugOutput->print("IP address: ");
        this->debugOutput->println(WiFi.localIP());

        this->connectedCallback();
        this->debugOutput->println("Callback complete");
    } else {
        delay(500);
        this->debugOutput->print(".");
    }
}

