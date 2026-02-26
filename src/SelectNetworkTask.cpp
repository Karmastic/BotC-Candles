#include <Arduino.h>
#include <WiFi.h>

#include "SelectNetworkTask.h"

const char *SelectNetworkTask::TaskName = "SelectNetworkTask";

SelectNetworkTask::SelectNetworkTask(IDebugStream *debug, std::function<void(const char *ssid, const char *pwd)> wifiSelectedCB)
    : Task(debug), AsyncWebServer(80)
{
    this->cb = wifiSelectedCB;
}

void SelectNetworkTask::setup()
{
    char buf[128];

    if (this->ssidList.empty())
    {
        this->debugOutput->println("Available WiFi networks:");
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; ++i)
        {
            this->debugOutput->printf("%d: %s (RSSI: %d)\n", i, WiFi.SSID(i).c_str(), WiFi.RSSI(i));

            snprintf(buf, sizeof(buf), "%s [%d]", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
            this->ssidList.push_back(buf);
        }

        AsyncWebServer::onNotFound(std::bind(&SelectNetworkTask::handleNotFound, this, std::placeholders::_1));
        AsyncWebServer::on(AsyncURIMatcher::exact("/"), HTTP_GET, [this](AsyncWebServerRequest *request)
                        { this->handleRoot(request); });
        AsyncWebServer::on(AsyncURIMatcher::exact("/usewifi"), HTTP_POST, [this](AsyncWebServerRequest *request)
                        { this->handleSelectionSubmit(request); });
    }
    else
    {
        this->debugOutput->println("WiFi networks already scanned");
    }

    snprintf(buf, sizeof(buf), "BotC-%06X", ESP.getEfuseMac() & 0xFFFFFF);
    WiFi.softAP(buf, nullptr, 1, false);

    AsyncWebServer::begin();

    this->debugOutput->printf("HTTP server started on %s\n", WiFi.softAPIP().toString().c_str());
}

void SelectNetworkTask::loop()
{
    delay(10);
}

void SelectNetworkTask::handleNotFound(AsyncWebServerRequest *request)
{
    this->debugOutput->println("Received unknown HTTP request");
    request->send(404, "text/plain", "Not found");
}

void SelectNetworkTask::handleRoot(AsyncWebServerRequest *request)
{
    String html = "<html><head><title>BotC WiFi Setup</title></head><body><h1>BotC WiFi Setup</h1><form method='POST' action='/usewifi'><label for='ssid'>SSID:</label><select name='ssid' id='ssid'>";
    for (const auto &ssid : this->ssidList)
    {
        html += "<option value='" + ssid + "'>" + ssid + "</option>";
    }
    html += "</select><br/><label for='pwd'>Password:</label><input type='password' name='pwd' id='pwd'/><br/><input type='submit' value='Connect'/></form></body></html>";

    request->send(200, "text/html", html);
}

void SelectNetworkTask::handleSelectionSubmit(AsyncWebServerRequest *request)
{
    if (!request->hasParam("ssid", true) || !request->hasParam("pwd", true))
    {
        request->send(400, "text/plain", "Missing ssid or pwd parameter");
        return;
    }

    String ssid = request->getParam("ssid", true)->value();
    String pwd = request->getParam("pwd", true)->value();

    auto end = strstr(ssid.c_str(), " [");
    if (end)
    {
        ssid = ssid.substring(0, end - ssid.c_str());
    }
    this->debugOutput->printf("Received WiFi credentials: SSID=%s\n", ssid.c_str());

    request->send(200, "text/plain", "Credentials received. Attempting to connect (this AP will now shut down)...");

    delay(1000);
    AsyncWebServer::end();
    WiFi.softAPdisconnect();

    this->cb(ssid.c_str(), pwd.c_str());
}
