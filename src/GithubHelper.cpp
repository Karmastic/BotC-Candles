#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "GithubHelper.h"

bool GithubHelper::getLatestReleaseTag(IDebugStream *debugOutput, const char *owner, const char *repo, String &tag)
{
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    char url[256];
    snprintf(url, sizeof(url), "https://api.github.com/repos/%s/%s/releases/latest", owner, repo);

    http.begin(client, url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0)
    {
        String payload = http.getString();

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload.c_str(), payload.length());
        if (error)
        {
            debugOutput->print("deserializeJson() failed: ");
            debugOutput->println(error.c_str());
        }
        else
        {
            tag = doc["tag_name"].as<String>();
            debugOutput->printf("Latest release tag: %s\n", tag.c_str());
        }
    }
    else
    {
        debugOutput->printf("Error retrieving latest release metadata: Code = %d\n", httpResponseCode);
    }

    http.end();
    return tag.length() > 0;
}
