#ifndef ETHERNET_MANAGER_H
#define ETHERNET_MANAGER_H

#include <ETH.h>

class EthernetManager {
public:
    EthernetManager(IPAddress ip, IPAddress gateway, IPAddress netMask);
    bool begin();
    void checkAndRestartEthernet();
    bool isConnected();
    IPAddress getIP();

private:
    IPAddress _ip;
    IPAddress _gateway;
    IPAddress _subnet;
    unsigned long _lastCheckTime = 0;
    static const unsigned long CHECK_INTERVAL = 2000; // Проверка каждые 5 секунд
    bool _isConnected = false;
};

#endif // ETHERNET_MANAGER_H