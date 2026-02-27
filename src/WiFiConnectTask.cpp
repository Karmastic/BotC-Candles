#include <WiFi.h>

#include "AppTasks.h"
#include "WiFiConnectTask.h"

const char *WiFiConnectTask::TaskName = "WiFiConnectTask";

const uint64_t WiFiConnectTimeoutMS = 15 * 1000 * 1000; // 15 seconds

WiFiConnectTask::WiFiConnectTask(IDebugStream *debugOutput, SavedConfig &config, std::function<void(void)> cb, std::function<void(void)> failCB)
    : Task(debugOutput), config(config)
{
    this->connectedCallback = cb;
    this->failedCallback = failCB;
    this->timeoutTime = 0;
}

const char *WiFiConnectTask::Name()
{
    return WiFiConnectTask::TaskName;
}

void WiFiConnectTask::setup()
{
    this->debugOutput->printf("\nConnecting to WiFi network: %s\n\n", config.SSID);

    this->timeoutTime = esp_timer_get_time() + WiFiConnectTimeoutMS;

    // NOTE: This is a blocking call - it does not return until it is connected or it encounters an error.
    WiFi.begin(config.SSID, config.WiFiPassword);
}

void WiFiConnectTask::loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        this->debugOutput->println("\nWiFi connected");
        this->debugOutput->print("IP address: ");
        this->debugOutput->println(WiFi.localIP());

        this->connectedCallback();
    }
    else
    {
        if (esp_timer_get_time() >= this->timeoutTime)
        {
            this->debugOutput->println("\nWiFi connection timed out");
            WiFi.disconnect();
            this->failedCallback();
            return;
        }

        delay(500);
        this->debugOutput->print(".");
    }
}
