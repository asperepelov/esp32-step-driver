#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiServer.h>
#include "ClientManager.h"

class WiFiManager {
public:
    WiFiManager(const char *ssid, const char *password,
                const IPAddress &apIP = IPAddress(10, 1, 1, 1),
                const IPAddress &netMask = IPAddress(255, 255, 255, 0)
    );

    void setup();

    bool isConnected();

    IPAddress getIP();

    void checkNewConnections();

private:
    const char *_ssid;
    const char *_password;
    const int MAX_CONNECTION_ATTEMPTS = 3;
    const int CONNECTION_ATTEMPT_DELAY = 500;
    const IPAddress _apIP;
    const IPAddress _apNetmask;

    void setupWiFi();

    void setupAccessPoint();
};

#endif // WIFI_MANAGER_H
