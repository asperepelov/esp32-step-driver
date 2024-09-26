#include "EthernetManager.h"

EthernetManager::EthernetManager(IPAddress ip, IPAddress gateway, IPAddress netMask)
    : _ip(ip), _gateway(gateway), _subnet(netMask) {
}

bool EthernetManager::begin() {
    ETH.begin();
    ETH.config(_ip, _gateway, _subnet);

    int attempts = 0;
    while (!ETH.linkUp() && attempts < 10) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (ETH.linkUp() && ETH.localIP() != IPAddress(0, 0, 0, 0)) {
        Serial.println("\nEthernet подключен");
        Serial.print("IP адрес: ");
        Serial.println(ETH.localIP());
        _isConnected = true;
        return true;
    } else {
        Serial.println("\nНе удалось подключить Ethernet");
        _isConnected = false;
        return false;
    }
}

void EthernetManager::checkAndRestartEthernet() {
    unsigned long currentTime = millis();
    if (currentTime - _lastCheckTime >= CHECK_INTERVAL) {
        _lastCheckTime = currentTime;
        if (!ETH.linkUp() || ETH.localIP() == IPAddress(0, 0, 0, 0)) {
            Serial.println("Ethernet отключен. Попытка переподключения...");
            _isConnected = false;
            begin();
        }
    }
}

bool EthernetManager::isConnected() {
    return _isConnected && ETH.linkUp() && ETH.localIP() != IPAddress(0, 0, 0, 0);
}

IPAddress EthernetManager::getIP() {
    return ETH.localIP();
}