//
// Created by bradleygumm on 9/1/2026.
//

#include "CommunicationsManager.h"

#include "settings.h"
CommunicationsManager* CommunicationsManager::instance = nullptr;



CommunicationsManager::CommunicationsManager(Stream& serial, const String& clientId) : serial(serial), clientId(clientId) {
    instance = this;
    instance->mqttClient.onConnect(onMqttConnect);
    instance->mqttClient.onDisconnect(onMqttDisconnect);
    instance->mqttClient.onSubscribe(onMqttSubscribe);
    instance->mqttClient.onUnsubscribe(onMqttUnsubscribe);
    instance->mqttClient.onMessage(onMqttMessage);
    instance->mqttClient.onPublish(onMqttPublish);
    instance->mqttClient.setServer(App::Settings::mqtt_broker, App::Settings::mqtt_broker_port);
    instance->mqttClient.setClientId(clientId.c_str());
};

bool CommunicationsManager::reconnectMqtt(StatusMessage& registerMessage) {
    if (shouldReconnect && millis() - lastConnectionAttempt > 10000) {
        lastConnectionAttempt = millis();
    }
    else {
        return this->mqttClient.connected();
    }
    const bool connected = connectToMQTT();
    if (connected) {
        setupSubscriptions();
        registerDevice(registerMessage);
    }
    // serial.printf("Reconnecting to MQTT broker failed. Reason: %d\n", this->mqttClient.);
    return false;
}

uint16_t CommunicationsManager::sendInformationStatusChange(StatusMessage& message) {
    String jsonString;
    serializeJson(message.toJson(),jsonString);
    return this->sendStringOverMqtt(App::Settings::status_topic.c_str(),jsonString, "Info Status Change");
}

uint16_t CommunicationsManager::sendStatusChange(StatusMessage& message) {
    String jsonString;
    serializeJson(message.toJson(),jsonString);
    return this->sendStringOverMqtt(App::Settings::status_topic.c_str(),jsonString, "Status Change");
}

uint16_t CommunicationsManager::stepCompleted(StepCompletedMessage& message) {
    String jsonString;
    serializeJson(message.toJson(),jsonString);
    return this->sendStringOverMqtt(App::Settings::status_topic.c_str(),jsonString, "Step Completed");
}

uint16_t CommunicationsManager::registerDevice(StatusMessage& message) {
    String jsonString;
    serializeJson(message.toJson(),jsonString);
    const uint16_t packetId = this->sendStringOverMqtt(App::Settings::register_topic.c_str(),jsonString, "Register Device");
    return packetId;
}

bool CommunicationsManager::setupSubscriptions() {
    if (instance != nullptr && this->mqttClient.connected()) {
        serial.printf("Attempting to subscribe to %s...\n", App::Settings::control_topic.c_str());
        const bool result = this->mqttClient.subscribe(App::Settings::control_topic.c_str(),2);
        if (!result) {
            serial.printf("Failed to subscribe to %s...", App::Settings::control_topic.c_str());
        }
        else {
            serial.printf("Subscribed to %s...", App::Settings::control_topic.c_str());
        }
        return result;
    }
    return false;
}

bool CommunicationsManager::connectToMQTT() {
    serial.printf("Connecting to MQTT broker...\n");
    const bool result = this->mqttClient.connect();
    serial.printf("Connected to MQTT broker? %d\n", this->mqttClient.connected());
    if (result) {
        shouldReconnect = false;
    }
    else {
        this->lastConnectionAttempt = millis();
        shouldReconnect = true;
    }
    return result;
}


void CommunicationsManager::disconnectMQTT() {
    if(this->mqttClient.connected()) {
        this->mqttClient.disconnect();
    }
}

bool CommunicationsManager::isMqttConnected() const {
    return this->mqttClient.connected();
}


// TMP

void CommunicationsManager::onMqttConnect(bool sessionPresent) {
  instance->serial.println("Connected to MQTT.");
  instance->serial.print("Session present: ");
  instance->serial.println(sessionPresent);

  /*uint16_t packetIdSub = instance->mqttClient.subscribe("foo/bar", 2);
  instance->serial.print("Subscribing at QoS 2, packetId: ");
  instance->serial.println(packetIdSub);
  instance->mqttClient.publish("foo/bar", 0, true, "test 1");
  instance->serial.println("Publishing at QoS 0");
  uint16_t packetIdPub1 = instance->mqttClient.publish("foo/bar", 1, true, "test 2");
  instance->serial.print("Publishing at QoS 1, packetId: ");
  instance->serial.println(packetIdPub1);
  uint16_t packetIdPub2 = instance->mqttClient.publish("foo/bar", 2, true, "test 3");
  instance->serial.print("Publishing at QoS 2, packetId: ");
  instance->serial.println(packetIdPub2);*/
}

uint16_t CommunicationsManager::sendStringOverMqtt(const String& topic, const String& message, const String& src) {
    const uint16_t packetId = this->mqttClient.publish(topic.c_str(),2,true, message.c_str());
    if (src != "") {
        instance->serial.printf("%s sent with packet id ", src.c_str());
        instance->serial.println(packetId);
    }
    return packetId;
}

void CommunicationsManager::onMqttDisconnect(espMqttClientTypes::DisconnectReason reason) {
  instance->serial.printf("Disconnected from MQTT: %u.\n", static_cast<uint8_t>(reason));

  if (WiFi.isConnected()) {
    instance->shouldReconnect = true;
    instance->lastConnectionAttempt = millis();
  }
}

void CommunicationsManager::onMqttSubscribe(uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len) {
  instance->serial.println("Subscribe acknowledged.");
  instance->serial.print("  packetId: ");
  instance->serial.println(packetId);
  for (size_t i = 0; i < len; ++i) {
    instance->serial.print("  qos: ");
    instance->serial.println(static_cast<uint8_t>(codes[i]));
  }
}

void CommunicationsManager::onMqttUnsubscribe(uint16_t packetId) {
  instance->serial.println("Unsubscribe acknowledged.");
  instance->serial.print("  packetId: ");
  instance->serial.println(packetId);
}

void CommunicationsManager::onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
  (void) payload;
  instance->serial.println("Publish received.");
  instance->serial.print("  topic: ");
  instance->serial.println(topic);
  instance->serial.print("  qos: ");
  instance->serial.println(properties.qos);
  instance->serial.print("  dup: ");
  instance->serial.println(properties.dup);
  instance->serial.print("  retain: ");
  instance->serial.println(properties.retain);
  instance->serial.print("  len: ");
  instance->serial.println(len);
  instance->serial.print("  index: ");
  instance->serial.println(index);
  instance->serial.print("  total: ");
  instance->serial.println(total);
}

void CommunicationsManager::onMqttPublish(uint16_t packetId) {
  instance->serial.println("Publish acknowledged.");
  instance->serial.print("  packetId: ");
  instance->serial.println(packetId);
}