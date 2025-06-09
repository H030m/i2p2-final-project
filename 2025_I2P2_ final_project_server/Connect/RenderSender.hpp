// RenderSender.hpp
#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#include <vector>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

#define TARGET_FPS 60
#define NUM 5

struct ClientContext {
        SOCKET socket;
        bool active = true;
        nlohmann::json lastInput;
        std::thread recvThread;
        std::thread sendThread;
        int id;
        int x, y;
        ClientContext(){
            x = 0, y = 0;
        }
        //camera position
};

class RenderSender {
public:
    RenderSender(int port);
    RenderSender() = default;
    ~RenderSender();

    void start();
    void recvOnce(std::shared_ptr<ClientContext> ctx);
    void sendOnce(std::shared_ptr<ClientContext> ctx);
    void cleanupInactiveClients();
    void AddToFrame(const nlohmann::json& object) {
        std::lock_guard<std::mutex> lock(clientMutex);
        frame[object["type"]].push_back(object);
    }
    std::vector<std::shared_ptr<ClientContext>>& getClients() { return clients; }
    nlohmann::json frame;
    std::vector<std::shared_ptr<ClientContext>> clients;
    std::mutex clientMutex;
private:

    SOCKET serverSock;
#ifdef _WIN32
    WSADATA wsa;
#endif
    
    
};
