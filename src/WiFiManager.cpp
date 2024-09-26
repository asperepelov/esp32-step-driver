#include "WiFiManager.h"

WiFiManager::WiFiManager(
    const char* ssid, const char* password,
    const IPAddress& apIP, const IPAddress& apNetmask
) : _ssid(ssid), _password(password),
    _apIP(apIP), _apNetmask(apNetmask) {}
void WiFiManager::setup() {
    setupWiFi();
}


void WiFiManager::setupWiFi() {
    Serial.println("Попытка подключения к Wi-Fi");
    WiFi.begin(_ssid, _password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < MAX_CONNECTION_ATTEMPTS) {
        delay(CONNECTION_ATTEMPT_DELAY);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nПодключено к Wi-Fi");
        Serial.print("IP-адрес Wi-Fi: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nНе удалось подключиться к Wi-Fi");
        // setupAccessPoint();
    }
}

void WiFiManager::setupAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(_apIP, _apIP, _apNetmask);
    if (WiFi.softAP(_ssid, _password)) {
        Serial.println("Точка доступа создана успешно");
        Serial.print("IP-адрес точки доступа: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Ошибка при создании точки доступа");
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

IPAddress WiFiManager::getIP() {
    if (isConnected()) {
        return WiFi.localIP();
    } else {
        return WiFi.softAPIP();
    }
}