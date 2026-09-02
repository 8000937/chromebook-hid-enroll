#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Adafruit_NeoPixel.h"
#include "StatusLED.h"
#include "StatusManager.h"
#include "InformationCode.h"
#include "settings.h"

Adafruit_NeoPixel led(1, 48, NEO_GRB + NEO_KHZ800);
StatusLED statusLED(led, Serial, 50);
StatusManager statusManager(statusLED, Serial);

InformationCode informationStatus = InformationCode::NONE;

// unsigned long timer = millis();
void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {

    Serial.println("Connected to WiFi");
}
void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    informationStatus = InformationCode::NONE;
    Serial.print("IP Address assigned: ");
    Serial.println(WiFi.localIP());
}
void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    informationStatus = InformationCode::NO_WIFI;
    Serial.print("Disconnected from WiFi. Reason code: ");

    Serial.println(info.wifi_sta_disconnected.reason);

    WiFi.begin(App::Settings::ssid.c_str(), App::Settings::password.c_str());
}
void setup() {
    Serial.begin(115200);

    WiFi.onEvent(onWiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(onWiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(onWiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.begin(App::Settings::ssid.c_str(), App::Settings::password.c_str());
    informationStatus = InformationCode::NO_WIFI;
}

void loop() {
    // Test to show realtime status changes and confirm it works as expected.
    statusManager.setStatus(StatusCode::EXECUTING, informationStatus);
}