//
// Created by bradleygumm on 9/1/2026.
//

#include "CommunicationsManager.h"

CommunicationsManager::CommunicationsManager(Stream& serial) : serial(serial) {};


void CommunicationsManager::sendInformationStatusChange(StatusMessage message) {

}

void CommunicationsManager::sendStatusChange(StatusMessage message) {

}

void CommunicationsManager::stepCompleted(StepCompletedMessage message) {

}

void CommunicationsManager::registerDevice(StatusMessage message) {

}
