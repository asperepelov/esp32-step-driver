#include "ClientManager.h"
#include <Arduino.h>

ClientManager::ClientManager() : clientCount(0) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clientConnected[i] = false;
    }
}

int ClientManager::addClient(WiFiClient& client) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clientConnected[i]) {
            clients[i] = client;
            clientConnected[i] = true;
            clientCount++;
            return i;
        }
    }
    return -1;
}

void ClientManager::removeClient(int index) {
    if (index >= 0 && index < MAX_CLIENTS && clientConnected[index]) {
        clients[index].stop();
        clientConnected[index] = false;
        clientCount--;
    }
}

bool ClientManager::isClientConnected(int index) {
    return (index >= 0 && index < MAX_CLIENTS && clientConnected[index]);
}

WiFiClient& ClientManager::getClient(int index) {
    return clients[index];
}

int ClientManager::getClientCount() {
    return clientCount;
}

void ClientManager::updateClients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientConnected[i] && !clients[i].connected()) {
            removeClient(i);
        }
    }
}

void ClientManager::handleNewConnection(WiFiClient& newClient) {
    int clientIndex = addClient(newClient);
    if (clientIndex != -1) {
        Serial.printf("\nНовый клиент подключен %d/%d\n", clientIndex, getClientCount());
    } else {
        Serial.println("\n\nДостигнуто максимальное количество клиентов");
        newClient.stop();
    }
}