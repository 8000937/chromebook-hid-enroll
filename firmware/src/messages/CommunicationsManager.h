//
// Created by bradleygumm on 9/1/2026.
//

#ifndef FIRMWARE_COMMUNICATIONSMANAGER_H
#define FIRMWARE_COMMUNICATIONSMANAGER_H
#include <Stream.h>
#include <WiFiClient.h>
#include <espMqttClient.h>

#include "ControlMessage.h"
#include "StatusMessage.h"
#include "StepCompletedMessage.h"


class CommunicationsManager {
    public:
        CommunicationsManager(Stream& serial, const String& clientId);

        typedef std::function<void(const ControlMessage& controlMessage)> ControlMessageCallback;
        void onControlMessageReceived(ControlMessageCallback message);
        uint16_t sendInformationStatusChange(StatusMessage& message);
        uint16_t sendStatusChange(StatusMessage& message);
        uint16_t registerDevice();
        uint16_t stepCompleted(StepCompletedMessage& message);
        bool connectToMQTT();
    bool setupSubscriptions();
        void disconnectMQTT();
        bool isMqttConnected() const;
        bool reconnectMqtt(StatusMessage& initialStatusMessage);


    private:
        static CommunicationsManager* instance;
        Stream& serial;
        espMqttClient mqttClient;
        WiFiClient wifiClient;
        String clientId;
        unsigned long lastConnectionAttempt = 0;
        bool shouldReconnect = true;

        ControlMessageCallback onControlMessageReceivedCb = nullptr;

        uint16_t sendStringOverMqtt(const String& topic, const String& message, const String& src = "");

        // use the will to broadcast to the "fleet manager" that this device is no longer active.
        static void setupWill();

        static void onMqttConnect(bool sessionPresent);
        static void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason);
        static void onMqttSubscribe(uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len);
        static void onMqttUnsubscribe(uint16_t packetId);
        static void onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total);

        static void onMqttPublish(uint16_t packetId);



        // static void receiveFromMQTT(const char* topic, const uint8_t* payload, unsigned int length);
 };


#endif //FIRMWARE_COMMUNICATIONSMANAGER_H
