#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <WiFiClient.h>

#define MAX_CLIENTS 5

class ClientManager {
public:
    ClientManager();
    int addClient(WiFiClient& client);
    void removeClient(int index);
    bool isClientConnected(int index);
    WiFiClient& getClient(int index);
    int getClientCount();
    void updateClients();
    void handleNewConnection(WiFiClient& newClient);

private:
    WiFiClient clients[MAX_CLIENTS];
    bool clientConnected[MAX_CLIENTS];
    int clientCount;
};

#endif // CLIENT_MANAGER_H