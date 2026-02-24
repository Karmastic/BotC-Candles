#include <Arduino.h>
#include <ArduinoJson.h>

#include <EEPROM.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <WiFi.h>

#include "main.h"
#include "CandleOperator.h"
#include "config.h"

#define LED_PIN 2
#define LED_BLINK_INTERVAL_MS (500 * 1000)
static uint64_t nextLEDBlinkTime = 0;

const char *STATUS_MESSAGE = "candle_status_update";
const char *REQUEST_STATUS_MESSAGE = "{\"event\":\"request_candle_status_update\"}";

WebSocketsClient webSocket;
WebServer server(80);

const uint8_t pins[] = { 13 };
CandleOperator co(1, 4, pins, 1, 0.05, 50);

#pragma pack(push, 2)
struct Config {
    uint32_t signature;
    char APIToken[64];
    char APICandleID[64];
    char SSID[32];
    char WiFiPassword[32];
    uint8_t reserved[1024];
} config;
#pragma pack(pop)

#define EEPROM_SIZE sizeof(Config)
#define EEPROM_SIGNATURE 0xDEADBEEF

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);

			    // send message to server when Connected
				webSocket.sendTXT("Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);
            handlePayload(payload, length);

			// send message to server
			// webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            // hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    pinMode(LED_PIN, OUTPUT);

    Serial.println();
    Serial.println();
    Serial.println();

    EEPROM.begin(EEPROM_SIZE);
    EEPROM.readBytes(0, &config, sizeof(config));

    if (config.signature != EEPROM_SIGNATURE) {
        Serial.println("\nStored Config Invalid - initializing...");
        strncpy(config.APIToken, APIToken, sizeof(config.APIToken));
        strncpy(config.APICandleID, APICandleID, sizeof(config.APICandleID));
        strncpy(config.SSID, WiFiSSID, sizeof(config.SSID));
        strncpy(config.WiFiPassword, WiFiPassword, sizeof(config.WiFiPassword));
        config.signature = EEPROM_SIGNATURE;
        auto written = EEPROM.writeBytes(0, &config, sizeof(config));
        Serial.printf("%d bytes written to EEPROM\n", written);
    } else {
        Serial.println("\nReusing Stored Config...");
    }
    EEPROM.end();

    Serial.printf("\nConnecting to WiFi network: %s\n", config.SSID);
    Serial.println();
 
    WiFi.begin(config.SSID, config.WiFiPassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    char url[256];
    snprintf(url, sizeof(url), "%s%s?token=%s", APIRootCandleURL, config.APICandleID, config.APIToken);
    webSocket.beginSSL(APIHost, APIPort, url);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(1000); // Default reconnect interval is 500.

    server.onNotFound(handle_NotFound);
    server.on("/u", handle_update);
    server.on("/c", handle_clear);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    webSocket.loop();
    server.handleClient();
    co.Animate();

    uint64_t time = esp_timer_get_time();
    if (time >= nextLEDBlinkTime) {
        nextLEDBlinkTime = time + LED_BLINK_INTERVAL_MS;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}

void handle_update() {
    webSocket.sendTXT(REQUEST_STATUS_MESSAGE);
    server.send(200, "text/html", "Request sent");
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void handle_clear() {
    co.Clear();
    server.send(200, "text/html", "OK");
}

void handlePayload(uint8_t *payload, size_t length) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    const char* event = doc["event"];
    Serial.print("Event: ");
    Serial.println(event);

    if (0 != strcmp(event, STATUS_MESSAGE)) {
        Serial.println("Unknown event");
        return;
    }

    JsonArray data = doc["payload"].as<JsonArray>();
    Serial.printf("Payload: %d Candles\n", data.size());

    co.SetCandleStates(data);
}
