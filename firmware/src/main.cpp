#include <Arduino.h>
#include <WiFi.h>
#include "Adafruit_NeoPixel.h"
#include "status/StatusLED.h"
#include "status/StatusManager.h"
#include "status/InformationCode.h"
#include "settings.h"
#include "execution/ExecutionManager.h"
#include "messages/CommunicationsManager.h"
#include "messages/StatusMessage.h"
#include "usb/UsbManager.h"

Stream &serial = Serial;
String clientId = WiFi.macAddress();

// I/O
const Adafruit_NeoPixel led(1, 48, NEO_GRB + NEO_KHZ800);
static USBHIDKeyboard Keyboard;

// subsystem controllers
static StatusManager statusManager(led, serial);
static CommunicationsManager communicationsManager(serial, clientId);
static UsbManager usbManager(serial);
static ExecutionManager executionManager(Keyboard,usbManager, serial);

// status fields
static volatile auto informationStatus = InformationCode::NONE;
static volatile auto statusCode = StatusCode::START_UP;
static StatusMessage currentStatusDetails;

// job state
static unsigned long timeStartedCurrentStep = 0;
static int8_t stepNumber = -1;
static int16_t jobId = -1;
static int16_t configVersion = -1;


// placeholder test steps.
static ExecutionStep executionSteps[] = {
    ExecutionStep(0, ExecutionStepType::WAIT_FOR_USB_CONNECT, "", nullptr, 0, false, 0, 1000, false),
    ExecutionStep(1, ExecutionStepType::DELAY, "", nullptr, 0, false, 10000, 10000, false),
    ExecutionStep(2, ExecutionStepType::TYPE_STRING, "Hello World", nullptr, 0, false, 0, 10000, false)
};
static uint8_t stepCount = 3;

// controller state
static volatile bool running = true;
const String firmwareVersion = "0.0.1";

// timing
static unsigned long timer = 0;
constexpr unsigned long executionDelay = 200;


void onWiFiConnected(WiFiEvent_t event, WiFiEventInfo_t info) {

    Serial.println("Connected to WiFi");
}
void onWiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    informationStatus = InformationCode::NONE;
    serial.print("IP Address assigned: ");
    serial.println(WiFi.localIP());
    // communicationsManager.reconnect(currentStatusDetails);
}
void onWiFiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    informationStatus = InformationCode::NO_WIFI;
    Serial.print("Disconnected from WiFi. Reason code: ");

    Serial.println(info.wifi_sta_disconnected.reason);

    WiFi.begin(App::Settings::ssid.c_str(), App::Settings::password.c_str());
}

static void updateCurrentStatusDetails() {
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

            //TODO i think this stepCompleted thing could also represent a execution failed status. maybe make this more general
            StepCompletedMessage msg(currentStatusDetails, millis()-timeStartedCurrentStep, false);
            communicationsManager.stepCompleted(msg);
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


static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    serial.println(event_id);
    usbManager.usbStatusCallback(arg, event_base, event_id, event_data);
}


void setup() {
    Serial.begin(115200);

    delay(1000);

    USB.onEvent(usbEventCallback);
    usbManager.setup(Keyboard);

    // Initial setup of the status message
    // these shouldnt change
    currentStatusDetails.clientId = clientId;
    currentStatusDetails.firmwareVersion = firmwareVersion;
    // the rest WILL change.
    updateCurrentStatusDetails();

    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);


    statusManager.onStatusChange(onStatusChange);

    WiFi.onEvent(onWiFiConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(onWiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(onWiFiDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.begin(App::Settings::ssid.c_str(), App::Settings::password.c_str());

    informationStatus = InformationCode::NO_WIFI;
}


void loop() {
    // check status of subsystems.
    if (WiFi.isConnected()) {
        if (!communicationsManager.isMqttConnected()) {
            communicationsManager.reconnectMqtt(currentStatusDetails);
            running = false;
        }
        else {
            running = true;
        }
    }
    else {
        running = false;
    }

    if ((millis() - timer) % 1000 <= 999) {
        if (!usbManager.isConnected()) {
            // TODO should this reset the step we're on?
            statusCode = StatusCode::UNEXPECTED_USB_DISCONNECT;
            running = false;
        }
        if (!running || statusCode == StatusCode::EXECUTION_ERROR) {
            // TODO fix this -- doesnt print negative statuses well.. and these are all negative.
            serial.printf("Program halted: %i\n", static_cast<int>(statusCode));
        }
    }

    // main execution loop
    if (statusCode != StatusCode::EXECUTION_ERROR && running && (timer == 0 || millis() - timer > executionDelay)) {
        if (stepNumber == -1 && stepCount > 0) {
            stepNumber = 0;
            updateCurrentStatusDetails();
        }

        // if no errors, steps are defined, and we are not at the end of the step array
        if (stepCount > 0 && stepNumber > -1 && stepCount != stepNumber) {
            const ExecutionStatus execStatus = executionManager.execute(executionSteps[stepNumber]);
            if ( ExecutionStatus::COMPLETE == execStatus) {
                statusCode = StatusCode::STEP_COMPLETED;
                stepNumber++;
                updateCurrentStatusDetails();
            }
            else if (ExecutionStatus::TIMED_OUT == execStatus) {
                statusCode = StatusCode::EXECUTION_ERROR;
            }
        }
        // if no errors, steps are defined, and we're at the end.
        else if (stepCount > 0 && stepNumber == stepCount) {
                statusCode = StatusCode::JOB_COMPLETE;
        }
        timer = millis();
    }


    // Test to show realtime status changes and confirm it works as expected.
    statusManager.setStatus(statusCode, informationStatus);

    // just a workaround to let us send the status update, and not lose the actual status.
    if (StatusCode::STEP_COMPLETED == statusCode) {
        statusCode = StatusCode::EXECUTING;
    }
}