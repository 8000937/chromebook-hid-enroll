#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Adafruit_NeoPixel.h"
#include "status/StatusLED.h"
#include "status/StatusManager.h"
#include "status/InformationCode.h"
#include "settings.h"
#include "messages/CommunicationsManager.h"
#include "messages/StatusMessage.h"

const Adafruit_NeoPixel led(1, 48, NEO_GRB + NEO_KHZ800);
StatusLED statusLED(led, Serial, 50);
StatusManager statusManager(statusLED, Serial);

CommunicationsManager communicationsManager(Serial);

InformationCode informationStatus = InformationCode::NONE;
StatusCode statusCode = StatusCode::START_UP;

unsigned long timeStartedCurrentStep = 0;
int8_t stepNumber = -1;
int16_t jobId = -1;
int16_t configVersion = -1;

const String clientId = WiFi.macAddress();
const String firmwareVersion = "0.0.1";

StatusMessage currentStatusDetails;



unsigned long timer = 0;
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
void updateCurrentStatusDetails() {
    currentStatusDetails.informationCode = static_cast<int8_t>(informationStatus);
    currentStatusDetails.statusCode = static_cast<int8_t>(statusCode);
    currentStatusDetails.configVersion = configVersion;
    currentStatusDetails.jobId = jobId;
    currentStatusDetails.stepNumber = stepNumber;
}
void onStatusChange(InformationCode newInfoCode, InformationCode oldInfoCode, StatusCode newStatusCode, StatusCode oldStatusCode)
{
    updateCurrentStatusDetails();
    if (newStatusCode != oldStatusCode) {
        Serial.print(clientId);
        Serial.print(" -- status changed from ");
        Serial.print(static_cast<int8_t>(oldStatusCode));
        Serial.print(" to ");
        Serial.println(static_cast<int8_t>(newStatusCode));

        updateCurrentStatusDetails();
        if (newStatusCode == StatusCode::STEP_COMPLETED) {
            //false is set for testing. this will come from the step eventually.
            //timeStartedCurrentStep will be set during the execution loop.
            communicationsManager.stepCompleted(StepCompletedMessage(currentStatusDetails, millis()-timeStartedCurrentStep, false));
        }
        else {
            communicationsManager.sendStatusChange(currentStatusDetails);
        }
    }
    if (newInfoCode != oldInfoCode) {
        Serial.print(clientId);
        Serial.print(" -- information status changed from ");
        Serial.print(static_cast<int8_t>(oldInfoCode));
        Serial.print(" to ");
        Serial.println(static_cast<int8_t>(newInfoCode));

        // send register command
        if (newInfoCode == InformationCode::NONE && oldInfoCode == InformationCode::NO_WIFI) {
            communicationsManager.registerDevice(currentStatusDetails);
        }
        else {
            communicationsManager.sendInformationStatusChange(currentStatusDetails);
        }
    }

}

// remove after testing
constexpr StatusCode codes[]= {StatusCode::EXECUTING, StatusCode::WAITING_FOR_CONFIG, StatusCode::EXECUTION_ERROR, StatusCode::UNEXPECTED_USB_DISCONNECT};

StatusCode current = codes[0];
//end remove after testing



void setup() {
    Serial.begin(115200);
    // Initial setup of the status message
    // these shouldnt change
    currentStatusDetails.clientId = clientId;
    currentStatusDetails.firmwareVersion = firmwareVersion;
    // the rest WILL change.
    updateCurrentStatusDetails();

    statusManager.onStatusChange(onStatusChange);
    WiFi.onEvent(onWiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(onWiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(onWiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.begin(App::Settings::ssid.c_str(), App::Settings::password.c_str());
    informationStatus = InformationCode::NO_WIFI;
}


void loop() {
    if (timer == 0 || millis() - timer > 10000) {
        const long r = random(0,4);
        if (r <= 3) {
            Serial.println(r);
            current = codes[r];
        }
        timer = millis();
    }
    // Test to show realtime status changes and confirm it works as expected.
    statusManager.setStatus(current, informationStatus);
}