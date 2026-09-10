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
    ExecutionStep(1, ExecutionStepType::DELAY, "", nullptr, 0, false, 20000, 20000, false),
    ExecutionStep(2, ExecutionStepType::WAIT_FOR_USB_DISCONNECT, "", nullptr, 0, false, 0, 100000, false),
    ExecutionStep(3, ExecutionStepType::WAIT_FOR_USB_CONNECT, "", nullptr, 0, false, 0, 100000, false),
    ExecutionStep(4, ExecutionStepType::TYPE_STRING, "Hello World", nullptr, 0, false, 0, 10000, false)
};
static uint8_t stepCount = 5;

// controller state
static volatile bool running = true;
static volatile bool paused = false;
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

    // we subtract 1 because the timing of incrementing stepNumber and sending the status update out.
    currentStatusDetails.stepNumber = static_cast<int8_t>(stepNumber-1);
    currentStatusDetails.paused = !(running && !paused);
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
            timeStartedCurrentStep = millis();
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
            communicationsManager.registerDevice();
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

// only receives messages when the client id matches ours.
static void onControlMessageReceived(const ControlMessage& controlMessage) {
    // controlMessage.
    serial.printf("Control Message Received: %i\n", static_cast<int>(controlMessage.command));
    if (ControlCode::PAUSE == controlMessage.command) {
        paused = true;
        updateCurrentStatusDetails();
        communicationsManager.sendStatusChange(currentStatusDetails);
    }
    else if (ControlCode::RESUME == controlMessage.command) {
        // used to make this idempotent. if you send a resume when not paused, it doesnt do anything.
        // setting the paused flag to false when its already false, and then sending a status update is no biggie.
        if (paused == true)
        {
            // restart current step.
            executionManager.resetState();
        }
        paused = false;
        updateCurrentStatusDetails();
        communicationsManager.sendStatusChange(currentStatusDetails);

    }
}

void resetProgramState() {
    executionManager.resetState();
    stepNumber = -1;
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

    communicationsManager.onControlMessageReceived(onControlMessageReceived);

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
            if (stepNumber >= 0 && (executionSteps[stepNumber].type == ExecutionStepType::WAIT_FOR_USB_DISCONNECT || executionSteps[stepNumber].type == ExecutionStepType::WAIT_FOR_USB_CONNECT)) {
                // do nothing. this is planned.
            }
            else {
                statusCode = StatusCode::UNEXPECTED_USB_DISCONNECT;
                resetProgramState();
                running = false;
            }
        }
    }

    // main execution loop
    if (statusCode != StatusCode::EXECUTION_ERROR && running && !paused && (timer == 0 || millis() - timer > executionDelay)) {
        if (stepNumber == -1 && stepCount > 0) {
            stepNumber = 0;
            updateCurrentStatusDetails();
        }

        // if no errors, steps are defined, and we are not at the end of the step array
        if (stepCount > 0 && stepNumber > -1 && stepCount != stepNumber) {
            const ExecutionStatus execStatus = executionManager.execute(executionSteps[stepNumber]);
            if ( ExecutionStatus::COMPLETE == execStatus) {
                statusCode = StatusCode::STEP_COMPLETED;
                updateCurrentStatusDetails();
                stepNumber++;
            }
            else if (ExecutionStatus::TIMED_OUT == execStatus) {
                statusCode = StatusCode::EXECUTION_ERROR;
                updateCurrentStatusDetails();
            }
        }
        // if no errors, steps are defined, and we're at the end.
        else if (stepCount > 0 && stepNumber == stepCount) {
            statusCode = StatusCode::EXECUTION_COMPLETE;
            updateCurrentStatusDetails();
        }
        timer = millis();
    }


    /*
    a bug is found here. If you pause during a step that uses a timer (IE: Delay, timeouts),
     the execution will not resume with, for example, the same 200ms from when you initially paused it.
     Unknown if this is an issue.
     Not sure if I explain that well, but basically, it doesnt pause the delay/timer.
     Potential remedy is calculating how much of the delay has passed at the time of the pause, and then keep updating
     the timer with the current time minus the part of the delay that has already passed.
     OR
     Just re-run the step.

     Presumably this problem also applies to timeouts.

     BG -- ended up making it start the current step over again when resumed, though, i do like the idea of just updating the time to current time minus elapsed delay, but that creates its own issues.
     Use pause/resume sparingly.
    */
    statusManager.signalExecutionPaused(paused);

    // Test to show realtime status changes and confirm it works as expected.
    statusManager.setStatus(statusCode, informationStatus);

    // just a workaround to let us send the status update, and not lose the actual status.
    if (StatusCode::STEP_COMPLETED == statusCode) {
        statusCode = StatusCode::EXECUTING;
        updateCurrentStatusDetails();
    }
}