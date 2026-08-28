#include <Arduino.h>

#include "Adafruit_NeoPixel.h"
#include "Color.h"
#include "StatusLED.h"
#include "StatusManager.h"
#include "InformationCode.h"
Adafruit_NeoPixel led(1, 48, NEO_GRB + NEO_KHZ800);
StatusLED statusLED(led, Serial, 50);
StatusManager statusManager(statusLED, Serial);

InformationCode informationStatus = InformationCode::NONE;

void setup() {
    Serial.begin(115200);
}

void loop() {

}